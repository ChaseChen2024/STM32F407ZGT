#if 1
//bsp

#include "atproc.h"
#include "bsp_debug_usart.h"
//C��
#include <string.h>
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
SemaphoreHandle_t BinarySem_Handle =NULL;
static TaskHandle_t AT_PROC_Task_Handle = NULL;
at_data_block user_at={0};


uint32_t osKernelGetTickCount1 (void) 
{
  TickType_t ticks;

    ticks = xTaskGetTickCountFromISR();

  return (ticks);
}
uint32_t  quec_get_time1(void) 
{
    uint32_t currentMs = 0;

    currentMs = osKernelGetTickCount1()/portTICK_PERIOD_MS;

    return currentMs;
}

static void AT_PROC_Task(void* parameter)
{	
      uint8_t recv_data[USART_RBUFF_SIZE] ;
      uint32_t recv_data_len = 0;
     BaseType_t xReturn = pdPASS;
    // printf("at_msg%x\r\n",at_msg);
  while (1)
  {
		xReturn = xSemaphoreTake(BinarySem_Handle,portMAX_DELAY);
      memset(recv_data,0,USART_RBUFF_SIZE);
      get_rx_data(recv_data);
      printf("�յ�����:%s\n",recv_data);
    recv_data_len=strlen(recv_data);
    // printf("len:%d \r\n",recv_data_len);

    // printf("\r\n  isRecvData:%d \r\n",user_at.isRecvData);
    if(user_at.isRecvData == 0)
    {
       user_at.at_complete = 1;
    }
    else
    {
        user_at.result_state = 1;
        if(user_at.at_result == 1)
        {
            user_at.recv_data_len = recv_data_len - 4;
        }
        if(user_at.at_result == 2)
        {
            user_at.recv_data_len = recv_data_len - 7;
        }
        memcpy(user_at.recv_data,recv_data,user_at.recv_data_len);
        // printf("user_at.recv_data:%s \r\n",user_at.recv_data);
        user_at.at_complete = 1;//�������
    }
  }
}


long AT_PROC_Task_Init(void)
{
		BaseType_t xReturn = pdPASS;

     /* ���� BinarySem */
  BinarySem_Handle = xSemaphoreCreateBinary();	
	    // Module_Queue = xQueueCreate(10,64);
			/* ����KEY_Task���� */
		xReturn = xTaskCreate((TaskFunction_t )AT_PROC_Task,  /* ������ں��� */
                            (const char*    )"AT_PROC_Task",/* �������� */
                            (uint16_t       )2048,  /* ����ջ��С */
                            (void*          )NULL,/* ������ں������� */
                            (UBaseType_t    )3, /* ��������ȼ� */
                            (TaskHandle_t*  )&AT_PROC_Task_Handle);/* ������ƿ�ָ�� */ 
		return xReturn;
}
uint8_t send_at(uint8_t* at_str,uint8_t* ack_data,uint8_t isRecvData,uint32_t timeout)
{
  uint8_t ret = 0;
  //  uart3_rx_clear();
  Usart_SendString(MODULE_USART,(char*)at_str);
  uint32_t begin_time = quec_get_time1();

  memcpy(user_at.send_at_cmd,at_str,sizeof(at_str));
  user_at.isRecvData = isRecvData;
 while((quec_get_time1()- begin_time) < timeout)
  {
    if(user_at.at_complete == 1)//ִ�����
    {
      if( user_at.isRecvData == 0)//
      {
          if(user_at.at_result == 1)
          {
            
            ret =  1;//����ִ�гɹ�
            break;
          }
          else
          {
            
            ret =  2;//����ִ��ʧ��
            break;
          }
      }
      else
      {
        if(user_at.result_state == 1)//�����ݷ���
        {
          memset(ack_data,0,256);
          memcpy(ack_data,user_at.recv_data,user_at.recv_data_len);
          
          ret =  1;
          break;
        }
        else
        {
          
          ret =  3;//ָ��Ϊ�����ݷ��صģ���û���յ����ݣ�ִ�д���
          break;
        }
      }
    }
  }
  if(ret == 0)
    ret =  3;//��ʱ

  memset(&user_at,0,sizeof(user_at));
  return ret;
}

void at_result_set(uint8_t at_result)
{
  user_at.at_result = at_result;
}
#endif