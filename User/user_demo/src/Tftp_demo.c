#if 0
#if 0

#include <string.h>

#include <stdio.h>

#include "lwip/apps/tftp_server.h"

/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"
#include "Tftp_demo.h"
static TaskHandle_t TFTP_Demo_Task_Handle = NULL;/* KEY������ */
FATFS fs1;	
FIL fnew1;   
static void file_init(void)
{
    FRESULT res_sd1;   
  //���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	res_sd1 = f_mount(&fs1,"1:",1);
	
/*----------------------- ��ʽ������ ---------------------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_sd1 == FR_NO_FILESYSTEM)
	{
		printf("��SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_sd1=f_mkfs("1:",0,0);							
		
		if(res_sd1 == FR_OK)
		{
			printf("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_sd1 = f_mount(NULL,"1:",1);			
      /* ���¹���	*/			
			res_sd1 = f_mount(&fs1,"1:",1);
		}
		else
		{
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(res_sd1!=FR_OK)
  {
    printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res_sd1);
    printf("��������ԭ��SD����ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
  }
}

 
typedef struct
{
	uint8_t isOpenOK;
	uint8_t type;//0:GCode 1:����GCode 2:�̼�
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
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void TFTP_Demo_Task(void* parameter)
{	
	// int a=0,b=0,c=0;
	file_init();
	vTaskDelay(2000);/* ��ʱ20��tick */
	tftp_init(&tftp);
  while (1)
  {
    vTaskDelay(20);/* ��ʱ20��tick */
  }
}


long Tftp_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;
	
			/* ����KEY_Task���� */
		xReturn = xTaskCreate((TaskFunction_t )TFTP_Demo_Task,  /* ������ں��� */
													(const char*    )"TFTP_Demo_Task",/* �������� */
													(uint16_t       )1024,  /* ����ջ��С */
													(void*          )NULL,/* ������ں������� */
													(UBaseType_t    )5, /* ��������ȼ� */
													(TaskHandle_t*  )&TFTP_Demo_Task_Handle);/* ������ƿ�ָ�� */ 
		return xReturn;
}
void tftp_Bsp_Init(void)
{
	/* ������ʼ��	*/
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
    uint8_t     type;   //0:GCode 1:����GCode 2:�̼�
    uint8_t     write;
    char        name[64];
}TFTP_Handler;
 
TFTP_Handler    tftp_Handler;
FIL             file;
FATFS fs1;	

static void file_init(void)
{
    FRESULT res_sd1;   
  //���ⲿSPI Flash�����ļ�ϵͳ���ļ�ϵͳ����ʱ���SPI�豸��ʼ��
	res_sd1 = f_mount(&fs1,"0:",1);
	
/*----------------------- ��ʽ������ ---------------------------*/  
	/* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
	if(res_sd1 == FR_NO_FILESYSTEM)
	{
		printf("��SD����û���ļ�ϵͳ���������и�ʽ��...\r\n");
    /* ��ʽ�� */
		res_sd1=f_mkfs("0:",0,0);							
		
		if(res_sd1 == FR_OK)
		{
			printf("��SD���ѳɹ���ʽ���ļ�ϵͳ��\r\n");
      /* ��ʽ������ȡ������ */
			res_sd1 = f_mount(NULL,"0:",1);			
      /* ���¹���	*/			
			res_sd1 = f_mount(&fs1,"0:",1);
		}
		else
		{
			printf("������ʽ��ʧ�ܡ�����\r\n");
			while(1);
		}
	}
  else if(res_sd1!=FR_OK)
  {
    printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",res_sd1);
    printf("��������ԭ��SD����ʼ�����ɹ���\r\n");
		while(1);
  }
  else
  {
    printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
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