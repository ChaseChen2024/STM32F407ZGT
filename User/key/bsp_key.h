#ifndef __KEY_H
#define	__KEY_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
//���Ŷ���
/*******************************************************/
#define KEY0_PIN                  GPIO_Pin_6                
#define KEY0_GPIO_PORT            GPIOF                   
#define KEY0_GPIO_CLK             RCC_AHB1Periph_GPIOF

#define KEY1_PIN                  GPIO_Pin_7                
#define KEY1_GPIO_PORT            GPIOF                   
#define KEY1_GPIO_CLK             RCC_AHB1Periph_GPIOF
/*******************************************************/

 /** �������±��ú�
	* ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	* ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON	0
#define KEY_OFF	1

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);

#endif /* __LED_H */

