#ifndef __LOG_H
#define	__LOG_H
 #include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "bsp_debug_usart.h"

#define LOG_MAX_BUFF 1024
void LOG(uint8_t MODULE_ID,char *sub_name,uint8_t level,const char *format,...);
char* subName1(char* file_name,char* fun_name,uint16_t line);

#define SUBNAME subName(__FILE__,__func__,__LINE__);

#define LOG_DEBUG_USER(fmt,arg...)        do{\
                                          printf("<<-DEBUG->>,[%s],[%s],[%d]"fmt"\n",__FILE__,__func__,__LINE__, ##arg);\
                                          }while(0)
// #define SUBNAME __FILE__"/"__func__"/"__LINE__
#endif /* __LOG_H */

