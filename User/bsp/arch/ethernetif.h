#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__
#ifdef USE_LWIP_CODE

#include "lwip/err.h"
#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
void ethernetif_input( void * pvParameters );
#endif 
#endif 