
#ifndef _USART6_H
#define	_USART6_H

#include "stm32f4xx.h"
#include <stdio.h>
#include "stm32f4xx_conf.h"

#define UART6_BUFF_SIZE 1024
#define USART                             USART6

#define USART_CLK                         RCC_APB2Periph_USART6
#define USART_BAUDRATE                    9600

#define USART_RX_GPIO_PORT                GPIOC
#define USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define USART_RX_PIN                      GPIO_Pin_7
#define USART_RX_AF                       GPIO_AF_USART6
#define USART_RX_SOURCE                   GPIO_PinSource7

#define USART_TX_GPIO_PORT                GPIOC
#define USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define USART_TX_PIN                      GPIO_Pin_6
#define USART_TX_AF                       GPIO_AF_USART6
#define USART_TX_SOURCE                   GPIO_PinSource6

#define USART_IRQHandler                  USART6_IRQHandler
#define USART_IRQ                 				USART6_IRQn
/************************************************************/
void USART6_Config(void);
void Uart6_DMA_Rx_Data(void);
// #define USART6_REC_NUM  			100  	//定义最大接收字节数 200
// extern u8 uart_byte_count;          //uart_byte_count要小于USART_REC_LEN
// extern u8 receive_str[USART6_REC_NUM];  

// void uart6_init(u32 bound);
// void uart6SendChars(u8 *str, u16 strlen);
#endif /* __USART1_H */
