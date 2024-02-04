
# C sources
C_SOURCES +=  \
Component/LWIP/api/api_lib.c \
Component/LWIP/api/api_msg.c \
Component/LWIP/api/err.c \
Component/LWIP/api/if_api.c \
Component/LWIP/api/netbuf.c \
Component/LWIP/api/netdb.c \
Component/LWIP/api/sockets.c \
Component/LWIP/api/tcpip.c \
Component/LWIP/core/ipv4/autoip.c \
Component/LWIP/core/ipv4/dhcp.c \
Component/LWIP/core/ipv4/etharp.c \
Component/LWIP/core/ipv4/icmp.c \
Component/LWIP/core/ipv4/igmp.c \
Component/LWIP/core/ipv4/ip4.c \
Component/LWIP/core/ipv4/ip4_addr.c \
Component/LWIP/core/ipv4/ip4_frag.c \
Component/LWIP/core/altcp.c \
Component/LWIP/core/altcp_alloc.c \
Component/LWIP/core/altcp_tcp.c \
Component/LWIP/core/def.c \
Component/LWIP/core/dns.c \
Component/LWIP/core/inet_chksum.c \
Component/LWIP/core/init.c \
Component/LWIP/core/ip.c \
Component/LWIP/core/mem.c \
Component/LWIP/core/memp.c \
Component/LWIP/core/netif.c \
Component/LWIP/core/pbuf.c \
Component/LWIP/core/raw.c \
Component/LWIP/core/stats.c \
Component/LWIP/core/sys.c \
Component/LWIP/core/tcp.c \
Component/LWIP/core/tcp_in.c \
Component/LWIP/core/tcp_out.c \
Component/LWIP/core/timeouts.c \
Component/LWIP/core/udp.c \
Component/LWIP/netif/ethernet.c \
Component/LWIP/netif/lowpan6.c \
Component/LWIP/netif/lowpan6_ble.c \
Component/LWIP/netif/lowpan6_common.c \
Component/LWIP/netif/zepif.c \
Component/LWIP/netif/bridgeif.c \
Component/LWIP/apps/sntp/sntp.c \
Component/LWIP/apps/tftp/tftp_server.c \

C_SOURCES +=  \
Component/FatFs/diskio.c \
Component/FatFs/ff.c \
Component/FatFs/option/cc936.c \
Component/FatFs/option/syscall.c \

C_INCLUDES +=  \
-IComponent/LWIP/include \
-IComponent/LWIP/include/lwip \
-IComponent/LWIP/include/netif \
-IComponent/LWIP/include/compat \
-IComponent/LWIP/include/lwi/apps \
-IComponent/LWIP/include/lwip/priv \
-IComponent/LWIP/include/lwip/prot \

C_INCLUDES +=  \
-IComponent/FatFs \

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections


# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif	


#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
