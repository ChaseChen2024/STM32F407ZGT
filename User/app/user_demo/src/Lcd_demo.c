#if 1
#include "lcd.h"
#include "Lcd_demo.h"
#include "bsp_debug_usart.h"
#include <string.h>
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


static TaskHandle_t Lcd_Demo_Task_Handle = NULL;



static void Lcd_Demo_Task(void* parameter)
{
   u8 i,m;
	 float t=0;
			
  Lcd_Init();	
   printf("LCD init\n\n");//��ʼ��OLED  
   SPI2_ReadWriteByte(0x12);
    SPI2_ReadWriteByte(0x34);
     SPI2_ReadWriteByte(0x56);
      SPI2_ReadWriteByte(0x78);
	 LCD_Clear(WHITE);
   printf("[%s][%d]\r\n",__FILE__,__LINE__);
	 BACK_COLOR=WHITE;
   	printf("[%s][%d]\r\n",__FILE__,__LINE__);
     while(1)
	 {
		 LCD_ShowChinese(10,0,0,32,RED);   //��
			LCD_ShowChinese(45,0,1,32,RED);   //��
			LCD_ShowChinese(80,0,2,32,RED);   //԰
			LCD_ShowChinese(115,0,3,32,RED);  //��
			LCD_ShowChinese(150,0,4,32,RED);  //��
		 
		 	LCD_ShowChinese(10,75,0,16,RED);   //��
			LCD_ShowChinese(45,75,1,16,RED);   //��
			LCD_ShowChinese(80,75,2,16,RED);   //԰
			LCD_ShowChinese(115,75,3,16,RED);  //��
			LCD_ShowChinese(150,75,4,16,RED);  //��
			LCD_ShowString(10,35,"2.4 TFT SPI 240*320",RED);
			LCD_ShowString(10,55,"LCD_W:",RED);	LCD_ShowNum(70,55,LCD_W,3,RED);
			LCD_ShowString(110,55,"LCD_H:",RED);LCD_ShowNum(160,55,LCD_H,3,RED);
		  for(i=0;i<5;i++)
		  {
				for(m=0;m<6;m++)
				{
				  LCD_ShowPicture(0+m*40,120+i*40,39+m*40,159+i*40);
				}
		  }
			while(1)
			{
				LCD_ShowNum1(80,95,t,5,RED);
		    t+=0.01;
      }
   }
    
}

long Lcd_Demo_Task_Init(void)
{
	BaseType_t xReturn = pdPASS;

	xReturn = xTaskCreate((TaskFunction_t )Lcd_Demo_Task,  /* ������ں��� */
											(const char*    )"Lcd_Demo_Task",/* �������� */
											(uint16_t       )2048,  /* ����ջ��С */
											(void*          )NULL,/* ������ں������� */
											(UBaseType_t    )3, /* ��������ȼ� */
											(TaskHandle_t*  )&Lcd_Demo_Task_Handle);/* ������ƿ�ָ�� */ 
	return xReturn;
}

#endif