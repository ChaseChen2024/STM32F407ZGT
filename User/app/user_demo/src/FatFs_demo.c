#if 1
#include "FatFs_demo.h"

#include "ff.h"
#include <nv.h>
#include <string.h>

/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
static TaskHandle_t FatFs_Demo_Task_Handle = NULL;/* FatFs_Demo任务句柄 */

// #define STR_WRITE  "a,b,c"
// BYTE WriteBuffer[] = "33333333333333\r\n"; 

#define SPI_FLASH_FATFS 1
#ifdef SPI_FLASH_FATFS
// FATFS fs;
// FRESULT res;
// FIL fil;
// UINT bw;
// UINT br;
// char read_buff[50];
#else
FATFS fs;													/* FatFs文件系统对象 */
FIL fnew;													/* 文件对象 */
FRESULT res_sd;                /* 文件操作结果 */
UINT fnum;            					  /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};        /* 读缓冲区 */
 
"欢迎使用野火STM32 F407开发板 今天是个好日子，新建文件系统测试文件\r\n";  
#endif

/**********************************************************************
  * @ 函数名  ： FatFs_Demo_Task
  * @ 功能说明： FatFs_Demo_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void FatFs_Demo_Task(void* parameter)
{	
#ifdef SPI_FLASH_FATFS
		char read_buf[4] = {0};
		  nv_init();
  FRESULT err = FR_OK;
  nv_write("1:test.txt","1024",4);
  err = nv_read("1:test.txt",read_buf,4);
  printf("\r\n1:20240828.txt,read:%d,%d,%d,%d,err:%d\r\n",read_buf[0],read_buf[1],read_buf[2],read_buf[3],err);
#if 0
		printf("\r\n这是一个文件系统移植实验 \r\n");

		res = f_mount(&fs,"1:",1);

		printf("\r\n,%d,fmount res=%d",__LINE__,res);

		if(res == FR_NO_FILESYSTEM)
		{
				//格式化
				res = f_mkfs("1:",0,0);
				printf("\r\n,%d,f_mkfs res=%d",__LINE__,res);

				//格式化后需要重新挂载文件系统
				res = f_mount(NULL,"1:",1);

				res = f_mount(&fs,"1:",1);		
		}

		res = f_open(&fil, "1:czs.csv", FA_CREATE_ALWAYS|FA_WRITE);
		printf("\r\n,%d,f_open res=%d",__LINE__,res);

		res = f_write(&fil, WriteBuffer, strlen(WriteBuffer) ,&bw);
		printf("\r\n,%d,f_write res=%d len=%d bw=%d",__LINE__,res,strlen(WriteBuffer),bw);

		res = f_close(&fil);
		printf("\r\n,%d,f_close res=%d",__LINE__,res);


		res = f_open(&fil, "1:czs.csv", FA_OPEN_EXISTING|FA_READ);

		res = f_read(&fil, read_buff, bw, &br);
		printf("\r\nf_read res=%d br=%d",res,br);
		read_buff[br] = '\0';
		printf("\r\n读取到的文件内容:\r\n");
		printf("%s", read_buff);

		res = f_close(&fil);
#endif

#else

	//在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	res_sd = f_mount(&fs,"0:",1);
	
/*----------------------- 格式化测试 ---------------------------*/  
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_sd=f_mkfs("0:",0,0);							
		
		if(res_sd == FR_OK)
		{
			printf("》SD卡已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_sd = f_mount(NULL,"0:",1);			
      /* 重新挂载	*/			
			res_sd = f_mount(&fs,"0:",1);
		}
		else
		{
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  else if(res_sd!=FR_OK)
  {
    printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
    printf("！！可能原因：SD卡初始化不成功。\r\n");
		while(1);
  }
  else
  {
    printf("》文件系统挂载成功，可以进行读写测试\r\n");
  }
  
/*----------------------- 文件系统测试：写测试 -----------------------------*/
	/* 打开文件，如果文件不存在则创建它 */
	printf("\r\n****** 即将进行文件写入测试... ******\r\n");	
	res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_sd == FR_OK )
	{
		printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
    /* 将指定存储区内容写入到文件内 */
		res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_sd==FR_OK)
    {
      printf("》文件写入成功，写入字节数据：%d\n",fnum);
      printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("！！文件写入失败：(%d)\n",res_sd);
    }    
		/* 不再读写，关闭文件 */
    f_close(&fnew);
	}
	else
	{	
		printf("！！打开/创建文件失败。\r\n");
	}
	
/*------------------- 文件系统测试：读测试 ------------------------------------*/
	printf("****** 即将进行文件读取测试... ******\r\n");
	res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ); 	 
	if(res_sd == FR_OK)
	{
		printf("》打开文件成功。\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_sd==FR_OK)
    {
      printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
      printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("！！文件读取失败：(%d)\n",res_sd);
    }		
	}
	else
	{
		printf("！！打开文件失败。\r\n");
	}
	/* 不再读写，关闭文件 */
	f_close(&fnew);	
  
	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(NULL,"0:",1);		
#endif		
		// while (1)
		// {
		// 	vTaskDelay(500);   /* 延时500个tick */
		// }
		vTaskDelete(NULL);
}

long FatFs_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;

		 
			/* 创建FatFs_Test_Task任务 */
			xReturn = xTaskCreate((TaskFunction_t )FatFs_Demo_Task, /* 任务入口函数 */
													(const char*    )"FatFs_Demo_Task",/* 任务名字 */
													(uint16_t       )1024,   /* 任务栈大小 */
													(void*          )NULL,	/* 任务入口函数参数 */
													(UBaseType_t    )4,	    /* 任务的优先级 */
													(TaskHandle_t*  )&FatFs_Demo_Task_Handle);/* 任务控制块指针 */
		return xReturn;
}

//将一串数据加入文件末尾，默认文件系统已经挂载
#ifdef SPI_FLASH_FATFS

// int add_data_file(int a,int b,int c)
// {

// 	char writ_buf[64]={0};
// 	sprintf(writ_buf,"%d,%d,%d\r\n",a,b,c);
// 	res = f_open(&fil, "1:czs.csv", FA_OPEN_EXISTING|FA_WRITE);
// 	printf("\r\n,%d,f_open res=%d",__LINE__,res);
	
// 	res=f_lseek(&fil,f_size(&fil));
// 	printf("\r\n,%d,f_lseek res=%d",__LINE__,res);
	
	
// 	res = f_write(&fil, writ_buf, strlen(writ_buf) ,&bw);
// 	printf("\r\n,%d,f_write res=%d len=%d bw=%d",__LINE__,res,strlen(writ_buf),bw);
	
// 	res = f_close(&fil);

// 	return 0;
// }

#endif


#endif
