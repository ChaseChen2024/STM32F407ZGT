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
#include "bsp_debug_usart.h"
#include "bsp_spi_flash.h"
#include "bsp_sram.h"	


/*device*/

#include <string.h>
#include <stdio.h>
#include <time.h>

//  EXSRAM
#define __EXRAM  __attribute__ ((section (".ext_sram")))


#endif //__USER_H



