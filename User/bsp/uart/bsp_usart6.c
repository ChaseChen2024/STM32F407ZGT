
#include "bsp_usart6.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

static void Usart6_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  NVIC_InitStructure.NVIC_IRQChannel = USART_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
extern char Usart6_Rx_Buf[UART6_BUFF_SIZE];
void Uart6_DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  DMA_DeInit(DMA2_Stream1);  
  DMA_InitStructure.DMA_Channel = DMA_Channel_5;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART6->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart6_Rx_Buf;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = UART6_BUFF_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;     
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
  DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF2);
  DMA_ITConfig(DMA2_Stream1, DMA_IT_TE, ENABLE);
  DMA_Cmd (DMA2_Stream1,ENABLE);
}

void USART6_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd(USART_RX_GPIO_CLK|USART_TX_GPIO_CLK,ENABLE);
 
  RCC_APB2PeriphClockCmd(USART_CLK, ENABLE);
  
  GPIO_PinAFConfig(USART_RX_GPIO_PORT,USART_RX_SOURCE,USART_RX_AF);

  GPIO_PinAFConfig(USART_TX_GPIO_PORT,USART_TX_SOURCE,USART_TX_AF);

  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = USART_TX_PIN  ;  
  GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = USART_RX_PIN;
  GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStructure);
  

  USART_InitStructure.USART_BaudRate = USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART, &USART_InitStructure); 
	
	Usart6_NVIC_Configuration();
	USART_ITConfig(USART, USART_IT_IDLE, ENABLE);
  USART_Cmd(USART, ENABLE);
  
	Uart6_DMA_Config();
  USART_DMACmd(USART, USART_DMAReq_Rx, ENABLE);
  
}


extern SemaphoreHandle_t Uart6_BinarySem_Handle;
void Uart6_DMA_Rx_Data(void)
{
  BaseType_t pxHigherPriorityTaskWoken;

  DMA_Cmd(DMA2_Stream1, DISABLE);      

  DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF2);         

  DMA_SetCurrDataCounter(DMA2_Stream1,UART6_BUFF_SIZE);     
  DMA_Cmd(DMA2_Stream1, ENABLE);       

  xSemaphoreGiveFromISR(Uart6_BinarySem_Handle,&pxHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);

}
