
#include <string.h>
#include <time.h>

#include "lwip/opt.h"
#include <sys/time.h>
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/apps/sntp.h"
#include <lwip/sockets.h>
#include "lwip/ip.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "user.h"

#include "netconf.h"
#include "sys_arch.h"

#ifdef USER_LEETTER_SHELL
#include "shell_port.h"
#endif


#ifdef USER_LEETTER_SHELL
void dhcp_test(void)
{
    ip_addr_t ipaddr = {0};
	int err = LwIP_DHCP(&ipaddr);
	if(err == 0)
	{
	shellPrint(&shell,"%d.%d.%d.%d\r\n",  \
        ((ipaddr.addr)&0x000000ff),       \
        (((ipaddr.addr)&0x0000ff00)>>8),  \
        (((ipaddr.addr)&0x00ff0000)>>16), \
        ((ipaddr.addr)&0xff000000)>>24);
	}
	else
		shellPrint(&shell,"dhcp error %d\r\n",err);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), dhcp, dhcp_test, lwip dhcp );

void ifconfig_test(void)
{
    shellPrint(&shell,"%d.%d.%d.%d\r\n",  \
        ((xnetif.ip_addr.addr)&0x000000ff),       \
        (((xnetif.ip_addr.addr)&0x0000ff00)>>8),  \
        (((xnetif.ip_addr.addr)&0x00ff0000)>>16), \
        ((xnetif.ip_addr.addr)&0xff000000)>>24);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ifconfig, ifconfig_test, lwip get ip );


#endif // USER_LEETTER_SHELL