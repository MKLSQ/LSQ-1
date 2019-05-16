/*****************************************************************************
 * main.c
 *
 * Copyright (C) 2018 liusiqun siqun.liu@gotechcn.cn
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 ****************************************************************************/


/*****************************************************************************
 * Included Files
 ****************************************************************************/
#include <main.h>
#include <sys.h>
#include <Syscfg.h>
#include <delay.h>
#include <usart.h>
#include <string.h>
#include <tim.h>
#include <tools.h>
#include <SmartLockCmdQueue.h>
#include <gpio.h>
#include <exti.h>
#include <time.h>

#include "cmd_manager.h"
#ifdef USE_IWDG
#include <iwdg.h>
#include <rtc.h>
#endif
#ifdef USE_WWDG
#include <wwdg.h>
#endif

/*****************************************************************************
 * Configure Definitions
 ****************************************************************************/

/*****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/*****************************************************************************
 * Public Types
 ****************************************************************************/
/*事件类型*/
typedef enum{
	/*----------
	 *组网事件
	 ----------*/
	/*门锁上报*/
	NetWorkConReuqest = 0,
	NetworkDisconRequest,
	NetworkConStatusRecSuccess,
	/*zigbee上报*/
	NetworkConSuccess,
	NetWorkConFailed,
	NetWorkDisconSuccess,
	NetWorkDisconFailed,
	ZigbeeConnectCmdExecSuccess,
	ZigbeeDisConCmdExecSuccess,
	
	/*----------
	 *网络状态
	 ----------*/
	/*门锁上报*/
	NetworkStatusRecSuccess,
	/*网络状态，zigbee上报*/
	NetWorkOnline,
	NetWorkOffline,
	
	/*----------
	 *报警事件
	 ----------*/
	/*门锁上报*/	
	IllegalOprAlarm,
	DemolitionAlarm,
	DoorNotClosedAlarm,
	LowPowerAlarm,
	LockKeyboardAlarm,
	FingerPrintVerfAlarm,//指纹验证多次失败
	FingerVeinVerfAlarm, //指静脉验证多次失败
	PasswordVerfAlarm,   //密码验证多次失败
	CardVerfAlarm,		 //感应卡验证多次失败
	LockDoorAlarm,		 //门内反锁报警
	UnlockDoorAlarm,	 //门内解除反锁报警
	/*zigbee上报*/
	ZigbeeDemolitionAlarmExecSuccess,
	ZigbeeLowePowerAlarmExecSuccess,
	ZigbeeLockKeyboardAlarmExecSuccess,
	ZigbeeDoorNotCloseAlarmExecSuccess,
	ZigbeeVerfyFailedAlarmExecSuccess,
	ZigbeeLockDoorAlarmExecSuccess,
	ZigbeeUnlockDoorAlarmExecSuccess,
	ZigbeeSetDoorAlwaysOpenExecSuccess,
	ZigbeeCancelDoorAlwaysOpenExecSuccess,
	
	/*----------
	 *解除报警事件
	 ----------*/
	/*门锁上报*/
	IllegalOprAlarmRemove,
	DemolitionAlarmRemove,
	UnlockDoorAlarmReomve,
	DoorNotClosedAlarmRemove,
	LowPowerAlarmRemove,
	DuressRemove,
	/*zigbee上报*/

	/*----------
	 *门锁工作状态
	 ----------*/
	/*门锁上报*/
	SmartLockEnterSleep,
	SmartLockEnterWork,
	WkupNetWorkRequest,
	SmartLockPowerOn,
	/*zigbee上报*/
	WkupSmartLockRequest,

	/*----------
	 *本地开锁事件
	 ----------*/
	/*门锁上报*/
	OpenDoorSuccess,
	OpenDoorFailed,
	ForbidOepnDoor,
	/*zigbee上报*/
	ZigbeeOpenDoorInfoRecvSuccess,
	ZigbeeKeyOpenDoorRecvSuccess,

	/*----------
	 *时间同步
	 ----------*/
	/*门锁上报*/
	SmartLockTimeSynRequest,
	/*zigbee上报*/
	NetWorkTimeSynRequest,
	ZigbeeTimeSynCmdExecSuccess,
	
	/*----------
	 *门铃事件
	 ----------*/
	/*门锁上报*/
	DoorBell,
	/*zigbee上报*/
	ZigbeeDoorBellExecSuccess,

	/*----------
	 *密钥操作事件
	 ----------*/
	/*门锁上报，自定义*/
	DownloadDataSuccess,
	VerifyTooMuch,
	CanNotFindTheID,
	NoOldPasswordOrCard,
	NoPermisToModifyAdmin,
	StartTimeIsBiggerThanEndTime,
	UserTypeNotMatch,
	MethodTypeNotMatch,
	PasswordRepeat,
	AdminVerifySuccess,
	AdminPasswordWrong,
	NoAdminPassword,
	NoFingerVein,
	NoPassword,
	NoICCard,
	/*zigbee上报*/
	NetWorkKeyRequest,
	
	/*----------
	 *常开事件
	 ----------*/
	/*门锁上报*/
	SetDoorAlwaysOpen,
	CancelDoorAlwaysOpen,
	DoorAlwaysOpenCmdExecSuccess,
	SetDoorAlwaysOpenSuccess,
	SetDoorAlwaysOpenFailed,
	SetDoorAlwaysOpenAdminVerifySuccess,
	SetDoorAlwaysOpenAdminPasswordWrong,
	SetDoorAlwaysOpenNoAdminPassword,
	/*zigbee上报*/
	DoorAlwaysOpenRequest,
	
	/*----------
	 *门锁本地常用模式设置事件
	 ----------*/
	/*门锁上报*/
	OpenDoorWithNormalMode,
	OpenDoorWithSafeMode,
	SlientMode,
	EnglishMode,
	InfraredOpen,
	InfraredClose,
	BlueToothOpen,
	BlueToothClose,
	GoHomeMode,
	GoOutMode,
	/*zigbee上报*/
	SmartLockModeChangedExecSuccess,
	
	/*----------
	 *用户数据上报处理
	 ----------*/
	/*门锁上报*/
	SmartLockUserDataUpload,
	/*zigbee上报*/
	ZigbeeUserDataUploadExecSuccess,
	ZigbeeUploadPasswordSuccess,
	ZigbeeUploadCardSuccess,
	ZigbeeUploadFingerVeinSuccess,
	ZigbeeUploadTimeStampSuccess,
	
	/*----------
	 *远程密码开锁事件
	 ----------*/
	/*门锁上报*/
	WkupZigbeeFor1Min,
	RemotePasswordOpenDoorResult,
	/*zigbee上报*/
	ZigbeeWkupFor1MinRecvSuccess,
	PasswordFromServer,
	ZigbeeRemoteOpenDoorResultExecSuccess,
	
	/*----------
	 *时间查询事件
	 ----------*/
	/*门锁上报*/
	GetSmartLockTIme,
	/*zigbee上报*/
	GetTimeRequest,
	
	/*未确认事件*/
	UnknowEvent,

}E_EventType;


/******************************
*	BCD时间
******************************/
typedef struct BCDTime{
	u8 seconds; // 0-59
	u8 minutes; // 0-59 
	u8 hour;	// 0-23 
	u8 day; 	// 1-31
	u8 month;	// 1-12
	u8 year;	// 0-255(2000开始)
}T_BCDTime, *PT_BCDTime;


/******************************
*	门锁向zigbee模块发数据
******************************/

 /* 错误码--zigbee模块回复 */
 // 无线加网 退网 用户数据上传   是否改为define？
typedef enum {
	E_Success = 0x00,
	E_Fail    = 0x01,
	
}E_ModuleReply;



/******************************
*  门锁状态上传
******************************/
/*状态类型*/
typedef enum {
	Flood_Control_Alarm 			= 0x00, 	// 防撬报警
	Door_Power_Low_Alarm			= 0x02,		// 低压报警（门锁电量过低时）	 	
	Keyboard_Lock_Alarm				= 0x03,		// 锁键盘报警（触发键盘锁定时）
	Doorbell_Alarm					= 0x04,		// 门铃（“#”键触发）
	Keyboard_Wakeup_Doorlock		= 0x05,		// 唤醒（键盘唤醒门锁时）
	Release_Open_state				= 0x06,		// 解除常开状态
	Set_Open_State					= 0x07,		// 设置常开状态
	Doorlock_Unclosed_Alarm			= 0x08,		// 未关门报警
	Vein_Exceed_Max_Auth_Times  	= 0x09,		// 指静脉验证多次失败
	Password_Exceed_Max_Auth_Times	= 0x0A,		// 密码验证多次失败
	Card_Exceed_Max_Auth_Times		= 0x0B,		// 感应卡验证多次失败
	Unlock_Auth_Mode_Change			= 0x0C,		// 开锁认证模式更改	
	Volume_Adjustment				= 0x0D,		// 音量调节
	Infrared_Mode_Change			= 0x0E,		// 红外模式更改
	Doorrlock_Profile_Change		= 0x0F,		// 门锁情景模式更改
	Indoor_Antilock_Alarm			= 0x10,		// 门内（内保险打开）反锁报警
	Indoor_Antilock_Alarm_Release	= 0x11,		// 门内（内保险关闭）解除反锁报警
	Mechanical_Key_Unlock			= 0x12,		// 机械钥匙开锁
	Doorlock_Sleep					= 0x13		// 门锁休眠
	
}E_DoorStateType;  

// 门锁状态上传
typedef struct D2M_DoorstateUploadInfo{
	E_DoorStateType 	StateType;
	u16 				UserId;
	
}T_D2M_DoorstateUploadInfo, *PT_D2M_DoorstateUploadInfo;

/******************************
* 与服务器同步时间 0xEC
******************************/
	typedef struct D2M_SyncServerTimeInfo{   
		
		u8 				eDataAndTime[6];
		
	}T_D2M_SyncServerTimeInfo, *PT_D2M_SyncServerTimeInfo;


/******************************
* 用户数据上传 0x22
******************************/
/*操作类型*/
typedef enum {
	Add_User	= 0x01, 		//添加用户
	Del_User	= 0x02, 		//删除用户
	Empty_User	= 0x03, 		//清空用户（可分别清空静脉、卡、密码等）
	Init_User	= 0x04, 		//初始化用户（恢复出厂设置）
	Modify_User = 0x05, 		//修改用户
	Freeze_User = 0x06, 		//冻结用户
	Thaw_User	= 0x07, 		//解冻用户

	Action_Type_Max
	
}E_ActionType; 
	
/*用户类型*/
typedef enum {
	User_ID_Admin		= 0x01, 		// 管理用户
	User_ID_Guest		= 0x02, 		// 宾客用户
	User_ID_General		= 0x03, 		// 普通用户
	User_ID_Duress		= 0x04, 		// 胁迫用户
	User_ID_Bonne 		= 0x05, 		// 保姆用户
	User_ID_Temporary 	= 0x06, 		// 临时用户

	User_Type_Max
  	
}E_UserType;  

/*用户数据上传*/	
typedef struct D2M_UserDataUploadInfo{
	u16 				UserId;
	E_ActionType 		eActionType;	// 操作类型
	E_UserType 			eUserType;		// 用户类型
	E_OpenDoorMethod 	eRegisterType;	// 登记类型
	T_BCDTime			tBCDStartTime;	// 有效起始时间
	T_BCDTime			tBCDEndTime;	// 有效结束时间
	
	/*
	后续是否要用到
	u8					NumOfTime;		// 使用次数
	u8					PasswordLen;
	u8 					Password[10];
	u8					CardLen;
	u8					Card[15];
	*/
}T_D2M_UserDataUploadInfo, *PT_D2M_UserDataUploadInfo;

/******************************
*开锁日志上传
******************************/
/*开锁验证方式/密钥类型*/
typedef enum {
  Vein = 0x01,
  Card = 0x02,
  Password = 0x03,
  KeyOpenDoor = 0x04,
  TemporaryPassword = 0x09,
 // CardAndFingerVein = 0x0A,
 // FingerVeinAndPassword = 0x0B,
  Face = 0x0A,
  Finger=0x0B,
  FingerVeinAndPasswordAndCard = 0x0C,
  CardAndPassword = 0x0D,
  Duress = 0x0E,
  RemotePassword = 0x0F,
  FingerVeinAndFingerVein = 0x10,
  UnknownMethod = 0xFF,
	
}E_OpenDoorMethod;

/*电量*/
typedef enum {
  Percent0To25   = 0x00,
  Percent25To50  = 0x01,
  Percent50To75  = 0x02,
  Percent75To100 = 0x03,
	
}E_PowerPercent;  

/* 开锁日志上传 */
typedef struct D2M_OpenDoorInfo{
	//char	No1UserName[15];//用户1名称，字符串,限制15字节
	u16 				UserID;				// 用户ID
	E_OpenDoorMethod	eOpenDoorMethod;	// 开门验证方式
	E_PowerPercent		ePowerPercent;		// 电量百分比
	char				Data_and_Time[6];	// 日期和时间
	u8					LogType;	
}T_D2M_OpenDoorInfo, *PT_D2M_OpenDoorInfo;



/******************************
*	zigbee模块向门锁发数据 M2D
******************************/

/*用户数据下发*/	
typedef struct M2D_UserDataDownloadInfo{
	u16 				UserId;
	E_ActionType 		eActionType;	// 操作类型
	E_UserType 			eUserType;		// 用户类型
	E_OpenDoorMethod 	eRegisterType;	// 登记类型
	T_BCDTime			tBCDStartTime;	// 有效起始时间
	T_BCDTime			tBCDEndTime;	// 有效结束时间
	
	/*
	后续是否要用到
	u8					NumOfTime;		// 使用次数
	u8					PasswordLen;
	u8 					Password[10];
	u8					CardLen;
	u8					Card[15];
	*/
}T_M2D_UserDataDownloadInfo, *PT_M2D_UserDataDownloadInfo;




/******************************
 *远程密码开锁
 ******************************/
/* 错误码--远程密码开锁 */
typedef enum {
	PasswordRight = 0x00,
	PasswordError = 0x02,
	//NoPermissToOpenDoor = 0x02,
	IsFreeze = 0x04,
	InvalidTime = 0x05,
	ErrorCode = 0xFF,
	
}E_RemoteOpenErrorCode;  
	

/*
解密密钥
解密方法：有效数据与解密密钥异或
注意：有效密码才会加密，无效数据为0x00
*/
const u8 DecryptionKey[]={0x49,0x46,0x45,0x49,0x42,0x49,0x47,0x42,0x46,0x45};


/*远程密码开锁结果返回信息*/	
typedef struct M2D_RemoteOpenDoorResultInfo{
	E_RemoteOpenErrorCode		eRemoteOpenErrorCode;
	u16 						UserId;
	
}T_M2D_RemoteOpenDoorResultInfo, *PT_M2D_RemoteOpenDoorResultInfo;


/******************************
 *获取设备版本号
 ******************************/
 /* 固定字节 */
const u8  M2DFixedByte = 0xFF;  


/******************************
 *入网/退网状态提示
 ******************************/
 /*操作状态*/
typedef enum {
	E_AccessNetwork = 0x00,
	E_ExitNetwork   = 0x01,
	
}E_OperateNetState; 

 /*反馈状态*/
typedef enum {
	E_Success = 0x00,
	E_Fail    = 0x01,
	
}E_OperateNetStateResult; 

/*入网/退网状态提示      		D2M*/	
typedef struct M2D_OperateNetStateInfo{
	E_OperateNetState			eOperateNetState;			/*操作状态*/
	E_OperateNetStateResult 	eOperateNetStateResult;		/*反馈状态*/					
	
}T_M2D_OperateNetStateInfo, *PT_M2D_OperateNetStateInfo;


/******************************
 * 网络状态提示
 ******************************/
/* 操作状态沿用入网/退网状态提示 */


/******************************
 * 门锁常开
 ******************************/
 /*常开状态*/
typedef enum {
	E_OpenDoor_Always_Enable  = 0xFF,
	E_OpenDoor_Always_Disable = 0x00,
	
}E_OpenDoorAlwaysState; 


/******************************
 * 时间查询
 ******************************/
/* 固定字节沿用获取设备版本号 */


/* 锁端返回 */
// 待确认补充！


/******************************
 * 历史开锁记录查询
 ******************************/
 /* 固定字节沿用获取设备版本号 */


 /* 锁端返回 */
 // 待确认补充！


/******************************
 * 请求用户信息
 ******************************/
 /* 固定字节沿用获取设备版本号 */


 /* 锁端返回 */
 // 待确认补充！


/******************************
 * 用户数据下发管理员验证
 ******************************/
/* 解密密钥沿用远程密码开锁 */


/* 错误码--用户数据下发管理员验证 */
typedef enum {
	E_Issued_Success 		= 0x00,
	E_Issued_Fail    		= 0x01,
	E_Admin_Password_Fail   = 0x02,
}E_IssueReply;


/******************************
 * 音量设置
 ******************************/
/* 音量大小设置 */
typedef enum {
	E_Set_Sound_Level_0 	= 0x00,
	E_Set_Sound_Level_1 	= 0x01,
	E_Set_Sound_Level_2 	= 0x02,
	E_Set_Sound_Level_3 	= 0x03,
	E_Set_Sound_Level_4 	= 0x04,
	E_Set_Sound_Level_5 	= 0x05,
	E_Set_Sound_Level_6 	= 0x06,
	E_Set_Sound_Level_7 	= 0x07,
	E_Set_Sound_Level_8 	= 0x08,
}E_SetSoundLevel;


/******************************
 * 门锁常开设置管理员验证
 ******************************/
/* 解密密钥沿用远程密码开锁 */

/* 错误码沿用用户数据下发管理员验证 */





/******************************
*通用事件数据
******************************/
/*
typedef union EventData{
	u8 							RemotePasswordBuf[10];
	u32							DoorAlwaysOpenTime;
	u8							DoorBellTime;			//门铃响铃时间
	T_OpenDoorInfo 				tOpenDoorInfo;
	T_NetWorkKeyRequestInfo		tNetWorkKeyRequestInfo;
	T_AddPasswordRespondInfo	tAddPasswordRespondInfo;
	T_UTCTime					tUTCTimeFromNetWork;
	T_BCDTime					tBCDTimeFromSmartLock;
	T_UserDataUploadInfo		tUserDataUploadInfo;	//用户数据上传
	T_OpenDoorRecordRequest		tOpenDoorRecordRequest;	//历史开锁记录数据
	T_RemoteOpenDoorResultInfo	tRemoteOpenDoorResultInfo;
}U_EventData, *PU_EventData;
*/

typedef union EventData{
	u8 								RemotePasswordBuf[10];
	u32								DoorAlwaysOpenTime;
	u8								DoorBellTime;			//门铃响铃时间

	
	T_D2M_DoorstateUploadInfo 		t_d2m_DoorstateUploadInfo;
	T_D2M_SyncServerTimeInfo		t_d2m_SyncServerTimeInfo;
	T_D2M_UserDataUploadInfo 		t_d2m_UserDataUploadInfo;
	T_D2M_OpenDoorInfo 				t_d2m_OpenDoorInfo;
	
	T_M2D_UserDataDownloadInfo 		t_m2d_UserDataDownloadInfo;
	T_M2D_RemoteOpenDoorResultInfo 	t_m2d_RemoteOpenDoorResultInfo;
	T_M2D_OperateNetStateInfo 		t_m2d_OperateNetStateInfo;
}U_EventData, *PU_EventData;

U_EventData u_EventData;


/******************************
*	D2M发送数据均保存在此
******************************/
typedef struct D2M_Frame_Data{
   u8								t_FrameHead;
   u8								t_FrameCmd;
   u16								t_FrameDateLen;
   U_EventData						u_EventData;
   u16								t_LrcCheckSum;
}U_D2M_Frame_Data, *PU_D2M_Frame_Data;

static U_D2M_Frame_Data u_D2M_Frame_Data;

/******************************
*	M2D发送数据均保存在此
******************************/
typedef struct M2D_Frame_Data{
   u8								t_FrameHead;
   u8								t_FrameCmd;
   u16								t_FrameDateLen;
   U_EventData						u_EventData;
   u16								t_LrcCheckSum;
}U_M2D_Frame_Data, *PU_M2D_Frame_Data;

/******************************
*通用事件信息
******************************/
typedef struct EventInfo{
	char *name;
	volatile u8 cHasEventData;
	E_EventType eEventType;
	U_EventData uEventData;
	struct EventInfo *next;
}T_EventInfo, *PT_EventInfo;  

/*****************************************************************************
 * Public Data
 ****************************************************************************/
static char LargeDataTmpFromSmartLock[SMART_LOCK_LARGE_BUF_LEN];	//用于存储门锁上报的大量数据
static char DataTmpFromSmartLock[BUFFER_LEN];						//用于存储门锁上报的少量数据
static char DataTmpFromZigbee[BUFFER_LEN];							//用于存储zigbee上报的少量数据
//static char DataTmpFromDebugPort[BUFFER_LEN];						//用于存储debug口上报的少量数据
static u32	SysWkupCnt = 0;											//检测系统唤醒次数


static void cmd_fun_init(){
		//register d2m_cmd_task
	register_cmd(D2M_ACCESS_NETWORK,d2m_access_network_fun);
	register_cmd(D2M_EXIT_NETWORK,d2m_exit_network_fun);
	register_cmd(D2M_UPLOAD_DOORLOCK_STATUS,d2m_upload_doorlock_status_fun);
	register_cmd(D2M_SYNC_SERVER_TIME,d2m_sync_server_time_fun);
	register_cmd(D2M_UPLOAD_USERDATA,d2m_upload_userdata_fun);
	register_cmd(D2M_UPLOAD_UNLOCK_RECORD,d2m_upload_unlock_record_fun);
	register_cmd(D2M_WAKEUP_ZIGBEE_MODULE,d2m_wakeup_zigbee_module_fun);
	
	/*
	//register m2d_cmd_task
	register_cmd(M2D_ISSUED_USERDATA,m2d_issued_userdata_fun);
	register_cmd(M2D_REMOTE_PASSWORD_UNLOCK,m2d_remote_password_unlock_fun);
	register_cmd(M2D_TEMPORARY_PASSWORD_UNKOCK,m2d_temporary_password_unkock_fun);
	register_cmd(M2D_GET_DEVICE_VERSION,m2d_get_device_version_fun);
	register_cmd(M2D_GET_DEVICE_POWER,m2d_get_device_power_fun);
	register_cmd(M2D_SHOW_ACCESS_EXIT_NET_RESULT,m2d_show_access_exit_net_result_fun);
	register_cmd(M2D_SHOW_NETWORK_STATUS,m2d_show_network_status_fun);
	register_cmd(M2D_SET_DOORLOCK_OPEN,m2d_set_doorlock_open_fun);
	register_cmd(M2D_TIME_INQUIRY,m2d_time_inquiry_fun);
	register_cmd(M2D_ACTIVE_DOORLOCK,m2d_active_doorlock_fun);
	register_cmd(M2D_GET_UNLOCK_RECORD,m2d_get_unlock_record_fun);
	*/
	
	//register m2d_cmd_return_task
	register_cmd(M2D_ISSUED_USERDATA_RETURN,m2d_issued_userdata_return_fun);
	register_cmd(M2D_REMOTE_PASSWORD_UNLOCK_RETURN,m2d_remote_password_unlock_return_fun);
	register_cmd(M2D_TEMPORARY_PASSWORD_UNKOCK_RETURN,m2d_temporary_password_unkock_return_fun);
	register_cmd(M2D_GET_DEVICE_VERSION_RETURN,m2d_get_device_version_return_fun);
	register_cmd(M2D_GET_DEVICE_POWER_RETURN,m2d_get_device_power_return_fun);
	register_cmd(M2D_SHOW_ACCESS_EXIT_NET_RESULT_RETURN,m2d_show_access_exit_net_result_return_fun);
	register_cmd(M2D_SHOW_NETWORK_STATUS_RETURN,m2d_show_network_status_return_fun);
	register_cmd(M2D_SET_DOORLOCK_OPEN_RETURN,m2d_set_doorlock_open_return_fun);
	register_cmd(M2D_TIME_INQUIRY_RETURN,m2d_time_inquiry_return_fun);
	register_cmd(M2D_ACTIVE_DOORLOCK_RETURN,m2d_active_doorlock_return_fun);
	register_cmd(M2D_GET_UNLOCK_RECORD_RETURN,m2d_get_unlock_record_return_fun);
}


void main(){
// 初始化模块 串口 时钟

// 注册任务处理函数
cmd_fun_init();

}




/*****************************
*	
*	D2M指令处理函数
*
*****************************/

/*****************************
*	d2m_access_network_fun
*****************************/

static int d2m_access_network_fun(char *param,int len){
	char response[256];   // 应答数组
	
	// 进入这里就是接收成功的
	encode_cmd_rsp_to_buffer((unsigned char *) response, D2M_ACCESS_NETWORK, ERROR_CODE_SUCCESS,
					ERROR_CODE_LEN);
	write_rsp_buffer_to_usart(response, RESULT_NO_PARAM_LEN);   // 调用串口发送函数
	} 
	
	// 往中控发数据zcl
	调用
	// 接收到中控发来的数据

	// 入网/退网状态提示
		
}


/*****************************
*	d2m_exit_network_fun
*****************************/

static int d2m_exit_network_fun(char *param,int len){
	char response[256];   // 应答数组
		
		// check param
		
		
		// 返回的错误码仅表示模块是否收到该指令还是说收到并完成对指令的处理？
	
		// 处理流程
	
		
		// 成功
		if(){	
		encode_cmd_rsp_to_buffer((unsigned char *) response, D2M_EXIT_NETWORK, ERROR_CODE_SUCCESS,
						ERROR_CODE_LEN);
		write_rsp_buffer_to_usart(response, RESULT_NO_PARAM_LEN);	// 调用串口发送函数
		} 
		// 失败
		else{
		encode_cmd_rsp_to_buffer((unsigned char *) response, D2M_EXIT_NETWORK, ERROR_CODE_FAIL,
						ERROR_CODE_LEN);
		write_rsp_buffer_to_usart(response, RESULT_NO_PARAM_LEN);	// 调用串口发送函数
		}

}

/*****************************
*	d2m_upload_doorlock_status_fun
*****************************/
static int doorlock_status_parse_param(unsigned char *params,int total,char *status_type,char *user_id){
	int offset = 0,len = 0;
	//return status_type
	len = 1;
	memcpy(status_type,params+offset,len); // 0  1
	//return user_id
	offset += len;
	len = 2;
	memcpy(user_id,params+offset,len);	   // 1  2
	offset += len;					
	
	if(total < offset)
		return -1;
	u_eventdata->t_d2m_DoorstateUploadInfo->StateType = status_type;
	u_eventdata->t_d2m_DoorstateUploadInfo->UserId   = user_id;	
	return 0;
}

static int d2m_upload_doorlock_status_fun(char *param,int len){
	char response[256];   // 应答数组
	char status_type[1];	
	char user_id[2];
	int ret = 0;
			// check param
			ret = doorlock_status_parse_param((unsigned char *)param,len,(char *)status_type,(char *)user_id);
			if(ret < 0){
				ERROR_LOG_PRINT("[Error][%s]--[%s]",__FILE__,__LINE__);
				encode_cmd_rsp_to_buffer((unsigned char *)response,D2M_UPLOAD_DOORLOCK_STATUS,ERROR_CODE_FAIL,ERROR_CODE_LEN);
				write_rsp_buffer_to_usart(response,10); // 调用串口发送函数
				return 1;
			}
			
			
			// 返回的错误码仅表示模块是否收到该指令还是说收到并完成对指令的处理？
		
			// 处理流程
		
			
			// 成功
			if(){	
			encode_cmd_rsp_to_buffer((unsigned char *) response, D2M_EXIT_NETWORK, ERROR_CODE_SUCCESS,
							ERROR_CODE_LEN);
			write_rsp_buffer_to_usart(response, RESULT_NO_PARAM_LEN);	// 调用串口发送函数
			} 
			// 失败
			else{
			encode_cmd_rsp_to_buffer((unsigned char *) response, D2M_EXIT_NETWORK, ERROR_CODE_FAIL,
							ERROR_CODE_LEN);
			write_rsp_buffer_to_usart(response, RESULT_NO_PARAM_LEN);	// 调用串口发送函数
			}

}	

/*****************************
*	d2m_sync_server_time_fun
*****************************/
static int d2m_sync_server_time_fun(char *param,int len){

}	

/*****************************
*	d2m_upload_userdata_fun
*****************************/
static int d2m_upload_userdata_fun(char *param,int len){

}	

/*****************************
*	d2m_upload_unlock_record_fun
*****************************/
static int d2m_upload_unlock_record_fun(char *param,int len){

}	

/*****************************
*	d2m_upload_doorlock_status_fun
*****************************/
static int d2m_wakeup_zigbee_module_fun(char *param,int len){

}



/*****************************
*	
*	M2D下发指令处理函数
*
*****************************/
static int m2d_cmd_processing_function(char cmdid,char *param,int len){
	char response[256];   // 应答数组
		
	encode_cmd_rsp_to_buffer((unsigned char *) response, cmdid, param,
					len);
	write_rsp_buffer_to_usart(response, RESULT_NO_PARAM_LEN);	// 调用串口发送函数

}



/*****************************
*	
*	M2D门锁返回处理函数
*
*****************************/

/*****************************
*	m2d_issued_userdata_return_fun
*****************************/
static int m2d_issued_userdata_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_remote_password_unlock_return_fun
*****************************/
static int m2d_remote_password_unlock_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_temporary_password_unkock_return_fun
*****************************/
static int m2d_temporary_password_unkock_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_get_device_version_return_fun
*****************************/
static int m2d_get_device_version_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_get_device_power_return_fun
*****************************/
static int m2d_get_device_power_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_show_access_exit_net_result_return_fun
*****************************/
static int m2d_show_access_exit_net_result_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_show_network_status_return_fun
*****************************/
static int m2d_show_network_status_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_set_doorlock_open_return_fun
*****************************/
static int m2d_set_doorlock_open_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_time_inquiry_return_fun
*****************************/
static int m2d_time_inquiry_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_active_doorlock_return_fun
*****************************/
static int m2d_active_doorlock_return_fun(char *param,int len){
		
}

/*****************************
*	m2d_get_unlock_record_return_fun
*****************************/
static int m2d_get_unlock_record_return_fun(char *param,int len){
		
}



