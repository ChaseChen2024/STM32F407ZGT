######################################
# target
######################################
TARGET = QT201

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O0
#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
User/main.c \
User/stm32f4xx_it.c \
User/uart/bsp_debug_usart.c \
User/key/bsp_key.c \
User/led/bsp_led.c \
User/rtc/bsp_rtc.c \
User/log/log.c \
User/flash/bsp_spi_flash.c \
User/sdio/bsp_sdio_sd.c \
User/FatFs/diskio.c \
User/FatFs/ff.c \
User/FatFs/option/cc936.c \
User/FatFs/option/syscall.c \
User/sram/bsp_sram.c \
User/module/module.c \
User/module/atproc.c \
User/app/usb_bsp.c \
User/app/usbd_desc.c \
User/app/usbd_storage_msd.c \
User/app/usbd_usr.c \
User/ETH/stm32f4x7_phy.c \
User/arch/sys_arch.c \
User/arch/ethernetif.c \
User/arch/netconf.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_aes.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_des.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_tdes.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dcmi.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dfsdm.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dsi.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash_ramfunc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmpi2c.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_md5.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_sha1.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_lptim.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_qspi.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sai.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spdifrx.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c \
Libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c \
Libraries/STM32_USB_Device_Library/Core/src/usbd_core.c \
Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
Libraries/STM32_USB_Device_Library/Core/src/usbd_req.c \
Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_bot.c \
Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_core.c \
Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_data.c \
Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_scsi.c \
Libraries/STM32_USB_OTG_Driver/src/usb_core.c \
Libraries/STM32_USB_OTG_Driver/src/usb_dcd.c \
Libraries/STM32_USB_OTG_Driver/src/usb_dcd_int.c \
Libraries/STM32F4x7_ETH_Driver/src/stm32f4x7_eth.c \
FreeRTOS/src/croutine.c \
FreeRTOS/src/event_groups.c \
FreeRTOS/src/list.c \
FreeRTOS/src/queue.c \
FreeRTOS/src/tasks.c \
FreeRTOS/src/timers.c \
FreeRTOS/port/MemMang/heap_4.c \
FreeRTOS/port/GCC/ARM_CM4F/port.c \
LWIP/api/api_lib.c \
LWIP/api/api_msg.c \
LWIP/api/err.c \
LWIP/api/if_api.c \
LWIP/api/netbuf.c \
LWIP/api/netdb.c \
LWIP/api/sockets.c \
LWIP/api/tcpip.c \
LWIP/core/ipv4/autoip.c \
LWIP/core/ipv4/dhcp.c \
LWIP/core/ipv4/etharp.c \
LWIP/core/ipv4/icmp.c \
LWIP/core/ipv4/igmp.c \
LWIP/core/ipv4/ip4.c \
LWIP/core/ipv4/ip4_addr.c \
LWIP/core/ipv4/ip4_frag.c \
LWIP/core/altcp.c \
LWIP/core/altcp_alloc.c \
LWIP/core/altcp_tcp.c \
LWIP/core/def.c \
LWIP/core/dns.c \
LWIP/core/inet_chksum.c \
LWIP/core/init.c \
LWIP/core/ip.c \
LWIP/core/mem.c \
LWIP/core/memp.c \
LWIP/core/netif.c \
LWIP/core/pbuf.c \
LWIP/core/raw.c \
LWIP/core/stats.c \
LWIP/core/sys.c \
LWIP/core/tcp.c \
LWIP/core/tcp_in.c \
LWIP/core/tcp_out.c \
LWIP/core/timeouts.c \
LWIP/core/udp.c \
LWIP/netif/ethernet.c \
LWIP/netif/lowpan6.c \
LWIP/netif/lowpan6_ble.c \
LWIP/netif/lowpan6_common.c \
LWIP/netif/zepif.c \
LWIP/netif/bridgeif.c \
LWIP/apps/sntp/sntp.c \
LWIP/apps/tftp/tftp_server.c \
User/user_demo/src/FatFs_demo.c \
User/user_demo/src/Sdio_demo.c \
User/user_demo/src/Key_demo.c \
User/user_demo/src/Usb_demo.c \
User/user_demo/src/Lwip_Tcp_Client_Socket_demo.c \
User/user_demo/src/Rtc_demo.c \
User/user_demo/src/Sntp_demo.c \
User/user_demo/src/Tftp_demo.c \
User/user_demo/src/client.c \
# ASM sources
ASM_SOURCES =  \
startup_stm32f40xx.s

# ASM sources
ASMM_SOURCES = 



#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_STDPERIPH_DRIVER\
-DSTM32F40_41xxx\
-DUSE_USB_OTG_FS\
-DUSE_EMBEDDED_PHY


# AS includes
AS_INCLUDES =  \
-IUser \

# C includes
C_INCLUDES =  \
-IUser \
-IUser/uart \
-IUser/key \
-IUser/led \
-IUser/rtc \
-IUser/flash \
-IUser/sdio \
-IUser/FatFs \
-IUser/app \
-IUser/ETH \
-IUser/arch \
-IUser/sram \
-IUser/log \
-IUser/module \
-IUser/user_demo/inc \
-ILibraries/STM32F4xx_StdPeriph_Driver/inc \
-ILibraries/CMSIS/Include \
-ILibraries/CMSIS/Device/ST/STM32F4xx/Include \
-ILibraries/STM32_USB_Device_Library/Core/inc \
-ILibraries/STM32_USB_Device_Library/Class/msc/inc \
-ILibraries/STM32_USB_OTG_Driver/inc \
-ILibraries/STM32F4x7_ETH_Driver/inc \
-IFreeRTOS/include \
-IFreeRTOS/port/GCC/ARM_CM4F \
-ILWIP/include \
-ILWIP/include/lwip \
-ILWIP/include/netif \
-ILWIP/include/compat \
-ILWIP/include/lwi/apps \
-ILWIP/include/lwip/priv \
-ILWIP/include/lwip/prot \


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F407ZGTx_FLASH.ld
# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#OBJECTS 按照你实际的工程设置来 参考你的elf文件相关的编译规则

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASMM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASMM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rmdir /s /q $(BUILD_DIR)
#   windows 上使用上面命令，linux使用下面的命令
#   -rm -fR $(BUILD_DIR)

#烧录命令
download:
	-openocd -f TOOL/cmsis-dap-v1.cfg -f TOOL/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
download_dap:
	-openocd -f TOOL/cmsis-dap-v1.cfg -f TOOL/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
download_stlinkv2:
	-openocd -f TOOL/stlink-v2.cfg -f TOOL/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
download_jlink:
	-openocd -f TOOL/jlink.cfg -f TOOL/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
