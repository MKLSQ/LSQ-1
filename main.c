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
#include <stm32l0xx_hal.h>
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
*用户数据上传
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
	u8					Data_and_Time[6];	// 日期和时间
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
	T_D2M_UserDataUploadInfo 		t_d2m_UserDataUploadInfo;
	T_D2M_OpenDoorInfo 				t_d2m_OpenDoorInfo;
	
	T_M2D_UserDataDownloadInfo 		t_m2d_UserDataDownloadInfo;
	T_M2D_RemoteOpenDoorResultInfo 	t_m2d_RemoteOpenDoorResultInfo;
	T_M2D_OperateNetStateInfo 		t_m2d_OperateNetStateInfo;
}U_EventData, *PU_EventData;



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
	
/*****************************
 *通讯模块定义
 *****************************/
static T_CommunicationModule SmartLockCommModule = {
	.name = "SmartLock",
	.FnInit = SmartLockCommModuleInit,
	.FnOpen = SmartLockCommModuleOpen,
	.FnClose = SmartLockCommModuleClose,
	.FnSendData = SendDataToSmartLock,
	.FnGetData = GetDataFromSmartLock,
	.FnGetLargeData = GetLageDataFromSmartLock,
	.FnWkupDevice = WkupSmartLock,
	.FnStatusDetect = SmartLockStatusDetect,
};

static T_CommunicationModule ZigbeeCommModule = {
	.name = "Zigbee",
	.FnInit = ZigbeeCommModuleInit,
	.FnOpen = ZigbeeCommModuleOpen,
	.FnClose = ZigbeeCommModuleClose,
	.FnSendData = SendDataToZigbee,
	.FnGetData = GetDataFromZigbee,
	.FnWkupDevice = WkupZigbee,
	.FnStatusDetect = ZigbeeStatusDetect,
};

static T_CommunicationModule DebugPortCommModule = {
	.name = "Debug",
	.FnInit = DebugCommModuleInit,
	.FnOpen = DebugCommModuleOpen,
	.FnClose = DebugCommModuleClose,
	.FnSendData = SendDataToDebugPort,
	.FnGetData = GetDataFromDebugPort,
	.FnWkupDevice = WkupDebug,
	.FnStatusDetect = DebugPortStatusDetect,
};

/*****************************
 *zigbee数据流定义
 *****************************/
/*非协议数据应答*/
u8 ZigbeeBadDataRespndStream[]={
0xAA, 0x01, 0xE1, 0x00, 0x00, 0x00, 0x01, 0x01, 0xFF, 0x28,
0x55
};

/*zigbee透传数据*/
u8 ZigbeePassThroughStream[64]={ 0x00 };

/*zigbee透传数据，用于门锁常开设置*/
u8 ZigbeeSetDoorOpenPassThroughStream[16]={ 0x00 };

/*上传开门记录结束*/
const u8 ZigbeeUploadRecordFinishStream[]={
0xAA, 0x02, 0xE1, 0x00, 0x00, 0x00, 0x01, 0x01, 0x4F, 0x4B,
0x4D, 0x55
};

/*上传用户信息结束*/
const u8 ZigbeeUploadUserInfoFinishStream[]={
0xAA, 0x02, 0xE2, 0x00, 0x00, 0x00, 0x01, 0x01, 0x4F, 0x4B,
0x4E, 0x55
};

/*获取服务器时间*/
const u8 ZigbeeGetServerTime[]={
0xAA, 0x01, 0xEC, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x46, 
0x55
};

/*入网退网提示应答*/
u8 ZigbeeConnectResultRespondStream[]={
0xAA, 0x01, 0x83, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x28,
0x55
};

/*网络状态显示应答*/
u8 ZigbeeNetworkStatusRespondStream[]={
0xAA, 0x01, 0x84, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x2F,
0x55
}; 

/*门锁常开应答*/
u8 ZigbeeDoorAlwaysOpenReauestStream[]={
0xAA, 0x01, 0x86, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x2D,
0x55
};

/*时间查询结果返回*/
u8 ZigbeeGetTimeResultStream[]={
0xAA, 0x0A, 0xC7, 0x00, 0x00, 0x00, 0x01, 0x01, 
0x19, 0x04, 0x29, 0x14, 0x09, 0x07,
0x2D, 0x55
};

/*时间同步应答*/
u8 ZigbeeTimeSynRespondStream[]={
0xAA, 0x01, 0x62, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0xC9, 
0x55
};

/*密钥操作应答*/
u8 ZigbeeKeyRequestRespondStream[]={
0xAA, 0x0C, 0x73, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
0xD4, 0x55
};

/*远程密码开锁应答*/
u8 ZigbeeRemotePasswordRespondStream[]={
0xAA, 0x01, 0x60, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0xCB, 
0x55
};

/*远程密码开锁结果发送*/
u8 ZigbeeRemoteOpenDoorResultStream[]={
0xAA, 0x0A, 0x54, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4, 0x55
};

/*入网指令*/
const u8 ZigbeeConnectStream[]={
0xAA, 0x0A, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE1, 0x55
};

/*退网指令*/
const u8 ZigbeeDisconStream[]={
0xAA, 0x0A, 0x41, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x55
};

/*本地开锁上报*/
u8 ZigbeeOpenDoorInfoStream[]={
0xAA, 0x0A, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
0x03, 0x01, 0x00, 0x09, 0xC0, 0x1B, 0x7D, 0x1F, 0x92, 0x55
};

/*钥匙开锁上报*/
const u8 ZigbeeKeyOpenDoorStream[]={
0xAA, 0x0A, 0xCC, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6D, 0x55
};

/*强拆报警*/
const u8 ZigbeeDemolitionAlarmStream[]={
0xAA, 0x0A, 0x22, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x55
};

/*低压报警*/
const u8 ZigbeeLowePowerAlarmStream[]={
0xAA, 0x0A, 0x30, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x55
};

/*锁键盘报警*/
const u8 ZigbeeLockKeyboardStream[]={
0xAA, 0x0A, 0xC3, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x55
};

/*门内反锁报警*/
const u8 ZigbeeLockDoorAlarmStream[]={
0xAA, 0x0A, 0xC9, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x55
};

/*门内反锁解除报警*/
const u8 ZigbeeUnlockDoorAlarmStream[]={
0xAA, 0x0A, 0xCA, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6B, 0x55
};

/*未关门报警*/
const u8 ZigbeeDoorNotCloseAlarmStream[]={
0xAA, 0x0A, 0x24, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0x55
};

/*门锁常开状态上报*/
const u8 ZigbeeSetDoorAlwaysOpenStream[]={
0xAA, 0x01, 0xE3, 0x00, 0x00, 0x00, 0x01, 0x01, 0xAA, 0xE2, 
0x55
};

/*门锁解除常开状态上报*/
const u8 ZigbeeCancelDoorAlwaysOpenStream[]={
0xAA, 0x01, 0xE4, 0x00, 0x00, 0x00, 0x01, 0x01, 0xAA, 0xE5, 
0x55
};

/*获取服务器时间*/
const u8 ZigbeeGetTimeFromServerStream[]={
0xAA, 0x0A, 0x85, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x55
};

const u8 ZigbeeWkupFor1MinStream[]={
0xAA, 0x0A, 0x77, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD6, 0x55
};

/*门铃*/
u8 ZigbeeDoorBellStream[]={
0xAA, 0x0A, 0x2A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x3C,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x55
};

/*指纹、密码、卡验证多次失败*/
u8 ZigbeeVerfFailedStream[]={
0xAA, 0x0A, 0xC4, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x55
};

/*门锁其他状态上报*/
u8 ZigbeeSmartLockModeChangedStream[]={
0xAA, 0x03, 0xC6, 0x00, 0x00, 0x00, 0x01, 0x01, 0xFF, 0xFF,
0xFF, 0x62, 0x55
};

/*用户信息变动上报*/
u8 ZigbeeUserDataChangedStream[]={
0xAA, 0x10, 0xC5, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x81, 0x55
};

/*****************************
 *门锁数据流定义
 *****************************/
/*门锁透传数据*/
u8 SmartLockPassThroughStream[64]={ 0x00 };

/*上报开锁记录完成通知*/
const u8 SmartLockUploadRecordSuccessStream[]={
0xF5, 0xE1, 0x00, 0x01, 0x00, 0x00, 0x01
};

/*上报开锁记录进行中通知*/
u8 SmartLockUploadRecordSendingStream[]={
0xF5, 0xE1, 0x00, 0x01, 0x01, 0x00, 0x02
};

/*上报用户信息完成通知*/
const u8 SmartLockUploadUserInfoSuccessStream[]={
0xF5, 0xE2, 0x00, 0x01, 0x00, 0x00, 0x01
};

/*上报用户信息进行中通知*/
u8 SmartLockUploadUserInfoSendingStream[]={
0xF5, 0xE2, 0x00, 0x01, 0x01, 0x00, 0x02
};

/*门锁入网请求应答*/
const u8 SmartLockNetworkConRespondStream[]={
0xF5, 0x33, 0x00, 0x01, 0x00, 0x00, 0x01
};

/*门锁退网请求应答*/
const u8 SmartLockNetworkdisconRespondStream[]={
0xF5, 0x34, 0x00, 0x01, 0x00, 0x00, 0x01
};

/*门锁状态上传应答*/
const u8 SmartLockStatusRespondStream[]={
0xF5, 0x24, 0x00, 0x01, 0x00, 0x00, 0x01
};

/*锁主动上传当前开门记录应答*/
const u8 SmartLockOpenDoorRespondStream[]={
0xF5, 0x23, 0x00, 0x01, 0x00, 0x00, 0x01
};

/*锁上传用户数据应答*/
const u8 SmartLockDataUploadRespondStream[]={
0xF5, 0x22, 0x00, 0x01, 0x00, 0x00, 0x01
};

/*门锁唤醒模块应答*/
const u8 SmartLockWkupZigbeeRespondStream[]={
0xF5, 0x35, 0x00, 0x01, 0x00, 0x00, 0x01
};

/*时间校准*/
u8 SmartLockTimeSynStream[]={
0xF5, 0x21, 0x00, 0x06, 0x19, 0x02, 0x22, 0x17, 0x42, 0x55,
0x00, 0xF1
};

/*门锁远程密码下发*/
u8 SmartLockRemotePasswordStream[]={
0xF5, 0x13, 0x00, 0x0A, 
0x47, 0x47, 0x4A, 0x46, 0x4C, 0x41, 0x00, 0x00, 0x00, 0x00,
0x00, 0xF1
};

/*入网退网状态提示*/
u8 SmartLockNetworkConStatusStream[]={
0xF5, 0x25, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02
};

/*网络状态提示*/
u8 SmartLockNetworkStatusStream[]={
0xF5, 0x26, 0x00, 0x01, 0x00, 0x00, 0x01
};

/*时间获取*/
const u8 SmartLockGetTimeStream[]={
0xF5, 0x28, 0x00, 0x00, 0x00, 0x00
}; 

/*门锁常开设置*/
u8 SmartLockAlwaysOpenStream[]={
0xF5, 0x27, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x0E
};

/*门锁数据下发管理员验证*/
u8 SmartLockAdminVerifyForDownloadDataStream[]={
0xF5, 0x29, 0x00, 0x0A, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
0x06, 0x07, 0x08, 0x09, 0x00, 0x23
};

/*门锁常开设置管理员验证*/
u8 SmartLockAdminVerifyForSetDoorOpenStream[]={
0xF5, 0x2A, 0x00, 0x0A, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
0x06, 0x07, 0x08, 0x09, 0x00, 0x23
};

/*用于除了指静脉模板外的所有操作数据下发*/
u8 SmartLockDownLoadDataStream[]={
/*0--头*/					0xF5, 
/*1--指令码*/					0x10, 
/*2--长度*/					0x00, 0x28, 
/*4--用户id*/					0x00, 0x01, 
/*6--操作类型*/					0x05, 
/*7--用户类型*/					0x03, 
/*8--登记类型*/					0x03, 
/*9--起始时间*/					0x19, 0x02, 0x22, 0x17, 0x42, 0x55, 
/*15--结束时间*/				0x19, 0x02, 0x23, 0x18, 0x55, 0x33,
/*21--使用次数*/				0xFF,
/*22--旧/新添加数据长度*/			0x0F, 
/*23--旧/新添加数据*/				0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x48,
							0x47, 0x46, 0x45, 0x44, 0x43,
/*38--修改的数据长度*/				0x0F,
/*39--修改的数据*/				0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x48,
							0x47, 0x46, 0x45, 0x44, 0x43,
/*54--校验*/					0x00, 0x01
};

/*允许门锁上报下一条开锁记录*/
const u8 SmartLockRecordRespondStream[]={
0xF5, 0xE1, 0x00, 0x02, 0x00, 0xFF, 0x01, 0x01
};

/*用于下载指静脉模板*/
u8 SmartLockDownLoadFingerVeinDataStream[]={
0x00
};

/*解密密钥*/
const u8 DecryptionKey[]={
0x49, 0x46, 0x45, 0x49, 0x42, 0x49, 0x47, 0x42, 0x46, 0x45
};


/***************************
 *监控消息定义
 ***************************/
/*门锁消息监控*/

static T_MsgSendMonitor MsgSendMonitorForSmartLockTimeSyn={
	.MsgName = "SmartLockTimeSyn",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)SmartLockTimeSynStream,
	.MsgLen = 12,
	.ptCommunicationModule = &SmartLockCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForSmartLockDownLoadData={
	.MsgName = "DownLoadData",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_4s,
	.SendData = (u8*)SmartLockDownLoadDataStream,
	.MsgLen = 56,
	.ptCommunicationModule = &SmartLockCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForSmartLockRemotePassword={
	.MsgName = "RemotePassword",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_9s,
	.SendData = (u8*)SmartLockRemotePasswordStream,
	.MsgLen = 16,
	.ptCommunicationModule = &SmartLockCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForSmartLockNetworkConStatus={
	.MsgName = "NetworkConStatus",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)SmartLockNetworkConStatusStream,
	.MsgLen = 8,
	.ptCommunicationModule = &SmartLockCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForSmartLockNetworkStatus={
	.MsgName = "NetworkStatus",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)SmartLockNetworkStatusStream,
	.MsgLen = 7,
	.ptCommunicationModule = &SmartLockCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForSmartLockAlawaysOpen={
	.MsgName = "DoorAlawaysOpen",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)SmartLockAlwaysOpenStream,
	.MsgLen = 10,
	.ptCommunicationModule = &SmartLockCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForSmartLockGetTime={
	.MsgName = "GetTime",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)SmartLockGetTimeStream,
	.MsgLen = 6,
	.ptCommunicationModule = &SmartLockCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForSmartLockPassThrough={
	.MsgName = "SmartLockPassThrough",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_9s,
	.SendData = (u8*)SmartLockPassThroughStream,
	.MsgLen = 64,
	.ptCommunicationModule = &SmartLockCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForSmartLockAdminVerifyNo1={
	.MsgName = "AdminVerifyNo1",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_5s,
	.SendData = (u8*)SmartLockAdminVerifyForDownloadDataStream,
	.MsgLen = 16,
	.ptCommunicationModule = &SmartLockCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForSmartLockAdminVerifyNo2={
	.MsgName = "AdminVerifyNo2",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_5s,
	.SendData = (u8*)SmartLockAdminVerifyForSetDoorOpenStream,
	.MsgLen = 16,
	.ptCommunicationModule = &SmartLockCommModule,
};

/*zigbee消息监控*/
static T_MsgSendMonitor MsgSendMonitorForNetWorkConnect={
	.MsgName = "NetWorkConnect",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeConnectStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForNetWorkDisconnect={
	.MsgName = "NetWorkDisconnect",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeDisconStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForOpenDoorInfo={
	.MsgName = "OpenDoorInfo",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeOpenDoorInfoStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForKeyOpenDoor={
	.MsgName = "KeyOpenDoor",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeKeyOpenDoorStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForDemolitionAlarm={
	.MsgName = "DemolitionAlarm",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeDemolitionAlarmStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForLowPowerAlarm={
	.MsgName = "LowPowerAlarm",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeLowePowerAlarmStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForLockKeyboardAlarm={
	.MsgName = "LockKeyboard",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeLockKeyboardStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForDoorBell={
	.MsgName = "DoorBell",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeDoorBellStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForDoorNotClose={
	.MsgName = "DoorNotClose",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeDoorNotCloseAlarmStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForVerfFailedAlarm={
	.MsgName = "VerfFailedAlarm",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeVerfFailedStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForLockDoorAlarm={
	.MsgName = "LockDoorAlarm",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeLockDoorAlarmStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForUnlockDoorAlarm={
	.MsgName = "UnlockDoorAlarm",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeUnlockDoorAlarmStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForGetTimeFromServer={
	.MsgName = "GetTimeFromServer",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeGetTimeFromServerStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForWkupZigbeeFor1Min={
	.MsgName = "WkupZigbeeFor1Min",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeWkupFor1MinStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForRemoteOpenDoorResult={
	.MsgName = "RemoteOpenDoorResult",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_1s,
	.SendData = (u8*)ZigbeeRemoteOpenDoorResultStream,
	.MsgLen = 20,
	.ptCommunicationModule = &ZigbeeCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForZigbeePassThrough={
	.MsgName = "ZigbeePassThrough",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_9s,
	.SendData = (u8*)ZigbeePassThroughStream,
	.MsgLen = 64,
	.ptCommunicationModule = &ZigbeeCommModule,
};
		
static T_MsgSendMonitor MsgSendMonitorForSetDoorAlwaysOpen={
	.MsgName = "SetDoorAlwaysOpen",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_9s,
	.SendData = (u8*)ZigbeeSetDoorAlwaysOpenStream,
	.MsgLen = 11,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForCancelDoorAlwaysOpen={
	.MsgName = "CancelDoorAlwaysOpen",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_9s,
	.SendData = (u8*)ZigbeeCancelDoorAlwaysOpenStream,
	.MsgLen = 11,
	.ptCommunicationModule = &ZigbeeCommModule,
};
	
static T_MsgSendMonitor MsgSendMonitorForRespondDoorStatus={
	.MsgName = "RespondDoorStatus",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_9s,
	.SendData = (u8*)ZigbeeSetDoorOpenPassThroughStream,
	.MsgLen = 11,
	.ptCommunicationModule = &ZigbeeCommModule,
};

static T_MsgSendMonitor MsgSendMonitorForUserDataChanged={
	.MsgName = "UserDataChanged",
	.TimeCout = 0,
	.SendCout = 0,
	.IsCanUsed = 0,
	.SetTimeOut = MSG_SEND_MONITOR_TIMEOUT_INVL_9s,
	.SendData = (u8*)ZigbeeUserDataChangedStream,
	.MsgLen = 26,
	.ptCommunicationModule = &ZigbeeCommModule,
};
	
/***************************
 *以下为所有定时器定义
 ***************************/	
/*系统重启定时器*/
static T_TimerModule SystemResetCnt={
	.TimerName = "SystemResetCnt",
	.TimerCnt = 0,
	.SetTimer = 0,
	.IsCanUsed = 0,
	.TimerCntEnd = 0,
	.FnSetTimer = SetTimer,
	.FnGetTimerStatus = GetTimerStatus,
	.FnCloseTimerModule = CloseTimerModule,
};

/*系统运行定时器*/
static T_TimerModule SystemRunningCnt={
	.TimerName = "SystemRunningCnt",
	.TimerCnt = 0,
	.SetTimer = 0,
	.IsCanUsed = 0,
	.TimerCntEnd = 0,
	.FnSetTimer = SetTimer,
	.FnGetTimerStatus = GetTimerStatus,
	.FnCloseTimerModule = CloseTimerModule,
};

/*定时唤醒zigbee*/
static T_TimerModule SingleWkupZigbeeCnt={
	.TimerName = "SingleWkupZigbeeCnt",
	.TimerCnt = 0,
	.SetTimer = 0,
	.IsCanUsed = 0,
	.TimerCntEnd = 0,
	.FnSetTimer = SetTimer,
	.FnGetTimerStatus = GetTimerStatus,
	.FnCloseTimerModule = CloseTimerModule,
};

/*循环唤醒zigbee*/
static T_TimerModule CycleWkupZigbeeCnt={
	.TimerName = "CycleWkupZigbeeCnt",
	.TimerCnt = 0,
	.SetTimer = 0,
	.IsCanUsed = 0,
	.TimerCntEnd = 0,
	.FnSetTimer = SetTimer,
	.FnGetTimerStatus = GetTimerStatus,
	.FnCloseTimerModule = CloseTimerModule,
};
	
/*****************************************************************************
 * Inline Functions
 ****************************************************************************/


/*****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/
void SystemClock_Config(void);
static int GetSmartLockEventInfo(char *buf, int len, PT_EventInfo ptEventInfo);
static int SmartLockEventPro(PT_EventInfo ptEventInfo);
static int GetZigbeeEventInfo(char * buf, int len, PT_EventInfo ptEventInfo);
static int ZigbeeEventPro(PT_EventInfo ptEventInfo);
static u8 XorCalcForZigbee(u8 *DataForCalc, u16 StartNum, u16 Length);
static void SystemResetEventDetectAndHandle(void);
static void CycleWkupZigbeeDetectAndHandle(void);
static void SingleWkupZigbeeDetectAndHandle(void);
static void SmartLockMsgHandle(void);
static void ZigbeeMsgHandle(void);
//static void DebugMsgHandle(void);
static void SmartLockLargeMsgHandle(void);
#ifdef USE_LOW_POWER
static int SystemStatusDetect(void);
#endif


/**
 * @fn		zbmodule_SetSerialPort
 *
 * @brief	设置串口名称
 *
 * @param[in]	serialPortName		 - 串口名称
 *
 * @return 0 - 成功, 否则表示失败
 */

 int main(void)
 {
	/*变量初始化*/
	int iRet = 0;
	T_EventInfo tSmartLockEventInfo = {
		.name = "SmartLockEvent",
		.cHasEventData = 0,
	};

	/*模块/硬件初始化*/
	HAL_Init();			 					//HAL库初始化
	SystemClock_Config();					//系统时钟初始化
	delay_init(16);		 					//延时函数初始化
	MX_GPIO_Init();		 					//GPIO初始化
	TIM2_Init();		 					//定时器初始化和开启中断
#ifdef USE_WWDG
    WWDG_Init(0X7F, 0X5F, WWDG_PRESCALER_8);//计数器值为7F，窗口寄存器为5F，分频数为8，实测中断时间128ms
#endif
#ifdef USE_IWDG
	IWDG_Init(IWDG_PRESCALER_128, 1000);					//4s定时
	RTC_Init();
	RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 1);		//2s中断一次
#endif

	/*注册使用的通讯模块*/
	RegisterCommModule(&DebugPortCommModule);
	RegisterCommModule(&SmartLockCommModule);
	RegisterCommModule(&ZigbeeCommModule);

	/*注册所有已定义消息监控*/
	RegisterMsgSendMonitor(&MsgSendMonitorForNetWorkConnect);
	RegisterMsgSendMonitor(&MsgSendMonitorForNetWorkDisconnect);
	RegisterMsgSendMonitor(&MsgSendMonitorForOpenDoorInfo);
	RegisterMsgSendMonitor(&MsgSendMonitorForKeyOpenDoor);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockTimeSyn);
	RegisterMsgSendMonitor(&MsgSendMonitorForDemolitionAlarm);
	RegisterMsgSendMonitor(&MsgSendMonitorForLowPowerAlarm);
	RegisterMsgSendMonitor(&MsgSendMonitorForLockKeyboardAlarm);
	RegisterMsgSendMonitor(&MsgSendMonitorForDoorBell);
	RegisterMsgSendMonitor(&MsgSendMonitorForDoorNotClose);
	RegisterMsgSendMonitor(&MsgSendMonitorForVerfFailedAlarm);
	RegisterMsgSendMonitor(&MsgSendMonitorForLockDoorAlarm);
	RegisterMsgSendMonitor(&MsgSendMonitorForUnlockDoorAlarm);
	RegisterMsgSendMonitor(&MsgSendMonitorForGetTimeFromServer);
	RegisterMsgSendMonitor(&MsgSendMonitorForWkupZigbeeFor1Min);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockDownLoadData);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockRemotePassword);
	RegisterMsgSendMonitor(&MsgSendMonitorForRemoteOpenDoorResult);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockNetworkConStatus);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockNetworkStatus);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockAlawaysOpen);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockGetTime);
	RegisterMsgSendMonitor(&MsgSendMonitorForZigbeePassThrough);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockPassThrough);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockAdminVerifyNo1);
	RegisterMsgSendMonitor(&MsgSendMonitorForSmartLockAdminVerifyNo2);
	RegisterMsgSendMonitor(&MsgSendMonitorForSetDoorAlwaysOpen);
	RegisterMsgSendMonitor(&MsgSendMonitorForCancelDoorAlwaysOpen);
	RegisterMsgSendMonitor(&MsgSendMonitorForRespondDoorStatus);
	RegisterMsgSendMonitor(&MsgSendMonitorForUserDataChanged);

	/*注册、启动定时器模块*/
	RegisterTimerModule(&SystemRunningCnt);
	SystemRunningCnt.FnSetTimer(&SystemRunningCnt, 50);//初始化时设定系统5秒运行时间
	RegisterTimerModule(&SystemResetCnt);
	RegisterTimerModule(&SingleWkupZigbeeCnt);
	RegisterTimerModule(&CycleWkupZigbeeCnt);
	
	/*执行通讯模块初始化*/
	iRet = CommModule("Debug")->FnInit();
	if(iRet)
	{
		DBG_PRINTF("\n\rCommModule:Debug init failed:%d\r\n", iRet);
		DBG_PRINTF("\n\rSystem start reset after 5s......\r\n");
		SystemResetCnt.FnSetTimer(&SystemResetCnt, 50);//5秒
		iRet = CommModule("Debug")->FnClose();
		if(iRet)
			DBG_PRINTF("\n\rCommModule Debug deinit failed: %d\r\n", iRet);
	}
	iRet = CommModule("SmartLock")->FnInit();
	if(iRet)
	{
		DBG_PRINTF("\n\rCommModule:SmartLock init failed:%d\r\n", iRet);
		DBG_PRINTF("\n\rSystem start reset after 5s......\r\n");
		SystemResetCnt.FnSetTimer(&SystemResetCnt, 50);//5秒
		iRet = CommModule("SmartLock")->FnClose();
		if(iRet)
			DBG_PRINTF("\n\rCommModule SmartLock deinit failed: %d\r\n", iRet);
	}
	iRet = CommModule("Zigbee")->FnInit();
	if(iRet)
	{
		DBG_PRINTF("\n\rCommModule:Zigbee init failed:%d\r\n", iRet);
		DBG_PRINTF("\n\rSystem start reset after 5s......\r\n");
		SystemResetCnt.FnSetTimer(&SystemResetCnt, 50);//5秒
		iRet = CommModule("Zigbee")->FnClose();
		if(iRet)
			DBG_PRINTF("\n\rCommModule Zigbee deinit failed: %d\r\n", iRet);
	}
	
	/*开启外部中断*/
	SmartLock_EXTI_Init();
	Zigbee_EXTI_Init();
	
	/*程序版本打印*/
	DBG_PRINTF("\n\rFGT comm module init success: %s\r\n", SYSTEM_VERSION);

	/*测试用*/
//	RTC_Init();
//	RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);	//1s中断一次
//	
	while(1)
	{
		
#ifdef USE_IWDG
		/*喂狗*/
		IWDG_Feed();
#endif
		/*Zigbee消息处理*/
		ZigbeeMsgHandle();
		
		/*SmartLock消息处理*/
		SmartLockMsgHandle();
				
		/*SmartLock大量数据消息处理*/
		SmartLockLargeMsgHandle();
		
		/*消息监控处理*/
		MsgSendMonitorCheckAndReSend();
		
		/*循环唤醒zigbee*/
		CycleWkupZigbeeDetectAndHandle();
		
		/*单次定时唤醒zigbee*/
		SingleWkupZigbeeDetectAndHandle();
		
		/*"重启"事件监测和处理*/
		SystemResetEventDetectAndHandle();
		
#ifdef USE_LOW_POWER
		/*低功耗处理
		 *注意，低功耗处理不能放到函数中执行，一定要在主循环中执行
		 */
		if((0 == SystemStatusDetect()) && (1 == SystemRunningCnt.FnGetTimerStatus(&SystemRunningCnt)))
		{
			SystemRunningCnt.FnCloseTimerModule(&SystemRunningCnt);
			/*进入低功耗*/
			DBG_PRINTF("\n\rSystem enter sleep\r\n");
			/*关掉zigbee唤醒中断*/
//			Zigbee_EXTI_DeInit();
			/*关闭门锁唤醒中断*/
//			SmartLock_EXTI_DeInit();
			/*清除计数*/
			SysWkupCnt = 0;
			/*关闭通讯模块(串口)*/
			CommModule("SmartLock")->FnClose();
			CommModule("Zigbee")->FnClose();	
			CommModule("Debug")->FnClose();
			/*关闭定时器*/
			TIM2_DeInit();
			/*关闭zigbee唤醒引脚*/
			CloseZigbeeWkupGPIO();
			/*关闭门锁唤醒引脚*/
			CloseSmartLockWkupGPIO();
			/*通知中断服务程序系统进入休眠*/
			SystemSleepNotify();
			/*重新打开门锁外部中断*/
			SmartLock_EXTI_Init();
			/*重新打开zigbee唤醒中断*/
			Zigbee_EXTI_Init();
#ifdef USE_WWDG
			/*关闭看门狗*/
			CloseWWDG();
#endif
			/*进入stop模式*/	
EnterStopMode:
			HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
			E_EXTIEvent eExtiEventTmp = GetEXTIEvent();
			switch (eExtiEventTmp)
			{
				case ZigbeeWkup:
				case SmartLockWkup:
					/*退出低功耗*/
#ifdef USE_WWDG
					WWDG_Init(0X7F, 0X5F, WWDG_PRESCALER_8);
#endif
					Zigbee_EXTI_DeInit();	
					SmartLock_EXTI_DeInit();
					ResetEXTIEvent();		//复位事件状态
					SystemRunningNotify();	//通知系统运行
					OpenZigbeeWkupGPIO();
					OpenSmartLockWkupGPIO();
					TIM2_Init();
					CommModule("SmartLock")->FnOpen();
					CommModule("Zigbee")->FnOpen(); 
					CommModule("Debug")->FnOpen();
//					SmartLock_EXTI_Init();
//					Zigbee_EXTI_Init();
					SystemRunningCnt.FnSetTimer(&SystemRunningCnt, SYSTEM_SLEEP_DELAY);
					DBG_PRINTF("\n\rSystem wake up...\r\n");
					break;
				case RTCAlarmA:
				case RTCWakeUp:
#ifdef USE_IWDG
					IWDG_Feed();
#endif
					goto EnterStopMode;
				default:
					goto EnterStopMode;
			}
		}
		/*如果系统正在运行，再启动定时器*/
		else if(SystemStatusDetect() > 0)
		{
			SystemRunningCnt.FnSetTimer(&SystemRunningCnt, SYSTEM_SLEEP_DELAY);
		}
#endif
	}
 
 }


/*解析门锁信息
 *buf - 从门锁获取到的数据
 *ptEventInfo - 用于保存解析完成后的事件信息
 *BufLen - 传入的数据长度
 *return 0 - 解析成功， 否则解析失败
 */
static int GetSmartLockEventInfo(char *buf, int len, PT_EventInfo ptEventInfo)
{
	int iRet = -1;
	T_BCDTime tBCDTimeTmp;
	struct tm tUTCTimeTmp;
	u8 	XorCalcTmp = 0;
	u16 SendDataLen = 0;
	u16 LRC = 0;
	u8	RecordPackageNum = 0;
	u8	UserInfoPackageNum = 0;
	u8	OnePackageDataLen = 0;
	u16	Offset = 0;
	/*判断命令字*/
	switch (buf[1])
	{
		case 0x29:
			CloseMsgSendMonitor("AdminVerifyNo1");
			switch(buf[4])
			{
				case 0x00:
					ptEventInfo->eEventType = AdminVerifySuccess;
					ptEventInfo->cHasEventData = 1;
					iRet = 0;
					break;
				case 0x01:
					CloseMsgSendMonitor("DownLoadData");
					ptEventInfo->eEventType = AdminPasswordWrong;
					ptEventInfo->cHasEventData = 1;
					iRet = 0;
					break;
				case 0x02:
					CloseMsgSendMonitor("DownLoadData");
					ptEventInfo->eEventType = NoAdminPassword;
					ptEventInfo->cHasEventData = 1;
					iRet = 0;
					break;
			}
			break;
		case 0x10://用户数据下发结果上传
			CloseMsgSendMonitor("DownLoadData");
			switch(buf[6])
			{
				case 0x00://无论操作类型是什么，操作成功统一返回0x00
					ptEventInfo->eEventType = DownloadDataSuccess;
					goto CommonHandle;
				case 0x01:
					ptEventInfo->eEventType = VerifyTooMuch;
					goto CommonHandle;
				case 0x02:
					ptEventInfo->eEventType = CanNotFindTheID;
					goto CommonHandle;
				case 0x03:
					ptEventInfo->eEventType = NoOldPasswordOrCard;
					goto CommonHandle;
				case 0x04:
					ptEventInfo->eEventType = NoPermisToModifyAdmin;
					goto CommonHandle;
				case 0x05:
					ptEventInfo->eEventType = StartTimeIsBiggerThanEndTime;
					goto CommonHandle;
				case 0x06:
					ptEventInfo->eEventType = UserTypeNotMatch;
					goto CommonHandle;
				case 0x07:
					ptEventInfo->eEventType = MethodTypeNotMatch;
					goto CommonHandle;
				case 0x08:
					ptEventInfo->eEventType = PasswordRepeat;
					goto CommonHandle;
				case 0x09:
					ptEventInfo->eEventType = NoAdminPassword;
					goto CommonHandle;
				case 0x0A:
					ptEventInfo->eEventType = NoFingerVein;
					goto CommonHandle;
				case 0x0B:
					ptEventInfo->eEventType = NoPassword;
					goto CommonHandle;
				case 0x0C:
					ptEventInfo->eEventType = NoICCard;
					goto CommonHandle;
		CommonHandle:
					ptEventInfo->uEventData.tAddPasswordRespondInfo.PasswordID = ((u16)buf[7] << 8) + ((u16)buf[8] & 0x00FF);
					ptEventInfo->cHasEventData = 1;
					iRet = 0;
					break;
			}
			break;
		case 0x13:
			CloseMsgSendMonitor("RemotePassword");
			switch(buf[4])
			{
				case 0x00:
					ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.eRemoteOpenErrorCode = PasswordRight;
					break;
				case 0x01:
					ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.eRemoteOpenErrorCode = NoPermissToOpenDoor;
					break;
				case 0x02:
					ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.eRemoteOpenErrorCode = PasswordError;
					break;
				case 0x04:
					ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.eRemoteOpenErrorCode = IsFreeze;
					break;
				case 0x05:
					ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.eRemoteOpenErrorCode = InvalidTime;
					break;
				default:
					ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.eRemoteOpenErrorCode = (E_RemoteOpenErrorCode)buf[4];
					break;
			}
			ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.UserId = ((u16)buf[5] << 8) + (u16)buf[6];
			ptEventInfo->eEventType = RemotePasswordOpenDoorResult;
			ptEventInfo->cHasEventData = 1;
			iRet = 0;
			break;
		case 0x28:
			CloseMsgSendMonitor("GetTime");
			ptEventInfo->uEventData.tBCDTimeFromSmartLock.year = buf[4];
			ptEventInfo->uEventData.tBCDTimeFromSmartLock.month = buf[5];
			ptEventInfo->uEventData.tBCDTimeFromSmartLock.day = buf[6];
			ptEventInfo->uEventData.tBCDTimeFromSmartLock.hour = buf[7];
			ptEventInfo->uEventData.tBCDTimeFromSmartLock.minutes = buf[8];
			ptEventInfo->uEventData.tBCDTimeFromSmartLock.seconds = buf[9];
			ptEventInfo->eEventType = GetSmartLockTIme;
			ptEventInfo->cHasEventData = 1;
			break;
		case 0x33://入网
			CommModule("SmartLock")->FnSendData((u8*)SmartLockNetworkConRespondStream, sizeof(SmartLockNetworkConRespondStream), TX_TIMEOUT);
			ptEventInfo->eEventType = NetWorkConReuqest;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x34://退网
			CommModule("SmartLock")->FnSendData((u8*)SmartLockNetworkdisconRespondStream, sizeof(SmartLockNetworkdisconRespondStream), TX_TIMEOUT);
			ptEventInfo->eEventType = NetworkDisconRequest;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x24://门锁状态上传
			switch (buf[4])
			{
				case 0x00://防拆报警
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = DemolitionAlarm;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x02:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = LowPowerAlarm;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x03:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = LockKeyboardAlarm;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x04:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = DoorBell;
					ptEventInfo->uEventData.DoorBellTime = buf[6];
					ptEventInfo->cHasEventData = 1;
					iRet = 0;
					break;
				case 0x05:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = WkupNetWorkRequest;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x06:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = CancelDoorAlwaysOpen;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x07:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = SetDoorAlwaysOpen;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x08:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = DoorNotClosedAlarm;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x09:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = FingerVeinVerfAlarm;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x0A:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = PasswordVerfAlarm;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x0B:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = CardVerfAlarm;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x0C:
				case 0x0D:
				case 0x0E:
				case 0x0F:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ZigbeeSmartLockModeChangedStream[8] = buf[4];
					ZigbeeSmartLockModeChangedStream[9] = buf[5];
					ZigbeeSmartLockModeChangedStream[10] = buf[6];
					XorCalcTmp = XorCalcForZigbee(ZigbeeSmartLockModeChangedStream, 0, 11);
					ZigbeeSmartLockModeChangedStream[11] = XorCalcTmp;
					ZigbeeSmartLockModeChangedStream[12] = 0x55;
					DBG_PRINTF("\n\rZigbee pass through data:\r\n");
					for(int i = 0; i < (u16)13; i++)
						DBG_PRINTF("0x%02X ", ZigbeeSmartLockModeChangedStream[i]);
					DBG_PRINTF("\n\r\r\n");
					CommModule("Zigbee")->FnSendData(ZigbeeSmartLockModeChangedStream, (u16)13, TX_TIMEOUT);
					break;
				case 0x10:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = LockDoorAlarm;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x11:
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = UnlockDoorAlarm;
					ptEventInfo->cHasEventData = 0;
					iRet = 0;
					break;
				case 0x12://历史遗留问题，钥匙开锁视为本地开锁上报
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					ptEventInfo->eEventType = OpenDoorSuccess;
					ptEventInfo->uEventData.tOpenDoorInfo.No1UserID = ((u16)buf[4]<<8) + (u16)buf[5];
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = KeyOpenDoor;
					ptEventInfo->uEventData.tOpenDoorInfo.OpenDuration = 0;//默认0
					ptEventInfo->cHasEventData = 1;
					iRet = 0;
					break;
				case 0x13://收到门锁上报休眠状态，不要应答门锁
					ptEventInfo->eEventType = SmartLockEnterSleep;
					iRet = 0;
					break;
				default://未增加协议
					CommModule("SmartLock")->FnSendData((u8*)SmartLockStatusRespondStream, sizeof(SmartLockStatusRespondStream), TX_TIMEOUT);
					iRet = -1;
					break;
			}	
			break;
		case 0x21:
			CloseMsgSendMonitor("SmartLockTimeSyn");
			ptEventInfo->eEventType = SmartLockTimeSynRequest;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x22://用户数据上传
			CommModule("SmartLock")->FnSendData((u8*)SmartLockDataUploadRespondStream, sizeof(SmartLockDataUploadRespondStream), TX_TIMEOUT);
			ptEventInfo->eEventType = SmartLockUserDataUpload;
			ptEventInfo->uEventData.tUserDataUploadInfo.UserId = ((u16)buf[4]<<8) + ((u16)buf[5]);			
			switch(buf[6])
			{
				case 0x01:
					ptEventInfo->uEventData.tUserDataUploadInfo.eActionType = AddUser;
					break;
				case 0x02:
					ptEventInfo->uEventData.tUserDataUploadInfo.eActionType = DelUser;
					break;
				case 0x03:
					ptEventInfo->uEventData.tUserDataUploadInfo.eActionType = CleanUserExceptAdmin;
					break;
				case 0x04:
					ptEventInfo->uEventData.tUserDataUploadInfo.eActionType = CleanAllData;
					break;
			}
			switch(buf[7])
			{
				case 0x01:
					ptEventInfo->uEventData.tUserDataUploadInfo.eUserType = GenernalUser;
					break;
				case 0x02:
					ptEventInfo->uEventData.tUserDataUploadInfo.eUserType = Guest;
					break;
				case 0x03:
					ptEventInfo->uEventData.tUserDataUploadInfo.eUserType = Admin;
					break;
				case 0x04:
					ptEventInfo->uEventData.tUserDataUploadInfo.eUserType = DuressUser;
					break;
				case 0x05:
					ptEventInfo->uEventData.tUserDataUploadInfo.eUserType = Nurse;
					break;
				case 0x06:
					ptEventInfo->uEventData.tUserDataUploadInfo.eUserType = TemporaryUser;
					break;
			}
			switch(buf[8])
			{
				case 0x01:
					ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType = Vein;
					break;
				case 0x02:
					ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType = Card;
					break;
				case 0x03:
					ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType = Password;
					break;
				case 0x04:
					ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType = Face;
					break;
				case 0x05:
					ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType = Fingerprint;
					break;
				default:
					break;
			}
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.year = buf[9];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.month = buf[10];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.day = buf[11];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.hour = buf[12];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.minutes = buf[13];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.seconds = buf[14];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.year = buf[15];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.month = buf[16];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.day = buf[17];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.hour = buf[18];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.minutes = buf[19];
			ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.seconds = buf[20];
			ptEventInfo->uEventData.tUserDataUploadInfo.NumOfTime = buf[21];
			/*如果是添加密码*/
			if(AddUser == ptEventInfo->uEventData.tUserDataUploadInfo.eActionType 
				&& Password == ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType)
			{
				ptEventInfo->uEventData.tUserDataUploadInfo.PasswordLen = buf[22];
				for(int i = 0; i < buf[22]; i++)
					ptEventInfo->uEventData.tUserDataUploadInfo.Password[i] = buf[i + 23];
			}
			/*如果是添加卡*/
			if(AddUser == ptEventInfo->uEventData.tUserDataUploadInfo.eActionType 
				&& Card == ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType)
			{
				ptEventInfo->uEventData.tUserDataUploadInfo.CardLen = buf[22];
				for(int i = 0; i < buf[22]; i++)
					ptEventInfo->uEventData.tUserDataUploadInfo.Card[i] = buf[i + 23];
			}
			/*如果是添加静脉*/
			if(AddUser == ptEventInfo->uEventData.tUserDataUploadInfo.eActionType 
				&& Vein == ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType)
			{
				;
			}
			ptEventInfo->cHasEventData = 1;
			iRet = 0;
			break;
		case 0x23://本地开锁上报
			CommModule("SmartLock")->FnSendData((u8*)SmartLockOpenDoorRespondStream, sizeof(SmartLockOpenDoorRespondStream), TX_TIMEOUT);
			ptEventInfo->eEventType = OpenDoorSuccess;
			ptEventInfo->uEventData.tOpenDoorInfo.No1UserID = ((u16)buf[4]<<8) + (u16)buf[5];
			switch (buf[6])
			{
				case 0x01:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = Vein;
					break;
				case 0x02:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = Card;
					break;
				case 0x03:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = Password;
					break;
				case 0x04:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = CardAndPassword;
					break;
				case 0x05:
//					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = TimeFingerVein;
					break;
				case 0x06:
//					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = TimeCard;
					break;
				case 0x07:
//					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = TimePassword;
					break;
				case 0x08:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = TemporaryPassword;
					break;
				case 0x09:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = Duress;
					break;
				case 0x0A:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = Face;
					break;
				case 0x0B:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = Fingerprint;
					break;
				case 0x0F:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = RemotePassword;
					break;
				case 0x13:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = CardAndFingerVein;
					break;
				case 0x14:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = FingerVeinAndPassword;
					break;
				case 0x15:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = FingerVeinAndPasswordAndCard;
					break;
				case 0x16:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = FingerVeinAndFingerVein;
					break;
				default:
					ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod = UnknownMethod;
					break;
			}
			if(0x03 == buf[7])
				ptEventInfo->uEventData.tOpenDoorInfo.ePowerPercent = Percent75To100;
			else if(0x02 == buf[7])	
				ptEventInfo->uEventData.tOpenDoorInfo.ePowerPercent = Percent50To75;
			else if(0x01 == buf[7])
				ptEventInfo->uEventData.tOpenDoorInfo.ePowerPercent = Percent25To50;
			else
				ptEventInfo->uEventData.tOpenDoorInfo.ePowerPercent = Percent0To25;
			tBCDTimeTmp.year = buf[8];
			tBCDTimeTmp.month = buf[9];
			tBCDTimeTmp.day = buf[10];
			tBCDTimeTmp.hour = buf[11];
			tBCDTimeTmp.minutes = buf[12];
			tBCDTimeTmp.seconds = buf[13];
			tUTCTimeTmp = BCDTimeConvertToUTCTime(tBCDTimeTmp);
			ptEventInfo->uEventData.tOpenDoorInfo.TimeSamp = (u32)mktime(&tUTCTimeTmp);
			ptEventInfo->uEventData.tOpenDoorInfo.OpenDuration = 0;//默认0
			ptEventInfo->cHasEventData = 1;
			iRet = 0;
			break;
		case 0x35:
			CommModule("SmartLock")->FnSendData((u8*)SmartLockWkupZigbeeRespondStream, sizeof(SmartLockWkupZigbeeRespondStream), TX_TIMEOUT);
			ptEventInfo->eEventType = WkupZigbeeFor1Min;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x25:
			CloseMsgSendMonitor("NetworkConStatus");
			ptEventInfo->eEventType = NetworkConStatusRecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x26:
			CloseMsgSendMonitor("NetworkStatus");
			ptEventInfo->eEventType = NetworkStatusRecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x27:
			CloseMsgSendMonitor("DoorAlawaysOpen");
			ptEventInfo->eEventType = DoorAlwaysOpenCmdExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		/*******************************************************************************
		 *对于透传数据，直接在GetEventInfo转发，透传数据不使用重发
		 *对于E1、E2需要拆包
		 *启动5s后唤醒zigbee的方法在GetSmartLockEventInfo执行
		 *******************************************************************************/
		case 0xE1:
			RecordPackageNum = 0;
			SendDataLen = ((u16)buf[2]<<8) + ((u16)buf[3]) - 1; //减掉一个字节(历史开锁记录总个数)
			DBG_PRINTF("\n\rSendDataLen = %d\r\n", SendDataLen);
			if(SendDataLen <= 40)
			{
				ZigbeePassThroughStream[0] = 0xAA;
				ZigbeePassThroughStream[1] = SendDataLen;
				ZigbeePassThroughStream[2] = 0xE1;	//门锁上报历史开门记录
				ZigbeePassThroughStream[7] = 0x01;	//必须是0x01
				for(int i = 0; i < SendDataLen; i++)		
					ZigbeePassThroughStream[i + 8] = buf[i + 5];//buf[5]开始
				XorCalcTmp = XorCalcForZigbee(ZigbeePassThroughStream, 0, (SendDataLen + 8));
				ZigbeePassThroughStream[SendDataLen + 8] = XorCalcTmp;
				ZigbeePassThroughStream[SendDataLen + 9] = 0x55;
				SendDataLen = SendDataLen + 10;
				DBG_PRINTF("\n\rZigbee pass through data:\r\n");
				for(int i = 0; i < SendDataLen; i++)
					DBG_PRINTF("0x%02X ", ZigbeePassThroughStream[i]);
				DBG_PRINTF("\n\r\r\n");
				CommModule("Zigbee")->FnSendData(ZigbeePassThroughStream, SendDataLen, TX_TIMEOUT);
				/*发送应答给门锁*/
				delay_ms(50);
				RecordPackageNum += 1;
				SmartLockUploadRecordSendingStream[4] = RecordPackageNum;
				LRC = 0;
				for(int i = 2; i < 5; i++)
					LRC += SmartLockUploadRecordSendingStream[i];
				SmartLockUploadRecordSendingStream[5] = (u8)(LRC >> 8);
				SmartLockUploadRecordSendingStream[6] = (u8)(LRC & 0x00FF);
				CommModule("SmartLock")->FnSendData(SmartLockUploadRecordSendingStream, 7, TX_TIMEOUT);
			}
			else
			{
				/*拆包发送*/
				while(1)
				{
					if(SendDataLen > 40)
						OnePackageDataLen = 40;
					else
						OnePackageDataLen = (u8)SendDataLen;
					Offset += 40;
					ZigbeePassThroughStream[0] = 0xAA;
					ZigbeePassThroughStream[1] = OnePackageDataLen;
					ZigbeePassThroughStream[2] = 0xE1;	//门锁上报历史开门记录
					ZigbeePassThroughStream[7] = 0x01;	//必须是0x01
					for(int i = 0; i < OnePackageDataLen; i++)		
						ZigbeePassThroughStream[i + 8] = buf[i + (Offset - 40) + 5];//buf[5]开始
					XorCalcTmp = XorCalcForZigbee(ZigbeePassThroughStream, 0, (OnePackageDataLen + 8));
					ZigbeePassThroughStream[OnePackageDataLen + 8] = XorCalcTmp;
					ZigbeePassThroughStream[OnePackageDataLen + 9] = 0x55;
					OnePackageDataLen += 10;
					DBG_PRINTF("\n\rZigbee pass through data:\r\n");
					for(int i = 0; i < OnePackageDataLen; i++)
						DBG_PRINTF("0x%02X ", ZigbeePassThroughStream[i]);
					DBG_PRINTF("\n\r\r\n");
					CommModule("Zigbee")->FnSendData(ZigbeePassThroughStream, OnePackageDataLen, TX_TIMEOUT);
					/*发送应答给门锁*/
					delay_ms(50);
					RecordPackageNum += 1;
					SmartLockUploadRecordSendingStream[4] = RecordPackageNum;
					LRC = 0;
					for(int i = 2; i < 5; i++)
						LRC += SmartLockUploadRecordSendingStream[i];
					SmartLockUploadRecordSendingStream[5] = (u8)(LRC >> 8);
					SmartLockUploadRecordSendingStream[6] = (u8)(LRC & 0x00FF);
					CommModule("SmartLock")->FnSendData(SmartLockUploadRecordSendingStream, 7, TX_TIMEOUT);
#ifdef USE_IWDG
					/*喂狗*/
					IWDG_Feed();
#endif
					delay_ms(250);
					if(SendDataLen <= 40)
						break;
					SendDataLen -= 40;
				}
				
			}
			CommModule("SmartLock")->FnSendData((u8*)SmartLockUploadRecordSuccessStream, sizeof(SmartLockUploadRecordSuccessStream), TX_TIMEOUT);
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeUploadRecordFinishStream, sizeof(ZigbeeUploadRecordFinishStream), TX_TIMEOUT);
			DBG_PRINTF("\n\rUpload record success\r\n");
			break;
		case 0xE2:
			UserInfoPackageNum = 0;
			SendDataLen = ((u16)buf[2]<<8) + ((u16)buf[3]) - 5; //减掉5个字节(密钥总个数)
			DBG_PRINTF("\n\rSendDataLen = %d\r\n", SendDataLen);
			if(SendDataLen <= 48)
			{
				ZigbeePassThroughStream[0] = 0xAA;
				ZigbeePassThroughStream[1] = SendDataLen;
				ZigbeePassThroughStream[2] = 0xE2;	//门锁上报用户信息
				ZigbeePassThroughStream[7] = 0x01;	//必须是0x01
				for(int i = 0; i < SendDataLen; i++)		
					ZigbeePassThroughStream[i + 8] = buf[i + 9];//buf[9]开始
				XorCalcTmp = XorCalcForZigbee(ZigbeePassThroughStream, 0, (SendDataLen + 8));
				ZigbeePassThroughStream[SendDataLen + 8] = XorCalcTmp;
				ZigbeePassThroughStream[SendDataLen + 9] = 0x55;
				SendDataLen = SendDataLen + 10;
				DBG_PRINTF("\n\rZigbee pass through data:\r\n");
				for(int i = 0; i < SendDataLen; i++)
					DBG_PRINTF("0x%02X ", ZigbeePassThroughStream[i]);
				DBG_PRINTF("\n\r\r\n");
				CommModule("Zigbee")->FnSendData(ZigbeePassThroughStream, SendDataLen, TX_TIMEOUT);
				/*发送应答给门锁*/
				delay_ms(50);
				UserInfoPackageNum += 1;
				SmartLockUploadUserInfoSendingStream[4] = UserInfoPackageNum;
				LRC = 0;
				for(int i = 2; i < 5; i++)
					LRC += SmartLockUploadUserInfoSendingStream[i];
				SmartLockUploadUserInfoSendingStream[5] = (u8)(LRC >> 8);
				SmartLockUploadUserInfoSendingStream[6] = (u8)(LRC & 0x00FF);
				CommModule("SmartLock")->FnSendData(SmartLockUploadUserInfoSendingStream, 7, TX_TIMEOUT);
			}
			else
			{
				/*拆包发送*/
				while(1)
				{
					if(SendDataLen > 48)
						OnePackageDataLen = 48;
					else
						OnePackageDataLen = (u8)SendDataLen;
					Offset += 48;
					ZigbeePassThroughStream[0] = 0xAA;
					ZigbeePassThroughStream[1] = OnePackageDataLen;
					ZigbeePassThroughStream[2] = 0xE2;	//门锁上报用户信息
					ZigbeePassThroughStream[7] = 0x01;	//必须是0x01
					for(int i = 0; i < OnePackageDataLen; i++)		
						ZigbeePassThroughStream[i + 8] = buf[i + (Offset - 48) + 9];
					XorCalcTmp = XorCalcForZigbee(ZigbeePassThroughStream, 0, (OnePackageDataLen + 8));
					ZigbeePassThroughStream[OnePackageDataLen + 8] = XorCalcTmp;
					ZigbeePassThroughStream[OnePackageDataLen + 9] = 0x55;
					OnePackageDataLen += 10;
					DBG_PRINTF("\n\rZigbee pass through data:\r\n");
					for(int i = 0; i < OnePackageDataLen; i++)
						DBG_PRINTF("0x%02X ", ZigbeePassThroughStream[i]);
					DBG_PRINTF("\n\r\r\n");
					CommModule("Zigbee")->FnSendData(ZigbeePassThroughStream, OnePackageDataLen, TX_TIMEOUT);
					/*发送应答给门锁*/
					delay_ms(50);
					UserInfoPackageNum += 1;
					SmartLockUploadUserInfoSendingStream[4] = UserInfoPackageNum;
					LRC = 0;
					for(int i = 2; i < 5; i++)
						LRC += SmartLockUploadUserInfoSendingStream[i];
					SmartLockUploadUserInfoSendingStream[5] = (u8)(LRC >> 8);
					SmartLockUploadUserInfoSendingStream[6] = (u8)(LRC & 0x00FF);
					CommModule("SmartLock")->FnSendData(SmartLockUploadUserInfoSendingStream, 7, TX_TIMEOUT);
#ifdef USE_IWDG
					/*喂狗*/
					IWDG_Feed();
#endif
					delay_ms(250);
					if(SendDataLen <= 48)
						break;
					SendDataLen -= 48;
				}
				
			}
			CommModule("SmartLock")->FnSendData((u8*)SmartLockUploadUserInfoSuccessStream, sizeof(SmartLockUploadUserInfoSuccessStream), TX_TIMEOUT);
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeUploadUserInfoFinishStream, sizeof(ZigbeeUploadUserInfoFinishStream), TX_TIMEOUT);
			DBG_PRINTF("\n\rUpload user info success\r\n");
			break;
		case 0xE5:
			ZigbeePassThroughStream[2] = 0xE5; 	//门锁上报设备版本号
			goto SmartLockPassThroughHandle;
		case 0xE6:
			ZigbeePassThroughStream[2] = 0xE6; 	//门锁上报电量
			goto SmartLockPassThroughHandle;
		case 0xEE:
			ZigbeePassThroughStream[2] = 0xEE;	//音量设置应答
			goto SmartLockPassThroughHandle;
SmartLockPassThroughHandle:
			ZigbeePassThroughStream[0] = 0xAA;
			ZigbeePassThroughStream[1] = buf[3];
			ZigbeePassThroughStream[7] = 0x01;	//必须是0x01
			for(int i = 0; i < buf[3]; i++)
				ZigbeePassThroughStream[i + 8] = buf[i + 4];
			XorCalcTmp = XorCalcForZigbee(ZigbeePassThroughStream, 0, (buf[3] + 8));
			ZigbeePassThroughStream[buf[3] + 8] = XorCalcTmp;
			ZigbeePassThroughStream[buf[3] + 9] = 0x55;
			SendDataLen = buf[3] + 10;
			DBG_PRINTF("\n\rZigbee pass through data:\r\n");
			for(int i = 0; i < SendDataLen; i++)
				DBG_PRINTF("0x%02X ", ZigbeePassThroughStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("Zigbee")->FnSendData(ZigbeePassThroughStream, SendDataLen, TX_TIMEOUT);
			break;
		case 0xED:
			switch(buf[4])
			{
				case 0x00:
					ptEventInfo->eEventType = SetDoorAlwaysOpenSuccess;
					break;
				case 0x01:
					ptEventInfo->eEventType = SetDoorAlwaysOpenFailed;
					break;
			}
			ZigbeeSetDoorOpenPassThroughStream[8] = buf[4];	//门锁常开操作结果
			goto ReturnSetDoorOpenResultHandle;
		case 0x2A:
			CloseMsgSendMonitor("AdminVerifyNo2");
			switch(buf[4])
			{
				case 0x00:
					ptEventInfo->eEventType = SetDoorAlwaysOpenAdminVerifySuccess;
					iRet = 0;
					break;
				/*管理员验证失败，直接返回结果给服务器*/
				case 0x01:
					ZigbeeSetDoorOpenPassThroughStream[8] = 0x02; 	//管理员密码错误
					ptEventInfo->eEventType = SetDoorAlwaysOpenAdminPasswordWrong;
					goto ReturnSetDoorOpenResultHandle;
				case 0x02:
					ZigbeeSetDoorOpenPassThroughStream[8] = 0x03; 	//没有管理员密码
					ptEventInfo->eEventType = SetDoorAlwaysOpenNoAdminPassword;
					goto ReturnSetDoorOpenResultHandle;
			}
ReturnSetDoorOpenResultHandle:
			ZigbeeSetDoorOpenPassThroughStream[2] = 0xED;	//门锁常开应答
			ZigbeeSetDoorOpenPassThroughStream[0] = 0xAA;
			ZigbeeSetDoorOpenPassThroughStream[1] = buf[3];
			ZigbeeSetDoorOpenPassThroughStream[7] = 0x01;	//必须是0x01
			XorCalcTmp = XorCalcForZigbee(ZigbeeSetDoorOpenPassThroughStream, 0, 9);
			ZigbeeSetDoorOpenPassThroughStream[9] = XorCalcTmp;
			ZigbeeSetDoorOpenPassThroughStream[10] = 0x55;
			DBG_PRINTF("\n\rZigbee pass through data:\r\n");
			for(int i = 0; i < 11; i++)
				DBG_PRINTF("0x%02X ", ZigbeeSetDoorOpenPassThroughStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("Zigbee")->FnSendData(ZigbeeSetDoorOpenPassThroughStream, 11, TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("RespondDoorStatus");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find RespondDoorStatus failed\r\n");
			SingleWkupZigbeeCnt.FnSetTimer(&SingleWkupZigbeeCnt, 50);//5s后触发
			iRet = 0;
			break;
		case 0xEC:
			DBG_PRINTF("\n\rRequest server time\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeGetServerTime, sizeof(ZigbeeGetServerTime), TX_TIMEOUT);
			SingleWkupZigbeeCnt.FnSetTimer(&SingleWkupZigbeeCnt, 90);//9s后触发
			break;
		default:
			DBG_PRINTF("\n\rUnknow event: 0x%02X\r\n", buf[1]);
			ptEventInfo->eEventType = UnknowEvent;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
	}
	
	return iRet;
}


/*门锁事件处理*/
static int SmartLockEventPro(PT_EventInfo ptEventInfo)
{
	int iRet = 0;
	u8 	XorCalcTmp = 0;
	switch (ptEventInfo->eEventType)
	{
		case NetWorkConReuqest:
			DBG_PRINTF("\n\rSend ZigbeeConnectStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeConnectStream, sizeof(ZigbeeConnectStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("NetWorkConnect");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find ZigbeeConnect failed\r\n");
			break;
		case NetworkDisconRequest:
			DBG_PRINTF("\n\rSend ZigbeeDisconnectStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeDisconStream, sizeof(ZigbeeDisconStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("NetWorkDisconnect");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find NetWorkDisconnect failed\r\n");
			break;
		case NetworkConStatusRecSuccess:
			DBG_PRINTF("\n\rSmart lock network connect status receive success\r\n");
			break;
		case NetworkStatusRecSuccess:
			DBG_PRINTF("\n\rSmart lock network status receive success\r\n");
			break;
		case DoorAlwaysOpenCmdExecSuccess:
			DBG_PRINTF("\n\rSmart lock always open receive success\r\n");
			break;
		case DemolitionAlarm:
			DBG_PRINTF("\n\rSend ZigbeeDemolitionAlarmStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeDemolitionAlarmStream, sizeof(ZigbeeDemolitionAlarmStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("DemolitionAlarm");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find NetWorkDisconnect failed\r\n");
			break;
		case LowPowerAlarm:
			DBG_PRINTF("\n\rSend ZigbeeLowePowerAlarmSream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeLowePowerAlarmStream, sizeof(ZigbeeLowePowerAlarmStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("LowPowerAlarm");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find LowPowerAlarm failed\r\n");
			break;
		case LockKeyboardAlarm:
			DBG_PRINTF("\n\rSend ZigbeeLockKeyboardSream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeLockKeyboardStream, sizeof(ZigbeeLockKeyboardStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("LockKeyboard");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find LockKeyboard failed\r\n");
			break;
		case DoorNotClosedAlarm:
			DBG_PRINTF("\n\rSend ZigbeeDoorNotCloseAlarmStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeDoorNotCloseAlarmStream, sizeof(ZigbeeDoorNotCloseAlarmStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("DoorNotClose");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find DoorNotClose failed\r\n");
			break;
		case LockDoorAlarm:
			DBG_PRINTF("\n\rSend ZigbeeLockDoorAlarmStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeLockDoorAlarmStream, sizeof(ZigbeeLockDoorAlarmStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("LockDoorAlarm");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find LockDoorAlarm failed\r\n");
			break;
		case UnlockDoorAlarm:
			DBG_PRINTF("\n\rSend ZigbeeUnlockDoorAlarmStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeUnlockDoorAlarmStream, sizeof(ZigbeeUnlockDoorAlarmStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("UnlockDoorAlarm");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find UnlockDoorAlarm failed\r\n");
			break;
		case SmartLockTimeSynRequest:
			DBG_PRINTF("\n\rSend ZigbeeGetTimeFromServerStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeGetTimeFromServerStream, sizeof(ZigbeeGetTimeFromServerStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("GetTimeFromServer");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find GetTimeFromServer failed\r\n");
			break;
		case WkupZigbeeFor1Min:
			DBG_PRINTF("\n\rSend ZigbeeWkupFor1MinStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeWkupFor1MinStream, sizeof(ZigbeeWkupFor1MinStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("WkupZigbeeFor1Min");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find WkupZigbeeFor1Min failed\r\n");
			break;
		case RemotePasswordOpenDoorResult:
			switch(ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.eRemoteOpenErrorCode)
			{
				case PasswordRight:
					ZigbeeRemoteOpenDoorResultStream[8] = 0x00;
					DBG_PRINTF("\n\rSend remote open door result to zigbee: password right\r\n");
					break;
				case PasswordError:
					ZigbeeRemoteOpenDoorResultStream[8] = 0x01;
					DBG_PRINTF("\n\rSend remote open door result to zigbee: password error\r\n");
					break;
				case NoPermissToOpenDoor:
					ZigbeeRemoteOpenDoorResultStream[8] = 0x02;
					DBG_PRINTF("\n\rSend remote open door result to zigbee: no permission\r\n");
					break;
				case IsFreeze:
					ZigbeeRemoteOpenDoorResultStream[8] = 0x04;
					ZigbeeSmartLockModeChangedStream[10] = 0x05;
					DBG_PRINTF("\n\rSend remote open door result to zigbee: is freeze\r\n");
					goto OtherResultHandle;
				case InvalidTime:
					ZigbeeRemoteOpenDoorResultStream[8] = 0x05;
					ZigbeeSmartLockModeChangedStream[10] = 0x04;
					DBG_PRINTF("\n\rSend remote open door result to zigbee: invaild time\r\n");
					goto OtherResultHandle;
				default:
					DBG_PRINTF("\n\rSend remote open door result to zigbee: error code = 0X%02X\r\n", ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.eRemoteOpenErrorCode);
					break;
			}
			ZigbeeRemoteOpenDoorResultStream[9] = (u8)(ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.UserId & 0x00FF);
			ZigbeeRemoteOpenDoorResultStream[10] = (u8)((ptEventInfo->uEventData.tRemoteOpenDoorResultInfo.UserId >> 8)& 0x00FF);
			DBG_PRINTF("\n\rSend ZigbeeRemoteOpenDoorResultStream to zigbee\r\n");
			XorCalcTmp = XorCalcForZigbee(ZigbeeRemoteOpenDoorResultStream, 0, 18);
			ZigbeeRemoteOpenDoorResultStream[18] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData(ZigbeeRemoteOpenDoorResultStream, 20, TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("RemoteOpenDoorResult");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find RemoteOpenDoorResult failed\r\n");
			break;
OtherResultHandle:
			ZigbeeSmartLockModeChangedStream[8] = 0x14;
			ZigbeeSmartLockModeChangedStream[9] = 0x00;
			XorCalcTmp = XorCalcForZigbee(ZigbeeSmartLockModeChangedStream, 0, 11);
			ZigbeeSmartLockModeChangedStream[11] = XorCalcTmp;
			ZigbeeSmartLockModeChangedStream[12] = 0x55;
			DBG_PRINTF("\n\rZigbee pass through data:\r\n");
			for(int i = 0; i < (u16)13; i++)
				DBG_PRINTF("0x%02X ", ZigbeeSmartLockModeChangedStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("Zigbee")->FnSendData(ZigbeeSmartLockModeChangedStream, (u16)13, TX_TIMEOUT);
		case GetSmartLockTIme:
			ZigbeeGetTimeResultStream[8] = ptEventInfo->uEventData.tBCDTimeFromSmartLock.year;
			ZigbeeGetTimeResultStream[9] = ptEventInfo->uEventData.tBCDTimeFromSmartLock.month;
			ZigbeeGetTimeResultStream[10] = ptEventInfo->uEventData.tBCDTimeFromSmartLock.day;
			ZigbeeGetTimeResultStream[11] = ptEventInfo->uEventData.tBCDTimeFromSmartLock.hour;
			ZigbeeGetTimeResultStream[12] = ptEventInfo->uEventData.tBCDTimeFromSmartLock.minutes;
			ZigbeeGetTimeResultStream[13] = ptEventInfo->uEventData.tBCDTimeFromSmartLock.seconds;
			XorCalcTmp = XorCalcForZigbee(ZigbeeGetTimeResultStream, 0, 14);
			ZigbeeGetTimeResultStream[14] = XorCalcTmp;
			DBG_PRINTF("\n\rSend ZigbeeGetTimeResultStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData(ZigbeeGetTimeResultStream, 16, TX_TIMEOUT);
			break;
		case FingerVeinVerfAlarm:
			DBG_PRINTF("\n\rSend ZigbeeVerfFailedStream(Vein) to zigbee\r\n");
			ZigbeeVerfFailedStream[8] = 0x01;//指静脉替换指纹
			XorCalcTmp = XorCalcForZigbee(ZigbeeVerfFailedStream, 0, 18);
			ZigbeeVerfFailedStream[18] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData(ZigbeeVerfFailedStream, 20, TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("VerfFailedAlarm");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find VerfFailedAlarm failed\r\n");
			break;
		case PasswordVerfAlarm:
			DBG_PRINTF("\n\rSend ZigbeeVerfFailedStream(Password) to zigbee\r\n");
			ZigbeeVerfFailedStream[8] = 0x02;
			XorCalcTmp = XorCalcForZigbee(ZigbeeVerfFailedStream, 0, 18);
			ZigbeeVerfFailedStream[18] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData(ZigbeeVerfFailedStream, 20, TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("VerfFailedAlarm");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find VerfFailedAlarm failed\r\n");
			break;
		case CardVerfAlarm:
			DBG_PRINTF("\n\rSend ZigbeeVerfFailedStream(card) to zigbee\r\n");
			ZigbeeVerfFailedStream[8] = 0x03;
			XorCalcTmp = XorCalcForZigbee(ZigbeeVerfFailedStream, 0, 18);
			ZigbeeVerfFailedStream[18] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData(ZigbeeVerfFailedStream, 20, TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("VerfFailedAlarm");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find VerfFailedAlarm failed\r\n");
			break;
		case DoorBell:
			DBG_PRINTF("\n\rSend ZigbeeDoorBellStream to zigbee\r\n");
			ZigbeeDoorBellStream[9] = ptEventInfo->uEventData.DoorBellTime;
			XorCalcTmp = XorCalcForZigbee(ZigbeeDoorBellStream, 0, 18);
			ZigbeeDoorBellStream[18] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData(ZigbeeDoorBellStream, 20, TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("DoorBell");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find DoorBell failed\r\n");
			break;
		case WkupNetWorkRequest:
			DBG_PRINTF("\n\rWkup zigbee\r\n");
			CommModule("Zigbee")->FnWkupDevice();
			CycleWkupZigbeeCnt.FnSetTimer(&CycleWkupZigbeeCnt, 50);//5s后唤醒zigbee
			break;
		case SmartLockEnterSleep:
			DBG_PRINTF("\n\rSmart Lock enter sleep\r\n");
			CycleWkupZigbeeCnt.FnCloseTimerModule(&CycleWkupZigbeeCnt);
			break;
		case SetDoorAlwaysOpenAdminVerifySuccess:
			DBG_PRINTF("\n\rSet door open admin verify success\r\n");
			break;
		case SetDoorAlwaysOpenAdminPasswordWrong:
			DBG_PRINTF("\n\rSet door open admin password wrong\r\n");
			break;
		case SetDoorAlwaysOpenNoAdminPassword:
			DBG_PRINTF("\n\rSet door open no admin password\r\n");
			break;
		case SetDoorAlwaysOpenSuccess:
			DBG_PRINTF("\n\rSet door always open success\r\n");
			break;
		case SetDoorAlwaysOpenFailed:
			DBG_PRINTF("\n\rSet door always open failed\r\n");
			break;
		/*****************************************************************************
		 *以下为密钥操作结果处理
		 *****************************************************************************/
		case AdminVerifySuccess:
			DBG_PRINTF("\n\rDownload data admin verify success\r\n");
			break;
		case DownloadDataSuccess:
			DBG_PRINTF("\n\rSmartLock download data success\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x00;//密码操作成功
			goto CommonHandleTwo;
		case VerifyTooMuch:
			DBG_PRINTF("\n\rVerify too much\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x01;//认证次数过多
			goto CommonHandleTwo;
		case CanNotFindTheID:
			DBG_PRINTF("\n\rCan not find the id\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x02;//查无此编号
			goto CommonHandleTwo;
		case NoOldPasswordOrCard:
			DBG_PRINTF("\n\rNo old password or card\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x03;//查无旧密码或卡
			goto CommonHandleTwo;
		case NoPermisToModifyAdmin:
			DBG_PRINTF("\n\rNo permission to modify admin\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x04;//无法操作管理员数据
			goto CommonHandleTwo;
		case StartTimeIsBiggerThanEndTime:
			DBG_PRINTF("\n\rStart time bigger than end time\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x05;//生效时间大于结束时间
			goto CommonHandleTwo;
		case UserTypeNotMatch:
			DBG_PRINTF("\n\rUser type not match\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x06;//用户类型不匹配
			goto CommonHandleTwo;
		case MethodTypeNotMatch:
			DBG_PRINTF("\n\rMethod type not match\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x07;//登记类型不匹配
			goto CommonHandleTwo;
		case PasswordRepeat:
			DBG_PRINTF("\n\rPassword repeat\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x08;//注册密码重复
			goto CommonHandleTwo;
		case AdminPasswordWrong:
			DBG_PRINTF("\n\rDownload data admin password wrong\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x09;//管理员密码错误
			goto CommonHandleTwo;
		case NoAdminPassword:
			DBG_PRINTF("\n\rDownload data no admin password\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x0A;//无管理员密码
			goto CommonHandleTwo;
		case NoFingerVein:
			DBG_PRINTF("\n\rNo Vein in smart lock\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x0B;//无管理员密码
			goto CommonHandleTwo;
		case NoPassword:
			DBG_PRINTF("\n\rNo password in smart lock\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x0C;//无管理员密码
			goto CommonHandleTwo;
		case NoICCard:
			DBG_PRINTF("\n\rNo card in smart lock\r\n");
			ZigbeeKeyRequestRespondStream[8] = 0x0D;//无管理员密码
			goto CommonHandleTwo;
CommonHandleTwo:
			ZigbeeKeyRequestRespondStream[9] = 0x00;
			ZigbeeKeyRequestRespondStream[10] = (u8)(ptEventInfo->uEventData.tAddPasswordRespondInfo.PasswordID & 0x00FF);//只取ID低字节返回
			XorCalcTmp = XorCalcForZigbee(ZigbeeKeyRequestRespondStream, 0, 20);
			ZigbeeKeyRequestRespondStream[20] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData(ZigbeeKeyRequestRespondStream, 22, TX_TIMEOUT);
			break;
		case OpenDoorSuccess:
			/*本地开锁上报*/
			DBG_PRINTF("\n\rSmartLock open door info:\r\n");
			DBG_PRINTF("\n\rNo1 user id: %d\r\n", ptEventInfo->uEventData.tOpenDoorInfo.No1UserID);
			DBG_PRINTF("\n\rTime samp: %d\r\n", ptEventInfo->uEventData.tOpenDoorInfo.TimeSamp);
			DBG_PRINTF("\n\rPower percent: %d\r\n", ptEventInfo->uEventData.tOpenDoorInfo.ePowerPercent);
			DBG_PRINTF("\n\rOpen door method: %d\r\n", ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod);
			ZigbeeOpenDoorInfoStream[8] = (u8)(ptEventInfo->uEventData.tOpenDoorInfo.No1UserID & 0x00FF);
			ZigbeeOpenDoorInfoStream[9] = (u8)((ptEventInfo->uEventData.tOpenDoorInfo.No1UserID & 0xFF00) >> 8);
			ZigbeeOpenDoorInfoStream[10] = ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod;
			if(Duress == ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod)
				ZigbeeOpenDoorInfoStream[13] = 0x80;//胁迫开门
			else if(KeyOpenDoor == ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod)
				ZigbeeOpenDoorInfoStream[13] = 0x24;//钥匙开门
			else if(Vein == ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod)
				ZigbeeOpenDoorInfoStream[13] = 0x20;//钥匙开门
			else if(Face == ptEventInfo->uEventData.tOpenDoorInfo.eOpenDoorMethod)
				ZigbeeOpenDoorInfoStream[13] = 0x04;//人脸开锁
			else
				ZigbeeOpenDoorInfoStream[13] = 0x00;//默认普通开门
			ZigbeeOpenDoorInfoStream[11] = ptEventInfo->uEventData.tOpenDoorInfo.ePowerPercent;
			ZigbeeOpenDoorInfoStream[12] = ptEventInfo->uEventData.tOpenDoorInfo.OpenDuration;
			ZigbeeOpenDoorInfoStream[14] = (u8)(ptEventInfo->uEventData.tOpenDoorInfo.TimeSamp & 0x000000FF);
			ZigbeeOpenDoorInfoStream[15] = (u8)((ptEventInfo->uEventData.tOpenDoorInfo.TimeSamp & 0x0000FF00) >> 8);
			ZigbeeOpenDoorInfoStream[16] = (u8)((ptEventInfo->uEventData.tOpenDoorInfo.TimeSamp & 0x00FF0000) >> 16);
			ZigbeeOpenDoorInfoStream[17] = (u8)((ptEventInfo->uEventData.tOpenDoorInfo.TimeSamp & 0xFF000000) >> 24);
			XorCalcTmp = XorCalcForZigbee(ZigbeeOpenDoorInfoStream, 0, 18);
			ZigbeeOpenDoorInfoStream[18] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData(ZigbeeOpenDoorInfoStream, 20, TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("OpenDoorInfo");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find OpenDoorInfo failed\r\n");
			break;
		case SmartLockPowerOn:
			DBG_PRINTF("\n\rSmartLock power on\r\n");
			break;
		case KeyOpenDoor:
			DBG_PRINTF("\n\rKey open door\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeKeyOpenDoorStream, 20, TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("KeyOpenDoor");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find KeyOpenDoor failed\r\n");
			break;
		case SmartLockUserDataUpload:
			DBG_PRINTF("\n\rStart upload, some data will be handle next version......\r\n");
			DBG_PRINTF("\n\rUserid: %d\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.UserId);
			DBG_PRINTF("\n\rAction type: 0x%02X\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.eActionType);
			DBG_PRINTF("\n\rUser type: 0x%02X\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.eUserType);
			DBG_PRINTF("\n\rRegisterd type: 0x%02X\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType);
			DBG_PRINTF("\n\rStart BCD time is:\r\n");
			DBG_PRINTF("\n\rYear: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.year);
			DBG_PRINTF("\n\rMonth: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.month);
			DBG_PRINTF("\n\rDay: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.day);
			DBG_PRINTF("\n\rHour: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.hour);
			DBG_PRINTF("\n\rMinute: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.minutes);
			DBG_PRINTF("\n\rSeconds: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.seconds);
			DBG_PRINTF("\n\rEnd BCD time is:\r\n");
			DBG_PRINTF("\n\rYear: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.year);
			DBG_PRINTF("\n\rMonth: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.month);
			DBG_PRINTF("\n\rDay: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.day);
			DBG_PRINTF("\n\rHour: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.hour);
			DBG_PRINTF("\n\rMinute: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.minutes);
			DBG_PRINTF("\n\rSeconds: %x\r\n", ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.seconds);
			switch(ptEventInfo->uEventData.tUserDataUploadInfo.eActionType)
			{
				case AddUser:
					ZigbeeUserDataChangedStream[8] = 0x01;
					break;
				case DelUser:
					ZigbeeUserDataChangedStream[8] = 0x02;
					break;
				case CleanUserExceptAdmin:
					ZigbeeUserDataChangedStream[8] = 0x03;
					break;
				case CleanAllData:
					ZigbeeUserDataChangedStream[8] = 0x04;
					break;
			}
			switch(ptEventInfo->uEventData.tUserDataUploadInfo.eRegisterType)
			{
				case Fingerprint:
					ZigbeeUserDataChangedStream[9] = 0x01;
					break;
				case Password:
					ZigbeeUserDataChangedStream[9] = 0x02;
					break;
				case Card:
					ZigbeeUserDataChangedStream[9] = 0x03;
					break;
				case Vein:
					ZigbeeUserDataChangedStream[9] = 0x04;
					break;
				case Face:
					ZigbeeUserDataChangedStream[9] = 0x05;
					break;
				case TemporaryPassword:
					ZigbeeUserDataChangedStream[9] = 0x06;
					break;
			}
			switch(ptEventInfo->uEventData.tUserDataUploadInfo.eUserType)
			{
				case Admin:
					ZigbeeUserDataChangedStream[10] = 0x03;
					break;
				case GenernalUser:
					ZigbeeUserDataChangedStream[10] = 0x01;
					break;
				case DuressUser:
					ZigbeeUserDataChangedStream[10] = 0x04;
					break;
				case Guest:
					ZigbeeUserDataChangedStream[10] = 0x02;
					break;
				case Nurse:
					ZigbeeUserDataChangedStream[10] = 0x05;
					break;
				case TemporaryUser:
					ZigbeeUserDataChangedStream[10] = 0x06;
					break;
			}
			ZigbeeUserDataChangedStream[11] = (u8)(ptEventInfo->uEventData.tUserDataUploadInfo.UserId & 0x00FF);
			ZigbeeUserDataChangedStream[12] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.year;
			ZigbeeUserDataChangedStream[13] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.month;
			ZigbeeUserDataChangedStream[14] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.day;
			ZigbeeUserDataChangedStream[15] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.hour;
			ZigbeeUserDataChangedStream[16] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.minutes;
			ZigbeeUserDataChangedStream[17] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDStartTime.seconds;
			ZigbeeUserDataChangedStream[18] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.year;
			ZigbeeUserDataChangedStream[19] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.month;
			ZigbeeUserDataChangedStream[20] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.day;
			ZigbeeUserDataChangedStream[21] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.hour;
			ZigbeeUserDataChangedStream[22] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.minutes;
			ZigbeeUserDataChangedStream[23] = ptEventInfo->uEventData.tUserDataUploadInfo.tBCDEndTime.seconds;
			XorCalcTmp = XorCalcForZigbee(ZigbeeUserDataChangedStream, 0, 24);
			ZigbeeUserDataChangedStream[24] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData(ZigbeeUserDataChangedStream, 26, TX_TIMEOUT);
			DBG_PRINTF("\n\rUser info changed data:\r\n");
			for(int i = 0; i < 26; i++)
				DBG_PRINTF("0x%02X ", ZigbeeUserDataChangedStream[i]);
			DBG_PRINTF("\n\r\r\n");
			iRet = OpenMsgSendMonitor("UserDataChanged");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find UserDataChanged failed\r\n");
			SingleWkupZigbeeCnt.FnSetTimer(&SingleWkupZigbeeCnt, 50);//5s后触发
			break;
		/*****************************************************************************
		 *以下为自定义透传数据，9s重发
		 *****************************************************************************/
		case SetDoorAlwaysOpen:
			DBG_PRINTF("\n\rSend ZigbeeSetDoorAlwaysOpenStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeSetDoorAlwaysOpenStream, sizeof(ZigbeeSetDoorAlwaysOpenStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("SetDoorAlwaysOpen");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find UserDataChanged failed\r\n");
			SingleWkupZigbeeCnt.FnSetTimer(&SingleWkupZigbeeCnt, 50);//5s后触发
			break;
		case CancelDoorAlwaysOpen:
			DBG_PRINTF("\n\rSend ZigbeeCancelDoorAlwaysOpenStream to zigbee\r\n");
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeCancelDoorAlwaysOpenStream, sizeof(ZigbeeCancelDoorAlwaysOpenStream), TX_TIMEOUT);
			iRet = OpenMsgSendMonitor("CancelDoorAlwaysOpen");
			if(iRet)
				DBG_PRINTF("\n\rOpenMsgSendMonitor:Find CancelDoorAlwaysOpen failed\r\n");
			SingleWkupZigbeeCnt.FnSetTimer(&SingleWkupZigbeeCnt, 50);//5s后触发
			break;
		case UnknowEvent:
			DBG_PRINTF("\n\rUnknowEvent\r\n");
			break;
		default:
			DBG_PRINTF("\n\rEvent data error:%d\r\n", ptEventInfo->eEventType);
			break;			
	}
	return iRet;
}


static int GetZigbeeEventInfo(char *buf, int len, PT_EventInfo ptEventInfo)
{
	int iRet = -1;
	u8 	XorCalcTmp = 0;
	u16 UserType = 0;
	u16 LRC = 0;
	u16 SendDataLen = 0;
	u8 	AdminPasswordLen = 0;
	switch (buf[2])
	{
		/*zigbee应答*/
		case 0x40:
			CloseMsgSendMonitor("NetWorkConnect");
			ptEventInfo->eEventType = ZigbeeConnectCmdExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x41:
			CloseMsgSendMonitor("NetWorkDisconnect");
			ptEventInfo->eEventType = ZigbeeDisConCmdExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x22:
			CloseMsgSendMonitor("DemolitionAlarm");
			ptEventInfo->eEventType = ZigbeeDemolitionAlarmExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x30:
			CloseMsgSendMonitor("LowPowerAlarm");
			ptEventInfo->eEventType = ZigbeeLowePowerAlarmExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x24:
			CloseMsgSendMonitor("DoorNotClose");
			ptEventInfo->eEventType = ZigbeeDoorNotCloseAlarmExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x54:
			CloseMsgSendMonitor("RemoteOpenDoorResult");
			ptEventInfo->eEventType = ZigbeeRemoteOpenDoorResultExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0xC3:
			CloseMsgSendMonitor("LockKeyboard");
			ptEventInfo->eEventType = ZigbeeLockKeyboardAlarmExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0xC4:
			CloseMsgSendMonitor("VerfFailedAlarm");
			ptEventInfo->eEventType = ZigbeeVerfyFailedAlarmExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0xC6:
			CloseMsgSendMonitor("SmartLockModeChanged");
			ptEventInfo->eEventType = SmartLockModeChangedExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x2A:
			CloseMsgSendMonitor("DoorBell");
			ptEventInfo->eEventType = ZigbeeDoorBellExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x80:
			CloseMsgSendMonitor("OpenDoorInfo");
			ptEventInfo->eEventType = ZigbeeOpenDoorInfoRecvSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0xC9:
			CloseMsgSendMonitor("LockDoorAlarm");
			ptEventInfo->eEventType = ZigbeeLockDoorAlarmExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0xCA:
			CloseMsgSendMonitor("UnlockDoorAlarm");
			ptEventInfo->eEventType = ZigbeeUnlockDoorAlarmExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0xCC:
			CloseMsgSendMonitor("KeyOpenDoor");
			ptEventInfo->eEventType = ZigbeeKeyOpenDoorRecvSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0xC5:
			/*先应答一条无效，阻止zigbee重发*/
			ZigbeeBadDataRespndStream[2] = 0xC5;
			memcpy(ZigbeeBadDataRespndStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeBadDataRespndStream, 0, 9);
			ZigbeeBadDataRespndStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeBadDataRespndStream, sizeof(ZigbeeBadDataRespndStream), TX_TIMEOUT);
			if(0xD0 == buf[8] && 0xD0 == buf[9])
			{
				CloseMsgSendMonitor("UserDataChanged");
				ptEventInfo->eEventType = ZigbeeUserDataUploadExecSuccess;
				ptEventInfo->cHasEventData = 0;
				iRet = 0;
			}
			break;
		case 0x77:
			CloseMsgSendMonitor("WkupZigbeeFor1Min");
			ptEventInfo->eEventType = ZigbeeWkupFor1MinRecvSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0x85:
			CloseMsgSendMonitor("GetTimeFromServer");
			ptEventInfo->eEventType = ZigbeeTimeSynCmdExecSuccess;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		/*zigbee命令*/
		case 0x83://入网退网状态提示
			memcpy(ZigbeeConnectResultRespondStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeConnectResultRespondStream, 0, 9);
			ZigbeeConnectResultRespondStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeConnectResultRespondStream, sizeof(ZigbeeConnectResultRespondStream), TX_TIMEOUT);
			if(buf[8] == 0x00 && buf[9] == 0x00)
			{
				ptEventInfo->eEventType = NetworkConSuccess;
				ptEventInfo->cHasEventData = 0;
				iRet = 0;
			}
			else if(buf[8] == 0x00 && buf[9] == 0x01)
			{
				ptEventInfo->eEventType = NetWorkConFailed;
				ptEventInfo->cHasEventData = 0;
				iRet = 0;
			}
			else if(buf[8] == 0x01 && buf[9] == 0x00)
			{
				ptEventInfo->eEventType = NetWorkDisconSuccess;
				ptEventInfo->cHasEventData = 0;
				iRet = 0;
			}
			else if(buf[8] == 0x01 && buf[9] == 0x01)
			{
				ptEventInfo->eEventType = NetWorkDisconFailed;
				ptEventInfo->cHasEventData = 0;
				iRet = 0;
			}
			else
			{
				DBG_PRINTF("\n\rZigbee protocol:0x83 bad data\r\n");
				iRet = -1;
			}
			break;
		case 0x84://网络状态显示
			memcpy(ZigbeeNetworkStatusRespondStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeNetworkStatusRespondStream, 0, 9);
			ZigbeeNetworkStatusRespondStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeNetworkStatusRespondStream, sizeof(ZigbeeNetworkStatusRespondStream), TX_TIMEOUT);
			if(0x00 == buf[8])
			{
				ptEventInfo->eEventType = NetWorkOnline;
				ptEventInfo->cHasEventData = 0;
				iRet = 0;
			}
			else if(0x01 == buf[8])
			{
				ptEventInfo->eEventType = NetWorkOffline;
				ptEventInfo->cHasEventData = 0;
				iRet = 0;
			}
			else
			{
				DBG_PRINTF("\n\rZigbee protocol:0x84 bad data\r\n");
				iRet = -1;
			}
			break;
		case 0x86://门锁常开设置
			memcpy(ZigbeeDoorAlwaysOpenReauestStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeDoorAlwaysOpenReauestStream, 0, 9);
			ZigbeeDoorAlwaysOpenReauestStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeDoorAlwaysOpenReauestStream, sizeof(ZigbeeDoorAlwaysOpenReauestStream), TX_TIMEOUT);
			ptEventInfo->uEventData.DoorAlwaysOpenTime = ((u32)buf[11]<<24) + ((u32)buf[10]<<16) + ((u32)buf[9]<<8) + ((u32)buf[8]);
			ptEventInfo->eEventType = DoorAlwaysOpenRequest;
			ptEventInfo->cHasEventData = 1;
			iRet = 0;
			break;
		case 0x60://远程密码下发
			memcpy(ZigbeeRemotePasswordRespondStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeRemotePasswordRespondStream, 0, 9);
			ZigbeeRemotePasswordRespondStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeRemotePasswordRespondStream, sizeof(ZigbeeRemotePasswordRespondStream), TX_TIMEOUT);
			ptEventInfo->eEventType = PasswordFromServer;
			for(int i = 0; i < 10; i++)
				ptEventInfo->uEventData.RemotePasswordBuf[i] = buf[i + 8];
			ptEventInfo->cHasEventData = 1;
			iRet = 0;
			break;
		case 0x62://时间同步
			memcpy(ZigbeeTimeSynRespondStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeTimeSynRespondStream, 0, 9);
			ZigbeeTimeSynRespondStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeTimeSynRespondStream, sizeof(ZigbeeTimeSynRespondStream), TX_TIMEOUT);
			ptEventInfo->eEventType = NetWorkTimeSynRequest;
			ptEventInfo->uEventData.tUTCTimeFromNetWork.year = ((u16)buf[9]<<8) + ((u16)buf[8]);
			ptEventInfo->uEventData.tUTCTimeFromNetWork.month = buf[10];
			ptEventInfo->uEventData.tUTCTimeFromNetWork.day = buf[11];
			ptEventInfo->uEventData.tUTCTimeFromNetWork.hour = buf[12];
			ptEventInfo->uEventData.tUTCTimeFromNetWork.minutes = buf[13];
			ptEventInfo->uEventData.tUTCTimeFromNetWork.seconds = buf[14];
			ptEventInfo->cHasEventData = 1;
			iRet = 0;
			break;
		case 0x73://密钥操作
			/*1，只复制命令自增，应答根据门锁密钥操作结果放到SmartLockEventPro处理
			 *2，先应答一条全FF的，用来停止zigbee重发
			 */
			memcpy(ZigbeeKeyRequestRespondStream+3, buf+3, 4);//cp命令自增ID
			ZigbeeKeyRequestRespondStream[8] = 0xFF;//固定FF
			ZigbeeKeyRequestRespondStream[9] = 0xFF;//固定FF
			ZigbeeKeyRequestRespondStream[10] = 0xFF;//固定FF
			XorCalcTmp = XorCalcForZigbee(ZigbeeKeyRequestRespondStream, 0, 20);
			ZigbeeKeyRequestRespondStream[20] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData(ZigbeeKeyRequestRespondStream, 22, TX_TIMEOUT);
			ptEventInfo->eEventType = NetWorkKeyRequest;
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID = ((u16)buf[9]<<8) + ((u16)buf[8]);
			UserType = ((u16)buf[11]<<8) + ((u16)buf[10]);
			switch(UserType)
			{
				case 0x01:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType = GenernalUser;
					break;
				case 0x02:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType = Guest;
					break;
				case 0x03:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType = Admin;
					break;
				case 0x04:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType = DuressUser;
					break;
				case 0x05:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType = Nurse;
					break;
				case 0x06:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType = TemporaryUser;
					break;
			}
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.year = buf[12];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.month = buf[13];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.day = buf[14];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.hour = buf[15];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.minutes = buf[16];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.seconds = buf[17];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.year = buf[18];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.month = buf[19];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.day = buf[20];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.hour = buf[21];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.minutes = buf[22];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.seconds = buf[23];			
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordEffectiveNumOfTime = buf[24];
			switch (buf[25])
			{
				case 0x01:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = AddPassword;
					break;
				case 0x02:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = ChangePassword;
					break;
				case 0x03:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = DelPassword;
					break;
				case 0x04:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = FreezePassword;
					break;
				case 0x05:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = UnfreezePassword;
					break;
				case 0x06:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = DelFingerPrint;
					break;
				case 0x07:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = DelIcCard;
					break;
				case 0x08:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = AddICCard;
					break;
				case 0x09:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = ChangeICCard;
					break;
				case 0x0A:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = FreezeICCard;
					break;
				case 0x0B:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = UnfreezeICCard;
					break;
				case 0x0C:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = AddFingerVein;
					break;
				case 0x0D:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = ChangeFingerVein;
					break;
				case 0x0E:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = DelFingerVein;
					break;
				case 0x0F:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = FreezeFingerVein;
					break;
				case 0x10:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = UnfreezeFingerVein;
					break;
				case 0x11:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = AddFingerPrint;
					break;
				case 0x12:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = ChangeFingerPrint;
					break;
				case 0x13:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = FreezeFingerPrint;
					break;
				case 0x14:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = UnfreezeFingerPrint;
					break;
				case 0x15:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = CleanAllFingerVeinData;
					break;
				case 0x16:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = CleanAllPasswordData;
					break;
				case 0x17:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = CleanAllCardData;
					break;
				case 0x18:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = CleanAllFingerPrintData;
					break;
				case 0x19:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = CleanAllUserData;
					break;
				default:
					ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction = UnknowAction;
					break;
			}
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPasswordLen = buf[26];
			memcpy(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPassword, buf+27, buf[26]);
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.NewPasswordLen = buf[(buf[26]+27)];
			memcpy(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.NewPassword, buf+(buf[26]+28), buf[(buf[26]+27)]);
			ptEventInfo->uEventData.tNetWorkKeyRequestInfo.AdminPasswordLen = buf[buf[26]+buf[(buf[26]+27)]+28];
			memcpy(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.AdminPassword, buf+(buf[26]+buf[(buf[26]+27)]+29), buf[buf[26]+buf[(buf[26]+27)]+28]);			
			ptEventInfo->cHasEventData = 1;
			iRet = 0;
			break;
		case 0xC7://时间查询
			/*先应答一条无效，阻止zigbee重发*/
			ZigbeeBadDataRespndStream[2] = 0xC7;
			memcpy(ZigbeeBadDataRespndStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeBadDataRespndStream, 0, 9);
			ZigbeeBadDataRespndStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeBadDataRespndStream, sizeof(ZigbeeBadDataRespndStream), TX_TIMEOUT);
			/*再通知门锁*/
			memcpy(ZigbeeGetTimeResultStream+3, buf+3, 4);
			ptEventInfo->eEventType = GetTimeRequest;
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		/*******************************************************************************
		 *以下是透传数据处理处理逻辑
		 *对于E1、E2有多包数据交互的指令，在门锁上传数据后，开启5s后唤醒zigbee功能,
		 *启动5s后唤醒zigbee的方法在GetSmartLockEventInfo执行
		 *对于门锁主动上报的透传数据，服务器应答0xFF
		 *不设置重发
		 *先应答一条无效，阻止重发
		 *******************************************************************************/
		case 0xE1:
			SmartLockPassThroughStream[1] = 0xE1;		//历史开锁记录查询
			ZigbeeBadDataRespndStream[2] = 0xE1;
			goto ZigbeePassThroughHandle;
		case 0xE2:
			SmartLockPassThroughStream[1] = 0xE2;		//请求用户信息
			ZigbeeBadDataRespndStream[2] = 0xE2;
			goto ZigbeePassThroughHandle;
		case 0xE5:
			SmartLockPassThroughStream[1] = 0xE5;		//获取设备版本号
			ZigbeeBadDataRespndStream[2] = 0xE5;
			goto ZigbeePassThroughHandle;
		case 0xE6:
			SmartLockPassThroughStream[1] = 0xE6;		//获取电池电量
			ZigbeeBadDataRespndStream[2] = 0xE6;
			goto ZigbeePassThroughHandle;
		case 0xEC:
			SmartLockPassThroughStream[1] = 0xEC;		//门锁同步服务器时间
			ZigbeeBadDataRespndStream[2] = 0xEC;
			goto ZigbeePassThroughHandle;
		case 0xEE:
			SmartLockPassThroughStream[1] = 0xEE;		//音量设置
			ZigbeeBadDataRespndStream[2] = 0xEE;
			goto ZigbeePassThroughHandle;			
ZigbeePassThroughHandle:
			/*先应答一条无效，阻止zigbee重发*/
			memcpy(ZigbeeBadDataRespndStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeBadDataRespndStream, 0, 9);
			ZigbeeBadDataRespndStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeBadDataRespndStream, sizeof(ZigbeeBadDataRespndStream), TX_TIMEOUT);
			/*然后再透传数据给门锁*/
			memcpy(ZigbeePassThroughStream+3, buf+3, 4);//保存命令自增ID 
			SmartLockPassThroughStream[0] = 0xF5;
			SmartLockPassThroughStream[2] = 0x00;
			SmartLockPassThroughStream[3] = buf[1];
			for(int i = 0; i < buf[1]; i++)
				SmartLockPassThroughStream[i + 4] = buf[8 + i];
			LRC = 0;
			for(int i = 2; i < (buf[1] + 4); i++)
				LRC += SmartLockPassThroughStream[i];
			SmartLockPassThroughStream[buf[1] + 4] = (u8)(LRC >> 8);
			SmartLockPassThroughStream[buf[1] + 5] = (u8)(LRC & 0x00FF);
			SendDataLen = buf[1] + 6;
			DBG_PRINTF("\n\rSmart lock pass through data:\r\n");
			for(int i = 0; i < SendDataLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockPassThroughStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockPassThroughStream, SendDataLen, TX_TIMEOUT);
			break;
		case 0xED:
			/*先应答一条无效，阻止zigbee重发*/
			ZigbeeBadDataRespndStream[2] = 0xED;
			memcpy(ZigbeeBadDataRespndStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeBadDataRespndStream, 0, 9);
			ZigbeeBadDataRespndStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeBadDataRespndStream, sizeof(ZigbeeBadDataRespndStream), TX_TIMEOUT);
			/*判断是否是服务器应答*/
			if(0xD0 == buf[8] && 0xD0 == buf[9])
			{
				CloseMsgSendMonitor("RespondDoorStatus");
				break;
			}
			/***********************************************************************
			 *发送0xED之前，先发送0x2A
			 ***********************************************************************/
			SmartLockAdminVerifyForSetDoorOpenStream[3] = buf[9];//管理员密码长度
			AdminPasswordLen = buf[9];
			memcpy(SmartLockAdminVerifyForSetDoorOpenStream+4, buf+10, AdminPasswordLen);
			LRC = 0;
			for(int i = 2; i < AdminPasswordLen+5; i++)
				LRC += SmartLockAdminVerifyForSetDoorOpenStream[i];
			SmartLockAdminVerifyForSetDoorOpenStream[AdminPasswordLen+5] = (u8)((LRC >> 8) & 0x00FF);
			SmartLockAdminVerifyForSetDoorOpenStream[AdminPasswordLen+6] = (u8)(LRC & 0x00FF);
			MsgSendMonitorForSmartLockAdminVerifyNo2.MsgLen = AdminPasswordLen + 7;
			DBG_PRINTF("\n\rSmart lock admin verify data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockAdminVerifyNo2.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockAdminVerifyForSetDoorOpenStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockAdminVerifyForSetDoorOpenStream, MsgSendMonitorForSmartLockAdminVerifyNo2.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("AdminVerifyNo2");
#ifdef USE_IWDG
			/*喂狗*/
			IWDG_Feed();
#endif
			delay_ms(1500);
			memcpy(ZigbeeSetDoorOpenPassThroughStream+3, buf+3, 4);//保存命令自增ID 
			SmartLockPassThroughStream[0] = 0xF5;
			SmartLockPassThroughStream[1] = 0xED;	//门锁常开设置
			SmartLockPassThroughStream[2] = 0x00;
			SmartLockPassThroughStream[3] = 0x01;	//固定一个字节
			SmartLockPassThroughStream[4] = buf[8];	//常开设置数据
			LRC = 0;
			for(int i = 2; i < 5; i++)
				LRC += SmartLockPassThroughStream[i];
			SmartLockPassThroughStream[5] = (u8)(LRC >> 8);
			SmartLockPassThroughStream[6] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rSmart lock pass through data:\r\n");
			for(int i = 0; i < 7; i++)
				DBG_PRINTF("0x%02X ", SmartLockPassThroughStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockPassThroughStream, 7, TX_TIMEOUT);
			break;
		/*由服务器应答，服务器接收成功返回D0 D0*/
		case 0xE3:
			/*先应答一条无效，阻止zigbee重发*/
			ZigbeeBadDataRespndStream[2] = 0xE3;
			memcpy(ZigbeeBadDataRespndStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeBadDataRespndStream, 0, 9);
			ZigbeeBadDataRespndStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeBadDataRespndStream, sizeof(ZigbeeBadDataRespndStream), TX_TIMEOUT);
			if(0xD0 == buf[8] && 0xD0 == buf[9])
			{
				CloseMsgSendMonitor("SetDoorAlwaysOpen");
				ptEventInfo->eEventType = ZigbeeSetDoorAlwaysOpenExecSuccess;
				ptEventInfo->cHasEventData = 0;
				iRet = 0;
			}
			break;
		case 0xE4:
			/*先应答一条无效，阻止zigbee重发*/
			ZigbeeBadDataRespndStream[2] = 0xE4;
			memcpy(ZigbeeBadDataRespndStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeBadDataRespndStream, 0, 9);
			ZigbeeBadDataRespndStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeBadDataRespndStream, sizeof(ZigbeeBadDataRespndStream), TX_TIMEOUT);
			if(0xD0 == buf[8] && 0xD0 == buf[9])
			{
				CloseMsgSendMonitor("CancelDoorAlwaysOpen");
				ptEventInfo->eEventType = ZigbeeCancelDoorAlwaysOpenExecSuccess;
				ptEventInfo->cHasEventData = 0;
				iRet = 0;	
			}
			break;
		/*与服务器建立重发，服务器接收成功返回FF*/
		case 0xE7:
			ptEventInfo->eEventType = ZigbeeUploadPasswordSuccess;
			CloseMsgSendMonitor("ZigbeePassThrough");
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0xE8:
			ptEventInfo->eEventType = ZigbeeUploadCardSuccess;
			CloseMsgSendMonitor("ZigbeePassThrough");
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		case 0xE9:
			ptEventInfo->eEventType = ZigbeeUploadFingerVeinSuccess;
			CloseMsgSendMonitor("ZigbeePassThrough");
			ptEventInfo->cHasEventData = 0;
			iRet = 0;
			break;
		/*******************************************************************************
		 *其他非协议数据直接应答，有效数据0xFF
		 *******************************************************************************/
		case 0x75:
			DBG_PRINTF("\n\rCmd:0x75, The data will not be handle......\r\n");
			iRet = -1;
			break;
		default:
			DBG_PRINTF("\n\rError Cmd:0x%X, The data will not be handle......\r\n", buf[2]);
			ZigbeeBadDataRespndStream[2] = buf[2];
			memcpy(ZigbeeBadDataRespndStream+3, buf+3, 4);
			XorCalcTmp = XorCalcForZigbee(ZigbeeBadDataRespndStream, 0, 9);
			ZigbeeBadDataRespndStream[9] = XorCalcTmp;
			CommModule("Zigbee")->FnSendData((u8*)ZigbeeBadDataRespndStream, sizeof(ZigbeeBadDataRespndStream), TX_TIMEOUT);
			iRet = -1;
			break;
	}
	
	return iRet;
}


static int ZigbeeEventPro(PT_EventInfo ptEventInfo)
{
	int iRet = 0;
	u16 LRC = 0;
	u8 OldPasswordLen = 0;
	u8 AdminPasswordLen = 0;
	T_BCDTime tBCDTime;
	T_BCDTime tBCDStartTime;
	T_BCDTime tBCDEndTime;
	u8 PasswordTmp[15] = { 0x00 };
	switch (ptEventInfo->eEventType)
	{
		case ZigbeeConnectCmdExecSuccess:
			DBG_PRINTF("\n\rZigbee connect cmd exec success\r\n");
			break;
		case ZigbeeDisConCmdExecSuccess:
			DBG_PRINTF("\n\rZigbee disconnect cmd exec success\r\n");
			break;
		case ZigbeeDemolitionAlarmExecSuccess:
			DBG_PRINTF("\n\rZigbee DemolitionAlarm cmd exec success\r\n");
			break;
		case ZigbeeLowePowerAlarmExecSuccess:
			DBG_PRINTF("\n\rZigbee LowePowerAlarm cmd exec success\r\n");
			break;
		case ZigbeeLockKeyboardAlarmExecSuccess:
			DBG_PRINTF("\n\rZigbee LockKeyboardAlarm cmd exec success\r\n");
			break;
		case ZigbeeDoorNotCloseAlarmExecSuccess:
			DBG_PRINTF("\n\rZigbee DoorNotCloseAlarm cmd exec success\r\n");
			break;
		case ZigbeeVerfyFailedAlarmExecSuccess:
			DBG_PRINTF("\n\rZigbee VerfyFailedAlarm cmd exec success\r\n");
			break;
		case ZigbeeDoorBellExecSuccess:
			DBG_PRINTF("\n\rZigbee DoorBell cmd exec success\r\n");
			break;
		case SmartLockModeChangedExecSuccess:
			DBG_PRINTF("\n\rZigbee SmartLockModeChanged cmd exec success\r\n");
			break;
		case ZigbeeLockDoorAlarmExecSuccess:
			DBG_PRINTF("\n\rZigbee LockDoorAlarm cmd exec success\r\n");
			break;
		case ZigbeeUnlockDoorAlarmExecSuccess:
			DBG_PRINTF("\n\rZigbee UnlockDoorAlarm cmd exec success\r\n");
			break;
		case ZigbeeWkupFor1MinRecvSuccess:
			DBG_PRINTF("\n\rZigbee ZigbeeWkupFor1MinRecvSuccess cmd exec success\r\n");
			break;
		case ZigbeeUserDataUploadExecSuccess:
			DBG_PRINTF("\n\rZigbee ZigbeeUserDataUploadExecSuccess cmd exec success\r\n");
			break;
		case ZigbeeRemoteOpenDoorResultExecSuccess:
			DBG_PRINTF("\n\rZigbee ZigbeeRemoteOpenDoorResultExecSuccess cmd exec success\r\n");
			break;
		case ZigbeeTimeSynCmdExecSuccess:
			DBG_PRINTF("\n\rZigbee ZigbeeTimeSynCmdExecSuccess cmd exec success\r\n");
			break;
		case ZigbeeSetDoorAlwaysOpenExecSuccess:
			DBG_PRINTF("\n\rZigbee ZigbeeSetDoorAlwaysOpen cmd exec success\r\n");
			break;
		case ZigbeeCancelDoorAlwaysOpenExecSuccess:
			DBG_PRINTF("\n\rZigbee ZigbeeCancelDoorAlwaysOpen cmd exec success\r\n");
			break;
		case ZigbeeUploadPasswordSuccess:
			DBG_PRINTF("\n\rZigbee Zigbee upload password success\r\n");
			break;
		case ZigbeeUploadCardSuccess:
			DBG_PRINTF("\n\rZigbee Zigbee upload card success\r\n");
			break;
		case ZigbeeUploadFingerVeinSuccess:
			DBG_PRINTF("\n\rZigbee Zigbee upload Vein success\r\n");
			break;
		case ZigbeeUploadTimeStampSuccess:
			DBG_PRINTF("\n\rZigbee Zigbee upload timestamp success\r\n");
			break;
		case NetworkConSuccess:
			DBG_PRINTF("\n\rNetwork connect success\r\n");
			SmartLockNetworkConStatusStream[4] = 0x00;
			SmartLockNetworkConStatusStream[5] = 0x00;
			LRC = 0;
			for(int i = 2; i < 6; i++)
				LRC += SmartLockNetworkConStatusStream[i];
			SmartLockNetworkConStatusStream[6] = (u8)(LRC >> 8);
			SmartLockNetworkConStatusStream[7] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rNetwork connect status data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockNetworkConStatus.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockNetworkConStatusStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockNetworkConStatusStream, MsgSendMonitorForSmartLockNetworkConStatus.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("NetworkConStatus");
			break;
		case NetWorkConFailed:
			DBG_PRINTF("\n\rNetwork connect failed\r\n");
			SmartLockNetworkConStatusStream[4] = 0x00;
			SmartLockNetworkConStatusStream[5] = 0x01;
			LRC = 0;
			for(int i = 2; i < 6; i++)
				LRC += SmartLockNetworkConStatusStream[i];
			SmartLockNetworkConStatusStream[6] = (u8)(LRC >> 8);
			SmartLockNetworkConStatusStream[7] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rNetwork connect status data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockNetworkConStatus.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockNetworkConStatusStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockNetworkConStatusStream, MsgSendMonitorForSmartLockNetworkConStatus.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("NetworkConStatus");
			break;
		case NetWorkDisconSuccess:
			DBG_PRINTF("\n\rNetwork disconnect success\r\n");
			SmartLockNetworkConStatusStream[4] = 0x01;
			SmartLockNetworkConStatusStream[5] = 0x00;
			LRC = 0;
			for(int i = 2; i < 6; i++)
				LRC += SmartLockNetworkConStatusStream[i];
			SmartLockNetworkConStatusStream[6] = (u8)(LRC >> 8);
			SmartLockNetworkConStatusStream[7] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rNetwork connect status data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockNetworkConStatus.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockNetworkConStatusStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockNetworkConStatusStream, MsgSendMonitorForSmartLockNetworkConStatus.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("NetworkConStatus");
			break;
		case NetWorkDisconFailed:
			DBG_PRINTF("\n\rNetwork disconnect failed\r\n");
			SmartLockNetworkConStatusStream[4] = 0x01;
			SmartLockNetworkConStatusStream[5] = 0x01;
			LRC = 0;
			for(int i = 2; i < 6; i++)
				LRC += SmartLockNetworkConStatusStream[i];
			SmartLockNetworkConStatusStream[6] = (u8)(LRC >> 8);
			SmartLockNetworkConStatusStream[7] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rNetwork connect status data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockNetworkConStatus.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockNetworkConStatusStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockNetworkConStatusStream, MsgSendMonitorForSmartLockNetworkConStatus.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("NetworkConStatus");
			break;
		case NetWorkOnline:
			DBG_PRINTF("\n\rNetwork online\r\n");
			SmartLockNetworkStatusStream[4] = 0x00;
			LRC = 0;
			for(int i = 2; i < 5; i++)
				LRC += SmartLockNetworkStatusStream[i];
			SmartLockNetworkStatusStream[5] = (u8)(LRC >> 8);
			SmartLockNetworkStatusStream[6] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rNetwork status data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockNetworkStatus.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockNetworkStatusStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockNetworkStatusStream, MsgSendMonitorForSmartLockNetworkStatus.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("NetworkStatus");
			break;
		case NetWorkOffline:
			DBG_PRINTF("\n\rNetwork offline\r\n");
			SmartLockNetworkStatusStream[4] = 0x01;
			LRC = 0;
			for(int i = 2; i < 5; i++)
				LRC += SmartLockNetworkStatusStream[i];
			SmartLockNetworkStatusStream[5] = (u8)(LRC >> 8);
			SmartLockNetworkStatusStream[6] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rNetwork status data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockNetworkStatus.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockNetworkStatusStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockNetworkStatusStream, MsgSendMonitorForSmartLockNetworkStatus.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("NetworkStatus");
			break;
		case DoorAlwaysOpenRequest:
			DBG_PRINTF("\n\rDoor always open request\r\n");
			SmartLockAlwaysOpenStream[7] = (u8)(ptEventInfo->uEventData.DoorAlwaysOpenTime & 0x000000FF);
			SmartLockAlwaysOpenStream[6] = (u8)((ptEventInfo->uEventData.DoorAlwaysOpenTime & 0x0000FF00) >> 8);
			SmartLockAlwaysOpenStream[5] = (u8)((ptEventInfo->uEventData.DoorAlwaysOpenTime & 0x00FF0000) >> 16);
			SmartLockAlwaysOpenStream[4] = (u8)((ptEventInfo->uEventData.DoorAlwaysOpenTime & 0xFF000000) >> 24);
			LRC = 0;
			for(int i = 2; i < 8; i++)
				LRC += SmartLockAlwaysOpenStream[i];
			SmartLockAlwaysOpenStream[8] = (u8)(LRC >> 8);
			SmartLockAlwaysOpenStream[9] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rAlways open door data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockAlawaysOpen.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockAlwaysOpenStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockAlwaysOpenStream, MsgSendMonitorForSmartLockAlawaysOpen.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("DoorAlawaysOpen");
			break;
		case ZigbeeOpenDoorInfoRecvSuccess:
			DBG_PRINTF("\n\rOpen door info xmit success\r\n");
			break;
		case ZigbeeKeyOpenDoorRecvSuccess:
			DBG_PRINTF("\n\rKey Open Door xmit success\r\n");
			break;
		case PasswordFromServer:
			DBG_PRINTF("\n\rGet password from server\r\n");
			for(int i = 0; i < 10; i++)
				SmartLockRemotePasswordStream[i + 4] = ptEventInfo->uEventData.RemotePasswordBuf[i];
			LRC = 0;
			for(int i = 2; i < 14; i++)
				LRC += SmartLockRemotePasswordStream[i];
			SmartLockRemotePasswordStream[14] = (u8)(LRC >> 8);
			SmartLockRemotePasswordStream[15] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rRemote password data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockRemotePassword.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockRemotePasswordStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockRemotePasswordStream, MsgSendMonitorForSmartLockRemotePassword.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("RemotePassword");
			delay_ms(500);
			break;
		case GetTimeRequest:
			DBG_PRINTF("\n\rSmart lock get time data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockGetTime.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockGetTimeStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData((u8*)SmartLockGetTimeStream, MsgSendMonitorForSmartLockGetTime.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("GetTime");
			break;
		case NetWorkTimeSynRequest:
			DBG_PRINTF("\n\rGet UTC time from network:\r\n");
			DBG_PRINTF("\n\rUTC year: %d\r\n", ptEventInfo->uEventData.tUTCTimeFromNetWork.year);
			DBG_PRINTF("\n\rUTC month: %d\r\n", ptEventInfo->uEventData.tUTCTimeFromNetWork.month);
			DBG_PRINTF("\n\rUTC day: %d\r\n", ptEventInfo->uEventData.tUTCTimeFromNetWork.day);
			DBG_PRINTF("\n\rUTC hour: %d\r\n", ptEventInfo->uEventData.tUTCTimeFromNetWork.hour);
			DBG_PRINTF("\n\rUTC minutes: %d\r\n", ptEventInfo->uEventData.tUTCTimeFromNetWork.minutes);
			DBG_PRINTF("\n\rUTC seconds: %d\r\n", ptEventInfo->uEventData.tUTCTimeFromNetWork.seconds);
			tBCDTime = UTCTimeConvertToBCDTime(ptEventInfo->uEventData.tUTCTimeFromNetWork);
			DBG_PRINTF("\n\rBCD time is:\r\n");
			DBG_PRINTF("\n\rBCD year: %x\r\n", tBCDTime.year);
			DBG_PRINTF("\n\rBCD month: %x\r\n", tBCDTime.month);
			DBG_PRINTF("\n\rBCD day: %x\r\n", tBCDTime.day);
			DBG_PRINTF("\n\rBCD hour: %x\r\n", tBCDTime.hour);
			DBG_PRINTF("\n\rBCD minutes: %x\r\n", tBCDTime.minutes);
			DBG_PRINTF("\n\rBCD seconds: %x\r\n", tBCDTime.seconds);
			SmartLockTimeSynStream[4] = tBCDTime.year;
			SmartLockTimeSynStream[5] = tBCDTime.month;
			SmartLockTimeSynStream[6] = tBCDTime.day;
			SmartLockTimeSynStream[7] = tBCDTime.hour;
			SmartLockTimeSynStream[8] = tBCDTime.minutes;
			SmartLockTimeSynStream[9] = tBCDTime.seconds;
			LRC = 0;
			for(int i = 2; i < 10; i++)
				LRC += SmartLockTimeSynStream[i];
			SmartLockTimeSynStream[10] = (u8)(LRC >> 8);
			SmartLockTimeSynStream[11] = (u8)(LRC & 0x00FF);
			DBG_PRINTF("\n\rTime syn data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockTimeSyn.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockTimeSynStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockTimeSynStream, MsgSendMonitorForSmartLockTimeSyn.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("SmartLockTimeSyn");
			break;
		case NetWorkKeyRequest:
			/*打印数据*/
			DBG_PRINTF("\n\rNetWorkKeyRequest info:\r\n");
			DBG_PRINTF("\n\rPasswordID: %d\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID);
			DBG_PRINTF("\n\rPasswordType: %d\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType);
			DBG_PRINTF("\n\rStart year: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.year);
			DBG_PRINTF("\n\rStart month: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.month);
			DBG_PRINTF("\n\rStart day: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.day);
			DBG_PRINTF("\n\rStart hour: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.hour);
			DBG_PRINTF("\n\rStart minute: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.minutes);
			DBG_PRINTF("\n\rStart seconds: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.seconds);
			DBG_PRINTF("\n\rEnd year: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.year);
			DBG_PRINTF("\n\rEnd month: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.month);
			DBG_PRINTF("\n\rEnd day: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.day);
			DBG_PRINTF("\n\rEnd hour: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.hour);
			DBG_PRINTF("\n\rEnd minute: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.minutes);
			DBG_PRINTF("\n\rEnd seconds: %x\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.seconds);
//			DBG_PRINTF("\n\rNum of time: %d\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordEffectiveNumOfTime);
			DBG_PRINTF("\n\rAction: %d\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction);			
			DBG_PRINTF("\n\rOldPasswordLen: %d\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPasswordLen);
			DBG_PRINTF("\n\rOldPassword:\r\n");
			DBG_PRINTF("\n\r\r\n");
			for(int i = 0; i < ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPasswordLen; i++)
			{
				PasswordTmp[i] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPassword[i] ^ DecryptionKey[i];
				DBG_PRINTF("0x%02X	", PasswordTmp[i]);
			}
			DBG_PRINTF("\n\r\r\n");
			DBG_PRINTF("\n\rNewPasswordLen: %d\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.NewPasswordLen);
			DBG_PRINTF("\n\rNewPassword:\r\n");
			DBG_PRINTF("\n\r\r\n");
			for(int i = 0; i < ptEventInfo->uEventData.tNetWorkKeyRequestInfo.NewPasswordLen; i++)
			{
				PasswordTmp[i] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.NewPassword[i] ^ DecryptionKey[i];
				DBG_PRINTF("0x%02X ", PasswordTmp[i]);
			}
			DBG_PRINTF("\n\r\r\n");
			DBG_PRINTF("\n\rAdminPasswordLen: %d\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.AdminPasswordLen);
			DBG_PRINTF("\n\rAdminPassword:\r\n");
			DBG_PRINTF("\n\r\r\n");
			for(int i = 0; i < ptEventInfo->uEventData.tNetWorkKeyRequestInfo.AdminPasswordLen; i++)
			{
				PasswordTmp[i] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.AdminPassword[i] ^ DecryptionKey[i];
				DBG_PRINTF("0x%02X ", PasswordTmp[i]);
			}
			DBG_PRINTF("\n\r\r\n");
			/***********************************************************************
			 *发送0x10之前，先发送0x29
			 ***********************************************************************/
			SmartLockAdminVerifyForDownloadDataStream[3] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.AdminPasswordLen;
			AdminPasswordLen = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.AdminPasswordLen;
			memcpy(SmartLockAdminVerifyForDownloadDataStream+4, ptEventInfo->uEventData.tNetWorkKeyRequestInfo.AdminPassword, AdminPasswordLen);
			LRC = 0;
			for(int i = 2; i < AdminPasswordLen+4; i++)
				LRC += SmartLockAdminVerifyForDownloadDataStream[i];
			SmartLockAdminVerifyForDownloadDataStream[AdminPasswordLen+5] = (u8)((LRC >> 8) & 0x00FF);
			SmartLockAdminVerifyForDownloadDataStream[AdminPasswordLen+6] = (u8)(LRC & 0x00FF);
			MsgSendMonitorForSmartLockAdminVerifyNo1.MsgLen = AdminPasswordLen + 6;
			DBG_PRINTF("\n\rSmart lock admin verify data:\r\n");
			for(int i = 0; i < MsgSendMonitorForSmartLockAdminVerifyNo1.MsgLen; i++)
				DBG_PRINTF("0x%02X ", SmartLockAdminVerifyForDownloadDataStream[i]);
			DBG_PRINTF("\n\r\r\n");
			CommModule("SmartLock")->FnSendData(SmartLockAdminVerifyForDownloadDataStream, MsgSendMonitorForSmartLockAdminVerifyNo1.MsgLen, TX_TIMEOUT);
			OpenMsgSendMonitor("AdminVerifyNo1");
			/***********************************************************************
			 *1.5秒后发送0x10
			 ***********************************************************************/
#ifdef USE_IWDG
			/*喂狗*/
			IWDG_Feed();
#endif
			delay_ms(1500);
			switch (ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction)
			{
				case AddPassword://添加密码
					DBG_PRINTF("\n\rKey request action: add password\r\n");					
					SmartLockDownLoadDataStream[8] = 0x03;//密码
					goto AddUserHandle;
				case AddICCard://添加卡
					DBG_PRINTF("\n\rKey request action: add card\r\n");
					SmartLockDownLoadDataStream[8] = 0x02;//卡
					goto AddUserHandle;
				case AddFingerVein:
					DBG_PRINTF("\n\rKey request action: add finger vein, will be handle next version......\r\n");
					break;
				case AddFingerPrint:
					DBG_PRINTF("\n\rKey request action: add finger print, will be handle next version......\r\n");
					break;
AddUserHandle:					
					SmartLockDownLoadDataStream[4] = (u8)((ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID & 0xFF00)>>8);
					SmartLockDownLoadDataStream[5] = (u8)(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID & 0x00FF);
					SmartLockDownLoadDataStream[6] = 0x01;//添加用户
					switch(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType)
					{
						case Admin:
							SmartLockDownLoadDataStream[7] = 0x03;
							break;
						case GenernalUser:
							SmartLockDownLoadDataStream[7] = 0x01;
							break;
						case Guest:
							SmartLockDownLoadDataStream[7] = 0x02;
							break;
						case DuressUser:
							SmartLockDownLoadDataStream[7] = 0x04;
							break;
						case Nurse:
							SmartLockDownLoadDataStream[7] = 0x05;
							break;
						case TemporaryUser:
							SmartLockDownLoadDataStream[7] = 0x06;
							break;
					}
					SmartLockDownLoadDataStream[9] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.year;
					SmartLockDownLoadDataStream[10] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.month;
					SmartLockDownLoadDataStream[11] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.day;
					SmartLockDownLoadDataStream[12] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.hour;
					SmartLockDownLoadDataStream[13] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.minutes;
					SmartLockDownLoadDataStream[14] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.seconds;
					SmartLockDownLoadDataStream[15] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.year;
					SmartLockDownLoadDataStream[16] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.month;
					SmartLockDownLoadDataStream[17] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.day;
					SmartLockDownLoadDataStream[18] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.hour;
					SmartLockDownLoadDataStream[19] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.minutes;
					SmartLockDownLoadDataStream[20] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.seconds;
					SmartLockDownLoadDataStream[21] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordEffectiveNumOfTime;
					SmartLockDownLoadDataStream[22] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPasswordLen;
					OldPasswordLen = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPasswordLen;
					memcpy(SmartLockDownLoadDataStream+23, ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPassword, 
						ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPasswordLen);
					SmartLockDownLoadDataStream[2] = 0x00;
					SmartLockDownLoadDataStream[3] = OldPasswordLen+19;
					LRC = 0;
					for(int i = 2; i < OldPasswordLen+23; i++)
						LRC += SmartLockDownLoadDataStream[i];
					SmartLockDownLoadDataStream[OldPasswordLen+23] = (u8)((LRC >> 8) & 0x00FF);
					SmartLockDownLoadDataStream[OldPasswordLen+24] = (u8)(LRC & 0x00FF);
					MsgSendMonitorForSmartLockDownLoadData.MsgLen = OldPasswordLen + 25;
					DBG_PRINTF("\n\rSmart lock download data:\r\n");
					for(int i = 0; i < MsgSendMonitorForSmartLockDownLoadData.MsgLen; i++)
						DBG_PRINTF("0x%02X ", SmartLockDownLoadDataStream[i]);
					DBG_PRINTF("\n\r\r\n");
					CommModule("SmartLock")->FnSendData(SmartLockDownLoadDataStream, MsgSendMonitorForSmartLockDownLoadData.MsgLen, TX_TIMEOUT);
					OpenMsgSendMonitor("DownLoadData");
					break;
				case ChangePassword://更改密码
					DBG_PRINTF("\n\rKey request action: change password\r\n");					
					SmartLockDownLoadDataStream[8] = 0x03;//密码
					goto ChangeUserHandle;
				case ChangeICCard://更改卡
					DBG_PRINTF("\n\rKey request action: change card\r\n");
					SmartLockDownLoadDataStream[8] = 0x02;//卡
					goto ChangeUserHandle;
				case ChangeFingerVein://更改静脉，不下发任何登记数据
					DBG_PRINTF("\n\rKey request action: change finger vein, can only change time......\r\n");
					SmartLockDownLoadDataStream[8] = 0x01;//静脉
					SmartLockDownLoadDataStream[4] = (u8)((ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID & 0xFF00)>>8);
					SmartLockDownLoadDataStream[5] = (u8)(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID & 0x00FF);
					SmartLockDownLoadDataStream[6] = 0x05;//修改用户
					switch(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType)
					{
						case Admin:
							SmartLockDownLoadDataStream[7] = 0x03;
							break;
						case GenernalUser:
							SmartLockDownLoadDataStream[7] = 0x01;
							break;
						case Guest:
							SmartLockDownLoadDataStream[7] = 0x02;
							break;
						case DuressUser:
							SmartLockDownLoadDataStream[7] = 0x04;
							break;
						case Nurse:
							SmartLockDownLoadDataStream[7] = 0x05;
							break;
						case TemporaryUser:
							SmartLockDownLoadDataStream[7] = 0x06;
							break;
					}
					SmartLockDownLoadDataStream[9] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.year;
					SmartLockDownLoadDataStream[10] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.month;
					SmartLockDownLoadDataStream[11] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.day;
					SmartLockDownLoadDataStream[12] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.hour;
					SmartLockDownLoadDataStream[13] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.minutes;
					SmartLockDownLoadDataStream[14] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.seconds;
					SmartLockDownLoadDataStream[15] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.year;
					SmartLockDownLoadDataStream[16] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.month;
					SmartLockDownLoadDataStream[17] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.day;
					SmartLockDownLoadDataStream[18] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.hour;
					SmartLockDownLoadDataStream[19] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.minutes;
					SmartLockDownLoadDataStream[20] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.seconds;
					SmartLockDownLoadDataStream[21] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordEffectiveNumOfTime;					
					LRC = 0;
					for(int i = 2; i < 22; i++)
						LRC += SmartLockDownLoadDataStream[i];
					SmartLockDownLoadDataStream[22] = (u8)((LRC >> 8) & 0x00FF);
					SmartLockDownLoadDataStream[23] = (u8)(LRC & 0x00FF);
					MsgSendMonitorForSmartLockDownLoadData.MsgLen = 24;
					DBG_PRINTF("\n\rSmart lock download data:\r\n");
					for(int i = 0; i < MsgSendMonitorForSmartLockDownLoadData.MsgLen; i++)
						DBG_PRINTF("0x%02X ", SmartLockDownLoadDataStream[i]);
					DBG_PRINTF("\n\r\r\n");
					CommModule("SmartLock")->FnSendData(SmartLockDownLoadDataStream, MsgSendMonitorForSmartLockDownLoadData.MsgLen, TX_TIMEOUT);
					OpenMsgSendMonitor("DownLoadData");
					break;
				case ChangeFingerPrint:
					DBG_PRINTF("\n\rKey request action: change finger print, will be handle next version......\r\n");
					break;
ChangeUserHandle:
					SmartLockDownLoadDataStream[4] = (u8)((ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID & 0xFF00)>>8);
					SmartLockDownLoadDataStream[5] = (u8)(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID & 0x00FF);
					SmartLockDownLoadDataStream[6] = 0x05;//修改用户
					switch(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType)
					{
						case Admin:
							SmartLockDownLoadDataStream[7] = 0x03;
							break;
						case GenernalUser:
							SmartLockDownLoadDataStream[7] = 0x01;
							break;
						case Guest:
							SmartLockDownLoadDataStream[7] = 0x02;
							break;
						case DuressUser:
							SmartLockDownLoadDataStream[7] = 0x04;
							break;
						case Nurse:
							SmartLockDownLoadDataStream[7] = 0x05;
							break;
						case TemporaryUser:
							SmartLockDownLoadDataStream[7] = 0x06;
							break;
					}
					SmartLockDownLoadDataStream[9] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.year;
					SmartLockDownLoadDataStream[10] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.month;
					SmartLockDownLoadDataStream[11] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.day;
					SmartLockDownLoadDataStream[12] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.hour;
					SmartLockDownLoadDataStream[13] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.minutes;
					SmartLockDownLoadDataStream[14] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDStartTime.seconds;
					SmartLockDownLoadDataStream[15] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.year;
					SmartLockDownLoadDataStream[16] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.month;
					SmartLockDownLoadDataStream[17] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.day;
					SmartLockDownLoadDataStream[18] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.hour;
					SmartLockDownLoadDataStream[19] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.minutes;
					SmartLockDownLoadDataStream[20] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordBCDEndTime.seconds;
					SmartLockDownLoadDataStream[21] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordEffectiveNumOfTime;					
					SmartLockDownLoadDataStream[22] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPasswordLen;
					OldPasswordLen = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPasswordLen;
					memcpy(SmartLockDownLoadDataStream+23, ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPassword, 
						ptEventInfo->uEventData.tNetWorkKeyRequestInfo.OldPasswordLen);
					SmartLockDownLoadDataStream[2] = 0x00;
					SmartLockDownLoadDataStream[3] = OldPasswordLen+19;
					LRC = 0;
					for(int i = 2; i < OldPasswordLen+23; i++)
						LRC += SmartLockDownLoadDataStream[i];
					SmartLockDownLoadDataStream[OldPasswordLen+23] = (u8)((LRC >> 8) & 0x00FF);
					SmartLockDownLoadDataStream[OldPasswordLen+24] = (u8)(LRC & 0x00FF);
					MsgSendMonitorForSmartLockDownLoadData.MsgLen = OldPasswordLen + 25;
					DBG_PRINTF("\n\rSmart lock download data:\r\n");
					for(int i = 0; i < MsgSendMonitorForSmartLockDownLoadData.MsgLen; i++)
						DBG_PRINTF("0x%02X ", SmartLockDownLoadDataStream[i]);
					DBG_PRINTF("\n\r\r\n");
					CommModule("SmartLock")->FnSendData(SmartLockDownLoadDataStream, MsgSendMonitorForSmartLockDownLoadData.MsgLen, TX_TIMEOUT);
					OpenMsgSendMonitor("DownLoadData");
					break;
				case DelPassword://删除密码
					DBG_PRINTF("\n\rKey request action: del password\r\n");					
					SmartLockDownLoadDataStream[6] = 0x02;//删除用户
					SmartLockDownLoadDataStream[8] = 0x03;//密码
					goto OtherUserHandle;
				case FreezePassword://冻结密码
					DBG_PRINTF("\n\rKey request action: freeze password\r\n");					
					SmartLockDownLoadDataStream[6] = 0x06;//冻结用户
					SmartLockDownLoadDataStream[8] = 0x03;//密码
					goto OtherUserHandle;
				case UnfreezePassword://解冻密码
					DBG_PRINTF("\n\rKey request action: unfreeze password\r\n");					
					SmartLockDownLoadDataStream[6] = 0x07;//解冻用户
					SmartLockDownLoadDataStream[8] = 0x03;//密码
					goto OtherUserHandle;
				case CleanAllPasswordData://清空密码
					DBG_PRINTF("\n\rKey request action: clean all password\r\n");					
					SmartLockDownLoadDataStream[6] = 0x03;//清空用户
					SmartLockDownLoadDataStream[8] = 0x03;//密码
					goto OtherUserHandle;
				case DelIcCard://删除卡
					DBG_PRINTF("\n\rKey request action: del card\r\n");
					SmartLockDownLoadDataStream[6] = 0x02;//删除用户
					SmartLockDownLoadDataStream[8] = 0x02;//卡
					goto OtherUserHandle;
				case FreezeICCard://冻结卡
					DBG_PRINTF("\n\rKey request action: freeze card\r\n");
					SmartLockDownLoadDataStream[6] = 0x06;//冻结用户
					SmartLockDownLoadDataStream[8] = 0x02;//卡
					goto OtherUserHandle;
				case UnfreezeICCard://解冻卡
					DBG_PRINTF("\n\rKey request action: unfreeze card\r\n");					
					SmartLockDownLoadDataStream[6] = 0x07;//解冻用户
					SmartLockDownLoadDataStream[8] = 0x02;//卡
					goto OtherUserHandle;
				case CleanAllCardData://清空卡
					DBG_PRINTF("\n\rKey request action: clean all card\r\n");					
					SmartLockDownLoadDataStream[6] = 0x03;//清空用户
					SmartLockDownLoadDataStream[8] = 0x02;//卡
					goto OtherUserHandle;
				case DelFingerVein://删除静脉
					DBG_PRINTF("\n\rKey request action: del finger vein\r\n"); 				
					SmartLockDownLoadDataStream[6] = 0x02;//删除用户
					SmartLockDownLoadDataStream[8] = 0x01;//静脉
					goto OtherUserHandle;
				case FreezeFingerVein://冻结静脉
					DBG_PRINTF("\n\rKey request action: freeze finger vein\r\n");					
					SmartLockDownLoadDataStream[6] = 0x06;//冻结用户
					SmartLockDownLoadDataStream[8] = 0x01;//密码
					goto OtherUserHandle;
				case UnfreezeFingerVein://解冻静脉
					DBG_PRINTF("\n\rKey request action: unfreeze finger vein\r\n");					
					SmartLockDownLoadDataStream[6] = 0x07;//解冻用户
					SmartLockDownLoadDataStream[8] = 0x01;//静脉
					goto OtherUserHandle;
				case CleanAllFingerVeinData://清空静脉
					DBG_PRINTF("\n\rKey request action: clean all finger vein\r\n");					
					SmartLockDownLoadDataStream[6] = 0x03;//清空用户
					SmartLockDownLoadDataStream[8] = 0x01;//静脉
					goto OtherUserHandle;
				case CleanAllUserData:
					DBG_PRINTF("\n\rKey request action: clean all data\r\n");
					SmartLockDownLoadDataStream[6] = 0x04;//初始化用户
					SmartLockDownLoadDataStream[8] = 0xFF;//所有登记类型
					goto OtherUserHandle;
OtherUserHandle:
					SmartLockDownLoadDataStream[4] = (u8)((ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID & 0xFF00)>>8);
					SmartLockDownLoadDataStream[5] = (u8)(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordID & 0x00FF);
					switch(ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordType)
					{
						case Admin:
							SmartLockDownLoadDataStream[7] = 0x03;
							break;
						case GenernalUser:
							SmartLockDownLoadDataStream[7] = 0x01;
							break;
						case Guest:
							SmartLockDownLoadDataStream[7] = 0x02;
							break;
						case DuressUser:
							SmartLockDownLoadDataStream[7] = 0x04;
							break;
						case Nurse:
							SmartLockDownLoadDataStream[7] = 0x05;
							break;
						case TemporaryUser:
							SmartLockDownLoadDataStream[7] = 0x06;
							break;
					}
					SmartLockDownLoadDataStream[9] = tBCDStartTime.year;
					SmartLockDownLoadDataStream[10] = tBCDStartTime.month;
					SmartLockDownLoadDataStream[11] = tBCDStartTime.day;
					SmartLockDownLoadDataStream[12] = tBCDStartTime.hour;
					SmartLockDownLoadDataStream[13] = tBCDStartTime.minutes;
					SmartLockDownLoadDataStream[14] = tBCDStartTime.seconds;
					SmartLockDownLoadDataStream[15] = tBCDEndTime.year;
					SmartLockDownLoadDataStream[16] = tBCDEndTime.month;
					SmartLockDownLoadDataStream[17] = tBCDEndTime.day;
					SmartLockDownLoadDataStream[18] = tBCDEndTime.hour;
					SmartLockDownLoadDataStream[19] = tBCDEndTime.minutes;
					SmartLockDownLoadDataStream[20] = tBCDEndTime.seconds;
					SmartLockDownLoadDataStream[21] = ptEventInfo->uEventData.tNetWorkKeyRequestInfo.PasswordEffectiveNumOfTime;
					SmartLockDownLoadDataStream[22] = 0x00; //旧密码长度为0
					SmartLockDownLoadDataStream[23] = 0x00; //新密码长度为0
					SmartLockDownLoadDataStream[2] = 0x00;
					SmartLockDownLoadDataStream[3] = 20;	//长度固定
					LRC = 0;
					for(int i = 2; i < 24; i++)
						LRC += SmartLockDownLoadDataStream[i];
					SmartLockDownLoadDataStream[24] = (u8)((LRC >> 8) & 0x00FF);
					SmartLockDownLoadDataStream[25] = (u8)(LRC & 0x00FF);
					MsgSendMonitorForSmartLockDownLoadData.MsgLen = 26;
					DBG_PRINTF("\n\rSmart lock download data:\r\n");
					for(int i = 0; i < MsgSendMonitorForSmartLockDownLoadData.MsgLen; i++)
						DBG_PRINTF("0x%02X ", SmartLockDownLoadDataStream[i]);
					DBG_PRINTF("\n\r\r\n");
					CommModule("SmartLock")->FnSendData(SmartLockDownLoadDataStream, MsgSendMonitorForSmartLockDownLoadData.MsgLen, TX_TIMEOUT);
					OpenMsgSendMonitor("DownLoadData");
					break;
				case DelFingerPrint:
					DBG_PRINTF("\n\rKey request action: del finger print, will be handle next version......\r\n");
					break;
				case FreezeFingerPrint:
					DBG_PRINTF("\n\rKey request action: freeze finger print, will be handle next version......\r\n");
					break;
				case UnfreezeFingerPrint:
					DBG_PRINTF("\n\rKey request action: unfreeze finger print, will be handle next version......\r\n");
					break;
				case CleanAllFingerPrintData:
					DBG_PRINTF("\n\rKey request action: clean all finger print, will be handle next version......\r\n");
					break;
				case UnknowAction:
					DBG_PRINTF("\n\rKey request action: unknown action\r\n");
					break;
				default:
					DBG_PRINTF("\n\rKey request action: data error:%d\r\n", ptEventInfo->uEventData.tNetWorkKeyRequestInfo.eKeyRequestAction);
					break;
			}
			break;
		default:
			break;
	}
	return iRet;
}


/*"重启"事件监测和处理*/
static void SystemResetEventDetectAndHandle()
{
	if(SystemResetCnt.FnGetTimerStatus(&SystemResetCnt))
	{
		/*系统重启*/
		SystemResetCnt.FnCloseTimerModule(&SystemResetCnt);
		DBG_PRINTF("System begin reset......");
		DBG_PRINTF("\n\r\r\n");
		HAL_NVIC_SystemReset();
	}
}


/*循环唤醒zigbee*/
static void CycleWkupZigbeeDetectAndHandle()
{
	if(CycleWkupZigbeeCnt.FnGetTimerStatus(&CycleWkupZigbeeCnt))
	{
#if 0
		/*唤醒zigbee1分钟*/
		int iRet = 0;
		CommModule("Zigbee")->FnSendData((u8*)ZigbeeWkupFor1MinStream, sizeof(ZigbeeWkupFor1MinStream), TX_TIMEOUT);
		iRet = OpenMsgSendMonitor("WkupZigbeeFor1Min");
		if(iRet)
			DBG_PRINTF("\n\rOpenMsgSendMonitor:Find WkupZigbeeFor1Min failed\r\n");
#else
		CommModule("Zigbee")->FnWkupDevice();
#endif
		CycleWkupZigbeeCnt.FnCloseTimerModule(&CycleWkupZigbeeCnt);
		CycleWkupZigbeeCnt.FnSetTimer(&CycleWkupZigbeeCnt, 50);//5s周期触发
	}
}


/*单次定时唤醒zigbee*/
static void SingleWkupZigbeeDetectAndHandle()
{
	if(SingleWkupZigbeeCnt.FnGetTimerStatus(&SingleWkupZigbeeCnt))
	{
#if 0
		/*唤醒zigbee1分钟*/
		int iRet = 0;
		CommModule("Zigbee")->FnSendData((u8*)ZigbeeWkupFor1MinStream, sizeof(ZigbeeWkupFor1MinStream), TX_TIMEOUT);
		iRet = OpenMsgSendMonitor("WkupZigbeeFor1Min");
		if(iRet)
			DBG_PRINTF("\n\rOpenMsgSendMonitor:Find WkupZigbeeFor1Min failed\r\n");
#else
		CommModule("Zigbee")->FnWkupDevice();
#endif
		SingleWkupZigbeeCnt.FnCloseTimerModule(&SingleWkupZigbeeCnt);
	}	
}


static void SmartLockLargeMsgHandle()
{
	int iRet = 0;
	char* pcLargeDataTmpFromSmartLock = LargeDataTmpFromSmartLock;
	T_EventInfo tSmartLockEventInfo = {
		.name = "SmartLockEvent",
		.cHasEventData = 0,
	};
	iRet = CommModule("SmartLock")->FnGetLargeData(LargeDataTmpFromSmartLock);
	if(iRet)
	{
		DBG_PRINTF("\n\rGet large data from Smart Lock:\r\n");
		for(int i = 0; i < iRet; i++)
			DBG_PRINTF("0x%02X ", *(pcLargeDataTmpFromSmartLock++));
		pcLargeDataTmpFromSmartLock = LargeDataTmpFromSmartLock;
		DBG_PRINTF("\n\r\r\n");
		/*协议解析*/
		iRet = GetSmartLockEventInfo(pcLargeDataTmpFromSmartLock, SMART_LOCK_LARGE_BUF_LEN, &tSmartLockEventInfo);
		if(0 == iRet)
		{
			DBG_PRINTF("\n\rEventType = %d\r\n", tSmartLockEventInfo.eEventType);
			iRet = SmartLockEventPro(&tSmartLockEventInfo);
		}
	}
}


/*SmartLock消息处理*/
static void SmartLockMsgHandle()
{
	int iRet = 0;
	char* pcDataTmpFromSmartLock = DataTmpFromSmartLock;
	T_EventInfo tSmartLockEventInfo = {
		.name = "SmartLockEvent",
		.cHasEventData = 0,
	};
	iRet = CommModule("SmartLock")->FnGetData(DataTmpFromSmartLock);
	if(iRet)
	{
		DBG_PRINTF("\n\rGet data from Smart Lock:\r\n");
		for(int i = 0; i < iRet; i++)
			DBG_PRINTF("0x%02X ", *(pcDataTmpFromSmartLock++));
		pcDataTmpFromSmartLock = DataTmpFromSmartLock;
		DBG_PRINTF("\n\r\r\n");
		/*协议解析*/
		iRet = GetSmartLockEventInfo(DataTmpFromSmartLock, BUFFER_LEN, &tSmartLockEventInfo);
		if(0 == iRet)
		{
			DBG_PRINTF("\n\rEventType = %d\r\n", tSmartLockEventInfo.eEventType);
			iRet = SmartLockEventPro(&tSmartLockEventInfo);
		}
		
	}
}


//static void DebugMsgHandle()
//{
//	int iRet = 0;
//	char* pcDataTmpFromDebugPort = DataTmpFromDebugPort;
//	T_EventInfo tDebugPortEventInfo = {
//		.name = "DebugPortEvent",
//		.cHasEventData = 0,
//	};
//	iRet = CommModule("Debug")->FnGetData(DataTmpFromDebugPort);
//	if(iRet)
//	{
//		DBG_PRINTF("\n\rGet data from Debug:\r\n");
//		for(int i = 0; i < iRet; i++)
//			DBG_PRINTF("0x%02X ", *(pcDataTmpFromDebugPort++));
//		pcDataTmpFromDebugPort = DataTmpFromDebugPort;
//		DBG_PRINTF("\n\r\r\n");
//		/*协议解析*/
////		iRet = GetDebugPortEventInfo(DataTmpFromDebugPort, BUFFER_LEN, &tDebugPortEventInfo);
////		if(0 == iRet)
////		{
////			DBG_PRINTF("\n\rEventType = %d\r\n", tDebugPortEventInfo.eEventType);
////			iRet = DebugPortEventPro(&tDebugPortEventInfo);
////		}
//	}
//	
//}


#ifdef USE_LOW_POWER
/*系统状态监测
 *1 -- 处于运行状态
 *0 -- 处于闲置状态
 */
static int SystemStatusDetect(void)
{
#if 1 
	int iRet = 0;
	iRet |= CommModule("SmartLock")->FnStatusDetect();
	iRet |= CommModule("Zigbee")->FnStatusDetect();
	iRet |= GetMsgMonitorStatus();
	iRet |= GetTimerModuleStatus();
	SysWkupCnt++;
//	DBG_PRINTF("\n\rSysWkupCnt=%d\r\n", SysWkupCnt);
	if(SysWkupCnt >= SYSTEM_WKUP_CNT)
	{		
		DBG_PRINTF("\n\rSystem start reset after 3s......\r\n");
		SystemResetCnt.FnSetTimer(&SystemResetCnt, 30);//3秒后重启
		SysWkupCnt = 0;
	}
	return iRet;
#else
	int iRet,iRet1,iRet2,iRet3,iRet4 = 0;
	iRet1 = CommModule("SmartLock")->FnStatusDetect();
	if(iRet1)
	{
		DBG_PRINTF("\n\riRet1 = %d, Comm module: smart lock working...\r\n", iRet1);
	}
	iRet2 = CommModule("Zigbee")->FnStatusDetect();
	if(iRet2)
	{
		DBG_PRINTF("\n\riRet2 = %d, Comm module: zigbee working...\r\n", iRet2);
	}
	iRet3 = GetMsgMonitorStatus();
	if(iRet3)
	{
		DBG_PRINTF("\n\rMsg monitor working...\r\n");
	}
	iRet4 = GetTimerModuleStatus();
	if(iRet4)
	{
		DBG_PRINTF("\n\rTimer module working...\r\n");
	}
	iRet = iRet1 + iRet2 + iRet3 + iRet4;
	return iRet;
#endif
}
#endif


/*Zigbee消息处理*/
static void ZigbeeMsgHandle()
{
	int iRet = 0;
	char* pcDataTmpFromZigbee = DataTmpFromZigbee;
	T_EventInfo tZigbeeEventInfo = {
		.name = "ZigbeeEvent",
		.cHasEventData = 0,
	};
	iRet = CommModule("Zigbee")->FnGetData(DataTmpFromZigbee);
	if(iRet)
	{
		DBG_PRINTF("\n\rGet data from Zigbee:\r\n");
		for(int i = 0; i < iRet; i++)
			DBG_PRINTF("0x%02X ", *(pcDataTmpFromZigbee++));
		pcDataTmpFromZigbee = DataTmpFromZigbee;
		DBG_PRINTF("\n\r\r\n");
		/*协议解析*/
		iRet = GetZigbeeEventInfo(DataTmpFromZigbee, BUFFER_LEN, &tZigbeeEventInfo);
		if(0 == iRet)
		{
			DBG_PRINTF("\n\rEventType = %d\r\n", tZigbeeEventInfo.eEventType);
			iRet = ZigbeeEventPro(&tZigbeeEventInfo);
		}
		
	}
}


/*起始0*/
static u8 XorCalcForZigbee(u8 *DataForCalc, u16 StartNum, u16 Length)
{	
	u16 i;
	u8 CalcResult = DataForCalc[StartNum];
	for(i = StartNum + 1; i < Length; i++) 
	{
		CalcResult ^= DataForCalc[i];
	}
	return CalcResult;
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	while(1)
	{
		DBG_PRINTF("\n\rEnter _Error_Handler,file: %s, line:%d\r\n", file, line);
		DBG_PRINTF("\n\rBegin reset system......\r\n");
		HAL_NVIC_SystemReset();
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/****************************************************************************/
/*                                                                          */
/*  End of file.                                                            */
/*                                                                          */
/****************************************************************************/



