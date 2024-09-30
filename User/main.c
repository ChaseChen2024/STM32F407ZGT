
#include "user.h"

#include <string.h>
#ifdef USE_FATFS_CODE
#include <nv.h>
#endif
#ifdef USER_LEETTER_SHELL
#include "shell_port.h"
#include "bsp_usart3.h"
#endif

#ifdef USE_SFUD_CODE
#include <sfud.h>
#ifdef USE_FAL_CODE
#include "fal.h"
#endif
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


void soft_reset(void)
{
    __set_FAULTMASK(1); 
    NVIC_SystemReset(); 
}
void _kill(void){};
void _getpid(void){};

uint32_t osKernelGetTickCount(void) 
{
  TickType_t ticks;

  if (xPortIsInsideInterrupt()) {
   ticks = xTaskGetTickCountFromISR();
  } else {
     ticks = xTaskGetTickCount();
  }

  return (ticks);
}
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
  elog_i(ELOG_APP,"xReturn:%d",xReturn);
	GET_FREERTOS_FLAG = 1;
  vTaskDelete(AppTaskCreate_Handle);
  
  taskEXIT_CRITICAL();
}


static void BSP_Init(void)
{
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x20000);
  __enable_irq();
  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
  FSMC_SRAM_Init();
  Usart3_Config();
 
  printf("\r\n-------------------------enter application-------------------------\r\n");
  printf("\r\n-----------------------------2024/09/10----------------------------\r\n");
  
	
  #ifdef USE_CMBACKTRACE_CODE
	cm_backtrace_init("QT201", HARDWARE_VERSION, SOFTWARE_VERSION);
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
  elog_i(ELOG_BSP, "0x%x",SCB->VTOR);
  #ifdef USE_SFUD_CODE
  sfud_user_init();
  #ifdef USE_FAL_CODE
  fal_init();
  #endif // USE_FAL_CODE
  #endif
 	#ifdef USER_LEETTER_SHELL
  Usart1_Config();
  #endif
#ifdef USE_LWIP_CODE
	ETH_BSP_Config();
#endif

}


static void APP_Init(void)
{
  elog_i(ELOG_APP, "APP init");
  #ifdef USE_FATFS_CODE
  nv_init();
  #endif

  #ifdef USER_LEETTER_SHELL
	userShellInit();
	#endif
  #ifdef USE_LWIP_CODE
  tcpip_init(NULL, NULL);
  tftpInit();
  #endif


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
#ifdef USE_EASYLOGGER_CODE
int log_test(int argc, char *argv[])
{
  if(!strcasecmp(argv[1], "on"))
  {
    elog_start();
  }
  else if(!strcasecmp(argv[1], "off"))
  {
    elog_stop();
  }
  else if(!strcasecmp(argv[1], "level"))
  {
    if(!strcasecmp(argv[3], "E"))
    {
      elog_set_filter_tag_lvl(argv[2], ELOG_LVL_ERROR);
    }
    else if(!strcasecmp(argv[3], "W"))
    {
      elog_set_filter_tag_lvl(argv[2], ELOG_LVL_WARN);
    }
    else if(!strcasecmp(argv[3], "I"))
    {
      elog_set_filter_tag_lvl(argv[2], ELOG_LVL_INFO);
    }
    else if(!strcasecmp(argv[3], "D"))
    {
      elog_set_filter_tag_lvl(argv[2], ELOG_LVL_DEBUG);
    }
    else if(!strcasecmp(argv[3], "V"))
    {
      elog_set_filter_tag_lvl(argv[2], ELOG_LVL_VERBOSE);
    }

  }
	shellPrint(&shell, "ok");
	return 0;
    
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), log, log_test, EasyLogger control);

#endif

#ifdef USE_FAL_CODE
#define BUF_SIZE 1024

static int fal_test(const char *partiton_name)
{
    int ret =  0;
    const struct fal_flash_dev *flash_dev = NULL;
    const struct fal_partition *partition = NULL;

    if (!partiton_name)
    {
        shellPrint(&shell,"Input param partition name is null!\r\n");
        return -1;
    }

    partition = fal_partition_find(partiton_name);
    if (partition == NULL)
    {
        shellPrint(&shell,"Find partition (%s) failed!\r\n", partiton_name);
        ret = -1;
        return ret;
    }

    flash_dev = fal_flash_device_find(partition->flash_name);
    if (flash_dev == NULL)
    {
        shellPrint(&shell,"Find flash device (%s) failed!\r\n", partition->flash_name);
        ret = -1;
        return ret;
    }

    shellPrint(&shell,"Flash device : %s   "
               "Flash size : %dK   \r\n"
               "Partition : %s   "
               "Partition size: %dK\r\n", 
                partition->flash_name, 
                flash_dev->len/1024,
                partition->name,
                partition->len/1024);

    return ret;
}

void fal_sample(void)
{

    if (fal_test("bootloader") == 0)
    {
        shellPrint(&shell,"Fal partition (%s) test success!\r\n", "param");
    }
    else
    {
        shellPrint(&shell,"Fal partition (%s) test failed!\r\n", "param");
    }

    if (fal_test("application") == 0)
    {
        shellPrint(&shell,"Fal partition (%s) test success!\r\n", "param");
    }
    else
    {
        shellPrint(&shell,"Fal partition (%s) test failed!\r\n", "param");
    }

    if (fal_test("download") == 0)
    {
        shellPrint(&shell,"Fal partition (%s) test success!\r\n", "param");
    }
    else
    {
        shellPrint(&shell,"Fal partition (%s) test failed!\r\n", "param");
    }

    if (fal_test("fatfs") == 0)
    {
        shellPrint(&shell,"Fal partition (%s) test success!\r\n", "download");
    }
    else
    {
        shellPrint(&shell,"Fal partition (%s) test failed!\r\n", "download");
    }
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), fal_sample, fal_sample,fal sample);

void fal_erase_download(void)
{
  int ret =  0;
  const struct fal_partition *partition = NULL;
  partition = fal_partition_find("download");
  if (partition == NULL)
  {
      shellPrint(&shell,"Find partition (%s) failed!\r\n", partition->name);
      ret = -1;
      return;
  }
  if ((ret = fal_partition_erase_all(partition)) < 0)
  {
      elog_e(LOG_TAG,"Firmware download failed! Partition (%s) erase error!", partition->name);
  }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), fal_erase_download, fal_erase_download,fal erase download);
#endif

void reboot(void)
{
  soft_reset();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), reboot, reboot,system restart);

#endif // USER_LEETTER_SHELL