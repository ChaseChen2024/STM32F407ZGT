#include "module.h"

#include "string.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
//   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* �����ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = MODULE_USART_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}
//����3��ʼ��
static void MODULE_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd( MODULE_USART_RX_GPIO_CLK|MODULE_USART_TX_GPIO_CLK, ENABLE);
 MODULE_DMA_Config();
  /* ʹ�� UART ʱ�� */
  RCC_APB1PeriphClockCmd(MODULE_USART_CLK, ENABLE);
  
  /* ���� PXx �� USARTx_Tx*/
  GPIO_PinAFConfig(MODULE_USART_RX_GPIO_PORT,MODULE_USART_RX_SOURCE, MODULE_USART_RX_AF);

  /*  ���� PXx �� USARTx__Rx*/
  GPIO_PinAFConfig(MODULE_USART_TX_GPIO_PORT,MODULE_USART_TX_SOURCE,MODULE_USART_TX_AF);

  /* ����Tx����Ϊ���ù���  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = MODULE_USART_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(MODULE_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* ����Rx����Ϊ���ù��� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = MODULE_USART_RX_PIN;
  GPIO_Init(MODULE_USART_RX_GPIO_PORT, &GPIO_InitStructure);
			
  /* ���ô���RS232_USART ģʽ */
  USART_InitStructure.USART_BaudRate = MODULE_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(MODULE_USART, &USART_InitStructure); 
	NVIC_Configuration();
	// ���� ���ڿ���IDEL �ж�
	USART_ITConfig(MODULE_USART, USART_IT_IDLE, ENABLE);  
  // ��������DMA����
	USART_DMACmd(MODULE_USART, USART_DMAReq_Rx, ENABLE); 
	// /*���ô��ڽ����ж�*/
	// USART_ITConfig(MODULE_USART, USART_IT_RXNE, ENABLE);
	
  USART_Cmd(MODULE_USART, ENABLE);
}
char Usart_Rx_Buf[USART_RBUFF_SIZE];
void MODULE_DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  // ����DMAʱ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  // DMA��λ
  DMA_DeInit(DEBUG_USART_DMA_STREAM);  
  // ����DMAͨ��
  DMA_InitStructure.DMA_Channel = USART_RX_DMA_CHANNEL;  
  /*����DMAԴ���������ݼĴ�����ַ*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART_DR_ADDRESS;
  // �ڴ��ַ(Ҫ����ı�����ָ��)
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Usart_Rx_Buf;
  // ���򣺴��ڴ浽����	
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  // �����С	
  DMA_InitStructure.DMA_BufferSize = USART_RBUFF_SIZE;
  // �����ַ����	    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  // �ڴ��ַ����
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  // �������ݵ�λ	
  DMA_InitStructure.DMA_PeripheralDataSize = 
  DMA_PeripheralDataSize_Byte;
  // �ڴ����ݵ�λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
  // DMAģʽ��һ�λ���ѭ��ģʽ
  //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
  // ���ȼ�����	
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 
  // ��ֹ�ڴ浽�ڴ�Ĵ���
  /*����FIFO*/
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*�洢��ͻ������ 1������*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*����ͻ������ 1������*/
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
  /*����DMA2��������7*/		   
  DMA_Init(DEBUG_USART_DMA_STREAM, &DMA_InitStructure);
  // ���DMA���б�־
  DMA_ClearFlag(DEBUG_USART_DMA_STREAM,DMA_FLAG_TCIF2);
  DMA_ITConfig(DEBUG_USART_DMA_STREAM, DMA_IT_TE, ENABLE);
  // ʹ��DMA
  DMA_Cmd (DEBUG_USART_DMA_STREAM,ENABLE);
}
void get_rx_data(uint8_t* ack_data)
{
	memcpy(ack_data,Usart_Rx_Buf,sizeof(Usart_Rx_Buf));
	memset(Usart_Rx_Buf,0,USART_RBUFF_SIZE);
}
extern SemaphoreHandle_t BinarySem_Handle;

void Uart_DMA_Rx_Data(void)
{
  BaseType_t pxHigherPriorityTaskWoken;
  // �ر�DMA ����ֹ����
  DMA_Cmd(DEBUG_USART_DMA_STREAM, DISABLE);      
  // ��DMA��־λ
  DMA_ClearFlag(DEBUG_USART_DMA_STREAM,DMA_FLAG_TCIF2);         
  //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
  DMA_SetCurrDataCounter(DEBUG_USART_DMA_STREAM,USART_RBUFF_SIZE);     
  DMA_Cmd(DEBUG_USART_DMA_STREAM, ENABLE);       
  /* 
  xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore,
                      BaseType_t *pxHigherPriorityTaskWoken);
  */

  //������ֵ�ź��� �����ͽ��յ������ݱ�־����ǰ̨�����ѯ
  xSemaphoreGiveFromISR(BinarySem_Handle,&pxHigherPriorityTaskWoken);	//�ͷŶ�ֵ�ź���
  //�����Ҫ�Ļ�����һ�������л���ϵͳ���ж��Ƿ���Ҫ�����л�
  portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);

}
static void MODULE_EN_PIN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2  ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
static void MODULE_REST_PIN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void MODULE_INIT(void)
{
	MODULE_EN_PIN_Init();
	MODULE_REST_PIN_Init();
	MODULE_USART_Config();
	GPIO_SetBits(GPIOE,GPIO_Pin_2);
	GPIO_SetBits(GPIOG,GPIO_Pin_15);
}

#if 0
void MODULE_Send(uint8_t *buff, uint16_t len)
{
	Usart2_Send_Poll(buff,len);
}
void MODULE_UartSend(uint8_t *s) 
{
  	Usart2_SendString(s);
}
void MODULE_RecvBuff_Clear(void)
{
	memset(Rx2Buff, 0, sizeof(Rx2Buff));
	Rx2BuffCount = 0;
}
uint8_t MODULE_str_find(uint8_t *buff, uint8_t *str)
{
	
	uint8_t *p,*q;
	
	p=buff;
	q=str;
	while(*p != '\0')
	{
		
		if(*p == *q)
		{
			q++;
			if(*q == '\0')   //ƥ�����
			{
			
				return 0;
			}
		}
		else
		{
			q=str;
			
		}
		p++;
	}
	return 1;
}
uint8_t MODULE_strstr(uint8_t *buff, uint8_t *str, uint16_t buff_len)
{
	uint8_t *p,*q;
	uint16_t i;
	
	p=buff;
	q=str;
	for(i = 0; i < buff_len; i++)
	{
		if(*p == *q)
		{
			q++;
			if(*q == '\0')   //ƥ�����
			{
				return 0;
			}
		}
		else
		{
			q=str;
		}
		p++;
	}
	return 1;
}
uint8_t MODULE_Cmd_Send(uint8_t *cmd, uint8_t *ack,uint16_t timeout)
{

	MODULE_RecvBuff_Clear();
	
	HAL_Delay(10);
	GM190_UartSend(cmd);         //��������
	
	while(timeout--)
	{
		
		//HAL_Delay(1);    //��ʱ1mS,�ȴ��������
		if(MODULE_str_find(Rx2Buff,(uint8_t *)ack) == 0)							//����յ�����
		{
				return 0;
		}
	}
	return 1;

}

void GM190_Client_ConnectServer(void)
{

	while(GM190_Cmd_Send((uint8_t *)"AT+CEREG?\r\n",(uint8_t*)"+CEREG: 0,1",1000));
	HAL_Delay(500);

	OLED_ShowString(16*0,16*2,"AT+CEREG?",16);
	OLED_Refresh();

	while(GM190_Cmd_Send((uint8_t *)"AT+ZIPCALL=1\r\n",(uint8_t*)"OK",1000));
		OLED_ShowString(16*0,16*2,"AT+ZIPCALL=1",16);
	OLED_Refresh();
	HAL_Delay(500);
	while(GM190_Cmd_Send((uint8_t *)"AT+ZMQGETINFO=1,0,a1Alavpm3AJ,Dm5OSQq1SGMoKYdN,GM190_1,c1be1116b03c15d66e794f48e9405341\r\n",(uint8_t*)"+ZMQGETINFO: 1,0",1000));
		OLED_ShowString(16*0,16*2,"AT+ZMQGETINFO=1",16);
	OLED_Refresh();
	HAL_Delay(500);
	while(GM190_Cmd_Send((uint8_t *)"AT+ZMQNEW=1,a1Alavpm3AJ.iot-as-mqtt.cn-shanghai.aliyuncs.com,443,500,1000,1\r\n",(uint8_t*)"OK",1000));
		OLED_ShowString(16*0,16*2,"AT+ZMQNEW=1",16);
	OLED_Refresh();
	
	HAL_Delay(500);
	while(GM190_Cmd_Send((uint8_t *)"AT+ZMQCON=1,4,\"GM190_1&a1Alavpm3AJ|timestamp=2524608000000,_v=paho-c-1.0.0,securemode=3,signmethod=hmacsha256,lan=C|\",1000,1,0,GM190_1&a1Alavpm3AJ,E069F6C5ECF83496F4049AB99038CBCE3D8A80F9C17A82F96FCC6FCFB20CD516\r\n",(uint8_t*)"OK",1000));
	OLED_ShowString(16*0,16*2,(uint8_t *)"AT+ZMQCON=1  ",16);
	OLED_Refresh();
	while(GM190_Cmd_Send((uint8_t *)"AT+ZMQSUB=1,/a1Alavpm3AJ/GM190_1/user/sub,1\r\n",(uint8_t*)"OK",1000));
	OLED_ShowString(16*0,16*2,(uint8_t *)"AT+ZMQSUB=1  ",16);
	OLED_Refresh();
	//AT+ZMQPUB=1,/sys/a1Alavpm3AJ/GM190_1/thing/event/property/post,1,0,0,91,7b226964223a22313233222c2276657273696f6e223a22312e30222c226d6574686f64223a227468696e672e6576656e742e70726f70657274792e706f7374222c22706172616d73223a7b224c696768744c7578223a3135357d7d
	while(GM190_Cmd_Send((uint8_t *)"AT+ZMQPUB=1,/sys/a1Alavpm3AJ/GM190_1/thing/event/property/post,1,0,0,91,7b226964223a22313233222c2276657273696f6e223a22312e30222c226d6574686f64223a227468696e672e6576656e742e70726f70657274792e706f7374222c22706172616d73223a7b224c696768744c7578223a3135357d7d\r\n",(uint8_t*)"OK",1000));

	OLED_Clear();
}
#endif