#if 1
#include "Rtc_demo.h"
#include "bsp_rtc.h"

#include <string.h>

/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

static TaskHandle_t RTC_Demo_Task_Handle = NULL;/* RTC������ */


/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void RTC_Demo_Task(void* parameter)
{	
//	int a=0,b=0,c=0;
  while (1)
  {
    /* ��ʾʱ������� */
		RTC_TimeAndDate_Show();
  }
}


long RTC_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;
	
			/* ����RTC_Task���� */
		xReturn = xTaskCreate((TaskFunction_t )RTC_Demo_Task,  /* ������ں��� */
													(const char*    )"RTC_Demo_Task",/* �������� */
													(uint16_t       )512,  /* ����ջ��С */
													(void*          )NULL,/* ������ں������� */
													(UBaseType_t    )3, /* ��������ȼ� */
													(TaskHandle_t*  )&RTC_Demo_Task_Handle);/* ������ƿ�ָ�� */ 
		return xReturn;
}
void Rtc_Bsp_Init(void)
{
	/* ������ʼ��	*/
  Rtc_Init();
}
#endif