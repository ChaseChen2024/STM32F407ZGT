
#include "bsp_usart1.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

static void Usart1_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = USART1_SHELL_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
uint8_t READ_IT_FLAG = 0;
uint8_t GET_FREERTOS_FLAG = 0;
char Usart1_Rx_Buf[USART1_BUFF_SIZE];
void Usart1_DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  DMA_DeInit(DMA2_Stream2);  
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart1_Rx_Buf;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = USART1_BUFF_SIZE;
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
  DMA_Init(DMA2_Stream2, &DMA_InitStructure);
  DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2);
  DMA_ITConfig(DMA2_Stream2, DMA_IT_TE, ENABLE);
  DMA_Cmd (DMA2_Stream2,ENABLE);
}


void Usart1_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd(USART1_RX_GPIO_CLK|USART1_TX_GPIO_CLK,ENABLE);
 
  RCC_APB2PeriphClockCmd(USART1_CLK, ENABLE);
  
  GPIO_PinAFConfig(USART1_RX_GPIO_PORT,USART1_RX_SOURCE,USART1_RX_AF);

  GPIO_PinAFConfig(USART1_TX_GPIO_PORT,USART1_TX_SOURCE,USART1_TX_AF);

  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;  
  GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
  GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStructure);


  USART_InitStructure.USART_BaudRate = USART1_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1_SHELL, &USART_InitStructure); 
	

	Usart1_NVIC_Configuration();
  USART_ITConfig(USART1_SHELL, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART1_SHELL, ENABLE);

  Usart1_DMA_Config();
  USART_DMACmd(USART1_SHELL, USART_DMAReq_Rx, ENABLE);
  
}
SemaphoreHandle_t Usart1_BinarySem_Handle;
void Usart1_DMA_Rx_Data(void)
{
  if(GET_FREERTOS_FLAG == 1)
  {
    BaseType_t pxHigherPriorityTaskWoken;

    DMA_Cmd(DMA2_Stream2, DISABLE);      

    DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2);         

    DMA_SetCurrDataCounter(DMA2_Stream2,USART1_BUFF_SIZE);     
    DMA_Cmd(DMA2_Stream2, ENABLE);

    xSemaphoreGiveFromISR(Usart1_BinarySem_Handle,&pxHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);  
  }
  
}

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
void Usart_Transmit( USART_TypeDef * pUSARTx, char *pBuffer, unsigned int len )
{
	  for (int i = 0; i < len; i++)
    {
        while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
        USART_SendData(pUSARTx, (uint8_t) pBuffer[i]);
    }
    // return size;

}