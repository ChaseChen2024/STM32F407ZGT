#ifndef __LCD_H
#define __LCD_H			  	 
#include "stm32f4xx.h"
#include "stdlib.h"	   

#define USE_HORIZONTAL 0  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 240

#else
#define LCD_W 240
#define LCD_H 240
#endif

#define	u8 unsigned char
#define	u16 unsigned int
#define	u32 unsigned long

   						  
//-----------------����LED�˿ڶ���---------------- 



//-----------------OLED�˿ڶ���---------------- 

#define DATAOUT(x) GPIOA->ODR=x; //�������
#define DATAIN     GPIOA->IDR;   //��������


#define	LCD_RST_SET	GPIO_SetBits(GPIOB,GPIO_Pin_0)    //��λ			
#define	LCD_CS_SET  GPIO_SetBits(GPIOB,GPIO_Pin_12)   //Ƭѡ�˿�  
#define	LCD_DC_SET	GPIO_SetBits(GPIOB,GPIO_Pin_14)   //����/����  
#define	LCD_BLK_SET	GPIO_SetBits(GPIOB,GPIO_Pin_1)    //����


#define	LCD_RST_CLR	GPIO_ResetBits(GPIOB,GPIO_Pin_0)     //��λ
#define	LCD_CS_CLR  GPIO_ResetBits(GPIOB,GPIO_Pin_12)    //Ƭѡ�˿�
#define	LCD_DC_CLR	GPIO_ResetBits(GPIOB,GPIO_Pin_14)    //����/����
#define	LCD_BLK_CLR	GPIO_ResetBits(GPIOB,GPIO_Pin_1)     //����

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

extern  u16 BACK_COLOR;   //����ɫ

void LCD_Writ_Bus(u8 dat);
void LCD_WR_DATA8(u8 dat);
void LCD_WR_DATA(u16 dat);
void LCD_WR_REG(u8 dat);
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);
void Lcd_Init(void); 
void LCD_Clear(u16 Color);
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color);
void LCD_DrawPoint(u16 x,u16 y,u16 color);
void LCD_DrawPoint_big(u16 x,u16 y,u16 colory);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u16 color);
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 color);
u32 mypow(u8 m,u8 n);
void LCD_ShowNum(u16 x,u16 y,u16 num,u8 len,u16 color);
void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color);
void LCD_ShowPicture(u16 x1,u16 y1,u16 x2,u16 y2);
void SPI2_Init();
u8 SPI2_ReadWriteByte(u8 writeData);
void lcd_hardware_init(void);
void fillScreen(u16 color);
void dma_hal_spi_clear(uint16_t color);
void dma_hal_spi_full(u16 xsta,u16 ysta,u16 xend,u16 yend,uint16_t color);
void LCD_Fill2(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 *color,u32 len);
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);
void LCD_DrawPoint_dma(u16 x,u16 y,u16 color);
void LCD_Fill3(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t *color);
// void LCD_Fill2(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 *color);
// #define SCREEN_SIZE      240*100
// #define DMA_SPI_BUF_SIZE (SCREEN_SIZE)

// extern uint16_t Spi2DmaBuffer[DMA_SPI_BUF_SIZE];	//�������ݻ�����
// extern volatile uint8_t sendFlg;
// void DMA_Config(uint32_t TX_Buff,uint32_t SENDBUFF_SIZE);
// void DMA_Write_buf(uint32_t SizeLen);
// void dma_hal_spi_clear(u16 x1,u16 y1,u16 x2,u16 y2,uint16_t color);
// void Lcd_Full(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
// void lcd_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //��ɫ
#define BRRED 			     0XFC07 //�غ�ɫ
#define GRAY  			     0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY 			     0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)


					  		 
#endif  
	 
	 



