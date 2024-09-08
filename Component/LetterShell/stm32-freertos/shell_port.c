/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */
#include "user.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "shell.h"
#include "bsp_usart3.h"
#include "stm32f4xx.h"
#ifdef USE_FATFS_CODE
#include "ff.h"
#endif
Shell shell;
char shellBuffer[512];


static SemaphoreHandle_t shellMutex;

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{
    Usart_Transmit(USART3,data,len);
    return len;
}


/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际读取到
 */
/*
使用串口中断接收，故不使用该查询接收函数
*/
#if 0
uint8_t uart3_shell_data = 0;
short userShellRead(char *data, unsigned short len)
{
    if(READ_IT_FLAG == 1)
    {
        READ_IT_FLAG = 0;
        data[0] = uart3_shell_data;
        return 1;
    }
    return 0;
}
#endif
/**
 * @brief 用户shell上锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
    return 0;
}

/**
 * @brief 用户shell解锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
    xSemaphoreGiveRecursive(shellMutex);
    return 0;
}


/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    shellMutex = xSemaphoreCreateMutex();

    shell.write = userShellWrite;
    #if 0
    // shell.read = userShellRead;
    #endif
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);
    Uart3_BinarySem_Handle = xSemaphoreCreateBinary();	
    BaseType_t pass = pdPASS;
    elog_i(ELOG_APP,"userShellInit");
    elog_i(ELOG_APP,"2-rtos free size: %d B",xPortGetFreeHeapSize());
    if ((pass = xTaskCreate(shellTask, "shell", 128*20, &shell, 5, NULL) )!= pdPASS)
    {
       elog_i(ELOG_APP,"userShellInit-fail:%d",pass);
    }
}
CEVENT_EXPORT(EVENT_INIT_STAGE2, userShellInit);

