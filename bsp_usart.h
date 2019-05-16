/*****************************************************************************
 * uart.h
 *
 * Copyright (C) 2018 liangyiping yiping.liang@gotechcn.cn
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
#ifndef _USART_H
#define _USART_H

/*****************************************************************************
 * Included Files
 ****************************************************************************/
#include <sys.h> 
#include <SysCfg.h>

/*****************************************************************************
 * Configure Definitions
 ****************************************************************************/

/*****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/*****************************************************************************
 * Public Types
 ****************************************************************************/
 typedef struct CommunicationModule{
	char *name;
	int (*FnInit)(void);
	int (*FnOpen)(void);
	int (*FnClose)(void);
	int (*FnSendData)(u8 *pcData, u16 Size, u32 Timeout);
	int (*FnGetData)(char* pcData);
	int (*FnGetLargeData)(char* pcLargeData);
	void (*FnWkupDevice)(void);
	int (*FnStatusDetect)(void);
	struct CommunicationModule *ptNext;
}T_CommunicationModule, *PT_CommunicationModule;

/*消息发送监测类*/
typedef struct MsgSendMonitor{
	char *MsgName;//消息名称
	volatile u8	TimeCout; //单位100ms，与定时器配合
	volatile u8	SendCout; //已经发送次数
	volatile u8 IsCanUsed;
	volatile u8 SetTimeOut;
	u8 *SendData;//消息数据
	u8 MsgLen;//消息长度
	PT_CommunicationModule ptCommunicationModule;//使用的发送模块
	struct MsgSendMonitor *ptNext;
}T_MsgSendMonitor, *PT_MsgSendMonitor;

/*****************************************************************************
 * Public Data
 ****************************************************************************/

/*****************************************************************************
 * Inline Functions
 ****************************************************************************/

/****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/
/**
 * @fn		
 *
 * @brief	设置串口名称
 *
 * @param[in]	serialPortName		 - 串口名称
 *
 * @return 0 - 成功, 否则表示失败
 */
int SmartLockCommModuleInit(void);
int SmartLockCommModuleOpen(void);
int SmartLockCommModuleClose(void);
int SendDataToSmartLock(u8 *pcData, u16 Size, u32 Timeout);
int GetDataFromSmartLock(char* pcDara);
int GetLageDataFromSmartLock(char* pcLargeData);
void WkupSmartLock(void);
int SmartLockStatusDetect(void);

int ZigbeeCommModuleInit(void);
int ZigbeeCommModuleOpen(void);
int ZigbeeCommModuleClose(void);
int SendDataToZigbee(u8 *pcData, u16 Size, u32 Timeout);
int GetDataFromZigbee(char* pcData);
void WkupZigbee(void);
int ZigbeeStatusDetect(void);

int DebugCommModuleInit(void);
int DebugCommModuleOpen(void);
int DebugCommModuleClose(void);
int SendDataToDebugPort(u8 *pcData, u16 Size, u32 Timeout);
int GetDataFromDebugPort(char* pcData);
void WkupDebug(void);
int DebugPortStatusDetect(void);

int RegisterCommModule(PT_CommunicationModule ptCommModule);
PT_CommunicationModule CommModule(char *name);

int RegisterMsgSendMonitor(PT_MsgSendMonitor ptMsgSendMonitor);
void MsgSendMonitorTimCntAdd(void);
PT_MsgSendMonitor MsgSendMonitor(char *name);
void MsgSendMonitorCheckAndReSend(void);
int OpenMsgSendMonitor(char *name);
int CloseMsgSendMonitor(char *name);
int GetMsgMonitorStatus(void);


/****************************************************************************/
#ifdef __cplusplus
}
#endif
/****************************************************************************/
#endif /* MY_QUEUE */
/****************************************************************************/
/*                                                                          */
/*  End of file.                                                            */
/*                                                                          */
/****************************************************************************/


