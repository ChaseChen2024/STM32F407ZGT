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
#include "bsp_spi_flash.h"
#ifdef SDIO_SD
#include "bsp_sdio_sd.h"
#endif // SDIO_SD

#include "bsp_rtc.h"
#include "string.h"


//��Ҫ��չ�豸ʱ����
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
//#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */
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
		//SPI_FLASH�豸״̬��ѯ��֧
		 if(sFLASH_ID == SPI_FLASH_ReadID())
      {
        /* �豸ID��ȡ�����ȷ */
        stat &= ~STA_NOINIT;
      }
      else
      {
        /* �豸ID��ȡ������� */
        stat = STA_NOINIT;;
      }
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
	uint16_t i;
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
		//��ʼ��SPI_FLASH
//		result = GET_SPIFLASH_STATE();
//		if(result != 0)
//		{
//				SPI_FLASH_Init();
//				i=500;
//				while(--i);
//		}
			SPI_FLASH_Init();
				i=500;
				while(--i);
      /* ����SPI Flash */
	    SPI_Flash_WAKEUP();
			//��ȡ�豸״̬��ȷ����ʼ�ɹ�
			stat = disk_status(SPI_FLASH);
			// translate the reslut code here

		return stat;

	case USB_OTG :
		//��ʼ��usb u��

		// translate the reslut code here

		return stat;
	}
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
		 sector+=1536;
		read_addr = sector*FLASH_SECTOP_SIZE;
		SPI_FLASH_BufferRead((u8*) buff, read_addr, count*FLASH_SECTOP_SIZE);
	

		// translate the reslut code here
		//��ǰĬ�϶���������ȡ
		return RES_OK;

	case USB_OTG :
		// translate the arguments here



		// translate the reslut code here

		return res;
	}

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
		sector+=1536;
		while(count--)
		{
			
			//Ҫд���������ת����Ϊ��ַ
			write_addr = sector*FLASH_SECTOP_SIZE;
			//д��ǰ��Ҫ����������
			SPI_FLASH_SectorErase(write_addr);
			SPI_FLASH_BufferWrite((u8*) buff, write_addr, FLASH_SECTOP_SIZE);
			
			sector++;
			buff += FLASH_SECTOP_SIZE;
		}

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
