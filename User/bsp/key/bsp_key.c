
#include "bsp_key.h" 

void Key_Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 


void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(KEY1_GPIO_CLK|KEY0_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);   
	GPIO_InitStructure.GPIO_Pin = KEY0_PIN; 
	GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure);  
}





uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			

	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}

//五维按键初始化
//PA3 
//PA4、
//PA5、
//PA6、
//PA8
void Five_Key_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_8; 
  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);   
}



/*********************************************END OF FILE**********************/
