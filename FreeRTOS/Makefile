
# C sources
#FreeRTOS´úÂë
C_SOURCES +=  \
FreeRTOS/src/croutine.c \
FreeRTOS/src/event_groups.c \
FreeRTOS/src/list.c \
FreeRTOS/src/queue.c \
FreeRTOS/src/tasks.c \
FreeRTOS/src/timers.c \
FreeRTOS/port/MemMang/heap_4.c \
FreeRTOS/port/GCC/ARM_CM4F/port.c \




C_INCLUDES +=  \
-IFreeRTOS \
-IFreeRTOS/include \
-IFreeRTOS/port/GCC/ARM_CM4F \


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
