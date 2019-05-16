#ifndef __CMD_MANAGER_H__
#define __CMD_MANAGER_H__

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MAX_SUPPORT_ITEMS  64

// 数据交互方式
#define FRAME_HEADER							0XF5	// 帧头

#define FRAME_HEADER_OFF 						0x00	// 帧头相对于一帧数据偏移量
#define FRAME_CMD_OFF    						0x01	// 指令码相对于一帧数据偏移量
#define FRAME_LEN_OFF    						0x02	// 有效数据长度起始位置相对于一帧数据偏移量
#define FRAME_PARAM_OFF							0x04	// 参数起始位置相对于一帧数据偏移量
#define FRAME_HEADER_LEN 						0x04   	// head(1byte) + cmd(1byte) + len(2byte)
#define FRAME_HEADER_LEN_OFF					0x03   	// head(1byte) + cmd(1byte) + len(2byte)   偏移量
#define FRAME_PARAM_LEN_NO_DATA  				0x06	// head(1byte) + cmd(1byte) + len(2byte) + LRC(2byte)

// 用户身份分类
#define USER_ID_ADMIN							0x01	// 管理用户
#define USER_ID_GUEST							0x02	// 宾客用户
#define USER_ID_GENERAL							0x03	// 普通用户
#define USER_ID_DURESS							0x04	// 胁迫用户
#define USER_ID_BONNE							0x05	// 保姆用户
#define USER_ID_TEMPORARY						0x06	// 临时用户

// 钥匙分类
#define VEIN_IDENTIFY							0x01	// 静脉识别
#define CARD_IDENTIFY							0x02	// 卡识别
#define PASSWORD_IDENTIFY						0x03	// 密码识别
#define MECHANICAL_KEY_IDENTIFY					0x04	// 钥匙识别

// 指令码说明
// M2D : ZIGBEE MODULE TO DOOR     
#define M2D_ISSUED_USERDATA						0X10	//用户数据下发	数据流向：下发
#define M2D_REMOTE_PASSWORD_UNLOCK				0x13	//远程密码开锁	数据流向：下发
#define M2D_TEMPORARY_PASSWORD_UNKOCK			0x14	//临时密码开锁	数据流向：下发
#define M2D_GET_DEVICE_VERSION					0x15	//获取设备版本号	数据流向：下发
#define M2D_GET_DEVICE_POWER					0x16	//获取电池电量	数据流向：下发
#define M2D_SHOW_ACCESS_EXIT_NET_RESULT			0x25	//入网/退网状态显示	数据流向：下发  	？？？？
#define M2D_SHOW_NETWORK_STATUS					0x26	//网络状态提示	数据流向：下发
#define M2D_SET_DOORLOCK_OPEN					0x27	//门锁常开	数据流向：下发
#define M2D_TIME_INQUIRY						0x28	//时间查询	数据流向：下发
#define M2D_ACTIVE_DOORLOCK						0x29	//门锁激活	数据流向：下发
#define M2D_GET_UNLOCK_RECORD					0x2A	//历史开锁记录查询	数据流向：下发

#define M2D_ISSUED_USERDATA_RETURN				0X10	//用户数据下发	数据流向：下发
#define M2D_REMOTE_PASSWORD_UNLOCK_RETURN		0x13	//远程密码开锁	数据流向：下发
#define M2D_TEMPORARY_PASSWORD_UNKOCK_RETURN	0x14	//临时密码开锁	数据流向：下发
#define M2D_GET_DEVICE_VERSION_RETURN			0x15	//获取设备版本号	数据流向：下发
#define M2D_GET_DEVICE_POWER_RETURN				0x16	//获取电池电量	数据流向：下发
#define M2D_SHOW_ACCESS_EXIT_NET_RESULT_RETURN	0x25	//入网/退网状态显示	数据流向：下发  	？？？？
#define M2D_SHOW_NETWORK_STATUS_RETURN			0x26	//网络状态提示	数据流向：下发
#define M2D_SET_DOORLOCK_OPEN_RETURN			0x27	//门锁常开	数据流向：下发
#define M2D_TIME_INQUIRY_RETURN					0x28	//时间查询	数据流向：下发
#define M2D_ACTIVE_DOORLOCK_RETURN				0x29	//门锁激活	数据流向：下发
#define M2D_GET_UNLOCK_RECORD_RETURN			0x2A	//历史开锁记录查询	数据流向：下发

			
// D2N : DOOR TO ZIGBEE MODULE			
#define D2M_ACCESS_NETWORK						0x33	//无线加网	数据流向：上传
#define D2M_EXIT_NETWORK						0x34	//无线退网	数据流向：上传
#define D2M_UPLOAD_DOORLOCK_STATUS				0x24	//门锁状态上传	数据流向：上传
#define D2M_SYNC_SERVER_TIME					0x21	//与服务器同步时间	数据流向：上传
#define D2M_UPLOAD_USERDATA						0x22	//用户数据上传	数据流向：上传
#define D2M_UPLOAD_UNLOCK_RECORD				0x23	//开锁日志上传	数据流向：上传
#define D2M_WAKEUP_ZIGBEE_MODULE				0x35	//门锁唤醒模块（用于远程密码开锁）	数据流向：上传


typedef int (*FUNC)(char *param,int len);
typedef struct __Cmd_t{
	int cmdid;
	FUNC func;  // func(char *param,int len)
}item_t;
int register_cmd(int cmdid,FUNC func);
int execute_cmd(int cmdid,char *param,int len);

#endif /* __CMD_MANAGER_H__ */

