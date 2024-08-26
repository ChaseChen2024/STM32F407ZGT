/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#if 0
#include "client.h"

#include "lwip/opt.h"

#include "lwip/sys.h"
#include "lwip/api.h"


#include <lwip/sockets.h>
#include "./sram/bsp_sram.h"  

#define IP_ADDR        "114.215.151.106"

#ifdef    LWIP_DNS
#define   HOST_NAME       "embedfire.com"     
#else
#define   HOST_NAME       "114.215.151.106"     //
#endif
#define DEST_PORT1                  80

uint8_t get_buf[]= "GET / HTTP/1.1\r\nHost: embedfire.com\r\n\r\n\r\n\r\n";


static void client(void *thread_param)
{
  int sock = -1,rece;
  struct sockaddr_in client_addr;

//  uint8_t *pbufdata = pvPortMalloc(5000);  
  
  char* host_ip;
  
  uint32_t *pSDRAM= pvPortMalloc(400000);
  // printf("111112222222222222 0x%x",&(*pSDRAM));
#ifdef  LWIP_DNS
    ip4_addr_t dns_ip;
    netconn_gethostbyname(HOST_NAME, &dns_ip);
    host_ip = ip_ntoa(&dns_ip);
    printf("host name : %s , host_ip : %s\n",HOST_NAME,host_ip);
#else
    host_ip = HOST_NAME;
#endif  
  
  printf("目标端口号是：%d\n\n",DEST_PORT1);
  
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
    client_addr.sin_port = htons(DEST_PORT1);   
    client_addr.sin_addr.s_addr = inet_addr(host_ip);
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
    memset(pSDRAM,0,400000);
    printf("\n************************************************************\n");
    write(sock,get_buf,sizeof(get_buf));
    while (1)
    {
//      if(write(sock,get_buf,sizeof(get_buf)) < 0)
//        break;

      rece = recv(sock, (uint8_t*)pSDRAM, 400000, 0);
      if (rece <= 0) 
        break;

//      printf("rece = %d\n",rece);
      printf("%s\n",(uint8_t*)pSDRAM);
//      pSDRAM = (uint32_t*)(pSDRAM + rece);

    }
//    printf("%s\n",(uint8_t*)pSDRAM);
    printf("\n**********************************************************\n");
    
    memset(pSDRAM,0,400000);
    closesocket(sock);
    vTaskDelay(10000);
  }

}

void
client_init(void)
{
  sys_thread_new("client", client, NULL, 2048, 4);
}
#endif