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


//需要拓展设备时增加
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
//#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */
//定义逻辑设备号
#define SD_CARD		  0	
#define SPI_FLASH		1
#define USB_OTG		  2

//定义扇区大小
#define FLASH_SECTOP_SIZE 4096
#ifdef SDIO_SD
//SD卡散区大小
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
		//SD卡设备状态查询分支
		stat &= ~STA_NOINIT;//默认返回OK状态

		break;

	case SPI_FLASH :
		//SPI_FLASH设备状态查询分支
		 if(sFLASH_ID == SPI_FLASH_ReadID())
      {
        /* 设备ID读取结果正确 */
        stat &= ~STA_NOINIT;
      }
      else
      {
        /* 设备ID读取结果错误 */
        stat = STA_NOINIT;;
      }
		// translate the reslut code here

		break;

	case USB_OTG :
		//USB存储设备状态查询分支

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
		//初始化SD 卡

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
		//初始化SPI_FLASH
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
      /* 唤醒SPI Flash */
	    SPI_Flash_WAKEUP();
			//获取设备状态，确定初始成功
			stat = disk_status(SPI_FLASH);
			// translate the reslut code here

		return stat;

	case USB_OTG :
		//初始化usb u盘

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
		//要读取的扇区号转换称为地址
		/* 扇区偏移6MB，外部Flash文件系统空间放在SPI Flash后面10MB空间 */
		 sector+=1536;
		read_addr = sector*FLASH_SECTOP_SIZE;
		SPI_FLASH_BufferRead((u8*) buff, read_addr, count*FLASH_SECTOP_SIZE);
	

		// translate the reslut code here
		//当前默认都能正常读取
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
			
			//要写入的扇区号转换称为地址
			write_addr = sector*FLASH_SECTOP_SIZE;
			//写入前需要擦除的扇区
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
				//查询存储介质有多少个扇区，文件系统通过该值获取存储介质的容量
				*(DWORD *)buff = (16*1024*1024/FLASH_SECTOP_SIZE)-1536;//4096-1536
				res = RES_OK;
				break;
			case GET_SECTOR_SIZE:
				//查询存储介质单个扇区的大小
				*(WORD *)buff =FLASH_SECTOP_SIZE;
				res =  RES_OK;
				break;
			case GET_BLOCK_SIZE:
				//查询存储介质擦除的最小单位（扇区）
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
//获取时间
DWORD get_fattime (void)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	RTC_GetTime(RTC_Format_BIN, &sTime);
	RTC_GetDate(RTC_Format_BIN, &sDate);
		/* 返回当前时间戳 */
	return	  ((DWORD)((2000+sDate.RTC_Year) - 1980) << 25)	/* Year 2015 */
			| ((DWORD)sDate.RTC_Month << 21)				/* Month 1 */
			| ((DWORD)sDate.RTC_Date << 16)				/* Mday 1 */
			| ((DWORD)sTime.RTC_Hours << 11)				/* Hour 0 */
			| ((DWORD)sTime.RTC_Minutes << 5)				  /* Min 0 */
			| ((DWORD)sTime.RTC_Seconds >> 1);				/* Sec 0 */
}
