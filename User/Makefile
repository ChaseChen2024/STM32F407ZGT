
# C sources

C_SOURCES +=  \
User/main.c \
User/stm32f4xx_it.c \
User/uart/bsp_debug_usart.c \
User/uart/bsp_usart6.c \
User/key/bsp_key.c \
User/led/bsp_led.c \
User/rtc/bsp_rtc.c \
User/log/log.c \
User/flash/bsp_spi_flash.c \
User/sdio/bsp_sdio_sd.c \
User/sram/bsp_sram.c \
User/module/module.c \
User/module/atproc.c \
User/app/usb_bsp.c \
User/app/usbd_desc.c \
User/app/usbd_storage_msd.c \
User/app/usbd_usr.c \
User/lcd/lcd.c \
User/my_gui/my_gui.c \



#demo 
C_SOURCES +=  \
User/user_demo/src/FatFs_demo.c \
User/user_demo/src/Sdio_demo.c \
User/user_demo/src/Key_demo.c \
User/user_demo/src/Usb_demo.c \
User/user_demo/src/Rtc_demo.c \
User/user_demo/src/Lcd_demo.c \

C_INCLUDES +=  \
-IUser \
-IUser/uart \
-IUser/key \
-IUser/led \
-IUser/rtc \
-IUser/flash \
-IUser/sdio \
-IUser/app \
-IUser/sram \
-IUser/log \
-IUser/module \
-IUser/lcd \
-IUser/user_demo/inc \
-IUser/my_gui \


ifeq ($(BUILE_LWIP),y)
C_SOURCES +=  \
User/ETH/stm32f4x7_phy.c \
User/arch/sys_arch.c \
User/arch/ethernetif.c \
User/arch/netconf.c \

#demo 
C_SOURCES +=  \
User/user_demo/src/Lwip_Tcp_Client_Socket_demo.c \
User/user_demo/src/Sntp_demo.c \
User/user_demo/src/Tftp_demo.c \
User/user_demo/src/client.c \


C_INCLUDES +=  \
-IUser/ETH \
-IUser/arch \

endif

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
