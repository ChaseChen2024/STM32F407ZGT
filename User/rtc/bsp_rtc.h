#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"

//是否使用LCD显示
#define USE_LCD_DISPLAY

// 时钟源宏定义
//#define RTC_CLOCK_SOURCE_LSE      
#define RTC_CLOCK_SOURCE_LSI

// 异步分频因子
#define ASYNCHPREDIV         0X7F
// 同步分频因子
#define SYNCHPREDIV          0XFF



// 时间宏定义
#define RTC_H12_AMorPM			 RTC_H12_AM  
#define HOURS                22          // 0~23
#define MINUTES              00          // 0~59
#define SECONDS              00          // 0~59

// 日期宏定义
#define WEEKDAY              6         // 1~7
#define DATE                 13         // 1~31
#define MONTH                1         // 1~12
#define YEAR                 24         // 0~99

// 时间格式宏定义
#define RTC_Format_BINorBCD  RTC_Format_BIN

// 备份域寄存器宏定义
#define RTC_BKP_DRX          RTC_BKP_DR0
// 写入到备份寄存器的数据宏定义
#define RTC_BKP_DATA         0X32F2
 
                                  
void RTC_CLK_Config(void);
void RTC_TimeAndDate_Set(void);
void RTC_TimeAndDate_Show(void);
void Rtc_Init(void);

#endif // __RTC_H__
