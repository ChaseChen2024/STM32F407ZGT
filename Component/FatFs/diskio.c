/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

// #include "./flash/bsp_spi_flash.h"
// #include "./sdio/bsp_sdio_sd.h"
#ifdef USE_FLASH_SPI1_CODE
#include "bsp_spi_flash.h"
#endif
#ifdef SDIO_SD
#include "bsp_sdio_sd.h"
#endif // SDIO_SD

#include "bsp_rtc.h"
#include "string.h"
#ifdef USE_SFUD_CODE
#include <sfud.h>
#ifdef USE_FAL_CODE
#include "fal.h"
#endif
#endif

//��Ҫ��չ�豸ʱ����
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
//#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */
//���µ����߼��̷���
/*
����fal����������
1 app ����  ����û�Ӧ���ļ�����
2 default ���� ���Ĭ�Ϲ̼�
3 downloade ���� ���������������صĹ̼�
*/
//�����߼��豸��
#define SD_CARD		  0	
#define SPI_FLASH		1
#define USB_OTG		  2

//����������С
#define FLASH_SECTOP_SIZE 4096
#ifdef SDIO_SD
//SD��ɢ����С
#define SD_BLOCKSIZE     512 

extern  SD_CardInfo SDCardInfo;
#endif
#ifdef USE_SFUD_CODE
#ifdef USE_FAL_CODE
extern struct fal_flash_dev nor_flash0;
const struct fal_partition *nor_flash0_partition = NULL;
#else
extern sfud_flash *fatfs_flash;
#endif

#endif
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
	case SD_CARD :
		//SD���豸״̬��ѯ��֧
		stat &= ~STA_NOINIT;//Ĭ�Ϸ���OK״̬

		break;

	case SPI_FLASH :
		stat &= ~STA_NOINIT;
		//SPI_FLASH�豸״̬��ѯ��֧
		#ifdef USE_SFUD_CODE

		#ifdef USE_FAL_CODE
		stat &= ~STA_NOINIT;
		#else
		fatfs_flash = sfud_get_device(SFUD_W25Q128BV_DEVICE_INDEX);
		if(fatfs_flash->init_ok)
		{
			stat &= ~STA_NOINIT;
		}
		else
		{
			stat = STA_NOINIT;
		}
		#endif
		#endif
		// translate the reslut code here

		break;

	case USB_OTG :
		//USB�洢�豸״̬��ѯ��֧

		// translate the reslut code here

		break;
	default:
			stat = STA_NOINIT;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
//	uint8_t result = 0;
	switch (pdrv) {
#ifdef SDIO_SD
	case SD_CARD :
		//��ʼ��SD ��
		if(SD_Init()==SD_OK)
		{
			stat &= ~STA_NOINIT;
		}
		else 
		{
			stat = STA_NOINIT;
		}
		return stat;
#endif
	case SPI_FLASH :
		#ifdef USE_SFUD_CODE
		#ifdef USE_FAL_CODE
		nor_flash0_partition = fal_partition_find("fatfs");
		if (nor_flash0_partition == NULL)
		{
			fal_init();

		}
		#else
		fatfs_flash = sfud_get_device(SFUD_W25Q128BV_DEVICE_INDEX);
		if(!fatfs_flash->init_ok)
		{
			sfud_init();
		}
		#endif
		#endif

		stat = disk_status(SPI_FLASH);
		return stat;
#ifdef USB_OTG
	case USB_OTG :
		//��ʼ��usb u��

		// translate the reslut code here

		return stat;
	}
#endif
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	uint32_t read_addr = 0;
	#ifdef SDIO_SD
	SD_Error SD_state = SD_OK;
#endif
	switch (pdrv) {
#ifdef SDIO_SD
	case SD_CARD :
		
		if((DWORD)buff&3)
		{
			DWORD scratch[SD_BLOCKSIZE / 4];

			while (count--) 
			{
				res = disk_read(SD_CARD,(void *)scratch, sector++, 1);

				if (res != RES_OK) 
				{
					break;
				}
				memcpy(buff, scratch, SD_BLOCKSIZE);
				buff += SD_BLOCKSIZE;
	    }
	    return res;
		}
		
		SD_state=SD_ReadMultiBlocks(buff,sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
		if(SD_state==SD_OK)
		{
			/* Check if the Transfer is finished */
			SD_state=SD_WaitReadOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
		}
		if(SD_state!=SD_OK)
			return RES_PARERR;
		else
		  return RES_OK;	
		break;   

		//return res;
#endif
	case SPI_FLASH :
		// translate the arguments here
		//Ҫ��ȡ��������ת����Ϊ��ַ
		/* ����ƫ��6MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����10MB�ռ� */
		#ifdef USE_SFUD_CODE
		#ifdef USE_FAL_CODE
		read_addr = sector*FLASH_SECTOP_SIZE;
		fal_partition_read(nor_flash0_partition, read_addr, (u8*) buff, count*FLASH_SECTOP_SIZE);
		#else
		sector+=1536;
		read_addr = sector*FLASH_SECTOP_SIZE;
		fatfs_flash = sfud_get_device(SFUD_W25Q128BV_DEVICE_INDEX);
		sfud_read(fatfs_flash, read_addr, count * FLASH_SECTOP_SIZE, buff);
		#endif
		#endif
		

		// translate the reslut code here
		//��ǰĬ�϶���������ȡ
		return RES_OK;
#ifdef USB_OTG
	case USB_OTG :
		// translate the arguments here
		// translate the reslut code here
		return res;
	}
#endif
	return RES_PARERR;
}



/*----------------------------------------void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);-------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	uint32_t write_addr = 0;
	#ifdef SDIO_SD
	SD_Error SD_state = SD_OK;
#endif
	switch (pdrv) {
#ifdef SDIO_SD
	case SD_CARD :
		
		if((DWORD)buff&3)
		{
			DWORD scratch[SD_BLOCKSIZE / 4];

			while (count--) 
			{
				memcpy( scratch,buff,SD_BLOCKSIZE);
				res = disk_write(SD_CARD,(void *)scratch, sector++, 1);
				if (res != RES_OK) 
				{
					break;
				}					
				buff += SD_BLOCKSIZE;
			}
			return res;
		}		
	
		SD_state=SD_WriteMultiBlocks((uint8_t *)buff,sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
		if(SD_state==SD_OK)
		{
			/* Check if the Transfer is finished */
			SD_state=SD_WaitWriteOperation();

			/* Wait until end of DMA transfer */
			while(SD_GetStatus() != SD_TRANSFER_OK);			
		}
		if(SD_state!=SD_OK)
			return RES_PARERR;
	  	else
		  	return RES_OK;	
		break;

		//return res;
#endif
	case SPI_FLASH :
		
		// translate the arguments here
		#ifdef USE_SFUD_CODE
		#ifdef USE_FAL_CODE
		write_addr = sector*FLASH_SECTOP_SIZE;
		fal_partition_erase(nor_flash0_partition, write_addr, FLASH_SECTOP_SIZE);
		fal_partition_write(nor_flash0_partition, write_addr, buff, FLASH_SECTOP_SIZE);
		#else
		sector+=1536;
		write_addr = sector*FLASH_SECTOP_SIZE;
		fatfs_flash = sfud_get_device(SFUD_W25Q128BV_DEVICE_INDEX);
		sfud_erase_write(fatfs_flash, write_addr,FLASH_SECTOP_SIZE, buff);
		#endif
		#endif
		// translate the reslut code here

		return RES_OK;

	case USB_OTG :
		// translate the arguments here



		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;


	switch (pdrv) {
#ifdef SDIO_SD
	case SD_CARD :

		switch (cmd) 
		{
			// Get R/W sector size (WORD) 
			case GET_SECTOR_SIZE :    
				*(WORD * )buff = SD_BLOCKSIZE;
			break;
			// Get erase block size in unit of sector (DWORD)
			case GET_BLOCK_SIZE :      
				*(DWORD * )buff = 1;//SDCardInfo.CardBlockSize;
			break;

			case GET_SECTOR_COUNT:
				*(DWORD * )buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
				break;
			case CTRL_SYNC :
			break;
		}
		res = RES_OK;
		return res;
#endif
	case SPI_FLASH :
		
		switch(cmd)
		{
			case GET_SECTOR_COUNT:
				//��ѯ�洢�����ж��ٸ��������ļ�ϵͳͨ����ֵ��ȡ�洢���ʵ�����
				*(DWORD *)buff = (16*1024*1024/FLASH_SECTOP_SIZE)-1536;//4096-1536
				res = RES_OK;
				break;
			case GET_SECTOR_SIZE:
				//��ѯ�洢���ʵ��������Ĵ�С
				*(WORD *)buff =FLASH_SECTOP_SIZE;
				res =  RES_OK;
				break;
			case GET_BLOCK_SIZE:
				//��ѯ�洢���ʲ�������С��λ��������
				*(DWORD *)buff =1;
				res =  RES_OK;
				break;
			case CTRL_SYNC:
				res =  RES_OK;
				break;
			default:
				res =  RES_OK;
				break;
		}
		// Process of the command for the MMC/SD card

		return res;

	case USB_OTG :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}
#endif
//��ȡʱ��
DWORD get_fattime (void)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	RTC_GetTime(RTC_Format_BIN, &sTime);
	RTC_GetDate(RTC_Format_BIN, &sDate);
		/* ���ص�ǰʱ��� */
	return	  ((DWORD)((2000+sDate.RTC_Year) - 1980) << 25)	/* Year 2015 */
			| ((DWORD)sDate.RTC_Month << 21)				/* Month 1 */
			| ((DWORD)sDate.RTC_Date << 16)				/* Mday 1 */
			| ((DWORD)sTime.RTC_Hours << 11)				/* Hour 0 */
			| ((DWORD)sTime.RTC_Minutes << 5)				  /* Min 0 */
			| ((DWORD)sTime.RTC_Seconds >> 1);				/* Sec 0 */
}
