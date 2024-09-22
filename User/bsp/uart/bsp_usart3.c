
#include "bsp_usart3.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

static void Usart3_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = USART3_SHELL_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
#if 0
uint8_t READ_IT_FLAG = 0;
uint8_t GET_FREERTOS_FLAG = 0;
char Usart3_Rx_Buf[UART3_BUFF_SIZE];
void Uart3_DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  DMA_DeInit(DMA1_Stream1);  
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart3_Rx_Buf;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = UART3_BUFF_SIZE;
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
  DMA_Init(DMA1_Stream1, &DMA_InitStructure);
  DMA_ClearFlag(DMA1_Stream1,DMA_FLAG_TCIF2);
  DMA_ITConfig(DMA1_Stream1, DMA_IT_TE, ENABLE);
  DMA_Cmd (DMA1_Stream1,ENABLE);
}

#endif
void Usart3_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd(USART3_RX_GPIO_CLK|USART3_TX_GPIO_CLK,ENABLE);
 
  RCC_APB1PeriphClockCmd(USART3_CLK, ENABLE);
  
  GPIO_PinAFConfig(USART3_RX_GPIO_PORT,USART3_RX_SOURCE,USART3_RX_AF);

  GPIO_PinAFConfig(USART3_TX_GPIO_PORT,USART3_TX_SOURCE,USART3_TX_AF);

  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = USART3_TX_PIN;  
  GPIO_Init(USART3_TX_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = USART3_RX_PIN;
  GPIO_Init(USART3_RX_GPIO_PORT, &GPIO_InitStructure);


  USART_InitStructure.USART_BaudRate = USART3_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3_SHELL, &USART_InitStructure); 
	

	Usart3_NVIC_Configuration();
  USART_ITConfig(USART3_SHELL, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3_SHELL, ENABLE);

#if 0
  USART_ITConfig(USART3_SHELL, USART_IT_IDLE, ENABLE);
	USART_Cmd(USART3_SHELL, ENABLE);
  Uart3_DMA_Config();
  USART_DMACmd(USART3_SHELL, USART_DMAReq_Rx, ENABLE);
#endif
  
}
#if 0
SemaphoreHandle_t Uart3_BinarySem_Handle;
void Uart3_DMA_Rx_Data(void)
{
  if(GET_FREERTOS_FLAG == 1)
  {
    BaseType_t pxHigherPriorityTaskWoken;

    DMA_Cmd(DMA1_Stream1, DISABLE);      

    DMA_ClearFlag(DMA1_Stream1,DMA_FLAG_TCIF2);         

    DMA_SetCurrDataCounter(DMA1_Stream1,UART3_BUFF_SIZE);     
    DMA_Cmd(DMA1_Stream1, ENABLE);

    xSemaphoreGiveFromISR(Uart3_BinarySem_Handle,&pxHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);  
  }
  
}
#endif
void USART3_SendData(USART_TypeDef* USARTx, uint16_t Data)
{
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  assert_param(IS_USART_DATA(Data)); 
    
  /* Transmit Data */
  USARTx->DR = (Data & (uint16_t)0x01FF);
}
int _write (int fd, char *pBuffer, int size)
{
    for (int i = 0; i < size; i++)
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
        USART3_SendData(USART3, (uint8_t) pBuffer[i]);
    }
    return size;
}
