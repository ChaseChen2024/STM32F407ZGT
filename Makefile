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
BUILD_DIR = Build

######################################
# source
######################################
# C sources
# #STM32库函数
C_SOURCES = \

# ASM sources
ASM_SOURCES =  \
Startup/startup_stm32f40xx.s \

# ASM sources
ASMM_SOURCES = \
Component/Cm_Backtrace/cmb_fault.S \

######
#
BUILE_LWIP = y

BUILE_LVGL = n

BUILE_MQTT = y

BUILE_LETTER_SHELL = y

BUILE_USB = n

BUILE_GNSS = n

BUILE_CMBACKTRACE = y
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
-DUSE_EMBEDDED_PHY\

ifeq ($(BUILE_LWIP),y)
C_DEFS +=  -DUSE_LWIP -DUSE_LWIP_CODE\

ifeq ($(BUILE_MQTT),y)
C_DEFS += -DUSE_MQTT_CODE\

endif

endif

ifeq ($(BUILE_LVGL),y)
C_DEFS += -DUSE_LVGL\

endif

ifeq ($(BUILE_LETTER_SHELL),y)
C_DEFS += -DUSER_LEETTER_SHELL\

endif

ifeq ($(BUILE_USB),y)
C_DEFS += -DUSE_USB_OTG_FS -DUSE_USB_CODE -DUSB_CODE\

endif

ifeq ($(BUILE_GNSS),y)
C_DEFS += -DUSE_GNSS_NMEA -DUSE_GNSS_CODE \

endif

ifeq ($(BUILE_CMBACKTRACE),y)
C_DEFS += -DUSE_CMBACKTRACE_CODE\

endif

# AS includes
AS_INCLUDES =  \
-IUser \

# C includes
C_INCLUDES =  \



CFLAGS =
#stm32
include Libraries/Makefile
#freertos
include FreeRTOS/Makefile
#Component
include Component/Makefile
#Component
include User/Makefile
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
LDSCRIPT = Startup/STM32F407ZGTx_FLASH.ld
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
down:
	-openocd -f TOOL/debug/stlink.cfg -f TOOL/debug/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
download_dap:
	-openocd -f TOOL/debug/cmsis-dap-v1.cfg -f TOOL/debug/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
download_stlinkv2:
	-openocd -f TOOL/debug/stlink-v2.cfg -f TOOL/debug/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
download_jlink:
	-openocd -f TOOL/debug/jlink.cfg -f TOOL/debug/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
