#ifndef __USER_H
#define __USER_H
 
/*freertos*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*stm32 lib*/
#include "stm32f4xx.h"
#ifdef USE_LWIP_CODE
#include "stm32f4x7_phy.h"
#endif
/*bsp driver*/
#include "bsp_usart3.h"
#ifdef USE_FLASH_SPI1_CODE
#include "bsp_spi_flash.h"
#endif
#include "bsp_sram.h"	


#ifdef USE_EASYLOGGER_CODE
#include "elog.h"
#endif // DEBUG
/*device*/

#include <string.h>
#include <stdio.h>
#include <time.h>

//  EXSRAM
#define __EXRAM  __attribute__ ((section (".ext_sram")))

#ifdef USE_EASYLOGGER_CODE
#define ELOG_BSP "ELOG_BSP"
#define ELOG_APP "ELOG_APP"
#define ELOG_NW "ELOG_NW"
#define ELOG_LVGL "ELOG_LVGL"
#define ELOG_LWIP "ELOG_LWIP"
#define ELOG_MQTT "ELOG_MQTT"
#define ELOG_LETTERSHELL "ELOG_LETTERSHELL"
#else
#define elog_a(tag, ...)     printf(__VA_ARGS__);printf("\r\n")
#define elog_e(tag, ...)     printf(__VA_ARGS__);printf("\r\n")
#define elog_w(tag, ...)     printf(__VA_ARGS__);printf("\r\n")
#define elog_i(tag, ...)     printf(__VA_ARGS__);printf("\r\n")
#define elog_d(tag, ...)     printf(__VA_ARGS__);printf("\r\n")

#define log_a(...)     printf(__VA_ARGS__);printf("\r\n")
#define log_e(...)     printf(__VA_ARGS__);printf("\r\n")
#define log_w(...)     printf(__VA_ARGS__);printf("\r\n")
#define log_i(...)     printf(__VA_ARGS__);printf("\r\n")
#define log_d(...)     printf(__VA_ARGS__);printf("\r\n")
#endif






void soft_reset(void);
#endif //__USER_H



