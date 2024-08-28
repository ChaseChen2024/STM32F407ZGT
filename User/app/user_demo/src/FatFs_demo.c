#if 1
#include "FatFs_demo.h"

#include "ff.h"
#include <nv.h>
#include <string.h>

/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
static TaskHandle_t FatFs_Demo_Task_Handle = NULL;/* FatFs_Demo������ */

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
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL fnew;													/* �ļ����� */
FRESULT res_sd;                /* �ļ�������� */
UINT fnum;            					  /* �ļ��ɹ���д���� */
BYTE ReadBuffer[1024]={0};        /* �������� */
 
"��ӭʹ��Ұ��STM32 F407������ �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";  
#endif

/**********************************************************************
  * @ ������  �� FatFs_Demo_Task
  * @ ����˵���� FatFs_Demo_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
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
		printf("\r\n����һ���ļ�ϵͳ��ֲʵ�� \r\n");

		res = f_mount(&fs,"1:",1);

		printf("\r\n,%d,fmount res=%d",__LINE__,res);

		if(res == FR_NO_FILESYSTEM)
		{
				//��ʽ��
				res = f_mkfs("1:",0,0);
				printf("\r\n,%d,f_mkfs res=%d",__LINE__,res);

				//��ʽ������Ҫ���¹����ļ�ϵͳ
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
		printf("\r\n��ȡ�����ļ�����:\r\n");
		printf("%s", read_buff);

		res = f_close(&fil);
#endif

#else

	//���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	res_sd = f_mount(&fs,"0:",1);
	
/*----------------------- ��ʽ������ ---------------------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_sd == FR_NO_FILESYSTEM)
	{
		printf("��SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_sd=f_mkfs("0:",0,0);							
		
		if(res_sd == FR_OK)
		{
			printf("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_sd = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			res_sd = f_mount(&fs,"0:",1);
		}
		else
		{
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(res_sd!=FR_OK)
  {
    printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res_sd);
    printf("��������ԭ��SD����ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  }
  
/*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
	/* ���ļ�������ļ��������򴴽��� */
	printf("\r\n****** ���������ļ�д�����... ******\r\n");	
	res_sd = f_open(&fnew, "0:FatFs��д�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_sd == FR_OK )
	{
		printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\r\n");
    /* ��ָ���洢������д�뵽�ļ��� */
		res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_sd==FR_OK)
    {
      printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
      printf("�����ļ�д�������Ϊ��\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_sd);
    }    
		/* ���ٶ�д���ر��ļ� */
    f_close(&fnew);
	}
	else
	{	
		printf("������/�����ļ�ʧ�ܡ�\r\n");
	}
	
/*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
	printf("****** ���������ļ���ȡ����... ******\r\n");
	res_sd = f_open(&fnew, "0:FatFs��д�����ļ�.txt", FA_OPEN_EXISTING | FA_READ); 	 
	if(res_sd == FR_OK)
	{
		printf("�����ļ��ɹ���\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_sd==FR_OK)
    {
      printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("�����ļ���ȡʧ�ܣ�(%d)\n",res_sd);
    }		
	}
	else
	{
		printf("�������ļ�ʧ�ܡ�\r\n");
	}
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);	
  
	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
	f_mount(NULL,"0:",1);		
#endif		
		// while (1)
		// {
		// 	vTaskDelay(500);   /* ��ʱ500��tick */
		// }
		vTaskDelete(NULL);
}

long FatFs_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;

		 
			/* ����FatFs_Test_Task���� */
			xReturn = xTaskCreate((TaskFunction_t )FatFs_Demo_Task, /* ������ں��� */
													(const char*    )"FatFs_Demo_Task",/* �������� */
													(uint16_t       )1024,   /* ����ջ��С */
													(void*          )NULL,	/* ������ں������� */
													(UBaseType_t    )4,	    /* ��������ȼ� */
													(TaskHandle_t*  )&FatFs_Demo_Task_Handle);/* ������ƿ�ָ�� */
		return xReturn;
}

//��һ�����ݼ����ļ�ĩβ��Ĭ���ļ�ϵͳ�Ѿ�����
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
