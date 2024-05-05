#if 1
#include "Usb_demo.h"

#include <string.h>

/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"


#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"

static TaskHandle_t Usb_Demo_Task_Handle = NULL;/* LED������ */


__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;
/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
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
    vTaskDelay(500);   /* ��ʱ500��tick */
  }
}

long Usb_Demo_Task_Init(void)
{
		BaseType_t xReturn = pdPASS;
	
			/* ����Test_Task���� */
		xReturn = xTaskCreate((TaskFunction_t )Usb_Demo_Task, /* ������ں��� */
													(const char*    )"Usb_Demo_Task",/* �������� */
													(uint16_t       )1024,   /* ����ջ��С */
													(void*          )NULL,	/* ������ں������� */
													(UBaseType_t    )4,	    /* ��������ȼ� */
													(TaskHandle_t*  )&Usb_Demo_Task_Handle);/* ������ƿ�ָ�� */
		return xReturn;
}

#endif
