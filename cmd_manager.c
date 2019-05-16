#include "cmd_manager.h"

static item_t total_items[MAX_SUPPORT_ITEMS];
static char inited_items = 0;

static void init_test_item(void){
	if(inited_items == 0){
		memset(total_items,0x00,sizeof(total_items));
		inited_items = 1;
	}
}

int register_cmd(int cmdid,FUNC func){  // 命令ID  函数名（参数1，参数2）
	int i = 0;
	init_test_item();
	for(i = 0; i < sizeof(total_items)/sizeof(total_items[0]); i++){
		if(total_items[i].cmdid == 0){        // 先全部清零然后再依次加载最多64项
			total_items[i].cmdid = cmdid;
			total_items[i].func = func;
			return 0;
		}
	}
	return -1;
}

int execute_cmd(int cmdid,char *param,int len){
	int i = 0;
	init_test_item();
	for(i = 0; i < sizeof(total_items)/sizeof(total_items[0]); i++){
		if(total_items[i].cmdid == cmdid){
			return total_items[i].func(param,len);
		}
	}
	return -1;
}

/*
param[input]:buffer  	串口收到的数据
param[out]:param		解包后得到的参数
param[out]:len			解包后得到的参数长度
*/
char decode_cmd_req_from_buffer(unsigned char *buffer, char *param, int *len) {
	int i = 0,tmp_len, lrc16, tmp_lrc16;
	char cmdid;
	unsigned char *cmd_buf;
	//find the cmd frame header
	while (buffer[i] != FRAME_HEADER) i++;
	if (!buffer[i])   // 如果一帧数据里面找不到 FRAME_HEADER
		return -1;
	cmd_buf = (buffer + i);    //指针！  
	//find cmdid
	cmdid = cmd_buf[FRAME_CMD_OFF];

	//find param length
	tmp_len = cmd_buf[FRAME_LEN_OFF] * 256 + cmd_buf[FRAME_LEN_OFF+1];
	*len = tmp_len;    //  why？ len = tmp_len;   不可以吗

	//copy the param
	memcpy(param, cmd_buf + FRAME_PARAM_OFF, tmp_len);  // 从buffer

	//check the crc16
	lrc16 = cmd_buf[FRAME_PARAM_OFF + tmp_len] * 256 + cmd_buf[FRAME_PARAM_OFF + tmp_len + 1];
	tmp_lrc16 = cal_lrc16((unsigned char*)cmd_buf + FRAME_LEN_OFF, FRAME_LEN_OFF + tmp_len); // 2 + tmp_len
	if (lrc16 != tmp_lrc16)
		return -1;

	// 把数据写入结构体中
	u_D2M_Frame_Data->t_FrameCmd  		= 	cmdid;
	u_D2M_Frame_Data->t_FrameDateLen	=	tmp_len;
	switch(cmdid){
		case D2M_UPLOAD_DOORLOCK_STATUS:
			u_D2M_Frame_Data->u_EventData->t_d2m_DoorstateUploadInfo->StateType		 	= cmd_buf[3];
			u_D2M_Frame_Data->u_EventData->t_d2m_DoorstateUploadInfo->UserId  			= cmd_buf[4] << 8 | cmd_buf[5];  // cmd_buf[4] << 8 + cmd_buf[5]
			break;

		case D2M_SYNC_SERVER_TIME:
			memcpy(u_D2M_Frame_Data->u_EventData->t_d2m_SyncServerTimeInfo->eDataAndTime,cmd_buf[3],6);
			break;

		case D2M_UPLOAD_USERDATA:
			u_D2M_Frame_Data->u_EventData->t_d2m_UserDataUploadInfo->UserId				= cmd_buf[3] << 8 | cmd_buf[4];
			u_D2M_Frame_Data->u_EventData->t_d2m_UserDataUploadInfo->eUserType  		= cmd_buf[5];
			u_D2M_Frame_Data->u_EventData->t_d2m_UserDataUploadInfo->eRegisterType		= cmd_buf[6];
			memcpy(u_D2M_Frame_Data->u_EventData->t_d2m_UserDataUploadInfo->tBCDStartTime,cmd_buf[7],6);
			memcpy(u_D2M_Frame_Data->u_EventData->t_d2m_UserDataUploadInfo->tBCDEndTime,cmd_buf[13],6);
			break;
		
		case D2M_UPLOAD_UNLOCK_RECORD:
			u_D2M_Frame_Data->u_EventData->t_d2m_OpenDoorInfo->UserID 					= cmd_buf[3] << 8 | cmd_buf[4];
			u_D2M_Frame_Data->u_EventData->t_d2m_OpenDoorInfo->eOpenDoorMethod 			= cmd_buf[5];
			u_D2M_Frame_Data->u_EventData->t_d2m_OpenDoorInfo->ePowerPercent  			= cmd_buf[6];
			memcpy(u_D2M_Frame_Data->u_EventData->t_d2m_OpenDoorInfo->Data_and_Time,cmd_buf[7],6);
			u_D2M_Frame_Data->u_EventData->t_d2m_OpenDoorInfo->LogType					= cmd_buf[13];
			break;
			
	}
	
	return cmdid;
}
#define D2M_ACCESS_NETWORK				0x33	//无线加网	数据流向：上传
#define D2M_EXIT_NETWORK				0x34	//无线退网	数据流向：上传
#define D2M_UPLOAD_DOORLOCK_STATUS		0x24	//门锁状态上传	数据流向：上传
#define D2M_SYNC_SERVER_TIME			0x21	//与服务器同步时间	数据流向：上传
#define D2M_UPLOAD_USERDATA				0x22	//用户数据上传	数据流向：上传
#define D2M_UPLOAD_UNLOCK_RECORD		0x23	//开锁日志上传	数据流向：上传
#define D2M_WAKEUP_ZIGBEE_MODULE		0x35	//门锁唤醒模块（用于远程密码开锁）	数据流向：上传


static U_D2M_Frame_Data u_D2M_Frame_Data;


/*****************************
*	计算校验码
*****************************/
int cal_lrc16(uint8_t *frame,uint16_t len){
	
	uint8_t lrcHi = 0xFF;
	uint8_t lrcLo = 0xFF;
	uint8_t temp = 0;
	unsigned long int iIndex = 0;
	
	while(len--){
		iIndex += *(frame++);   //
	}

	//if(iIndex <= 0xFFFF){
		lrcHi = (iIndex & 0xff00)>>8;   //前面要补零
		lrcLo = uint8_t(iIndex & 0x00ff);
	//}

	
	return (uint16_t)(lrcHi << 8 | lrcLo);
}

/*****************************
*	组帧发送给门锁
*****************************/
/*
int encode_cmd_rsp_to_buffer(unsigned char *buffer, int cmdid, char *param, int len) {
	int lrc16;
	//offset 0 byte
	buffer[FRAME_HEADER_OFF] = FRAME_HEADER;
	//offset 1 byte
	buffer[FRAME_CMD_OFF] = cmdid;
	//offset 2-3 byte
	if (param == NULL) {
		return param_err;
	}
	else {
		buffer[FRAME_LEN_OFF] = len >> 8;
		buffer[FRAME_LEN_OFF+1] = len & 0xff;
		memcpy(buffer+FRAME_PARAM_OFF+1, param, len);   // 为什么要加一
	}

	lrc16 = cal_lrc16((buffer + FRAME_CMD_OFF), FRAME_CMD_OFF + len);  // 计算校验码
	buffer[FRAME_HEADER_LEN_OFF + len    ] = (lrc16 >> 8) & 0xff;
	buffer[FRAME_HEADER_LEN_OFF + len + 1] = lrc16 & 0xff;
	return 0;
}
*/

int encode_cmd_rsp_to_buffer(unsigned char *buffer, int cmdid, char *param, int len) {
	int lrc16;
	//offset 0 byte
	buffer[FRAME_HEADER_OFF] = FRAME_HEADER;
	//offset 1 byte
	buffer[FRAME_CMD_OFF]		= cmdid;
	//offset 2-3 byte
	//buffer[FRAME_LEN_OFF] 		= (len & 0xFF00) >> 8;
	//buffer[FRAME_LEN_OFF + 1] 	= (unsigned char)(len & 0x00FF);
	buffer[FRAME_LEN_OFF] = len >> 8;
	buffer[FRAME_LEN_OFF+1] = len & 0xff;
	
	if(param != NULL)		
		memcpy(buffer+FRAME_PARAM_OFF+1, param, len);	// 为什么要加一

	lrc16 = cal_lrc16((buffer + FRAME_CMD_OFF), FRAME_CMD_OFF + len);  // 计算校验码
	buffer[FRAME_HEADER_LEN_OFF + len	 ] = (lrc16 >> 8) & 0xff;
	buffer[FRAME_HEADER_LEN_OFF + len + 1] = lrc16 & 0xff;
	return 0;
}


/*****************************
 *D2M 注册指令解析函数
 *****************************/
static void _cmd_task(void *pvParameters){
int cmdid = 0,len,ret;
char buffer[256],param[256]={0};




while(1){
	memset(buffer,0x00,sizeof(buffer));
	memset(param,0x00,sizeof(param));


/*****************************
 *D2M 指令处理流程函数
 *****************************/
static void d2m_cmd_processing_flow(){
	char cmdid[0];
	//wait cmd
	len = read_cmd_buffer_from_usart((unsigned char*)buffer,sizeof(buffer) - 1);  // 等待串口中断  最小长度  寻找帧头
	if(len > 0){
		//parse cmd
		cmdid = decode_cmd_req_from_buffer((unsigned char*)buffer,param,&len);	  //  从注册表中找到对应函数
		if(cmdid != DECODE_CMD_ERR){
			//execute cmd
			execute_cmd(cmdid,param,len);								  //  解析并返回
			//clear cmd
			cmdid = 0;
		}
		else
			ERROR_LOG_PRINT("[Error][%s]--[%s]",__FILE__,__LINE__);
	}

}	
	


