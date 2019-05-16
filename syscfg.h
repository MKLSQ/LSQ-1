/*****************************************************************************
 * SysCfg.h
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
#ifndef _SYSCFG_H
#define _SYSCFG_H

/*****************************************************************************
 * Included Files
 ****************************************************************************/
//#include <sys.h>
//#include <Syscfg.h>
//#include <stm32l0xx_hal_iwdg.h>

/*****************************************************************************
 * Configure Definitions
 ****************************************************************************/
/*-----------------------------------
 *用于配置系统
 *----------------------------------*/
/*****************************
 *	Print enable
 *****************************/
#define _DEBUG_PRINT_ENABLE 1
#define _ERROR_PRINT_ENABLE 1
	
	// #define DBG_PRINTF							printf
	
#if (_DEBUG_PRINT_ENABLE == 1)
    #define DEBUG_LOG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_LOG_PRINT
#endif 
	
#if (_ERROR_PRINT_ENABLE == 1)
    #define ERROR_LOG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define ERROR_LOG_PRINT
#endif 


/*测试用*/
//#define SYSTEM_ENTER_RUN_FLEET								//程序在发送"添加密码"指令时会跑飞

/*系统版本*/
#define SYSTEM_VERSION						"V1.0.0.15"

#define USE_LOW_POWER										//启动低功耗 

/*系统延迟休眠时间*/
#define SYSTEM_SLEEP_DELAY					(u8)20			//单位100ms	

/*系统时钟设置*/
#define USE_EXT_16M_CLOCK 									//使用16M外部晶振，否则使用16MHz内部晶振

/*使用独立看门狗和rtc时钟配合，注意使用rtc要修改sys.c系统时钟配置*/
#define USE_IWDG											//独立看门狗进入低功耗后仍会工作

/*RTC配置标志*/
#define RTC_BKUP_CONF_FLAG					(u16)0X5050
//#define RTC_USE_LSE											//使用LES，否则使用LSI

/*使用窗口看门狗，此功能有问题*/
//#define USE_WWDG

/*门锁唤醒检测引脚*/
#define USE_PA5												//PA5作为检测引脚
//#define USE_PA3												//PA3(串口RX复用)作为检测引脚

/*门锁唤醒信号检测*/
#define WKUP_USE_DELAY_US									//使用us延时
#define SMART_LOCK_WKUP_TH_CNT				(u8)15			//高电平次数
#define SMART_LOCK_WKUP_TL_CNT  			(u8)15			//低电平次数
#define SMART_LOCK_WKUP_CHECK_DELAY_TIME	(u8)500			//宏定义us延时单位1us，否则单位1ms

/*zigbee唤醒信号检测*/
#define ZIGBEE_WKUP_TH_CNT					(u8)50			//高电平次数
#define ZIGBEE_WKUP_TL_CNT  				(u8)50			//低电平次数
#define ZIGBEE_WKUP_CHECK_DELAY_TIME		(u8)1			//单位1ms

/*队列大小设置*/
#define BUFFER_INDEX						(u16)5			//buffer数
#define BUFFER_LEN  						(u16)64  		//单个buffer容量
#define SMART_LOCK_LARGE_BUF_LEN			(u16)2048		//用于门锁大数据缓冲区
#define ZIGBEE_LARGE_BUF_LEN				(u16)100		//用于zigbee大数据缓冲区

/*门锁发送命令队列大小*/
#define SMART_LOCK_CMD_BUF_SIZE				(u8)2			//队列大小

/*串口共用设置*/
#define RX_END_FLAG							(u16)0x8000		//接收结束标志
#define RX_START_FLAG						(u16)0x4000 	//接收开始标志
#define RX_BUFFER_INDEX_FLAG				(u16)0X3FFF		//接收缓冲区索引
#define RXBUFFERSIZE   						1 				//串口接收缓存大小
#define TX_TIMEOUT							(u16)150		//串口发送超时时间,单位ms
#define TX_TIMEOUT_3s						(u16)3000		//串口发送超时时间,单位ms	
#define RX_TIMEOUT							(u16)150		//串口接收超时时间,单位ms

/*调试相关*/
//#define DBG_PRINTF(...)  
#define DBG_PRINTF 							printf
#define	DEBUG_PORT							USART5			//选择调试口
#define Dbg_BaudRate						115200			//调试口波特率
#define REDEFINE_FPUTC										//重定义fputc
#define UART5_RX_BUF_LEN					(u16)10			//缓冲区长度，要求小于等于队列长度
#define UART5_USE_CALLBACK					1				//定义是否使用回调逻辑，1：使用回调；0：不使用回调
#define UART5_RX_ENABLE						0				//允许uart5接收中断，有问题不要开启

/*门锁数据交互相关*/
#define	RX_SMART_LOCK_DATA_STX				(u8)0xF5 		//接收开始标志
#define LPUART1_RX_BUF_LEN					(u16)64			//缓冲区长度，要求小于等于队列长度
#define LPUART1_USE_CALLBACK				0				//定义是否使用回调逻辑，1：使用回调；0：不使用回调；宾利锁不要使用回调


/*zigbee数据交互相关*/
#define RX_ZIGBEE_DATA_STX					(u8)0xAA 		//接收开始标志
#define RX_ZIGBEE_DATA_END					(u8)0x55 		//接收结束标志
#define UART1_RX_BUF_LEN					(u16)64			//缓冲区长度，要求小于等于队列长度
#define UART1_USE_CALLBACK					0				//定义是否使用回调逻辑，1：使用回调；0：不使用回调


/*消息监控相关宏*/
#define MSG_SEND_MONITOR_TIMEOUT_INVL_1s	(u8)10			//消息监控检查间隔，实际间隔时间=定时器中断时间*间隔宏定义
#define MSG_SEND_MONITOR_TIMEOUT_INVL_4s	(u8)40			//消息监控检查间隔，实际间隔时间=定时器中断时间*间隔宏定义
#define MSG_SEND_MONITOR_TIMEOUT_INVL_5s	(u8)50			//消息监控检查间隔，实际间隔时间=定时器中断时间*间隔宏定义
#define MSG_SEND_MONITOR_TIMEOUT_INVL_9s	(u8)90
#define MSG_SEND_MONITOR_STOP_RESEND_CNT	(u8)3			//停止发送次数

/*消息计数宏定义*/
#define EVENT_CNT_IS_STOP					(u8)0
#define EVENT_CNT_IS_RUNNING				(u8)1

/*系统唤醒计数*/
#define SYSTEM_WKUP_CNT						(u32)175000		//用于重启系统的计数

/*****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/*****************************************************************************
 * Public Types
 ****************************************************************************/

/*****************************************************************************
 * Public Data
 ****************************************************************************/

/*****************************************************************************
 * Inline Functions
 ****************************************************************************/
#ifdef __cplusplus
	extern "C" {
#endif
/*****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/
/**
 * @fn		zbmodule_SetSerialPort
 *
 * @brief	设置串口名称
 *
 * @param[in]	serialPortName		 - 串口名称
 *
 * @return 0 - 成功, 否则表示失败
 */
/****************************************************************************/
#ifdef __cplusplus
}
#endif
/****************************************************************************/
#endif /* __IWDG_H */
/****************************************************************************/
/*                                                                          */
/*  End of file.                                                            */
/*                                                                          */
/****************************************************************************/



