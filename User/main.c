
#include "user.h"

#include <string.h>
#include <nv.h>
#ifdef USER_LEETTER_SHELL
#include "shell_port.h"
#include "bsp_usart3.h"
#endif


#ifdef USE_CMBACKTRACE_CODE
#include <cm_backtrace.h>

#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"

#endif

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
                        (uint16_t       )1024*1, 
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
  
 #ifdef USE_LWIP_CODE
	xReturn = SNTP_Demo_Task_Init();
	if(pdPASS == xReturn)
		elog_i(ELOG_APP,"SNTP Client");

#endif

	GET_FREERTOS_FLAG = 1;
  vTaskDelete(AppTaskCreate_Handle);
  
  taskEXIT_CRITICAL();
}


static void BSP_Init(void)
{
  
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
  FSMC_SRAM_Init();
	Debug_USART_Config();
  #ifdef USE_CMBACKTRACE_CODE
	cm_backtrace_init("CmBacktrace", HARDWARE_VERSION, SOFTWARE_VERSION);
  #endif
  #ifdef USE_EASYLOGGER_CODE
  elog_init();

  elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);
  elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
  elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_P_INFO));
  elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_P_INFO));

  elog_start();
  #endif
  elog_i(ELOG_BSP, "BSP init");
  
 	 #ifdef USER_LEETTER_SHELL
  USART3_Config();
  #endif // USER_LEETTER_SHELL
#ifdef USE_LWIP_CODE
	ETH_BSP_Config();
#endif // USE_LWIP
 
}


static void APP_Init(void)
{
  elog_i(ELOG_APP, "APP init");
  nv_init();
  #ifdef USER_LEETTER_SHELL
	userShellInit();
	#endif // USER_LEETTER_SHELL
  #ifdef USE_LWIP_CODE
  tcpip_init(NULL, NULL);
  #endif // USE_LWIP


}

#ifdef USER_LEETTER_SHELL
void rtos_heap_size_test(void)
{
  shellPrint(&shell,"rtos free size: %d B\r\nrtos free size: %d KB\r\n",xPortGetFreeHeapSize(),xPortGetFreeHeapSize()/1024);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), os_free, rtos_heap_size_test, freertos free heap size);


char rtos_list_buf[256] __EXRAM;
void rtos_list_test(void)
{

    vTaskList(rtos_list_buf);
    shellPrint(&shell,"----------------------------------------------\r\n");
    shellPrint(&shell,"name          state  priority  stack   num\r\n");
                 //  lwip            B       28      353     4
    shellPrint(&shell,"%s", rtos_list_buf);
    shellPrint(&shell,"----------------------------------------------\r\n");
    memset(rtos_list_buf,0,2048);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), os_list, rtos_list_test, freertos list);


#ifdef USE_CMBACKTRACE_CODE
void rtos_assert_test(void)
{
    cm_backtrace_assert(cmb_get_sp());
    shellPrint(&shell,"----------------------------------------------\r\n");
    
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), os_assert, rtos_assert_test, freertos assert);

#endif

#endif // USER_LEETTER_SHELL