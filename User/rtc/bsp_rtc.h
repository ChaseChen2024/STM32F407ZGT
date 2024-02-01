#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"

//�Ƿ�ʹ��LCD��ʾ
#define USE_LCD_DISPLAY

// ʱ��Դ�궨��
//#define RTC_CLOCK_SOURCE_LSE      
#define RTC_CLOCK_SOURCE_LSI

// �첽��Ƶ����
#define ASYNCHPREDIV         0X7F
// ͬ����Ƶ����
#define SYNCHPREDIV          0XFF



// ʱ��궨��
#define RTC_H12_AMorPM			 RTC_H12_AM  
#define HOURS                22          // 0~23
#define MINUTES              00          // 0~59
#define SECONDS              00          // 0~59

// ���ں궨��
#define WEEKDAY              6         // 1~7
#define DATE                 13         // 1~31
#define MONTH                1         // 1~12
#define YEAR                 24         // 0~99

// ʱ���ʽ�궨��
#define RTC_Format_BINorBCD  RTC_Format_BIN

// ������Ĵ����궨��
#define RTC_BKP_DRX          RTC_BKP_DR0
// д�뵽���ݼĴ��������ݺ궨��
#define RTC_BKP_DATA         0X32F2
 
                                  
void RTC_CLK_Config(void);
void RTC_TimeAndDate_Set(void);
void RTC_TimeAndDate_Show(void);
void Rtc_Init(void);

#endif // __RTC_H__
