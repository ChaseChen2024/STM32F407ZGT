
# C sources
C_SOURCES +=  \
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
Libraries/STM32F4x7_ETH_Driver/src/stm32f4x7_eth.c \
# Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_lptim.c \
# Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c \


ifeq ($(BUILE_USB),y)
C_SOURCES +=  \
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

C_INCLUDES +=  \
-ILibraries/STM32_USB_Device_Library/Core/inc \
-ILibraries/STM32_USB_Device_Library/Class/msc/inc \
-ILibraries/STM32_USB_OTG_Driver/inc \

endif


C_INCLUDES +=  \
-ILibraries/STM32F4xx_StdPeriph_Driver/inc \
-ILibraries/CMSIS/Include \
-ILibraries/CMSIS/Device/ST/STM32F4xx/Include \
-ILibraries/STM32F4x7_ETH_Driver/inc \




CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections


# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# ifeq ($(DEBUG), 1)
# CFLAGS += -g -gdwarf-2
# endif	


#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
