#if 1
//bsp

#include "atproc.h"
#include "bsp_debug_usart.h"
//C库
#include <string.h>
/* FreeRTOS头文件 */
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

/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void AT_PROC_Task(void* parameter)
{	
      uint8_t recv_data[USART_RBUFF_SIZE] ;
      uint32_t recv_data_len = 0;
     BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
    // printf("at_msg%x\r\n",at_msg);
  while (1)
  {

    //获取二值信号量 xSemaphore,没获取到则一直等待
		xReturn = xSemaphoreTake(BinarySem_Handle,/* 二值信号量句柄 */
                              portMAX_DELAY); /* 等待时间 */
      memset(recv_data,0,USART_RBUFF_SIZE);
      get_rx_data(recv_data);
      printf("收到数据:%s\n",recv_data);
    recv_data_len=strlen(recv_data);
    // printf("len:%d \r\n",recv_data_len);
    //这里要处理所有返回参数，将返回数据分为，数据和结果码

    // printf("\r\n  isRecvData:%d \r\n",user_at.isRecvData);
    if(user_at.isRecvData == 0)//只返回执行结果
    {
       user_at.at_complete = 1;//处理完成
    }
    else //需要有数据返回
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
        user_at.at_complete = 1;//处理完成
    }
  }
}


long AT_PROC_Task_Init(void)
{
		BaseType_t xReturn = pdPASS;

     /* 创建 BinarySem */
  BinarySem_Handle = xSemaphoreCreateBinary();	
	    // Module_Queue = xQueueCreate(10,64);
			/* 创建KEY_Task任务 */
		xReturn = xTaskCreate((TaskFunction_t )AT_PROC_Task,  /* 任务入口函数 */
                            (const char*    )"AT_PROC_Task",/* 任务名字 */
                            (uint16_t       )2048,  /* 任务栈大小 */
                            (void*          )NULL,/* 任务入口函数参数 */
                            (UBaseType_t    )3, /* 任务的优先级 */
                            (TaskHandle_t*  )&AT_PROC_Task_Handle);/* 任务控制块指针 */ 
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
    if(user_at.at_complete == 1)//执行完成
    {
      if( user_at.isRecvData == 0)//
      {
          if(user_at.at_result == 1)
          {
            
            ret =  1;//返回执行成功
            break;
          }
          else
          {
            
            ret =  2;//返回执行失败
            break;
          }
      }
      else
      {
        if(user_at.result_state == 1)//有数据返回
        {
          memset(ack_data,0,256);
          memcpy(ack_data,user_at.recv_data,user_at.recv_data_len);
          
          ret =  1;
          break;
        }
        else
        {
          
          ret =  3;//指令为有数据返回的，但没有收到数据，执行错误
          break;
        }
      }
    }
  }
  if(ret == 0)
    ret =  3;//超时

  memset(&user_at,0,sizeof(user_at));
  return ret;
}

void at_result_set(uint8_t at_result)
{
  user_at.at_result = at_result;
}
#endif