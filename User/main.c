/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 固件库例程
  *********************************************************************
  * @attention
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "./flash/bsp_spi_flash.h"
#include "stm32f4x7_phy.h"
#include "ff.h"
//#include "client.h"
#include "./sram/bsp_sram.h"	
#include <string.h>
#include "module.h"

//demo 用例头文件
#include "FatFs_demo.h"

#include "Key_demo.h"
//#include "Usb_demo.h"
//#include "Sdio_demo.h"
#include "Lwip_Tcp_Client_Socket_demo.h"
#include "Rtc_demo.h"
#include "Sntp_demo.h"
#include "Tftp_demo.h"
#include "client.h"
#include "atproc.h"
//extern void TCPIP_Init(void);
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
//设置变量定义到“EXRAM”节区的宏
#define __EXRAM  __attribute__ ((section (".ext_sram")))

//将内存定义在SDRAM上
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __EXRAM;

//定义数组到SDRAM
uint8_t testGrup[3] __EXRAM ={1,2,3};
//定义数组到SRAM
uint8_t testGrup2[3] ={1,2,3};
// static uint32_t aMemory[100] __attribute__ ((section (".ext_sram")));
// static uint32_t aMemory2[100] __attribute__ ((section (".ext_sram")));
// static uint32_t aMemory3[100] __attribute__ ((section (".ext_sram")));
/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */



/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
 



/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */

static void BSP_Init(void);/* 用于初始化板载相关资源 */

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  /* 开发板硬件初始化 */
  BSP_Init();
  
  printf("这是一个-STM32全系列开发板-FreeRTOS固件库例程！\n\n");

  
   /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}

static TaskHandle_t Ftp_Demo_Task_Handle = NULL;/* KEY任务句柄 */
static void Ftp_Demo_Task(void* parameter)
{
		
    while (1)
    {
      vTaskDelay(1000);
    }
    
}
/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  // LwIP_Init();
  // tftpInit();
  // Sntp_BSP_Init();
  //  file_init();
  //   ftpd_init();
  //  SDRAM_Test();
  // client_init();
  taskENTER_CRITICAL();           //进入临界区

  printf("开始创建任务!\r\n");
 
#if 0
	xReturn = FatFs_Demo_Task_Init();
	if(pdPASS == xReturn)
			printf("创建FatFs_Demo_Task任务成功!\r\n");
#endif
#if 1
  xReturn = Key_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("创建KEY_Task任务成功!\r\n");
#endif
#if 0
  xReturn = Usb_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("创建Test_Task任务成功!\r\n");
#endif
#if 0
	xReturn = Sdio_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("创建SDIO_Demo_Task任务成功!\r\n");
#endif
#if 0
  xReturn = RTC_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("创建RTC_Task任务成功!\r\n");
#endif
#if 0
  xReturn = SNTP_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("创建SNTP-Task任务成功!\r\n");
#endif
#if 0
  AT_PROC_Task_Init();
  // xReturn = Tftp_Demo_Task_Init();
  // if(pdPASS == xReturn)
  //   printf("创建Tftp-Task任务成功!\r\n");
#endif
printf("xReturn=%ld\r\n",xReturn);  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}



/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	//LED_GPIO_Config();

	/* 串口初始化	*/
	Debug_USART_Config();
  // MODULE_INIT();
  FSMC_SRAM_Init();
  Key_Bsp_Init();
  Rtc_Bsp_Init();
	//  ETH_BSP_Config();
  
  // printf("开始初始化LWIP\n\n");
  // LwIP_Init();
    
  printf("BSP初始化完成!\r\n");

  
  /* Initilaize the LwIP stack */
 // LwIP_Init();
	//SPI_FLASH_Init();

}

/********************************END OF FILE****************************/
