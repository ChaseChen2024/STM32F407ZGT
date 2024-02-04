#if 1
#include "Key_demo.h"
#include "bsp_key.h"
#include "bsp_debug_usart.h"
#include "module.h"
#include "atproc.h"
#include <string.h>
 #include "log.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FatFs_demo.h"
static TaskHandle_t KEY_Demo_Task_Handle = NULL;/* KEY������ */
extern QueueHandle_t Module_Queue;

/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void KEY_Demo_Task(void* parameter)
{	
	// int a=0,b=0,c=0;
	// uint8_t ret = 0;
	// uint8_t Rxdata[256] = {0};
	//  vTaskDelay(10000);/* ��ʱ20��tick */
	// ret = send_at((uint8_t*)"ATE0\r\n",NULL,0,10000);
	// 	printf("ret=%d\r\n",ret);
  while (1)
  {

    if( Key_Scan(KEY0_GPIO_PORT,KEY0_PIN) == KEY_ON )
    {/* K0 ������ */
      printf("����0��\n");
    }
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )
    {/* K1 ������ */
		printf("����1��\n");

    } 
    vTaskDelay(20);/* ��ʱ20��tick */
  }
}


long Key_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;
	
			/* ����KEY_Task���� */
		xReturn = xTaskCreate((TaskFunction_t )KEY_Demo_Task,  /* ������ں��� */
													(const char*    )"KEY_Demo_Task",/* �������� */
													(uint16_t       )2048,  /* ����ջ��С */
													(void*          )NULL,/* ������ں������� */
													(UBaseType_t    )3, /* ��������ȼ� */
													(TaskHandle_t*  )&KEY_Demo_Task_Handle);/* ������ƿ�ָ�� */ 
		return xReturn;
}
void Key_Bsp_Init(void)
{
	/* ������ʼ��	*/
  Key_GPIO_Config();
}
#endif