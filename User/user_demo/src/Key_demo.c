#if 1
#include "lcd.h"
#include "Key_demo.h"
#include "bsp_key.h"
#include "bsp_debug_usart.h"
#include "module.h"
#include "atproc.h"
#include <string.h>
 #include "log.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FatFs_demo.h"
static TaskHandle_t KEY_Demo_Task_Handle = NULL;/* KEY任务句柄 */
extern QueueHandle_t Module_Queue;

/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void KEY_Demo_Task(void* parameter)
{	
	// int a=0,b=0,c=0;
	// uint8_t ret = 0;
	// uint8_t Rxdata[256] = {0};
	//  vTaskDelay(10000);/* 延时20个tick */
	// ret = send_at((uint8_t*)"ATE0\r\n",NULL,0,10000);
	// 	printf("ret=%d\r\n",ret);
	 Lcd_Init();	
   printf("LCD init\n\n");//初始化OLED  
	 LCD_Clear(WHITE);
   printf("[%s][%d]\r\n",__FILE__,__LINE__);
	 BACK_COLOR=WHITE;
   	printf("[%s][%d]\r\n",__FILE__,__LINE__);

		 LCD_ShowChinese(10,0,0,32,RED);   //中
			LCD_ShowChinese(45,0,1,32,RED);   //景
			LCD_ShowChinese(80,0,2,32,RED);   //园
			LCD_ShowChinese(115,0,3,32,RED);  //电
			LCD_ShowChinese(150,0,4,32,RED);  //子
		 
		 	LCD_ShowChinese(10,75,0,16,RED);   //中
			LCD_ShowChinese(45,75,1,16,RED);   //景
			LCD_ShowChinese(80,75,2,16,RED);   //园
			LCD_ShowChinese(115,75,3,16,RED);  //电
			LCD_ShowChinese(150,75,4,16,RED);  //子
  while (1)
  {

    if( Key_Scan(KEY0_GPIO_PORT,KEY0_PIN) == KEY_ON )
    {/* K0 被按下 */
      printf("按键0！\n");
    }
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )
    {/* K1 被按下 */
		printf("按键1！\n");

    } 

	 if( Key_Scan(GPIOA,GPIO_Pin_3) == KEY_ON )
    {/* K0 被按下 */
      printf("MID\n");
	//    LCD_Clear(WHITE);
	//     BACK_COLOR=WHITE;
		// fillScreen(WHITE);
		dma_hal_spi_clear(WHITE);
		LCD_DrawLine(0,10,239,10,BLUE);
    }
		if(  Key_Scan(GPIOA,GPIO_Pin_4) == KEY_ON )
    {/* K1 被按下 */
		printf("R\n");
 	// LCD_Clear(BLUE);
	//  BACK_COLOR=BLUE;
	 dma_hal_spi_clear(BLUE);
    } 
	if(  Key_Scan(GPIOA,GPIO_Pin_5) == KEY_ON )
    {/* K1 被按下 */
		printf("L\n");
		//  LCD_Clear(BRED);
		//  BACK_COLOR=BRED;
		//  dma_hal_spi_clear(BRED);
		 dma_hal_spi_full(15,15,100,100,RED);
    } 
	if(  Key_Scan(GPIOA,GPIO_Pin_6) == KEY_ON )
    {/* K1 被按下 */
		printf("D！\n");
		//  LCD_Clear(GRED);
		//  BACK_COLOR=GRED;
		 dma_hal_spi_clear(GRED);
    } 
	if(  Key_Scan(GPIOA,GPIO_Pin_8) == KEY_ON )
    {/* K1 被按下 */
		printf("U\n");
		//  LCD_Clear(YELLOW);
		//  BACK_COLOR=YELLOW;
		 dma_hal_spi_clear(YELLOW);
    } 
    vTaskDelay(10);/* 延时20个tick */
  }
}


long Key_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;
	
			/* 创建KEY_Task任务 */
		xReturn = xTaskCreate((TaskFunction_t )KEY_Demo_Task,  /* 任务入口函数 */
													(const char*    )"KEY_Demo_Task",/* 任务名字 */
													(uint16_t       )2048,  /* 任务栈大小 */
													(void*          )NULL,/* 任务入口函数参数 */
													(UBaseType_t    )3, /* 任务的优先级 */
													(TaskHandle_t*  )&KEY_Demo_Task_Handle);/* 任务控制块指针 */ 
		return xReturn;
}
void Key_Bsp_Init(void)
{
	/* 按键初始化	*/
  Key_GPIO_Config();
  Five_Key_GPIO_Init();
}
#endif