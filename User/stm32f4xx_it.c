/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"

#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h" 
#include "queue.h"
#include "semphr.h"

#include "user_define.h"
#ifdef USE_LWIP_CODE
/* lwip includes */
#include "lwip/sys.h"
#include "stm32f4x7_eth.h"
#endif
#include "bsp_debug_usart.h"
#include<string.h>
// #include "./flash/bsp_spi_flash.h"

// #include "usbd_msc_core.h"
// #include "usbd_usr.h"
// #include "usbd_desc.h"
// #include "usb_conf.h"

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	printf("\r\n  HardFault_Handler");
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void xPortSysTickHandler(void);
//systick中断服务函数
void SysTick_Handler(void)
{	
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  /* INCLUDE_xTaskGetSchedulerState */  
        xPortSysTickHandler();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  /* INCLUDE_xTaskGetSchedulerState */
}
#ifdef USE_LWIP_CODE
/**
  * @brief  This function handles ethernet DMA interrupt request.
  * @param  None
  * @retval None
  */
 extern xSemaphoreHandle s_xSemaphore;
extern xSemaphoreHandle ETH_link_xSemaphore;
void ETH_IRQHandler(void)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  /* Frame received */
  if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET) 
  {
    /* Give the semaphore to wakeup LwIP task */
    xSemaphoreGiveFromISR( s_xSemaphore, &xHigherPriorityTaskWoken );   
  }

  /* Clear the interrupt flags. */
  /* Clear the Eth DMA Rx IT pending bits */
  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);

  /* Switch tasks if necessary. */	
  if( xHigherPriorityTaskWoken != pdFALSE )
  {
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  }
}
#include "module.h"
// #include "atproc.h"
void MODULE_USART_IRQHandler(void)
{

    uint32_t ulReturn;
  /* 进入临界段，临界段可以嵌套 */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();

	if(USART_GetITStatus(MODULE_USART,USART_IT_IDLE)!=RESET)
	{		
		Uart_DMA_Rx_Data();       /* 释放一个信号量，表示数据已接收 */
		USART_ReceiveData(MODULE_USART); /* 清除标志位 */
	}	 
  
  /* 退出临界段 */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}	
#endif
#include "bsp_usart6.h"
void USART6_IRQHandler(void)  
{
	uint32_t ulReturn;
  u8 rec_data;
  ulReturn = taskENTER_CRITICAL_FROM_ISR();

	if(USART_GetITStatus(USART6,USART_IT_IDLE)!=RESET)
	{		
		Uart6_DMA_Rx_Data();
		rec_data = USART_ReceiveData(USART6);
	}	 
  
  /* 退出临界段 */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
} 

// extern USB_OTG_CORE_HANDLE  USB_OTG_dev;

// /* Private function prototypes -----------------------------------------------*/
// extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

// #ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
// extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
// extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
// #endif

// #ifdef USE_USB_OTG_FS  
// void OTG_FS_IRQHandler(void)
// {
//   uint32_t ulReturn;
//   /* 进入临界段，临界段可以嵌套 */
//   ulReturn = taskENTER_CRITICAL_FROM_ISR();
	
//   USBD_OTG_ISR_Handler (&USB_OTG_dev);
	
//   /* 退出临界段 */
//   taskEXIT_CRITICAL_FROM_ISR( ulReturn );
// }
// #endif

// #ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
// /**
//   * @brief  This function handles EP1_IN Handler.
//   * @param  None
//   * @retval None
//   */
// void OTG_HS_EP1_IN_IRQHandler(void)
// {
//   uint32_t ulReturn;
//   /* 进入临界段，临界段可以嵌套 */
//   ulReturn = taskENTER_CRITICAL_FROM_ISR();

//   USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);

//   /* 退出临界段 */
//   taskEXIT_CRITICAL_FROM_ISR( ulReturn );
// }

// /**
//   * @brief  This function handles EP1_OUT Handler.
//   * @param  None
//   * @retval None
//   */
// void OTG_HS_EP1_OUT_IRQHandler(void)
// {
//   uint32_t ulReturn;
//   /* 进入临界段，临界段可以嵌套 */
//   ulReturn = taskENTER_CRITICAL_FROM_ISR();
	
//   USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
	  
// 	/* 退出临界段 */
//   taskEXIT_CRITICAL_FROM_ISR( ulReturn );
// }
// #endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
