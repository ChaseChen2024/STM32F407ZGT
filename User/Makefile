
# C sources

C_SOURCES +=  \
User/main.c \
User/stm32f4xx_it.c \
User/bsp/uart/bsp_debug_usart.c \
User/bsp/uart/bsp_usart3.c \
User/bsp/key/bsp_key.c \
User/bsp/rtc/bsp_rtc.c \
User/bsp/flash/bsp_spi_flash.c \
User/bsp/sram/bsp_sram.c \
User/app/nv/nv.c \
# User/bsp/led/bsp_led.c \
# User/bsp/sdio/bsp_sdio_sd.c \
# User/bsp/lcd/lcd.c \


ifeq ($(BUILE_USB),y)
C_SOURCES +=  \
User/bsp/usb/usb_bsp.c \
User/bsp/usb/usbd_desc.c \
User/bsp/usb/usbd_storage_msd.c \
User/bsp/usb/usbd_usr.c \

endif

ifeq ($(BUILE_LVGL),y)
C_SOURCES +=  \
User/app/my_gui/my_gui.c \
User/app/pg/pm_utils.c \
User/app/pg/pm.c \
User/app/pg/anima.c \

endif

#demo 
C_SOURCES +=  \
# User/app/user_demo/src/FatFs_demo.c \
# User/app/user_demo/src/Sdio_demo.c \
# User/app/user_demo/src/Key_demo.c \
# User/app/user_demo/src/Usb_demo.c \
# User/app/user_demo/src/Rtc_demo.c \
# User/app/user_demo/src/Lcd_demo.c \

C_INCLUDES +=  \
-IUser \
-IUser/bsp/uart \
-IUser/bsp/key \
-IUser/bsp/led \
-IUser/bsp/rtc \
-IUser/bsp/flash \
-IUser/bsp/sram \
-IUser/app/user_demo/inc \
-IUser/app/nv \
# -IUser/bsp/sdio \
# -IUser/bsp/lcd \
# -IUser/app/pg \
# -IUser/app/my_gui \



ifeq ($(BUILE_GNSS),y)
C_SOURCES +=  \
User/bsp/uart/bsp_usart6.c \
User/bsp/GNSS/gnss.c \
User/bsp/GNSS/NMEA0183.c \

C_INCLUDES +=  \
-IUser/bsp/GNSS \

endif

ifeq ($(BUILE_USB),y)
C_INCLUDES +=  \
-IUser/bsp/usb \

endif

ifeq ($(BUILE_LWIP),y)
C_SOURCES +=  \
User/bsp/ETH/stm32f4x7_phy.c \
User/bsp/arch/sys_arch.c \
User/bsp/arch/ethernetif.c \
User/bsp/arch/netconf.c \

#demo 
C_SOURCES +=  \
User/app/user_demo/src/Sntp_demo.c \
# User/app/user_demo/src/Lwip_Tcp_Client_Socket_demo.c \
# User/app/user_demo/src/Tftp_demo.c \
# User/app/user_demo/src/client.c \

C_SOURCES +=  \
User/app/nw/nw.c \


C_INCLUDES +=  \
-IUser/bsp/ETH \
-IUser/bsp/arch \

C_INCLUDES +=  \
-IUser/app/nw \

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
