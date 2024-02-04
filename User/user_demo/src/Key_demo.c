#if 1
#include "Key_demo.h"
#include "bsp_key.h"
#include "bsp_debug_usart.h"
#include "module.h"
#include "atproc.h"
#include <string.h>
 #include "log.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FatFs_demo.h"
static TaskHandle_t KEY_Demo_Task_Handle = NULL;/* KEY任务句柄 */
extern QueueHandle_t Module_Queue;

/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void KEY_Demo_Task(void* parameter)
{	
	// int a=0,b=0,c=0;
	// uint8_t ret = 0;
	// uint8_t Rxdata[256] = {0};
	//  vTaskDelay(10000);/* 延时20个tick */
	// ret = send_at((uint8_t*)"ATE0\r\n",NULL,0,10000);
	// 	printf("ret=%d\r\n",ret);
  while (1)
  {

    if( Key_Scan(KEY0_GPIO_PORT,KEY0_PIN) == KEY_ON )
    {/* K0 被按下 */
      printf("按键0！\n");
    }
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )
    {/* K1 被按下 */
		printf("按键1！\n");

    } 
    vTaskDelay(20);/* 延时20个tick */
  }
}


long Key_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;
	
			/* 创建KEY_Task任务 */
		xReturn = xTaskCreate((TaskFunction_t )KEY_Demo_Task,  /* 任务入口函数 */
													(const char*    )"KEY_Demo_Task",/* 任务名字 */
													(uint16_t       )2048,  /* 任务栈大小 */
													(void*          )NULL,/* 任务入口函数参数 */
													(UBaseType_t    )3, /* 任务的优先级 */
													(TaskHandle_t*  )&KEY_Demo_Task_Handle);/* 任务控制块指针 */ 
		return xReturn;
}
void Key_Bsp_Init(void)
{
	/* 按键初始化	*/
  Key_GPIO_Config();
}
#endif