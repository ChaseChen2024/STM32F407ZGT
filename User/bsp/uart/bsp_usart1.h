
#ifndef _USART1_H
#define	_USART1_H

#include "stm32f4xx.h"
#include <stdio.h>
#include "stm32f4xx_conf.h"


#define USART1_BUFF_SIZE 64
#define USART1_SHELL                             USART1

#define USART1_CLK                         RCC_APB2Periph_USART1
#define USART1_BAUDRATE                    115200

#define USART1_RX_GPIO_PORT                GPIOA
#define USART1_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define USART1_RX_PIN                      GPIO_Pin_10
#define USART1_RX_AF                       GPIO_AF_USART1
#define USART1_RX_SOURCE                   GPIO_PinSource10

#define USART1_TX_GPIO_PORT                GPIOA
#define USART1_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define USART1_TX_PIN                      GPIO_Pin_9
#define USART1_TX_AF                       GPIO_AF_USART1
#define USART1_TX_SOURCE                   GPIO_PinSource9

#define USART1_SHELL_IRQHandler                  USART1_IRQHandler
#define USART1_SHELL_IRQ                 				USART1_IRQn
/************************************************************/
extern char Usart1_Rx_Buf[USART1_BUFF_SIZE];
extern uint8_t READ_IT_FLAG;
extern uint8_t GET_FREERTOS_FLAG;
void Usart1_Config(void);
void Usart1_DMA_Rx_Data(void);
#endif /* __USART1_H */
