/**
  ******************************************************************************
  * @file    netconf.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   Network connection configuration
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
// #ifdef USE_LWIP_CODE
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/priv/tcp_priv.h"
#include "ethernetif.h"
#include "netconf.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "sys_arch.h"
#include "tcpip.h"

#include "user.h"

#define MAX_DHCP_TRIES 20
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t TCPTimer = 0;
uint32_t ARPTimer = 0 ;
uint32_t DHCPfineTimer = 0 ;
uint32_t DHCPcoarseTimer = 0 ;
uint32_t IPaddress = 0 ;
__IO uint8_t DHCP_state;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif xnetif; /* network interface structure */

/* Private functions ---------------------------------------------------------*/
void LwIP_DHCP_Process_Handle(void);
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void LwIP_NW_Init(void)
{
 elog_i(ELOG_LWIP,"LwIP_Init-111");
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;


#if LWIP_DHCP
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
  DHCP_state = DHCP_START;
#else
  IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif


  /* - netif_add(struct xnetif *xnetif, ip_addr_t *ipaddr,
            ip_addr_t *netmask, ip_addr_t *gw,
            void *state, err_t (* init)(struct xnetif *xnetif),
            err_t (* input)(struct pbuf *p, struct xnetif *xnetif))
    
   Adds your network interface to the netif_list. Allocate a struct
  xnetif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.

  The init function pointer must point to a initialization function for
  your ethernet xnetif interface. The following code illustrates it's use.*/

  netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);


  /*  Registers the default network interface.*/
  netif_set_default(&xnetif);


  /*  When the xnetif is fully configured this function must be called.*/
  netif_set_up(&xnetif);
	netif_set_link_up(&xnetif);
#if LWIP_DHCP	   			//若使用了DHCP
  int err;
  /*  Creates a new DHCP client for this interface on the first call.
  Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
  the predefined regular intervals after starting the client.
  You can peek in the netif->dhcp struct for the actual DHCP status.*/

  elog_i(ELOG_LWIP,"本例程将使用DHCP动态分配IP地址,如果不需要则在lwipopts.h中将LWIP_DHCP定义为0");
  
  err = dhcp_start(&xnetif);      //开启dhcp
  if(err == ERR_OK)
    elog_i(ELOG_LWIP,"lwip dhcp init success...");
  else
    elog_i(ELOG_LWIP,"lwip dhcp init fail...");

   int count=0;
  while(ip_addr_cmp(&(xnetif.ip_addr),&ipaddr))   //等待dhcp分配的ip有效
  {
    vTaskDelay(100);
    count+=100;
    printf("count:%d",count);
    if(count > 5000)
    {
    	break;
    }
    
  } 
#endif
    elog_i(ELOG_LWIP,"本地IP地址是:%d.%d.%d.%d",  \
        ((xnetif.ip_addr.addr)&0x000000ff),       \
        (((xnetif.ip_addr.addr)&0x0000ff00)>>8),  \
        (((xnetif.ip_addr.addr)&0x00ff0000)>>16), \
        ((xnetif.ip_addr.addr)&0xff000000)>>24);
}


#if LWIP_DHCP
int LwIP_DHCP(ip_addr_t *addr)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  int err;
  int count=0;
 
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
  DHCP_state = DHCP_START;

  
  err = dhcp_start(&xnetif);      //开启dhcp
  if(err == ERR_OK)
    elog_i(ELOG_LWIP,"lwip dhcp init success...");
  else
    elog_i(ELOG_LWIP,"lwip dhcp init fail...");

   
  while(ip_addr_cmp(&(xnetif.ip_addr),&ipaddr))   //等待dhcp分配的ip有效
  {
    vTaskDelay(100);
    count+=100;
    elog_i(ELOG_LWIP,"count:%d",count);
    if(count > 5000)
    {
		elog_i(ELOG_LWIP,"dhcp获取IP超时...");
		return -1;
    }
  } 
  addr->addr = xnetif.ip_addr.addr;
    elog_i(ELOG_LWIP,"本地IP地址是:%d.%d.%d.%d",  \
        ((xnetif.ip_addr.addr)&0x000000ff),       \
        (((xnetif.ip_addr.addr)&0x0000ff00)>>8),  \
        (((xnetif.ip_addr.addr)&0x00ff0000)>>16), \
        ((xnetif.ip_addr.addr)&0xff000000)>>24);
	return 0;
}
#endif
/**
  * @brief  LwIP periodic tasks
  * @param  localtime the current LocalTime value
  * @retval None
  */
void LwIP_Periodic_Handle(__IO uint32_t localtime)
{
#if LWIP_TCP
  /* TCP periodic process every 250 ms */
  if (localtime - TCPTimer >= TCP_TMR_INTERVAL)
  {
    TCPTimer =  localtime;
    tcp_tmr();
  }
#endif
  
  /* ARP periodic process every 5s */
  if ((localtime - ARPTimer) >= ARP_TMR_INTERVAL)
  {
    ARPTimer =  localtime;
    etharp_tmr();
    PR_DEBUG("ARPTimer:%d", ARPTimer);
  }

#if LWIP_DHCP
  /* Fine DHCP periodic process every 500ms */
  if (localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
  {
    DHCPfineTimer =  localtime;
    dhcp_fine_tmr();
    if ((DHCP_state != DHCP_ADDRESS_ASSIGNED)&&(DHCP_state != DHCP_TIMEOUT))
    {
      /* toggle LED1 to indicate DHCP on-going process */
      // STM_EVAL_LEDToggle(LED1);
      
      /* process DHCP state machine */
      LwIP_DHCP_Process_Handle();    
    }
  }

  /* DHCP Coarse periodic process every 60s */
  if (localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
  {
    DHCPcoarseTimer =  localtime;
    dhcp_coarse_tmr();
  }
  
#endif
}

#if LWIP_DHCP
/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
void LwIP_DHCP_Process_Handle()
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  uint8_t iptab[4];
  uint8_t iptxt[20];
  static uint8_t dhcp_count = 0;

  switch (DHCP_state)
  {
    case DHCP_START:
    {
      dhcp_start(&xnetif);
      IPaddress = 0;
      DHCP_state = DHCP_WAIT_ADDRESS;
    }
    break;

    case DHCP_WAIT_ADDRESS:
    {
      /* Read the new IP address */
      IPaddress = xnetif.ip_addr.addr;

      if (IPaddress!=0) 
      {
        DHCP_state = DHCP_ADDRESS_ASSIGNED;	

        /* Stop DHCP */
        dhcp_stop(&xnetif);
        PR_INFO("dhcp ip:%d:%d:%d:%d", (uint8_t)(IPaddress >> 24), (uint8_t)(IPaddress >> 16), (uint8_t)(IPaddress >> 8), (uint8_t)(IPaddress >> 0));
      }
      else
      {
        /* DHCP timeout */
        if (dhcp_count++ > MAX_DHCP_TRIES)
        {
          dhcp_count = 0;
          DHCP_state = DHCP_TIMEOUT;

          /* Stop DHCP */
          dhcp_stop(&xnetif);

          /* Static address used */
          IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3);
          IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
          IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
          netif_set_addr(&xnetif, &ipaddr , &netmask, &gw);
          PR_INFO("static ip:%d:%d:%d:%d", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        }
      }
    }
    break;
    default: break;
  }
}
#endif      

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
// #endif   
