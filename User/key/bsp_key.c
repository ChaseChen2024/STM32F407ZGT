
#include "bsp_key.h" 

/// 不精确的延时
void Key_Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 

/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(KEY1_GPIO_CLK|KEY0_GPIO_CLK,ENABLE);
	
  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN; 
  
  /*设置引脚为输入模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  
  /*设置引脚上拉*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
  /*使用上面的结构体初始化按键*/
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);   
  
  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = KEY0_PIN; 
  
  /*使用上面的结构体初始化按键*/
	GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure);  
}

/**
  * @brief   检测是否有按键按下     
  *	@param 	GPIOx:具体的端口, x可以是（A...K） 
	*	@param 	GPIO_PIN:具体的端口位， 可以是GPIO_PIN_x（x可以是0...15）
  * @retval  按键的状态
  *		@arg KEY_ON:按键按下
  *		@arg KEY_OFF:按键没按下
  */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*检测是否有按键按下 */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*等待按键释放 */
		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}


/*********************************************END OF FILE**********************/
