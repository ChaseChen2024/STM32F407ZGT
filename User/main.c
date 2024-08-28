
#include "user.h"
#include "user_define.h"

#include <string.h>
#include <nv.h>



static TaskHandle_t AppTaskCreate_Handle = NULL;

static void AppTaskCreate(void);

static void BSP_Init(void);
static void APP_Init(void);


int main(void)
{	
  BaseType_t xReturn = pdPASS;
  
  BSP_Init();

  
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  
                        (const char*    )"AppTaskCreate",
                        (uint16_t       )1024*2, 
                        (void*          )NULL,
                        (UBaseType_t    )1,
                        (TaskHandle_t*  )&AppTaskCreate_Handle);
        
  if(pdPASS == xReturn)
    vTaskStartScheduler();
  else
    return -1;  
  
  while(1);
}



static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;
 

  taskENTER_CRITICAL();
  APP_Init();
  
#if 1
	xReturn = SNTP_Demo_Task_Init();
	if(pdPASS == xReturn)
		printf("SNTP Client\r\n");
	
#endif
	
  vTaskDelete(AppTaskCreate_Handle);
  
  taskEXIT_CRITICAL();
}


static void BSP_Init(void)
{
	
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	Debug_USART_Config();
	printf("[%s],[%d],[%s]\r\n",__FUNCTION__,__LINE__,"BSP init");
 	FSMC_SRAM_Init();
  	// Rtc_Bsp_Init();
#ifdef USE_LWIP_CODE
	 ETH_BSP_Config();
#endif // USE_LWIP
  	
}


static void APP_Init(void)
{
  printf("[%s],[%d],[%s]\r\n",__FUNCTION__,__LINE__,"APP init");
  nv_init();
  // FRESULT err = FR_OK;
  // err = nv_read("1:test.txt",(char*)&user_tset,sizeof(test_t));
  // nv_write("1:test.txt",(char*)&user_tset,sizeof(test_t));
  #ifdef USE_LWIP_CODE
  tcpip_init(NULL, NULL);
  #endif // USE_LWIP
}


