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
// #include "shell_fs.h"
#include "ff.h"
Shell shell;
char shellBuffer[512];
// ShellFs shellFs;
// char shellPathBuffer[512] = "/";

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

// uint8_t uart3_shell_data = 0;
// short userShellRead(char *data, unsigned short len)
// {
//     if(READ_IT_FLAG == 1)
//     {
//         READ_IT_FLAG = 0;
//         data[0] = uart3_shell_data;
//         return 1;
//     }
//     return 0;
// }

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
size_t userShellListDir(char *path, char *buffer, size_t maxLen)
{
    DIR *dir;
    FRESULT ret = FR_OK;
    // struct dirent *ptr;
    static FILINFO fno;
    int i;
     elog_i(ELOG_APP,"userShellListDir_1");
    ret = f_opendir(dir,path);
    elog_i(ELOG_APP,"userShellListDir_2");
    memset(buffer, 0, maxLen);
    // while(f_readdir(dir, &fno) == FR_OK)
    // {
    //     strcat(buffer, fno.fname);
    //     strcat(buffer, "\r\n");
    //     printf("%s", buffer);
    // }
     for (;;) {
        elog_i(ELOG_APP,"userShellListDir_3\r\n");
        ret = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (ret != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        strcat(buffer, fno.fname);
        strcat(buffer, "\r\n");
        elog_i(ELOG_APP,"%s", buffer);
            
        }
        elog_i(ELOG_APP,"userShellListDir_4");
    f_closedir(dir);
    return ret;
}
// #define START_STK_SIZE 128*10
// StaticTask_t __EXRAM StartTaskTCB; 
// StackType_t __EXRAM StartTaskStack[START_STK_SIZE]; 
/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    shellMutex = xSemaphoreCreateMutex();

    shell.write = userShellWrite;
    // shell.read = userShellRead;
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);
    Uart3_BinarySem_Handle = xSemaphoreCreateBinary();	
    BaseType_t pass = pdPASS;
    elog_i(ELOG_APP,"userShellInit");
    elog_i(ELOG_APP,"2-rtos free size: %d B",xPortGetFreeHeapSize());
    if ((pass = xTaskCreate(shellTask, "shell", 128*10, &shell, 5, NULL) )!= pdPASS)
    {
       elog_i(ELOG_APP,"userShellInit-fail:%d",pass);
    }
    // xTaskCreateStatic(shellTask,"shell",START_STK_SIZE,NULL,5,StartTaskStack,&StartTaskTCB);
}
CEVENT_EXPORT(EVENT_INIT_STAGE2, userShellInit);

// void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
//                                     StackType_t ** ppxIdleTaskStackBuffer,
//                                     uint32_t * pulIdleTaskStackSize )
// {
//     *ppxIdleTaskTCBBuffer = &StartTaskTCB;
//     *ppxIdleTaskStackBuffer = StartTaskStack;
//     *pulIdleTaskStackSize = START_STK_SIZE;
// }