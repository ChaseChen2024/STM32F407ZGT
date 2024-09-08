
#include "FreeRTOS.h"
#include "task.h"

#include "lcd.h"
#include "oledfont.h"
#include "bmp.h"
#include "bsp_debug_usart.h"

u16 BACK_COLOR;

u8 SPI2_ReadWriteByte(u8 writeData);
SPI_InitTypeDef  SPI_InitStructure;

void spi2_set_speed(uint8_t spi_baudrate_prescaler);

void SPI2_Init(void)
{
	printf("[%s][%d]\r\n",__FILE__,__LINE__);
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_12|GPIO_Pin_14;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_12|GPIO_Pin_14);

 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);


	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
 
	SPI_Cmd(SPI2, ENABLE);

} 

void spi2_set_speed(uint8_t spi_baudrate_prescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(spi_baudrate_prescaler));
    SPI2->CR1 &= 0XFFC7;
    SPI2->CR1 |= spi_baudrate_prescaler;                
    SPI_Cmd(SPI2, ENABLE);
} 

u8 SPI2_ReadWriteByte(u8 writeData)
{		 			 
  	u8 waitnum=0;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}
	{  
		waitnum++;  
		if(waitnum>200)return 0;  
	}
	SPI_I2S_SendData(SPI2, writeData);
		
  	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){}
	{  
		waitnum++;  
		if(waitnum>200)return 0;  
	}
	return SPI_I2S_ReceiveData(SPI2);
 		    
}
#if 1
	
void DMA_Config(uint32_t TX_Buff,uint32_t SENDBUFF_SIZE)
{
    // �жϽṹ��
    NVIC_InitTypeDef NVIC_InitStructure;		
    // DMA�ṹ��
    DMA_InitTypeDef DMA_InitStructure;  		
    /* ʹ��DMAʱ�� */  		
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);	
    /* ��λ��ʼ��DMA������ */  
    DMA_DeInit(DMA1_Stream4);								
    /* ȷ��DMA��������λ��� */  
    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);	

    /* ���� DMA Stream */
    /* ͨ��0��������4 */	  
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    /* �����ַ */  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;	
    /* �ڴ��ַ(Ҫ����ı�����ָ��) ,DMA�洢��0��ַ*/  	
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TX_Buff;	
    /* ���򣺴洢�������� */			
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    /* ���ݴ����� ,������Ϊ0�� ʵ�ʷ���ʱ����������*/	    
    DMA_InitStructure.DMA_BufferSize = (uint32_t)SENDBUFF_SIZE;		
    /* ���������ģʽ */		
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    /* �洢������ģʽ */  	
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    /* �������ݳ���:16λ */	 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    /* �ڴ����ݳ���:16λ */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
    /* DMAģʽ������ģʽ */  		
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    /* ���ȼ����� */	 		
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    /* ����FIFO */
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;        		
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;   
    /* ����ͻ�����δ��� */  
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    		
    /* �洢��ͻ�����δ��� */  
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 

    /* ��ʼ��DMA Stream */		
    DMA_Init(DMA1_Stream4, &DMA_InitStructure);

    // /* ������������ж�  */		
    // DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);

    // // �жϳ�ʼ�� 
    // /* DMA�����ж�Դ */  
    // NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;	
    // /* �������ȼ� */  
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    // /* ��Ӧ���ȼ� */  
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				
    // /* ʹ���ⲿ�ж�ͨ�� */ 
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						 
    // /* ����NVIC */		
    // NVIC_Init(&NVIC_InitStructure);
}
// void DMA1_Stream4_IRQHandler(void)
// {
//     // DMA �������
//     if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4))	
//     {
//         // ���DMA������ɱ�־
//         //  printf("[%s][%d] dma send OK\r\n",__FILE__,__LINE__);
//         DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);	
//         // Ƭѡ���ߣ����ݷ������	
//         LCD_CS_SET;	
//         // LCD_DC_SET;
// 		sendFlg = 1;
// 		memset(Spi2DmaBuffer,0,240*60);
// 		//  lv_disp_flush_ready(&disp_drv_p);
//     }
// }
void DMA_Write_buf(uint32_t SizeLen)
{   
	// printf("[%s][%d] start dma\r\n",__FILE__,__LINE__);
	// �رշ��� DMA		
	DMA_Cmd(DMA1_Stream4, DISABLE);	
	// ���÷��͵�������    
	DMA_SetCurrDataCounter(DMA1_Stream4, SizeLen);
	// // �������
	// SPI2->DR;		
	// // ����DMA��־λ	
	// DMA_ClearFlag(DMA1_Stream4, DMA_IT_TCIF4);
	// Ƭѡ����,��������
	LCD_DC_SET;
	LCD_CS_CLR;

	// �������� DMA
	DMA_Cmd(DMA1_Stream4, ENABLE);
}

void LCD_Fill3(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t *color)
{
	uint16_t t = 1;
	uint32_t num,num1;
	num=(xend-xsta)*(yend-ysta);
	LCD_Address_Set(xsta, ysta, xend,yend);//������ʾ��Χ
	LCD_CS_CLR;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;	
	SPI_Init(SPI2, &SPI_InitStructure); 
	SPI_Cmd(SPI2, ENABLE);    //ʹ��SPI����

	while(t)
	{
	  if(num>(240*240))
		{
			num-=(240*240);
			num1=(240*240);
		}
		else
		{
			t=0;
			num1=num;
		}
		DMA_Config((uint32_t)color,num1);
		SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);
		DMA_Write_buf(num1);
		while(1)
		{
			if(DMA_GetFlagStatus(DMA1_Stream4,DMA_IT_TCIF4)!=RESET)//�ȴ�ͨ��4�������
			{
				DMA_ClearFlag(DMA1_Stream4,DMA_IT_TCIF4);//���ͨ��4������ɱ�־
				break; 
			}
		}
	}
	LCD_CS_SET;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	
	SPI_Init(SPI2, &SPI_InitStructure); 
	SPI_Cmd(SPI2, ENABLE);    //ʹ��SPI����

}

#endif
/******************************************************************************
      ����˵����LCD��������д�뺯��
      ������ݣ�dat  Ҫд��Ĳ�������
      ����ֵ��  ��
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	LCD_CS_CLR;
	SPI2_ReadWriteByte(dat);
	LCD_CS_SET;
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_DC_SET;//д����
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_DC_SET;//д����
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_CLR;//д����
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{

	if(USE_HORIZONTAL==0)
	{

		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+80);
		LCD_WR_DATA(y2+80);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//������д
	}
	else
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+80);
		LCD_WR_DATA(x2+80);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//������д
	}
}

void lcd_hardware_init(void)
{
	SPI2_Init();
}
/******************************************************************************
      ����˵����LCD��ʼ������
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void Lcd_Init(void)
{
	
	//  DMA_Config((uint32_t)Spi2DmaBuffer,240*60);
	LCD_RST_CLR;
	vTaskDelay(200);
	LCD_RST_SET;
	vTaskDelay(20);
//	OLED_BLK_Set();
	vTaskDelay(200);	
//************* Start Initial Sequence **********// 
LCD_WR_REG(0x36);
if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
else LCD_WR_DATA8(0xA0);

LCD_WR_REG(0x3A); 
LCD_WR_DATA8(0x05);

LCD_WR_REG(0xB2);
LCD_WR_DATA8(0x0C);
LCD_WR_DATA8(0x0C);
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0x33);
LCD_WR_DATA8(0x33); 

LCD_WR_REG(0xB7); 
LCD_WR_DATA8(0x35);  

LCD_WR_REG(0xBB);
LCD_WR_DATA8(0x37);

LCD_WR_REG(0xC0);
LCD_WR_DATA8(0x2C);

LCD_WR_REG(0xC2);
LCD_WR_DATA8(0x01);

LCD_WR_REG(0xC3);
LCD_WR_DATA8(0x12);   

LCD_WR_REG(0xC4);
LCD_WR_DATA8(0x20);  

LCD_WR_REG(0xC6); 
LCD_WR_DATA8(0x0F);    

LCD_WR_REG(0xD0); 
LCD_WR_DATA8(0xA4);
LCD_WR_DATA8(0xA1);

LCD_WR_REG(0xE0);
LCD_WR_DATA8(0xD0);
LCD_WR_DATA8(0x04);
LCD_WR_DATA8(0x0D);
LCD_WR_DATA8(0x11);
LCD_WR_DATA8(0x13);
LCD_WR_DATA8(0x2B);
LCD_WR_DATA8(0x3F);
LCD_WR_DATA8(0x54);
LCD_WR_DATA8(0x4C);
LCD_WR_DATA8(0x18);
LCD_WR_DATA8(0x0D);
LCD_WR_DATA8(0x0B);
LCD_WR_DATA8(0x1F);
LCD_WR_DATA8(0x23);

LCD_WR_REG(0xE1);
LCD_WR_DATA8(0xD0);
LCD_WR_DATA8(0x04);
LCD_WR_DATA8(0x0C);
LCD_WR_DATA8(0x11);
LCD_WR_DATA8(0x13);
LCD_WR_DATA8(0x2C);
LCD_WR_DATA8(0x3F);
LCD_WR_DATA8(0x44);
LCD_WR_DATA8(0x51);
LCD_WR_DATA8(0x2F);
LCD_WR_DATA8(0x1F);
LCD_WR_DATA8(0x1F);
LCD_WR_DATA8(0x20);
LCD_WR_DATA8(0x23);

LCD_WR_REG(0x21); 

LCD_WR_REG(0x11); 
//Delay (120); 

LCD_WR_REG(0x29); 
} 


/******************************************************************************
      ����˵����LCD��������
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_Clear(u16 Color)
{
	u16 i,j;  	
	LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	 {
	  for (j=0;j<LCD_H;j++)
	   	{
        	LCD_WR_DATA(Color);	 			 
	    }

	  }
}


/******************************************************************************
      ����˵����LCD��ʾ����
      ������ݣ�x,y   ��ʼ����
                index ���ֵ����
                size  �ֺ�
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color)	
{  
	u8 i,j;
	u8 *temp,size1;
	if(size==16){temp=Hzk16;}//ѡ���ֺ�
	if(size==32){temp=Hzk32;}
  LCD_Address_Set(x,y,x+size-1,y+size-1); //����һ�����ֵ�����
  size1=size*size/8;//һ��������ռ���ֽ�
	temp+=index*size1;//д�����ʼλ��
	for(j=0;j<size1;j++)
	{
		for(i=0;i<8;i++)
		{
		 	if((*temp&(1<<i))!=0)//�����ݵĵ�λ��ʼ��
			{
				LCD_WR_DATA(color);//����
			}
			else
			{
				LCD_WR_DATA(BACK_COLOR);//������
			}
		}
		temp++;
	 }
}


/******************************************************************************
      ����˵����LCD��ʾ����
      ������ݣ�x,y   ��ʼ����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//���ù��λ�� 
	LCD_WR_DATA(color);
} 
void LCD_DrawPoint_dma(u16 x,u16 y,u16 color)
{
	// LCD_Address_Set(x,y,x,y);//���ù��λ�� 
	dma_hal_spi_full(x,y,x,y,color);
} 

/******************************************************************************
      ����˵����LCD��һ����ĵ�
      ������ݣ�x,y   ��ʼ����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawPoint_big(u16 x,u16 y,u16 color)
{
	LCD_Fill(x-1,y-1,x+1,y+1,color);
} 


/******************************************************************************
      ����˵������ָ�����������ɫ
      ������ݣ�xsta,ysta   ��ʼ����
                xend,yend   ��ֹ����
      ����ֵ��  ��
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{          
	u16 i,j; 
	LCD_Address_Set(xsta,ysta,xend,yend);      //���ù��λ�� 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color);//���ù��λ�� 	    
	} 					  	    
}



/******************************************************************************
      ����˵��������
      ������ݣ�x1,y1   ��ʼ����
                x2,y2   ��ֹ����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1;
	uRow=x1;//�����������
	uCol=y1;
	if(delta_x>0)incx=1; //���õ������� 
	else if (delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//ˮƽ�� 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//����
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/******************************************************************************
      ����˵����������
      ������ݣ�x1,y1   ��ʼ����
                x2,y2   ��ֹ����
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}


/******************************************************************************
      ����˵������Բ
      ������ݣ�x0,y0   Բ������
                r       �뾶
      ����ֵ��  ��
******************************************************************************/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//�ж�Ҫ���ĵ��Ƿ��Զ
		{
			b--;
		}
	}
}


/******************************************************************************
      ����˵������ʾ�ַ�
      ������ݣ�x,y    �������
                num    Ҫ��ʾ���ַ�
                mode   1���ӷ�ʽ  0�ǵ��ӷ�ʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color)
{
    u8 temp;
    u8 pos,t;
	  u16 x0=x;     
    if(x>LCD_W-16||y>LCD_H-16)return;	    //���ô���		   
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	LCD_Address_Set(x,y,x+8-1,y+16-1);      //���ù��λ�� 
	if(!mode) //�ǵ��ӷ�ʽ
	{
		for(pos=0;pos<16;pos++)
		{ 
			temp=asc2_1608[(u16)num*16+pos];		 //����1608����
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_WR_DATA(color);
				    else LCD_WR_DATA(BACK_COLOR);
				    temp>>=1; 
				    x++;
		    }
			x=x0;
			y++;
		}	
	}else//���ӷ�ʽ
	{
		for(pos=0;pos<16;pos++)
		{
		    temp=asc2_1608[(u16)num*16+pos];		 //����1608����
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//��һ����     
		        temp>>=1; 
		    }
		}
	}   	   	 	  
}


/******************************************************************************
      ����˵������ʾ�ַ���
      ������ݣ�x,y    �������
                *p     �ַ�����ʼ��ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color)
{         
    while(*p!='\0')
    {       
        if(x>LCD_W-16){x=0;y+=16;}
        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,0,color);
        x+=8;
        p++;
    }  
}
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color)
{         	
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+8*t,y,' ',0,color);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color); 
	}
} 



void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color)
{         	
	u8 t,temp;
	u8 enshow=0;
	u16 num1;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+8*(len-2),y,'.',0,color);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+8*t,y,temp+48,0,color);
	}
}

void LCD_ShowPicture(u16 x1,u16 y1,u16 x2,u16 y2)
{
	int i;
	  LCD_Address_Set(x1,y1,x2,y2);
		for(i=0;i<1600;i++)
	  { 	
			LCD_WR_DATA8(image[i*2+1]);	 
			LCD_WR_DATA8(image[i*2]);			
	  }			
}




