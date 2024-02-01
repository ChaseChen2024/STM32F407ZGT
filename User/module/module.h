#ifndef _MODULE_H_
#define _MODULE_H_

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"


//引脚定义
/*******************************************************/
#define MODULE_USART                             USART3
#define MODULE_USART_CLK                         RCC_APB1Periph_USART3

#define MODULE_USART_RX_GPIO_PORT                GPIOB
#define MODULE_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define MODULE_USART_RX_PIN                      GPIO_Pin_11
#define MODULE_USART_RX_AF                       GPIO_AF_USART3
#define MODULE_USART_RX_SOURCE                   GPIO_PinSource11

#define MODULE_USART_TX_GPIO_PORT                GPIOB
#define MODULE_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOB
#define MODULE_USART_TX_PIN                      GPIO_Pin_10
#define MODULE_USART_TX_AF                       GPIO_AF_USART3
#define MODULE_USART_TX_SOURCE                   GPIO_PinSource10


#define MODULE_USART_IRQHandler                   USART3_IRQHandler
#define MODULE_USART_IRQ                 					USART3_IRQn
/************************************************************/
//串口波特率
#define MODULE_USART_BAUDRATE                    115200


#define POWER_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET)
#define POWER_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET)
#define RET_L   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)
#define RET_H   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)


// 串口对应的DMA请求通道
#define  USART_RX_DMA_CHANNEL      DMA_Channel_4
#define  DEBUG_USART_DMA_STREAM    DMA1_Stream1

// 一次发送的数据量
#define  USART_RBUFF_SIZE            256 

// 外设寄存器地址
#define  USART_DR_ADDRESS        (&USART3->DR)
void MODULE_INIT(void);
// void GM190_Send(uint8_t *buff, uint16_t len);
// void GM190_UartSend(uint8_t *s);
// void GM190_RecvBuff_Clear(void);
// uint8_t GM190_str_find(uint8_t *buff, uint8_t *str);
// uint8_t GM190_strstr(uint8_t *buff, uint8_t *str, uint16_t buff_len);
// uint8_t GM190_Cmd_Send(uint8_t *cmd, uint8_t *ack,uint16_t timeout);
void MODULE_DMA_Config(void);
void Uart_DMA_Rx_Data(void);
void get_rx_data(uint8_t* ack_data);
// void GM190_OPEN(void);
// void GM190_Client_ConnectServer(void);
#endif //_MODULE_H_
