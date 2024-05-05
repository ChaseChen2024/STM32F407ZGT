#if 1
#include "Usb_demo.h"

#include <string.h>

/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"


#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"

static TaskHandle_t Usb_Demo_Task_Handle = NULL;/* LED任务句柄 */


__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;
/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Usb_Demo_Task(void* parameter)
{	
	 USBD_Init(&USB_OTG_dev,
						USB_OTG_FS_CORE_ID,
						&USR_desc,
						&USBD_MSC_cb, 
						&USR_cb);
  while (1)
  {
    vTaskDelay(500);   /* 延时500个tick */
  }
}

long Usb_Demo_Task_Init(void)
{
		BaseType_t xReturn = pdPASS;
	
			/* 创建Test_Task任务 */
		xReturn = xTaskCreate((TaskFunction_t )Usb_Demo_Task, /* 任务入口函数 */
													(const char*    )"Usb_Demo_Task",/* 任务名字 */
													(uint16_t       )1024,   /* 任务栈大小 */
													(void*          )NULL,	/* 任务入口函数参数 */
													(UBaseType_t    )4,	    /* 任务的优先级 */
													(TaskHandle_t*  )&Usb_Demo_Task_Handle);/* 任务控制块指针 */
		return xReturn;
}

#endif
