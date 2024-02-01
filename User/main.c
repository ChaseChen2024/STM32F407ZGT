/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 �̼�������
  *********************************************************************
  * @attention
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_debug_usart.h"
#include "./flash/bsp_spi_flash.h"
#include "stm32f4x7_phy.h"
#include "ff.h"
//#include "client.h"
#include "./sram/bsp_sram.h"	
#include <string.h>
#include "module.h"

//demo ����ͷ�ļ�
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
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
//���ñ������嵽��EXRAM�������ĺ�
#define __EXRAM  __attribute__ ((section (".ext_sram")))

//���ڴ涨����SDRAM��
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __EXRAM;

//�������鵽SDRAM
uint8_t testGrup[3] __EXRAM ={1,2,3};
//�������鵽SRAM
uint8_t testGrup2[3] ={1,2,3};
// static uint32_t aMemory[100] __attribute__ ((section (".ext_sram")));
// static uint32_t aMemory2[100] __attribute__ ((section (".ext_sram")));
// static uint32_t aMemory3[100] __attribute__ ((section (".ext_sram")));
/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */



/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
 



/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();
  
  printf("����һ��-STM32ȫϵ�п�����-FreeRTOS�̼������̣�\n\n");

  
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}

static TaskHandle_t Ftp_Demo_Task_Handle = NULL;/* KEY������ */
static void Ftp_Demo_Task(void* parameter)
{
		
    while (1)
    {
      vTaskDelay(1000);
    }
    
}
/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  // LwIP_Init();
  // tftpInit();
  // Sntp_BSP_Init();
  //  file_init();
  //   ftpd_init();
  //  SDRAM_Test();
  // client_init();
  taskENTER_CRITICAL();           //�����ٽ���

  printf("��ʼ��������!\r\n");
 
#if 0
	xReturn = FatFs_Demo_Task_Init();
	if(pdPASS == xReturn)
			printf("����FatFs_Demo_Task����ɹ�!\r\n");
#endif
#if 1
  xReturn = Key_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("����KEY_Task����ɹ�!\r\n");
#endif
#if 0
  xReturn = Usb_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("����Test_Task����ɹ�!\r\n");
#endif
#if 0
	xReturn = Sdio_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("����SDIO_Demo_Task����ɹ�!\r\n");
#endif
#if 0
  xReturn = RTC_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("����RTC_Task����ɹ�!\r\n");
#endif
#if 0
  xReturn = SNTP_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("����SNTP-Task����ɹ�!\r\n");
#endif
#if 0
  AT_PROC_Task_Init();
  // xReturn = Tftp_Demo_Task_Init();
  // if(pdPASS == xReturn)
  //   printf("����Tftp-Task����ɹ�!\r\n");
#endif
printf("xReturn=%ld\r\n",xReturn);  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	//LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	Debug_USART_Config();
  // MODULE_INIT();
  FSMC_SRAM_Init();
  Key_Bsp_Init();
  Rtc_Bsp_Init();
	//  ETH_BSP_Config();
  
  // printf("��ʼ��ʼ��LWIP\n\n");
  // LwIP_Init();
    
  printf("BSP��ʼ�����!\r\n");

  
  /* Initilaize the LwIP stack */
 // LwIP_Init();
	//SPI_FLASH_Init();

}

/********************************END OF FILE****************************/
