
#ifndef _USART3_H
#define	_USART3_H

#include "stm32f4xx.h"
#include <stdio.h>
#include "stm32f4xx_conf.h"
// #include "FreeRTOS.h"
// #include "task.h"
// #include "queue.h"
// #include "semphr.h"

#define UART3_BUFF_SIZE 64
#define USART3_SHELL                             USART3

#define USART3_CLK                         RCC_APB1Periph_USART3
#define USART3_BAUDRATE                    115200

#define USART3_RX_GPIO_PORT                GPIOB
#define USART3_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define USART3_RX_PIN                      GPIO_Pin_11
#define USART3_RX_AF                       GPIO_AF_USART3
#define USART3_RX_SOURCE                   GPIO_PinSource11

#define USART3_TX_GPIO_PORT                GPIOB
#define USART3_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define USART3_TX_PIN                      GPIO_Pin_10
#define USART3_TX_AF                       GPIO_AF_USART3
#define USART3_TX_SOURCE                   GPIO_PinSource10

#define USART3_SHELL_IRQHandler                  USART3_IRQHandler
#define USART3_SHELL_IRQ                 				USART3_IRQn
/************************************************************/
// extern char Usart3_Rx_Buf[UART3_BUFF_SIZE];
// extern uint8_t READ_IT_FLAG;
// extern uint8_t GET_FREERTOS_FLAG;
// extern SemaphoreHandle_t Uart3_BinarySem_Handle;
void Usart3_Config(void);
// void Uart3_DMA_Rx_Data(void);
#endif /* __USART3_H */
