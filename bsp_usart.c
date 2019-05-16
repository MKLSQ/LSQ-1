/*****************************************************************************
 * usart.c
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


/*****************************************************************************
 * Included Files
 ****************************************************************************/
#include <string.h>
#include <sys.h>
#include <SysCfg.h>
#include <delay.h>
#include <MyQueue.h>
#include <usart.h>
#include <iwdg.h>

/*****************************************************************************
 * Configure Definitions
 ****************************************************************************/

/*****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
static PT_CommunicationModule ptCommModuleHead;
static PT_MsgSendMonitor ptMsgSendMonitorHead;

/*****************************************************************************
 * Public Types
 ****************************************************************************/

/*****************************************************************************
 * Public Data
 ****************************************************************************/
/*LPUart1队列模块*/
static T_QueueModule LPUart1QueueModule = {
	.name = "LPUart1",
	.ModuleInit = QueueModuleInit,
	.FnFullDetect = QueueIsFull,
	.FnEmptyDetect = QueueIsEmpty,
	.FnPutData = QueuePutData,
	.FnGetData = QueueGetData,
};
/*Uart1队列模块*/
static T_QueueModule Uart1QueueModule = {
	.name = "Uart1",
	.ModuleInit = QueueModuleInit,
	.FnFullDetect = QueueIsFull,
	.FnEmptyDetect = QueueIsEmpty,
	.FnPutData = QueuePutData,
	.FnGetData = QueueGetData,
};
/*Uart5队列模块*/
static T_QueueModule Uart5QueueModule = {
	.name = "Uart5",
	.ModuleInit = QueueModuleInit,
	.FnFullDetect = QueueIsFull,
	.FnEmptyDetect = QueueIsEmpty,
	.FnPutData = QueuePutData,
	.FnGetData = QueueGetData,
};
volatile		u32					SmartLockSumCheckTmp=0;//门锁校验
volatile		u16					SmartLockDataLen=0xFFFF;
volatile		u8					ZigbeeXorCheckTmp=1;//飞比校验
static	 		u8 					LPUART1_RX_BUF[LPUART1_RX_BUF_LEN];
volatile 		u8					USART1_RX_BUF[UART1_RX_BUF_LEN];
volatile 		u8					USART5_RX_BUF[UART5_RX_BUF_LEN];
volatile		u8					lpuart1_aRxBuffer[RXBUFFERSIZE];
volatile		u8					uart1_aRxBuffer[RXBUFFERSIZE];
volatile		u8					uart5_aRxBuffer[RXBUFFERSIZE];
volatile		u16 				LPUART1_RX_STA=0;	   	//lpuart1接收状态标记
volatile		u16 				USART1_RX_STA=0;	   	//uart1接收状态标记	
volatile		u16 				USART5_RX_STA=0;	   	//uart3接收状态标记	
static			UART_HandleTypeDef	hlpuart1;
static			UART_HandleTypeDef	huart1;
static			UART_HandleTypeDef	huart5;
/*测试用*/
//static 		u8 					LPUART1_RX_BUF_Test[LPUART1_RX_BUF_LEN];
//static		u16 				LPUART1_RX_STA_Test=0;	
volatile 		u8 					UART1_RX_BUF_Test[UART1_RX_BUF_LEN];
volatile		u16 				UART1_RX_STA_Test=0;	
volatile 		u8 					UART5_RX_BUF_Test[UART5_RX_BUF_LEN];
volatile		u16 				UART5_RX_STA_Test=0;
static  		u8					Lpuart1RxLargeBuf[SMART_LOCK_LARGE_BUF_LEN];//lpuart1用大数据缓冲区					
volatile		u8 					SmartLockLargeDataHandleFlag = 0;
volatile 		u8 					LargeDataFromSmartLock = 0;


/*****************************************************************************
 * Inline Functions
 ****************************************************************************/

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
/*加入以下代码,支持printf函数,而不需要选择use MicroLIB
 *#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
 */   
#ifdef REDEFINE_FPUTC
#pragma import(__use_no_semihosting)             
	//标准库需要的支持函数				 
	struct __FILE 
	{ 
		int handle; 
	}; 
	
	FILE __stdout;		 
	//定义_sys_exit()以避免使用半主机模式	 
	void _sys_exit(int x) 
	{ 
		x = x; 
	} 
	//重定义fputc函数 
	int fputc(int ch, FILE *f)
	{	
#if 0//此分支有问题，会死机
		while((DEBUG_PORT->ISR&0X40)==0);//循环发送,直到发送完毕	 
		DEBUG_PORT->TDR=(u8)ch; 	 
		return ch;
#else
		HAL_UART_Transmit(&huart5, (u8*)(&ch), 1, TX_TIMEOUT);
		while(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC)!=SET);
		return ch;
#endif
	}
#endif


/*串口回调
 *只进行校验判断，校验成功后把收据保存到环形队列中
 *校验不成功的数据丢弃
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#if(UART5_USE_CALLBACK == 1)
	/*如果是串口5，调试，接收任意数据*/
	if(huart->Instance == USART5)
	{
		char *name = "Uart5";
		UART5_RX_BUF_Test[UART5_RX_STA_Test++] = uart5_aRxBuffer[0];
//		DBG_PRINTF("\n\r0x%02x\r\n", uart5_aRxBuffer[0]);
		if(UART5_RX_STA_Test == UART5_RX_BUF_LEN)//注意不要溢出
			UART5_RX_STA_Test = 0;
		if(USART5_RX_STA <= (u16)(UART5_RX_BUF_LEN-1))
			USART5_RX_BUF[USART5_RX_STA++] = uart5_aRxBuffer[0];
		else
		{
			int iRet = Queue(name)->FnPutData(name, (char*)USART5_RX_BUF, UART5_RX_BUF_LEN);
//			if(iRet)
//				DBG_PRINTF("\n\rFnPutData failed: %d\r\n", iRet);
			USART5_RX_STA = 0;
			USART5_RX_BUF[USART5_RX_STA++] = uart5_aRxBuffer[0];		
		} 
	}
#endif

#if(UART1_USE_CALLBACK == 1)
	/*串口1，zigbee*/
	if(huart->Instance == USART1)
	{
		char *name = "Uart1";
		if((USART1_RX_STA&RX_END_FLAG) == 0)//接收未完成
		{
			/*索引溢出*/
			if((USART1_RX_STA&RX_BUFFER_INDEX_FLAG) >= UART1_RX_BUF_LEN)
			{
				USART1_RX_STA = 0;
//				DBG_PRINTF("\n\rUSART1_RX_BUF overflow\r\n");
			}
			/*接收到头*/
			else if((uart1_aRxBuffer[0] == RX_ZIGBEE_DATA_STX) && (USART1_RX_STA == 0))
			{
				USART1_RX_STA |= RX_START_FLAG;	//接收开始 
				USART1_RX_BUF[USART1_RX_STA&RX_BUFFER_INDEX_FLAG] = uart1_aRxBuffer[0];
				ZigbeeXorCheckTmp = uart1_aRxBuffer[0];
				USART1_RX_STA++;
			}
			/*接收到0x55结束标志*/
			else if((uart1_aRxBuffer[0] == RX_ZIGBEE_DATA_END) && USART1_RX_STA)
			{
				/*接收到了0x55，同时校验字节为0，说明校验字节已经和正确的校验字节相等*/
				if(ZigbeeXorCheckTmp == 0)
				{ 
					USART1_RX_STA |= RX_END_FLAG;	//接收完成了 
					USART1_RX_BUF[USART1_RX_STA&RX_BUFFER_INDEX_FLAG]=uart1_aRxBuffer[0];
					int iRet = Queue(name)->FnPutData(name, (char*)USART1_RX_BUF, BUFFER_LEN);
//					if(iRet)
//						DBG_PRINTF("\n\rFnPutData failed: %d\r\n", iRet);
					USART1_RX_STA = 0;
					ZigbeeXorCheckTmp = 1;
				}
				/*否则仍然认为是数据*/
				else
				{
					USART1_RX_BUF[USART1_RX_STA&RX_BUFFER_INDEX_FLAG] = uart1_aRxBuffer[0];
					ZigbeeXorCheckTmp = uart1_aRxBuffer[0];
					USART1_RX_STA++;
				}
			}
			/*接收中*/
			else if(USART1_RX_STA&RX_START_FLAG)
			{	
				USART1_RX_BUF[USART1_RX_STA&RX_BUFFER_INDEX_FLAG]=uart1_aRxBuffer[0];
				ZigbeeXorCheckTmp = uart1_aRxBuffer[0] ^ ZigbeeXorCheckTmp;/*异或校验*/
				USART1_RX_STA++;  					 
			}
		}

	}
#endif

	/*LPUART1，宾利门锁*/
#if(LPUART1_USE_CALLBACK == 1)//HAL_UART_Receive_IT方式	
	if(huart->Instance == LPUART1)
	{	
		char *name = "LPUart1";
		if((LPUART1_RX_STA&RX_END_FLAG) == 0)//接收未完成
		{
			/*索引溢出*/
			if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) >= LPUART1_RX_BUF_LEN)//注意加括号
			{
//				DBG_PRINTF("\n\rLPUSART1_RX_BUF overflow\r\n");
				LPUART1_RX_STA = 0;
				return;
			}
			/*接收到头*/
			if((lpuart1_aRxBuffer[0] == RX_SMART_LOCK_DATA_STX) && (LPUART1_RX_STA == 0))
			{
				LPUART1_RX_STA |= RX_START_FLAG; //接收开始 
				LPUART1_RX_BUF[LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG] = lpuart1_aRxBuffer[0]; 			
				LPUART1_RX_STA++;
				return;
			}
			/*校验*/
			if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) >= 2)
			{
				if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) == 4)
					SmartLockDataLen = ((u16)LPUART1_RX_BUF[2]<<8) + ((u16)LPUART1_RX_BUF[3]);
				if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) < (SmartLockDataLen+4))
					SmartLockSumCheckTmp = (u32)lpuart1_aRxBuffer[0] + SmartLockSumCheckTmp;
				LPUART1_RX_BUF[LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG] = lpuart1_aRxBuffer[0];
				/*接收完毕*/
				if(((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG)-5) == SmartLockDataLen)
				{
					/*校验成功*/
					if(SmartLockSumCheckTmp == (((u32)LPUART1_RX_BUF[SmartLockDataLen+4] << 8) + (u32)LPUART1_RX_BUF[SmartLockDataLen+5]))
					{
//						DBG_PRINTF("\n\rCheck success\r\n");
						int iRet = Queue(name)->FnPutData(name, (char*)LPUART1_RX_BUF, LPUART1_RX_BUF_LEN);
//						if(iRet)
//							DBG_PRINTF("\n\rFnPutData failed: %d\r\n", iRet);
						LPUART1_RX_STA = 0;
						SmartLockSumCheckTmp = 0;
						SmartLockDataLen = 0xFFFF;
						return;
					}
					/*校验失败*/
					else
					{
//						DBG_PRINTF("\n\rSmart lock data check failed: 0x%02X\r\n", SmartLockSumCheckTmp);
						LPUART1_RX_STA = 0;
						SmartLockSumCheckTmp = 0;
						SmartLockDataLen = 0xFFFF;
						return;
					}
				}
				LPUART1_RX_STA++;
			}
			/*接收中*/
			else if(LPUART1_RX_STA&RX_START_FLAG)
			{	
				LPUART1_RX_BUF[LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG] = lpuart1_aRxBuffer[0];
				LPUART1_RX_STA++;						 
			}
		}		
	}
#endif

}


/*串口5中断服务程序*/
void USART4_5_IRQHandler(void)                	
{ 
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif

#if(UART5_USE_CALLBACK == 1)
	u32 timeout=0;
    u32 maxDelay=0x1FFFF;
	u32 uart_state;
	u32 uart_receive_state;
	HAL_UART_IRQHandler(&huart5);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&huart5)!=HAL_UART_STATE_READY)//等待就绪
	{
        timeout++;////超时处理
        uart_state = HAL_UART_GetState(&huart5);
        if(timeout>maxDelay){
			DBG_PRINTF("\n\rUart5 state is: 0x%x\r\n", uart_state);
			break;
			} 		
	}
     
	timeout=0;
	while(HAL_UART_Receive_IT(&huart5,(u8 *)uart5_aRxBuffer, RXBUFFERSIZE)!=HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
        timeout++; //超时处理
		uart_receive_state = HAL_UART_Receive_IT(&huart5,(u8 *)uart5_aRxBuffer, RXBUFFERSIZE);
        if(timeout>maxDelay){
			DBG_PRINTF("\n\rUart5 receive it state is: %d\r\n", uart_receive_state);
			break;
			}	
	}
	
#else
	int UartFlag = 0;
	UartFlag = __HAL_UART_GET_FLAG(&huart5, UART_FLAG_RXNE);
	if((UartFlag != 0))
	{
        HAL_UART_Receive(&huart5, (u8 *)uart5_aRxBuffer, RXBUFFERSIZE, RxTimeout);
		char *name = "Uart5";
		UART5_RX_BUF_Test[UART5_RX_STA_Test++] = uart5_aRxBuffer[0];
//		DBG_PRINTF("\n\r0x%02x\r\n", uart5_aRxBuffer[0]);
		if(UART5_RX_STA_Test == UART5_RX_BUF_LEN)//注意不要溢出
			UART5_RX_STA_Test = 0;
		if(USART5_RX_STA <= (u16)(UART5_RX_BUF_LEN-1))
			USART5_RX_BUF[USART5_RX_STA++] = uart5_aRxBuffer[0];
		else
		{
			int iRet = Queue(name)->FnPutData(name, (char*)USART5_RX_BUF, UART5_RX_BUF_LEN);
//			if(iRet)
//				DBG_PRINTF("\n\rFnPutData failed: %d\r\n", iRet);
			USART5_RX_STA = 0;
			USART5_RX_BUF[USART5_RX_STA++] = uart5_aRxBuffer[0];		
		} 
	}
	else
	{
//		DBG_PRINTF("\n\rUART flag is 0\r\n");
	}


#endif

#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
} 


/*串口1中断服务程序*/
void USART1_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif

#if(UART1_USE_CALLBACK == 1)
	u32 timeout=0;
    u32 maxDelay=0x0FFFF;
	u32 uart_state;
	u32 uart_receive_state;
	HAL_UART_IRQHandler(&huart1);	//调用HAL库中断处理公用函数
	
	timeout=0;
    while (HAL_UART_GetState(&huart1)!=HAL_UART_STATE_READY)//等待就绪
	{
        timeout++;////超时处理
        uart_state = HAL_UART_GetState(&huart1);
        if(timeout>maxDelay){
			DBG_PRINTF("\n\rUart1 state is: 0x%x\r\n", uart_state);
			break;
			} 		
	}
     
	timeout=0;
	while((uart_receive_state = HAL_UART_Receive_IT(&huart1,(u8 *)uart1_aRxBuffer, RXBUFFERSIZE))!=HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
        timeout++; //超时处理
        if(timeout>maxDelay){
			DBG_PRINTF("\n\rUart1 receive it state is: %d\r\n", uart_receive_state);
			break;
			}	
	}
	
#else
	int UartFlag = 0;
	UartFlag = __HAL_UART_GET_FLAG(&huart1,UART_FLAG_RXNE);
	if((UartFlag != 0))
	{
		char *name = "Uart1";
        HAL_UART_Receive(&huart1, (u8 *)uart1_aRxBuffer, RXBUFFERSIZE, RX_TIMEOUT);
		if((USART1_RX_STA&RX_END_FLAG) == 0)//接收未完成
		{
			/*索引溢出*/
			if((USART1_RX_STA&RX_BUFFER_INDEX_FLAG) >= UART1_RX_BUF_LEN)
			{
				USART1_RX_STA = 0;
//				DBG_PRINTF("\n\rUSART1_RX_BUF overflow\r\n");
			}
			/*接收到头*/
			else if((uart1_aRxBuffer[0] == RX_ZIGBEE_DATA_STX) && (USART1_RX_STA == 0))
			{
				USART1_RX_STA |= RX_START_FLAG; //接收开始 
				USART1_RX_BUF[USART1_RX_STA&RX_BUFFER_INDEX_FLAG] = uart1_aRxBuffer[0];
				ZigbeeXorCheckTmp = uart1_aRxBuffer[0];
				USART1_RX_STA++;
			}
			/*接收到0x55结束标志*/
			else if((uart1_aRxBuffer[0] == RX_ZIGBEE_DATA_END) && USART1_RX_STA)
			{
				/*接收到了0x55，同时校验字节为0，说明校验字节已经和正确的校验字节相等*/
				if(ZigbeeXorCheckTmp == 0)
				{ 
					USART1_RX_STA |= RX_END_FLAG;	//接收完成了 
					USART1_RX_BUF[USART1_RX_STA&RX_BUFFER_INDEX_FLAG]=uart1_aRxBuffer[0];
					int iRet = Queue(name)->FnPutData(name, (char*)USART1_RX_BUF, BUFFER_LEN);
//					if(iRet)
//						DBG_PRINTF("\n\rFnPutData failed: %d\r\n", iRet);
					USART1_RX_STA = 0;
					ZigbeeXorCheckTmp = 1;
				}
				/*否则仍然认为是数据*/
				else
				{
					USART1_RX_BUF[USART1_RX_STA&RX_BUFFER_INDEX_FLAG] = uart1_aRxBuffer[0];
					ZigbeeXorCheckTmp = uart1_aRxBuffer[0] ^ ZigbeeXorCheckTmp;/*异或校验*/
					USART1_RX_STA++;
				}
			}
			/*接收中*/
			else if(USART1_RX_STA&RX_START_FLAG)
			{	
				USART1_RX_BUF[USART1_RX_STA&RX_BUFFER_INDEX_FLAG]=uart1_aRxBuffer[0];
				ZigbeeXorCheckTmp = uart1_aRxBuffer[0] ^ ZigbeeXorCheckTmp;/*异或校验*/
				USART1_RX_STA++;					 
			}
		}

	}
	else
	{
//		DBG_PRINTF("\n\rUART1 flag is 0\r\n");
	}
//	HAL_UART_IRQHandler(&huart1); //调用HAL库中断处理公用函数，最终调用回调，可以不调用
//	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); //调用DBG_PRINTF可能会导致中断关闭，因此重新打开中断

#endif

#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif	
}


/*LPUART1中断服务程序*/
void AES_RNG_LPUART1_IRQHandler(void)                	
{
	
#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntEnter();    
#endif

/*HAL_UART_Receive_IT 方式*/
#if(LPUART1_USE_CALLBACK == 1) 
	u32 timeout=0;
    u32 maxDelay=0x1FFFF;
	u32 uart_state;
	u32 uart_receive_state;

	HAL_UART_IRQHandler(&hlpuart1); //调用HAL库中断处理公用函数

	timeout=0;
	while (HAL_UART_GetState(&hlpuart1)!=HAL_UART_STATE_READY)//等待就绪
	{
		timeout++;////超时处理
		uart_state = HAL_UART_GetState(&hlpuart1);
		if(timeout>maxDelay){
			DBG_PRINTF("\n\rLPUart1 state is: 0x%x\r\n", uart_state);
			hlpuart1.RxState = HAL_UART_STATE_READY;
			__HAL_UNLOCK(&hlpuart1);
			break;
			}		
	}
	 
	timeout=0;
	while((uart_receive_state = HAL_UART_Receive_IT(&hlpuart1,(u8 *)lpuart1_aRxBuffer, RXBUFFERSIZE))!=HAL_OK)//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
		timeout++; //超时处理
		if(timeout>maxDelay){
			DBG_PRINTF("\n\rLPUart1 receive it state is: %d\r\n", uart_receive_state);
			hlpuart1.RxState = HAL_UART_STATE_READY;
			__HAL_UNLOCK(&hlpuart1);
			break;
			}	
	}
	
/*HAL_UART_Receive 方式*/
#else 
	int UartFlag = 0;
	UartFlag = __HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_RXNE);
	if((UartFlag != 0))
	{
        HAL_UART_Receive(&hlpuart1, (u8 *)lpuart1_aRxBuffer, RXBUFFERSIZE, RX_TIMEOUT);
		char *name = "LPUart1";
		/*少量数据处理*/
		if(0 == (LPUART1_RX_STA&RX_END_FLAG) &&  0 == SmartLockLargeDataHandleFlag)
		{
			/*索引溢出*/
			if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) >= LPUART1_RX_BUF_LEN)//注意加括号
			{
//				DBG_PRINTF("\n\rLPUSART1_RX_BUF overflow\r\n");
				LPUART1_RX_STA = 0;
				goto LpuartExit;
			}
			/*接收到头*/
			if((lpuart1_aRxBuffer[0] == RX_SMART_LOCK_DATA_STX) && (LPUART1_RX_STA == 0))
			{
//				DBG_PRINTF("\n\r-\r\n");
				LPUART1_RX_STA |= RX_START_FLAG; //接收开始 
				LPUART1_RX_BUF[LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG] = lpuart1_aRxBuffer[0]; 			
				LPUART1_RX_STA++;
				goto LpuartExit;
			}
			/*校验*/
			if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) >= 2)
			{
				if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) == 4)
				{
					SmartLockDataLen = ((u16)LPUART1_RX_BUF[2]<<8) + ((u16)LPUART1_RX_BUF[3]);
					/*如果长度大于队列大小，进入特殊处理*/
					if(SmartLockDataLen >= (BUFFER_LEN-5))
					{
//						DBG_PRINTF("\n\rGet large data\r\n");
						SmartLockLargeDataHandleFlag = 1;
						memcpy(Lpuart1RxLargeBuf, LPUART1_RX_BUF, (LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG));
						Lpuart1RxLargeBuf[LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG] = lpuart1_aRxBuffer[0];
						SmartLockSumCheckTmp = (u32)lpuart1_aRxBuffer[0] + SmartLockSumCheckTmp;
						LPUART1_RX_STA++;
						goto LpuartExit;
					}
				}
				if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) < (SmartLockDataLen+4))
					SmartLockSumCheckTmp = (u32)lpuart1_aRxBuffer[0] + SmartLockSumCheckTmp;
				LPUART1_RX_BUF[LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG] = lpuart1_aRxBuffer[0];
				/*接收完毕*/
				if(((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG)-5) == SmartLockDataLen)
				{
					/*校验成功*/
					if(SmartLockSumCheckTmp == (((u32)LPUART1_RX_BUF[SmartLockDataLen+4] << 8) + (u32)LPUART1_RX_BUF[SmartLockDataLen+5]))
					{
						DBG_PRINTF("\n\rCheck success\r\n");
						int iRet = Queue(name)->FnPutData(name, (char*)LPUART1_RX_BUF, LPUART1_RX_BUF_LEN);
//						if(iRet)
//							DBG_PRINTF("\n\rFnPutData failed: %d\r\n", iRet);
						LPUART1_RX_STA = 0;
						SmartLockSumCheckTmp = 0;
						SmartLockDataLen = 0xFFFF;
						goto LpuartExit;
					}
					/*校验失败*/
					else
					{
						DBG_PRINTF("\n\rSmart lock data check failed,should 0x%04X\r\n", SmartLockSumCheckTmp);
						SmartLockSumCheckTmp = ((u32)LPUART1_RX_BUF[SmartLockDataLen+4] << 8) + (u32)LPUART1_RX_BUF[SmartLockDataLen+5];
						DBG_PRINTF("\n\rSmart lock data check failed,but 0x%04X\r\n", SmartLockSumCheckTmp);
						LPUART1_RX_STA = 0;
						SmartLockSumCheckTmp = 0;
						SmartLockDataLen = 0xFFFF;
						goto LpuartExit;
					}
				}
				LPUART1_RX_STA++;
			}
			/*接收中*/
			else if(LPUART1_RX_STA&RX_START_FLAG)
			{				
//				DBG_PRINTF("\n\r-\r\n");
				LPUART1_RX_BUF[LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG] = lpuart1_aRxBuffer[0];
				LPUART1_RX_STA++;						 
			}
		}	
		/*大量数据处理*/
		else if(0 == (LPUART1_RX_STA&RX_END_FLAG) &&  1 == SmartLockLargeDataHandleFlag)
		{
			/*索引溢出*/
			if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) >= SMART_LOCK_LARGE_BUF_LEN)//注意加括号
			{
//				DBG_PRINTF("\n\rLpuart1RxLargeBuf overflow\r\n");
				LPUART1_RX_STA = 0;
				SmartLockLargeDataHandleFlag = 0;
				LargeDataFromSmartLock = 0;
				goto LpuartExit;
			}
			if((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG) < (SmartLockDataLen+4))
				SmartLockSumCheckTmp = (u32)lpuart1_aRxBuffer[0] + SmartLockSumCheckTmp;
			Lpuart1RxLargeBuf[LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG] = lpuart1_aRxBuffer[0];
//			DBG_PRINTF("\n\r--\r\n");
			/*接收完毕*/
			if(((LPUART1_RX_STA&RX_BUFFER_INDEX_FLAG)-5) == SmartLockDataLen)
			{
				/*校验成功*/
				if(SmartLockSumCheckTmp == (((u32)Lpuart1RxLargeBuf[SmartLockDataLen+4] << 8) + (u32)Lpuart1RxLargeBuf[SmartLockDataLen+5]))
				{
					DBG_PRINTF("\n\rLarge data check success\r\n");
					LargeDataFromSmartLock = 1;		
					SmartLockLargeDataHandleFlag = 0;
					LPUART1_RX_STA = 0;
					SmartLockSumCheckTmp = 0;
					SmartLockDataLen = 0xFFFF;
					goto LpuartExit;
				}
				/*校验失败*/
				else
				{
					DBG_PRINTF("\n\rSmart lock data check failed,should 0x%04X\r\n", SmartLockSumCheckTmp);
					SmartLockSumCheckTmp = ((u32)Lpuart1RxLargeBuf[SmartLockDataLen+4] << 8) + (u32)Lpuart1RxLargeBuf[SmartLockDataLen+5];
					DBG_PRINTF("\n\rSmart lock data check failed,but 0x%04X\r\n", SmartLockSumCheckTmp);
					LargeDataFromSmartLock = 1;//暂时校验失败也获取数据		
					SmartLockLargeDataHandleFlag = 0;
					LPUART1_RX_STA = 0;
					SmartLockSumCheckTmp = 0;
					SmartLockDataLen = 0xFFFF;
					goto LpuartExit;
				}
			}
			LPUART1_RX_STA++;
		}
	}
	else
	{
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_REACK);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_REACK\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_TEACK);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_TEACK\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_WUF);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_WUF\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_RWU);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_RWU\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_SBKF);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_SBKF\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_CMF);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_CMF\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_BUSY);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_BUSY\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_ABRF);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_ABRF\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_ABRE);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_ABRE\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_EOBF);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_EOBF\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_RTOF);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_RTOF\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_CTS);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_CTS\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_CTSIF);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_CTSIF\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_LBDF);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_LBDF\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_TXE);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_TXE\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_TC);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_TC\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_RXNE);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_RXNE\r\n");
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_IDLE);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_IDLE\r\n");
			__HAL_UART_CLEAR_IDLEFLAG(&hlpuart1);
			HAL_UART_Receive(&hlpuart1, (u8 *)lpuart1_aRxBuffer, RXBUFFERSIZE, RX_TIMEOUT);
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_ORE);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_ORE\r\n");
			__HAL_UART_CLEAR_OREFLAG(&hlpuart1);
			HAL_UART_Receive(&hlpuart1, (u8 *)lpuart1_aRxBuffer, RXBUFFERSIZE, RX_TIMEOUT);
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_NE);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_NE\r\n");
			__HAL_UART_CLEAR_NEFLAG(&hlpuart1);
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_FE);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_FE\r\n");
			__HAL_UART_CLEAR_FEFLAG(&hlpuart1);
		}
		UartFlag = __HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_PE);
		if(UartFlag!=0)
		{
//			DBG_PRINTF("\n\rLPUART1 flag is UART_FLAG_PE\r\n");
			__HAL_UART_CLEAR_PEFLAG(&hlpuart1);
		}
	}

LpuartExit:
	;
//	HAL_UART_IRQHandler(&hlpuart1); //调用HAL库中断处理公用函数，最终调用回调，可以不调用
//	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE); //调用DBG_PRINTF可能会导致中断关闭，因此重新打开中断
#endif

#if SYSTEM_SUPPORT_OS	 	//使用OS
	OSIntExit();  											 
#endif
} 


//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	if(huart->Instance==LPUART1)
	{
		/* Peripheral clock enable */
		__HAL_RCC_LPUART1_CLK_ENABLE();

		/**LPUART1 GPIO Configuration    
		PA2     ------> LPUART1_TX
		PA3     ------> LPUART1_RX 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF6_LPUART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* LPUART1 interrupt Init */
		HAL_NVIC_SetPriority(AES_RNG_LPUART1_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(AES_RNG_LPUART1_IRQn);
	}
	else if(huart->Instance==USART1)
	{
		/* Peripheral clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();

		/**USART1 GPIO Configuration    
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
	else if(huart->Instance==USART5)
	{
		/* Peripheral clock enable */
		__HAL_RCC_USART5_CLK_ENABLE();

		/**USART5 GPIO Configuration    
		PB3     ------> USART5_TX
		PB4     ------> USART5_RX 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF6_USART5;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* USART5 interrupt Init */
		HAL_NVIC_SetPriority(USART4_5_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(USART4_5_IRQn);
	}

}


//
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspDeInit 0 */

  /* USER CODE END LPUART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPUART1_CLK_DISABLE();
  
    /**LPUART1 GPIO Configuration    
    PA2     ------> LPUART1_TX
    PA3     ------> LPUART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* LPUART1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(AES_RNG_LPUART1_IRQn);
  /* USER CODE BEGIN LPUART1_MspDeInit 1 */

  /* USER CODE END LPUART1_MspDeInit 1 */
  }
  else if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(huart->Instance==USART5)
  {
  /* USER CODE BEGIN USART5_MspDeInit 0 */

  /* USER CODE END USART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART5_CLK_DISABLE();
  
    /**USART5 GPIO Configuration    
    PB3     ------> USART5_TX
    PB4     ------> USART5_RX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4);

    /* USART5 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART4_5_IRQn);
  /* USER CODE BEGIN USART5_MspDeInit 1 */

  /* USER CODE END USART5_MspDeInit 1 */
  }

}


/* LPUART1 init function */
void MX_LPUART1_UART_Init(void)
{

  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 57600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)//HAL_UART_Init会调用HAL_UART_MspInit进行对应的IO口设置
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}


/* USART1 init function */
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 57600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)//HAL_UART_Init会调用HAL_UART_MspInit进行对应的IO口设置
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}


/* USART5 init function */
void MX_USART5_UART_Init(void)
{

  	huart5.Instance = USART5;
  	huart5.Init.BaudRate = Dbg_BaudRate;
  	huart5.Init.WordLength = UART_WORDLENGTH_8B;
  	huart5.Init.StopBits = UART_STOPBITS_1;
  	huart5.Init.Parity = UART_PARITY_NONE;
  	huart5.Init.Mode = UART_MODE_TX_RX;
  	huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  	huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  	huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  	huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart5) != HAL_OK)//HAL_UART_Init会调用HAL_UART_MspInit进行对应的IO口设置
	{
	_Error_Handler(__FILE__, __LINE__);
	}
}


/*门锁串口模块初始化*/
int SmartLockCommModuleInit(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	MX_LPUART1_UART_Init();
	RegisterQueue(&LPUart1QueueModule);
	Queue("LPUart1")->ModuleInit("LPUart1");
	
	/*开启接收中断*/
#if(LPUART1_USE_CALLBACK == 1) //方法1
	status = HAL_UART_Receive_IT(&hlpuart1, (u8 *)lpuart1_aRxBuffer, RXBUFFERSIZE);
#else //方法2
	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);
#endif
	return (int)status;
}


/*打开门锁串口模块*/
int SmartLockCommModuleOpen(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	MX_LPUART1_UART_Init();

	/*开启接收中断*/
#if(LPUART1_USE_CALLBACK == 1) //方法1
	status = HAL_UART_Receive_IT(&hlpuart1, (u8 *)lpuart1_aRxBuffer, RXBUFFERSIZE);
#else //方法2
	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);
#endif
	return (int)status;
}


/*门锁串口模块关闭*/
int SmartLockCommModuleClose(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	/*关闭中断*/
#if(LPUART1_USE_CALLBACK == 1) //方法1
	status = HAL_UART_AbortReceive_IT(&hlpuart1);
#else //方法2
	__HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_RXNE);
#endif
	status = HAL_UART_DeInit(&hlpuart1);
	return (int)status;
}


/*门锁串口模块状态监测
 *1 -- 数据接收中
 *2 -- 缓冲区非空
 *0 -- 模块处于闲置状态
 */
int SmartLockStatusDetect(void)
{
	if(0 != LPUART1_RX_STA)
		return 1;
	else if(0 == LPUart1QueueModule.FnEmptyDetect("LPUart1"))
		return 2;
	else
		return 0;
}


/*Zigbee串口模块初始化*/
int ZigbeeCommModuleInit(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	MX_USART1_UART_Init();
	RegisterQueue(&Uart1QueueModule);
	Queue("Uart1")->ModuleInit("Uart1");

#if(UART1_USE_CALLBACK == 1)
	status = HAL_UART_Receive_IT(&huart1, (u8 *)uart1_aRxBuffer, RXBUFFERSIZE);
#else
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
#endif
	return (int)status;
}


/*打开zigbee串口模块*/
int ZigbeeCommModuleOpen(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	MX_USART1_UART_Init();

#if(UART1_USE_CALLBACK == 1)
	status = HAL_UART_Receive_IT(&huart1, (u8 *)uart1_aRxBuffer, RXBUFFERSIZE);
#else
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
#endif
	return (int)status;
}


/*Zigbee串口模块关闭*/
int ZigbeeCommModuleClose(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	/*关闭中断*/
#if(LPUART1_USE_CALLBACK == 1) //方法1
	status = HAL_UART_AbortReceive_IT(&huart1);
#else //方法2
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
#endif
	status = HAL_UART_DeInit(&huart1);
	return (int)status;
}


/*zigbee串口模块状态监测
 *1 -- 数据接收中
 *2 -- 缓冲区非空
 *0 -- 模块处于闲置状态
 */
int ZigbeeStatusDetect(void)
{
	if(0 != USART1_RX_STA)
		return 1;
	else if(0 == Uart1QueueModule.FnEmptyDetect("Uart1"))
		return 2;
	else
		return 0;
}


/*调试串口模块初始化*/
int DebugCommModuleInit(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	MX_USART5_UART_Init();
	RegisterQueue(&Uart5QueueModule);
	Queue("Uart5")->ModuleInit("Uart5");

#if(UART5_RX_ENABLE == 1)
#if(UART5_USE_CALLBACK == 1)
	status = HAL_UART_Receive_IT(&huart5, (u8 *)uart5_aRxBuffer, RXBUFFERSIZE); 
#else
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);
#endif
#endif
	return (int)status;
}


/*打开调试串口模块*/
int DebugCommModuleOpen(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	MX_USART5_UART_Init();

#if(UART5_RX_ENABLE == 1)
#if(UART5_USE_CALLBACK == 1)
	status = HAL_UART_Receive_IT(&huart5, (u8 *)uart5_aRxBuffer, RXBUFFERSIZE); 
#else
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE);
#endif
#endif
	return (int)status;
}


/*调试串口模块关闭*/
int DebugCommModuleClose(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	/*关闭中断*/
#if(LPUART1_USE_CALLBACK == 1) //方法1
	status = HAL_UART_AbortReceive_IT(&huart5);
#else //方法2
	__HAL_UART_DISABLE_IT(&huart5, UART_IT_RXNE);
#endif
	status = HAL_UART_DeInit(&huart5);
	return (int)status;
}


/*调试口模块状态监测
 *1 -- 数据接收中
 *2 -- 缓冲区非空
 *0 -- 模块处于闲置状态
 */
int DebugPortStatusDetect(void)
{
	if(0 != USART5_RX_STA)
		return 1;
	else if(0 == Uart5QueueModule.FnEmptyDetect("Uart5"))
		return 2;
	else
		return 0;
}


/*发送数据给菲格特静脉门锁*/
int SendDataToSmartLock(u8 *pcData, u16 Size, u32 Timeout)
{
	
#if(LPUART1_USE_CALLBACK == 1) 
	__HAL_UART_DISABLE_IT(&hlpuart1, UART_IT_RXNE);
#endif
	u8 WakupDoorBuf[2] = {0x00};//用于唤醒门锁
	HAL_StatusTypeDef iRet;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delay_ms(20);//延时20ms
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delay_ms(300);
#ifdef USE_IWDG
	/*喂狗*/
	IWDG_Feed();
#endif
	HAL_UART_AbortReceive(&hlpuart1);
	iRet = HAL_UART_Transmit(&hlpuart1, WakupDoorBuf, 1, TX_TIMEOUT);
	while(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_TC)!=SET);		
	delay_ms(100);//根据协议要求延时
	iRet = HAL_UART_Transmit(&hlpuart1, pcData, Size, Timeout);
	while(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_TC)!=SET);
	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);
#if(LPUART1_USE_CALLBACK == 1) 
	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);
#endif
	return (int)iRet;
}


/*发送数据给zigbee*/
int SendDataToZigbee(u8 * pcData, u16 Size, u32 Timeout)
{
	HAL_UART_AbortReceive(&huart1);
	HAL_StatusTypeDef iRet;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	delay_ms(20);//延时20ms
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	delay_ms(20);
	iRet = HAL_UART_Transmit(&huart1, pcData, Size, Timeout);
	while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC)!=SET);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	return (int)iRet;
}


/*发送数据给调试口*/
int SendDataToDebugPort(u8 *pcData, u16 Size, u32 Timeout)
{
	return 0;
}


/*唤醒门锁*/
void WkupSmartLock()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delay_ms(10);//10ms高电平触发
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	delay_ms(100);//100ms后发数据
}


/*唤醒zigbee*/
void WkupZigbee()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	delay_ms(20);//20ms高电平触发
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	delay_ms(20);//20ms后发数据
}


/*唤醒调试器*/
void WkupDebug()
{
	;
}


/*从门锁获取数据*/
//0 - 失败(无数据) , >=0 - 数据长度
int GetDataFromSmartLock(char* pcData)
{
	char* name = "LPUart1";
	int iRet = 0;
	iRet = Queue(name)->FnGetData(name, pcData);
	return iRet;
}


/*从门锁获取大数据量*/
//0 - 失败(无数据) , >=0 - 数据长度
int GetLageDataFromSmartLock(char* pcLargeData)
{
	int iRet = 0;
	if (0 == LargeDataFromSmartLock)
		iRet = 0;//空了说明没有数据
	else
	{
		memcpy(pcLargeData, Lpuart1RxLargeBuf, SMART_LOCK_LARGE_BUF_LEN);		
		LargeDataFromSmartLock = 0;
		iRet = SMART_LOCK_LARGE_BUF_LEN;
	}
	return iRet;
}


/*从zigbee获取数据*/
int GetDataFromZigbee(char* pcData)
{
	char *name = "Uart1";
	int iRet = 0;
	iRet = Queue(name)->FnGetData(name, pcData);
	return iRet;
}


/*从调试口获取数据*/
int GetDataFromDebugPort(char* pcData)
{
	char *name = "Uart5";
	int iRet = 0;
	iRet = Queue(name)->FnGetData(name, pcData);
	return iRet;
}


/*注册通讯模块*/
int RegisterCommModule(PT_CommunicationModule ptCommModule)
{
	PT_CommunicationModule ptTmp;

	if (!ptCommModuleHead)
	{
		ptCommModuleHead = ptCommModule;
		ptCommModule->ptNext = NULL;
	}
	else
	{
		ptTmp = ptCommModuleHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext = ptCommModule;
		ptCommModule->ptNext = NULL;
	}
	return 0;
}


/*通过名字获取通讯模块对象
 */   
PT_CommunicationModule CommModule(char *name)
{
	PT_CommunicationModule ptTmp = ptCommModuleHead;
	
	while (ptTmp)
	{
		if (strncmp(ptTmp->name, name, strlen(ptTmp->name)) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}


/*注册消息监测模块*/
int RegisterMsgSendMonitor(PT_MsgSendMonitor ptMsgSendMonitor)
{
	PT_MsgSendMonitor ptTmp;

	if (!ptMsgSendMonitorHead)
	{
		ptMsgSendMonitorHead = ptMsgSendMonitor;
		ptMsgSendMonitor->ptNext = NULL;
	}
	else
	{
		ptTmp = ptMsgSendMonitorHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext = ptMsgSendMonitor;
		ptMsgSendMonitor->ptNext = NULL;
	}
	return 0;
}


/*消息监控时间计数*/
void MsgSendMonitorTimCntAdd()
{
	PT_MsgSendMonitor ptTmp = ptMsgSendMonitorHead;
	while (ptTmp)
	{
		if(ptTmp->IsCanUsed)
		{
			ptTmp->TimeCout++;
		}
		ptTmp = ptTmp->ptNext;
	}
}


/*通过名字获取消息监控对象*/
PT_MsgSendMonitor MsgSendMonitor(char *name)
{
	PT_MsgSendMonitor ptTmp = ptMsgSendMonitorHead;

	while (ptTmp)
	{
		if (strncmp(ptTmp->MsgName, name, strlen(ptTmp->MsgName)) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}


/*打开消息监控
 *0 - 成功 ； -1 - 没查到此消息
 */
int OpenMsgSendMonitor(char *name)
{
	PT_MsgSendMonitor ptTmp = ptMsgSendMonitorHead;

	while (ptTmp)
	{
		if (strncmp(ptTmp->MsgName, name, strlen(ptTmp->MsgName)) == 0)
		{
			ptTmp->IsCanUsed = 1;
			ptTmp->SendCout = 0;
			ptTmp->TimeCout = 0;
			return 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return -1;
}


/*关闭消息监控
 *0 - 成功 ； -1 - 没查到此消息
 */
int CloseMsgSendMonitor(char *name)
{
	PT_MsgSendMonitor ptTmp = ptMsgSendMonitorHead;

	while (ptTmp)
	{
		if (strncmp(ptTmp->MsgName, name, strlen(ptTmp->MsgName)) == 0)
		{
			ptTmp->IsCanUsed = 0;
			ptTmp->SendCout = 0;
			ptTmp->TimeCout = 0;
			return 0;
		}
		ptTmp = ptTmp->ptNext;
	}
	return -1;
}


/*检查超时消息并重新发送*/
void MsgSendMonitorCheckAndReSend()
{
	PT_MsgSendMonitor ptTmp = ptMsgSendMonitorHead;
	while (ptTmp)
	{
		if(ptTmp->IsCanUsed)
		{
			if(ptTmp->SendCout >= MSG_SEND_MONITOR_STOP_RESEND_CNT)
			{
				DBG_PRINTF("\n\rClose MsgSendMonitor: %s\r\n", ptTmp->MsgName);
				int iRet = CloseMsgSendMonitor(ptTmp->MsgName);
				if(iRet)
					DBG_PRINTF("\n\rClose MsgSendMonitor: %s failed\r\n", ptTmp->MsgName);
				goto FindNext;
			}
			if(ptTmp->TimeCout >= ptTmp->SetTimeOut)
			{
				ptTmp->ptCommunicationModule->FnSendData(ptTmp->SendData, ptTmp->MsgLen, TX_TIMEOUT);
				ptTmp->TimeCout = 0;
				ptTmp->SendCout++;
				DBG_PRINTF("\n\rReSend message: %s\r\n", ptTmp->MsgName);
			}
		}
FindNext:
		ptTmp = ptTmp->ptNext;
	}
}


/*获取消息监控工作状态
 *1 -- 工作中(有消息正在重发)
 *0 -- 闲置中
 */
int GetMsgMonitorStatus(void)
{
	int iRet = 0;
	PT_MsgSendMonitor ptTmp = ptMsgSendMonitorHead;
	while (ptTmp)
	{
		if(ptTmp->IsCanUsed)
		{
			iRet = 1;
			goto exit;
		}
		ptTmp = ptTmp->ptNext;
	}
exit:
	return iRet;
}


/****************************************************************************/
/*                                                                          */
/*  End of file.                                                            */
/*                                                                          */
/****************************************************************************/



