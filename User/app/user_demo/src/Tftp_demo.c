#if 0
#if 0

#include <string.h>

#include <stdio.h>

#include "lwip/apps/tftp_server.h"

/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"
#include "Tftp_demo.h"
static TaskHandle_t TFTP_Demo_Task_Handle = NULL;/* KEY任务句柄 */
FATFS fs1;	
FIL fnew1;   
static void file_init(void)
{
    FRESULT res_sd1;   
  //在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	res_sd1 = f_mount(&fs1,"1:",1);
	
/*----------------------- 格式化测试 ---------------------------*/  
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(res_sd1 == FR_NO_FILESYSTEM)
	{
		printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_sd1=f_mkfs("1:",0,0);							
		
		if(res_sd1 == FR_OK)
		{
			printf("》SD卡已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_sd1 = f_mount(NULL,"1:",1);			
      /* 重新挂载	*/			
			res_sd1 = f_mount(&fs1,"1:",1);
		}
		else
		{
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  else if(res_sd1!=FR_OK)
  {
    printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd1);
    printf("！！可能原因：SD卡初始化不成功。\r\n");
		while(1);
  }
  else
  {
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }
}

 
typedef struct
{
	uint8_t isOpenOK;
	uint8_t type;//0:GCode 1:缓存GCode 2:固件
	uint8_t write;
	char name[64];
}TFTP_Handler;
TFTP_Handler tftp_Handler;


#if LWIP_UDP

static void*
tftp_open(const char* fname, const char* mode, u8_t is_write)
{
  uint8_t res;
	char name[64];
	// tftp_Handler.write=is_write;
	
	// tftp_Handler.type=0;
	strcpy(name,"1:");
	strcat(name,fname);
	
	if(is_write)
	{
		res=f_open(&fnew1,name,FA_CREATE_ALWAYS|FA_WRITE);
	}
	else
	{
		res=f_open(&fnew1,name,FA_OPEN_EXISTING|FA_READ);
	}
	if(res == FR_OK)
	{
		return (void*)&fnew1;
	}
	else
	{
		return  (void*)NULL;
	}
	
}

static void 
tftp_close(void* handle)
{
//  uint8_t res=0;
	 
	f_close(handle);
	printf("Transfer file completed!\r\n");
}

static int
tftp_read(void* handle, void* buf, int bytes)
{
  uint32_t count;
	f_read(&fnew1,(uint8_t*)buf,bytes,&count);
 
	printf("Read File: Len:%d  count:%d\r\n",bytes,count);
	return count;

}

static int
tftp_write(void* handle, struct pbuf* p)
{
 uint32_t count;
	f_write(handle,p->payload,p->len,&count);
 
	printf("Write File: Len:%d  bw:%d\r\n",p->len,count);
	return count;
}

static const struct tftp_context tftp = {
  tftp_open,
  tftp_close,
  tftp_read,
  tftp_write
};



#endif /* LWIP_UDP */

/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void TFTP_Demo_Task(void* parameter)
{	
	// int a=0,b=0,c=0;
	file_init();
	vTaskDelay(2000);/* 延时20个tick */
	tftp_init(&tftp);
  while (1)
  {
    vTaskDelay(20);/* 延时20个tick */
  }
}


long Tftp_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;
	
			/* 创建KEY_Task任务 */
		xReturn = xTaskCreate((TaskFunction_t )TFTP_Demo_Task,  /* 任务入口函数 */
													(const char*    )"TFTP_Demo_Task",/* 任务名字 */
													(uint16_t       )1024,  /* 任务栈大小 */
													(void*          )NULL,/* 任务入口函数参数 */
													(UBaseType_t    )5, /* 任务的优先级 */
													(TaskHandle_t*  )&TFTP_Demo_Task_Handle);/* 任务控制块指针 */ 
		return xReturn;
}
void tftp_Bsp_Init(void)
{
	/* 按键初始化	*/
 file_init();
 tftp_init(&tftp);
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "ff.h"
#include "lwip/opt.h"
#include <lwip/sockets.h>
#include "lwip/api.h"
#include "lwip/sys.h"
#include "ethernetif.h"
#include "lwip/apps/tftp_server.h"
struct tftp_context mytftp;
 
typedef struct
{
    uint8_t     isOpenOK;
    uint8_t     type;   //0:GCode 1:缓存GCode 2:固件
    uint8_t     write;
    char        name[64];
}TFTP_Handler;
 
TFTP_Handler    tftp_Handler;
FIL             file;
FATFS fs1;	

static void file_init(void)
{
    FRESULT res_sd1;   
  //在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	res_sd1 = f_mount(&fs1,"0:",1);
	
/*----------------------- 格式化测试 ---------------------------*/  
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(res_sd1 == FR_NO_FILESYSTEM)
	{
		printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_sd1=f_mkfs("0:",0,0);							
		
		if(res_sd1 == FR_OK)
		{
			printf("》SD卡已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_sd1 = f_mount(NULL,"0:",1);			
      /* 重新挂载	*/			
			res_sd1 = f_mount(&fs1,"0:",1);
		}
		else
		{
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  else if(res_sd1!=FR_OK)
  {
    printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd1);
    printf("！！可能原因：SD卡初始化不成功。\r\n");
		while(1);
  }
  else
  {
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }
}
void* TFTP_Open(const char* fname, const char* mode, u8_t write)
{
    uint8_t     res;
    char        name[64];
	
    tftp_Handler.write  = write;
    tftp_Handler.type   = 0;
 
    strcpy(name, "0:");
    strcat(name, fname);
  printf("TFTP_Open:%s,write:%d!\r\n",name,write);
    if (write)
    {
        res= f_open(&file, name, FA_CREATE_ALWAYS|FA_WRITE);
    }
    else
    {
        res = f_open(&file, name, FA_OPEN_EXISTING|FA_READ);
    }
 
    tftp_Handler.isOpenOK = res;
	
    return &tftp_Handler;
}
 
void TFTP_Close(void* handle)
{
    uint8_t res = 0;
 
    if (((TFTP_Handler*)handle)->isOpenOK)
    {
        return;
    }
 
    f_close(&file);
    printf("Transfer file completed!\r\n");
}
 
int TFTP_Read(void* handle, void* buf, int bytes)
{
    uint8_t     res;
    UINT        br  = 0;
 
    res = ((TFTP_Handler*)handle)->isOpenOK;
    if (res)
    {
        return -1;
    }
 
    res = f_read(&file, (uint8_t*)buf, bytes, &br);
    printf("Read File:%d  Len:%d  br:%d\r\n", res, bytes, br);
 
    return br;
}
 
int TFTP_Write(void* handle, struct pbuf* p)
{
    uint8_t     res;
    UINT        bw  = 0;
 
    res = ((TFTP_Handler*)handle)->isOpenOK;
    if(res)
    {
        return -1;
    }
 
    res = f_write(&file, p->payload, p->len, &bw);
    printf("Write File:%d  Len:%d  bw:%d\r\n", res, p->len, bw);
 
    return 0;
}
 
void TFTP_ContextInit(void)
{
    mytftp.open     = TFTP_Open;
    mytftp.close    = TFTP_Close;
    mytftp.read     = TFTP_Read;
    mytftp.write    = TFTP_Write;
}
 
void tftpInit(void)
{
		file_init();
    TFTP_ContextInit();
    tftp_init(&mytftp);
 
    return;
}
#endif