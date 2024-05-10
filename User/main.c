
#include "user.h"
#include "user_define.h"



#include "lcd.h"
#include "ff.h"
#include <string.h>
#include "module.h"
#include "lvgl.h"
// #include "lv_demos.h"
// #include "lv_demo_widgets.h"
/*demo test*/
#include "FatFs_demo.h"
#include "Key_demo.h"
//#include "Usb_demo.h"
//#include "Sdio_demo.h"
#ifdef USE_LWIP_CODE
#include "Lwip_Tcp_Client_Socket_demo.h"
#include "Sntp_demo.h"
#include "client.h"
#endif // USE_LWIP

#include "Rtc_demo.h"

#include "Tftp_demo.h"
#include "Lcd_demo.h"

#include "atproc.h"

#include "my_gui.h"
#include "gnss.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;

//  FREERTOS EXRAM
//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __EXRAM;


static void AppTaskCreate(void);

static void BSP_Init(void);
SemaphoreHandle_t MutexSemaphore;
static void lvgl_handler( void *pvParameters )
{
  int a=0;
  printf("[%s],[%d],[0x%x]\r\n",__FUNCTION__,__LINE__,&a);
  for( ;; )
  {    
    xSemaphoreTake(MutexSemaphore,portMAX_DELAY);  
    lv_task_handler();
    xSemaphoreGive(MutexSemaphore); 
    vTaskDelay(10);
  }
}



int main(void)
{	
  BaseType_t xReturn = pdPASS;
  
  BSP_Init();

  
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  
                        (const char*    )"AppTaskCreate",
                        (uint16_t       )512, 
                        (void*          )NULL,
                        (UBaseType_t    )1,
                        (TaskHandle_t*  )&AppTaskCreate_Handle);
        
  if(pdPASS == xReturn)
    vTaskStartScheduler();
  else
    return -1;  
  
  while(1);
}

extern void lv_demo_widgets(void);
extern void lv_demo_stress(void);
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;

#ifdef USE_LWIP_CODE
 // LwIP_Init();
// tftpInit();
// Sntp_BSP_Init();
//  file_init();
//   ftpd_init();
//  SDRAM_Test();
// client_init();
#endif // USE_LWIP
  /*lvgl init ,must in task create */
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();
  // lv_port_fs_init();  
  // lv_demo_widgets();
  // lv_demo_stress();
  
  my_gui_xtrack();
  //my_gui();
  printf("[%s],[%d],[%s]\r\n",__FUNCTION__,__LINE__,"run my gui xtrack");
  MutexSemaphore=xSemaphoreCreateMutex(); 
  xTaskCreate(lvgl_handler, "lvgl_handler", 1000, NULL, 3, NULL);
  printf("[%s],[%d],[%s]\r\n",__FUNCTION__,__LINE__,"run lvgl handler");
  taskENTER_CRITICAL();

  Gnss_Resolve_Task_Init();
#if 0
	xReturn = FatFs_Demo_Task_Init();
	if(pdPASS == xReturn)
			printf("ĺĺťşFatFs_Demo_TaskäťťĺĄćĺ!\r\n");
#endif
#if 0
  xReturn = Key_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("ĺĺťşKEY_TaskäťťĺĄćĺ!\r\n");
#endif
#if 0
  xReturn = Usb_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("ĺĺťşTest_TaskäťťĺĄćĺ!\r\n");
#endif
#if 0
	xReturn = Sdio_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("ĺĺťşSDIO_Demo_TaskäťťĺĄćĺ!\r\n");
#endif
#if 0
  xReturn = RTC_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("[%s],[%d],[%s]\r\n",__FUNCTION__,__LINE__,"run rtc demo");
#endif
#if 0
  xReturn = SNTP_Demo_Task_Init();
  if(pdPASS == xReturn)
    printf("ĺĺťşSNTP-TaskäťťĺĄćĺ!\r\n");
#endif
#if 0
  AT_PROC_Task_Init();
  // xReturn = Tftp_Demo_Task_Init();
  // if(pdPASS == xReturn)
  //   printf("ĺĺťşTftp-TaskäťťĺĄćĺ!\r\n");
#endif
#if 0
  xReturn = Lcd_Demo_Task_Init();
  if(pdPASS == xReturn)
  printf("ĺĺťşLCD-TaskäťťĺĄćĺ!\r\n");
#endif
	
  vTaskDelete(AppTaskCreate_Handle);
  
  taskEXIT_CRITICAL();
}


static void BSP_Init(void)
{
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	//LED_GPIO_Config();
  
	Debug_USART_Config();
  USART6_Config();
  // MODULE_INIT();
  FSMC_SRAM_Init();
  Key_Bsp_Init();
  lcd_hardware_init();//st7789 lcd handle init
  Rtc_Bsp_Init();

#ifdef USE_LWIP_CODE
	//  ETH_BSP_Config();
#endif // USE_LWIP
  printf("[%s],[%d],[%s]\r\n",__FUNCTION__,__LINE__,"BSP init");
	//SPI_FLASH_Init();

}



