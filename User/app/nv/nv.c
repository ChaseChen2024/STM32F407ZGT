
#include <stdio.h>
#include <nv.h>
#include "user.h"
#ifdef USER_LEETTER_SHELL
#include "shell_port.h"
#endif

typedef struct FaftsParams_Tag{
	FATFS fs;
	char LDriveId[2];//Logical drive number
}FaftsParams;

FaftsParams UserFaftsParams;

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

	// err = f_open(&fd, file_name, FA_OPEN_EXISTING|FA_READ);
	err = f_open(&fd, file_name, FA_READ);
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
printf("\r\nread file,err:%d,br:%d,%s\r\n", err,br,p_read_buf);
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
#ifdef USER_LEETTER_SHELL

void nv_read_test(char *str,int len)
{
	FRESULT err = FR_OK;
	char nv_read_buf[64]= {0};
	char buf[128]= {0};
	err = nv_read(str,nv_read_buf,len);
    
    sprintf(buf, "read string: %s\r\nlen:%d,err:%d\r\n",nv_read_buf,len,err);
    Usart_SendString(USART3,buf);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nvread, nv_read_test, fatfs file read);

void nv_wirte_test(char *filename,char *str,int len)
{
	FRESULT err = FR_OK;
	char buf[128]= {0};
	err = nv_write(filename,str,len);
    
    sprintf(buf, "write string: %s\r\nlen:%d,err:%d\r\n",str,len,err);
    Usart_SendString(USART3,buf);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nvwrite, nv_wirte_test, fatfs file wirte);

void nv_size_test(char *LDriveId)
{
	FRESULT err = FR_OK;
	FATFS *pfs;
	char buf[64]= {0};
	DWORD fre_clust, fre_size, tot_size;
    err = f_getfree(LDriveId, &fre_clust, &pfs);
    
    if( err == FR_OK )
    {
        // 总容量计算方法
        // pfs->csize 该参数代表一个簇占用几个 SD卡物理扇区，每个扇区512字节
        // pfs->n_fatent 簇的数量+2
        // 总容量 = 总簇数*一个簇占用大小
        // 剩余容量 = 剩余簇数*一个簇占用大小
        tot_size = (pfs->n_fatent - 2) * pfs->csize; // 总容量    单位Kbyte
        fre_size = fre_clust * pfs->csize;           // 可用容量  单位Kbyte

        printf("\r\nnv_size_test_1\r\ntot_size:%10lu KB\r\nfre_size:%10lu KB\r\n",tot_size *4, fre_size *4);
    }
    
    sprintf(buf, "tot_size:%10lu KB\r\nfre_size:%10lu KB\r\n",tot_size *4, fre_size *4);
    Usart_SendString(USART3,buf);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nvsize, nv_size_test, fatfs size);

void nv_scan_all_files_test(char* path)
{
	FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
	

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                nv_scan_all_files_test(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\n", path, fno.fname);
				// shellPrint(&shell,"%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nvscanall, nv_scan_all_files_test, fatfs scan all files);

char patch_name[64] = "1:";
void nv_scan_files_test(void)
{
	FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
	char buf[32]= {0};

    res = f_opendir(&dir, patch_name);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
			memset(&buf, 0, 32);
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(patch_name);
				
				printf("/%s\n", fno.fname);
				sprintf(buf, "/%s\r\n",fno.fname);
    			Usart_SendString(USART3,buf);
                if (res != FR_OK) break;
                patch_name[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s\n",fno.fname);
				sprintf(buf, "%s\r\n",fno.fname);
    			Usart_SendString(USART3,buf);

            }
        }
        f_closedir(&dir);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ls, nv_scan_files_test, fatfs scan files);

void nv_open_dir_test(char *dir)
{
	if(dir[0] == '.' && dir[1] == '.')
	{
		char *p = strrchr(patch_name,'/');
        if(p) *p = 0;
	}
	else
	{
		strcat(patch_name,"/");	
		strcat(patch_name,dir);
	}
	
	printf("%s\r\n",patch_name);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), cd, nv_open_dir_test, fatfs open dir);


void nv_mkdir_dir_test(char *dir)
{
	FRESULT res;
	char patch_mk[32] = {0};
	sprintf(patch_mk,"%s/%s",patch_name,dir);

	printf("%s\r\n",patch_mk);
	res = f_mkdir(patch_mk);
	if(res != FR_OK)
	{
		Usart_SendString(USART3,"error\r\n");
	}
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), mkdir, nv_mkdir_dir_test, fatfs mkdir dir);
void nv_rm_test(char *dir)
{
	FRESULT res;
	char patch_mk[32] = {0};
	sprintf(patch_mk,"%s/%s",patch_name,dir);

	printf("%s\r\n",patch_mk);
	res = f_unlink(patch_mk);
	if(res != FR_OK)
	{
		Usart_SendString(USART3,"error\r\n");
	}
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), rm, nv_rm_test, fatfs rm);
void nv_pwd_test(void)
{
	char patch_mk[64] = {0};
	printf("%s\r\n",patch_name);
	sprintf(patch_mk,"%s",patch_name);
	Usart_SendString(USART3,patch_mk);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), pwd, nv_pwd_test, fatfs pwd);

void shellKeyTest(void)
{
    char data;
    Shell *shell1 = shellGetCurrent();
    SHELL_ASSERT(shell1 && shell1->read, return);
    while (1)
    {
        if (shell1->read(&data,1) == 0)
        {
            if (data == '\n' || data == '\r')
            {
                return;
            }
            shellPrint(shell1, "%02x ", data);
        }
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
keyTest, shellKeyTest, key test);

#endif
