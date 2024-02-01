#if 1
#include <string.h>

/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/api.h"

#include <lwip/sockets.h>
#include "sys_arch.h"
#include "Lwip_Tcp_Client_Socket_demo.h"

#include "bsp_rtc.h"
static TaskHandle_t Tcp_Client_Socket_Demo_Task_Handle = NULL;/* KEY任务句柄 */


/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Tcp_Client_Socket_Demo_Task(void* parameter)
{	
 int sock = -1;
  struct sockaddr_in client_addr;
  
  ip4_addr_t ipaddr;
  RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
  uint8_t send_buf[]= "This is a TCP Client test...\n";
  uint8_t send_buf_time[64]={0};
  printf("目地IP地址:%d.%d.%d.%d \t 端口号:%d\n\n",      \
          DEST_IP_ADDR0,DEST_IP_ADDR1,DEST_IP_ADDR2,DEST_IP_ADDR3,DEST_PORT);
  
  printf("请将电脑上位机设置为TCP Server.在User/arch/sys_arch.h文件中将目标IP地址修改为您电脑上的IP地址\n\n");
  
  printf("修改对应的宏定义:DEST_IP_ADDR0,DEST_IP_ADDR1,DEST_IP_ADDR2,DEST_IP_ADDR3,DEST_PORT\n\n");
  
  IP4_ADDR(&ipaddr,DEST_IP_ADDR0,DEST_IP_ADDR1,DEST_IP_ADDR2,DEST_IP_ADDR3);
  while(1)
  {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
      printf("Socket error\n");
      vTaskDelay(10);
      continue;
    } 

    client_addr.sin_family = AF_INET;      
    client_addr.sin_port = htons(DEST_PORT);   
    client_addr.sin_addr.s_addr = ipaddr.addr;
    memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));    

    if (connect(sock, 
               (struct sockaddr *)&client_addr, 
                sizeof(struct sockaddr)) == -1) 
    {
        printf("Connect failed!\n");
        closesocket(sock);
        vTaskDelay(10);
        continue;
    }                                           
    
    printf("Connect to server successful!\n");
    
    while (1)
    {
      RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	  	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
      sprintf((char* )send_buf_time,"[20%0.2d/%0.2d/%0.2d %0.2d:%0.2d:%0.2d]%s",RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,
                                                            RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds,send_buf);
      if(write(sock,send_buf_time,sizeof(send_buf_time)) < 0)
        break;
   
      vTaskDelay(1000);
    }
    
    closesocket(sock);
  }

}


void Tcp_Client_Socket_Task_Init(void)
{
    sys_thread_new("Tcp_Client_Socket_Demo_Task", Tcp_Client_Socket_Demo_Task, Tcp_Client_Socket_Demo_Task_Handle, 1024, 4);
		 
}


#endif