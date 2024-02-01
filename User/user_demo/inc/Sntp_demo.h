#ifndef __SNTP_DEMO_H
#define __SNTP_DEMO_H



#include "stm32f4xx.h"
#include <stdio.h>


long SNTP_Demo_Task_Init(void);
void Sntp_BSP_Init(void);
void sntp_set_time(uint32_t sntp_time);
#endif
