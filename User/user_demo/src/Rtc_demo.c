#if 1
#include "Rtc_demo.h"
#include "bsp_rtc.h"

#include <string.h>

/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

static TaskHandle_t RTC_Demo_Task_Handle = NULL;/* RTC任务句柄 */


/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void RTC_Demo_Task(void* parameter)
{	
//	int a=0,b=0,c=0;
  while (1)
  {
    /* 显示时间和日期 */
		RTC_TimeAndDate_Show();
  }
}


long RTC_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;
	
			/* 创建RTC_Task任务 */
		xReturn = xTaskCreate((TaskFunction_t )RTC_Demo_Task,  /* 任务入口函数 */
													(const char*    )"RTC_Demo_Task",/* 任务名字 */
													(uint16_t       )512,  /* 任务栈大小 */
													(void*          )NULL,/* 任务入口函数参数 */
													(UBaseType_t    )3, /* 任务的优先级 */
													(TaskHandle_t*  )&RTC_Demo_Task_Handle);/* 任务控制块指针 */ 
		return xReturn;
}
void Rtc_Bsp_Init(void)
{
	/* 按键初始化	*/
  Rtc_Init();
}
#endif