#if 1
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

#ifdef USE_FAL_CODE
#include "fal.h"
#endif
#include "user.h"

#ifdef USER_LEETTER_SHELL
#include "shell_port.h"
#include "bsp_usart3.h"
#endif
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
uint8_t location_flag = 0; //tftp下载文件的位置 0:文件系统  1:download分区


static const struct fal_partition *dl_part = NULL;
static size_t update_file_cur_size;
static uint32_t read_addr;
void* TFTP_Open(const char* fname, const char* mode, u8_t write)
{
    uint8_t     res;
    char        name[64];
	
    tftp_Handler.write  = write;
    tftp_Handler.type   = 0;
 
    strcpy(name, "1:");
    strcat(name, fname);
    elog_e(LOG_TAG,"TFTP_Open:%s,write:%d!",name,write);

    if(location_flag == 0)
    {
        if (write)
        {
            res= f_open(&file, name, FA_CREATE_ALWAYS|FA_WRITE);
        }
        else
        {
            res = f_open(&file, name, FA_OPEN_EXISTING|FA_READ);
        }
    }
    else
    {
        if (write)
        {
            if ((dl_part = fal_partition_find("download")) == NULL)
            {
                res = 1;
                elog_e(LOG_TAG,"Firmware download failed! Partition (%s) find error!", "download");
            }
            if ((res = fal_partition_erase_all(dl_part)) < 0)
            {
                elog_e(LOG_TAG,"Firmware download failed! Partition (%s) erase error!", dl_part->name);
            }
            update_file_cur_size = 0;
            res = 0;
        }
        else
        {
            read_addr = 0;
        }
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
    elog_e(LOG_TAG,"Transfer file completed!");
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
 
    if(location_flag == 0)
    {
        res = f_read(&file, (uint8_t*)buf, bytes, &br);
    }
    else
    {
        if ((res = fal_partition_read(dl_part, read_addr, buf, bytes)) < 0)
        {
            elog_e(LOG_TAG,"Partition[%s] read error!", dl_part->name);
        }
        read_addr += bytes;
    }
    elog_e(LOG_TAG,"Read File:%d  Len:%d  br:%d read_addr:%d", res, bytes, br,read_addr);
 
    return br;
}
 
int TFTP_Write(void* handle, struct pbuf* p)
{
    uint8_t     res;
    UINT        bw  = 0;

    elog_e(LOG_TAG,"data len %ld",p->len);
    res = ((TFTP_Handler*)handle)->isOpenOK;
    if(res)
    {
        return -1;
    }
    if(location_flag == 0)
    {
        res = f_write(&file, p->payload, p->len, &bw);
    }
    else
    {
        if ((res = fal_partition_write(dl_part, update_file_cur_size, p->payload, p->len))< 0)
        {
            elog_e(LOG_TAG,"Firmware download failed! Partition (%s) write data error!", dl_part->name);
        }
        update_file_cur_size += p->len;
    }
    elog_e(LOG_TAG,"Write File:%d  Len:%d  bw:%d", res, p->len, bw);
 
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
    TFTP_ContextInit();
    tftp_init(&mytftp);
 
    return;
}

#ifdef USER_LEETTER_SHELL
void tftp_location_test(int mode)
{
    location_flag = mode;
    shellPrint(&shell,"-----------------------tftp location %d-----------------------\r\n",location_flag);
    
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), tftp_location, tftp_location_test, Set the partition for tftp operations 0:fatfs 1:download);

#endif
#endif