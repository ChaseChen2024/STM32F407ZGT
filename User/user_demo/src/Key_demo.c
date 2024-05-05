#if 1
#include "lcd.h"
#include "Key_demo.h"
#include "bsp_key.h"
#include "bsp_debug_usart.h"
#include "module.h"
#include "atproc.h"
#include <string.h>
 #include "log.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FatFs_demo.h"
static TaskHandle_t KEY_Demo_Task_Handle = NULL;/* KEY������ */
extern QueueHandle_t Module_Queue;

/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void KEY_Demo_Task(void* parameter)
{	
	// int a=0,b=0,c=0;
	// uint8_t ret = 0;
	// uint8_t Rxdata[256] = {0};
	//  vTaskDelay(10000);/* ��ʱ20��tick */
	// ret = send_at((uint8_t*)"ATE0\r\n",NULL,0,10000);
	// 	printf("ret=%d\r\n",ret);
	 Lcd_Init();	
   printf("LCD init\n\n");//��ʼ��OLED  
	 LCD_Clear(WHITE);
   printf("[%s][%d]\r\n",__FILE__,__LINE__);
	 BACK_COLOR=WHITE;
   	printf("[%s][%d]\r\n",__FILE__,__LINE__);

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
  while (1)
  {

    if( Key_Scan(KEY0_GPIO_PORT,KEY0_PIN) == KEY_ON )
    {/* K0 ������ */
      printf("����0��\n");
    }
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )
    {/* K1 ������ */
		printf("����1��\n");

    } 

	 if( Key_Scan(GPIOA,GPIO_Pin_3) == KEY_ON )
    {/* K0 ������ */
      printf("MID\n");
	//    LCD_Clear(WHITE);
	//     BACK_COLOR=WHITE;
		// fillScreen(WHITE);
		dma_hal_spi_clear(WHITE);
		LCD_DrawLine(0,10,239,10,BLUE);
    }
		if(  Key_Scan(GPIOA,GPIO_Pin_4) == KEY_ON )
    {/* K1 ������ */
		printf("R\n");
 	// LCD_Clear(BLUE);
	//  BACK_COLOR=BLUE;
	 dma_hal_spi_clear(BLUE);
    } 
	if(  Key_Scan(GPIOA,GPIO_Pin_5) == KEY_ON )
    {/* K1 ������ */
		printf("L\n");
		//  LCD_Clear(BRED);
		//  BACK_COLOR=BRED;
		//  dma_hal_spi_clear(BRED);
		 dma_hal_spi_full(15,15,100,100,RED);
    } 
	if(  Key_Scan(GPIOA,GPIO_Pin_6) == KEY_ON )
    {/* K1 ������ */
		printf("D��\n");
		//  LCD_Clear(GRED);
		//  BACK_COLOR=GRED;
		 dma_hal_spi_clear(GRED);
    } 
	if(  Key_Scan(GPIOA,GPIO_Pin_8) == KEY_ON )
    {/* K1 ������ */
		printf("U\n");
		//  LCD_Clear(YELLOW);
		//  BACK_COLOR=YELLOW;
		 dma_hal_spi_clear(YELLOW);
    } 
    vTaskDelay(10);/* ��ʱ20��tick */
  }
}


long Key_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;
	
			/* ����KEY_Task���� */
		xReturn = xTaskCreate((TaskFunction_t )KEY_Demo_Task,  /* ������ں��� */
													(const char*    )"KEY_Demo_Task",/* �������� */
													(uint16_t       )2048,  /* ����ջ��С */
													(void*          )NULL,/* ������ں������� */
													(UBaseType_t    )3, /* ��������ȼ� */
													(TaskHandle_t*  )&KEY_Demo_Task_Handle);/* ������ƿ�ָ�� */ 
		return xReturn;
}
void Key_Bsp_Init(void)
{
	/* ������ʼ��	*/
  Key_GPIO_Config();
  Five_Key_GPIO_Init();
}
#endif