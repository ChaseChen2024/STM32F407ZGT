
#include <stdio.h>
#include <nv.h>
#include "user.h"
typedef struct FaftsParams_Tag{
	FATFS fs;
	char LDriveId[2];//Logical drive number
}FaftsParams;

FaftsParams UserFaftsParams __EXRAM;

#define NV_NETWORK_CFG	"/nv_network_cfg/"
#define NV_BSP_CFG		"/nv_bsp_cfg/"
#define NV_APP_CFG		"/nv_app_cfg/"
FRESULT nv_read(char *file_name,char *p_read_buf, UINT read_len)
{
	FRESULT err = FR_OK;
	FIL fd;
	UINT br = 0;
	//检查传入参数
	if ((file_name == NULL) || (p_read_buf == NULL))
	{
		return FR_INVALID_PARAMETER;	
	}

	err = f_open(&fd, file_name, FA_OPEN_EXISTING|FA_READ);
    if (FR_OK != err)
    {
		printf("open file failed,err:%d\r\n", err);
		return err;
    }

	err = f_read(&fd, p_read_buf, read_len, &br);
	if (FR_OK != err)
    {
		printf("read file failed,err:%d\r\n", err);
		return err;
    }
printf("\r\nread file failed,err:%d,br:%d,%s\r\n", err,br,p_read_buf);
err = f_close(&fd);
	printf("\r\n,%d,f_close res=%d",__LINE__,err);
    return err;	
}



FRESULT nv_write(char *file_name, char *p_write_buf, UINT write_len)
{
	FRESULT err = FR_OK;
	FIL fd;
	UINT bw = 0;
	if ((file_name == NULL) || (p_write_buf == NULL) || (write_len > 4096))
	{
		return FR_INVALID_PARAMETER;	
	}

	err = f_open(&fd, file_name, FA_CREATE_ALWAYS|FA_WRITE);
	printf("\r\n,%d,f_open res=%d",__LINE__,err);
	
	if(err != FR_OK)
	{
		printf("\r\n,%d,f_write res=%d",__LINE__,err);
        return err;
	}

	err = f_write(&fd, p_write_buf, write_len ,&bw);
	printf("\r\n,%d,f_write res=%d len=%d bw=%d",__LINE__,err,strlen(p_write_buf),bw);

	err = f_close(&fd);
	printf("\r\n,%d,f_close res=%d",__LINE__,err);
    return err;
}
	
FRESULT nv_init(void)
{
	FRESULT err = FR_OK;
	FATFS *pfs;
	memset(&UserFaftsParams,0,sizeof(UserFaftsParams));
	DWORD fre_clust, fre_size, tot_size;
	strcpy(UserFaftsParams.LDriveId,"1:");
	printf("\r\nnv_init_1,UserFaftsParams.LDriveId:%s\r\n",UserFaftsParams.LDriveId);
	err = f_mount(&(UserFaftsParams.fs),(UserFaftsParams.LDriveId),1);
	printf("\r\nnv_init_2,%d,fmount err=%d",__LINE__,err);
	if(err == FR_NO_FILESYSTEM)
	{
		err = f_mkfs("1:",0,1024*1024*10);
		printf("\r\nnv_init_3,f_mkfs res=%d\r\n",err);
		err = f_mount(NULL,(UserFaftsParams.LDriveId),1);
		err = f_mount(&(UserFaftsParams.fs),(UserFaftsParams.LDriveId),1);		
	}
    err = f_getfree(UserFaftsParams.LDriveId, &fre_clust, &pfs );
    
    if( err == FR_OK )
    {
        // 总容量计算方法
        // pfs->csize 该参数代表一个簇占用几个 SD卡物理扇区，每个扇区512字节
        // pfs->n_fatent 簇的数量+2
        // 总容量 = 总簇数*一个簇占用大小
        // 剩余容量 = 剩余簇数*一个簇占用大小
        tot_size = (pfs->n_fatent - 2) * pfs->csize; // 总容量    单位Kbyte
        fre_size = fre_clust * pfs->csize;           // 可用容量  单位Kbyte

        printf("\r\nnv_init_4\r\ntot_size:%10lu KB\r\nfre_size:%10lu KB\r\n",tot_size *4, fre_size *4);
    }
	return err;
}