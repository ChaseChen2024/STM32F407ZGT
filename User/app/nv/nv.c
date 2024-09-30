
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
		log_i("open file failed,err:%d\r\n", err);
		return err;
    }

	err = f_read(&fd, p_read_buf, read_len, &br);
	if (FR_OK != err)
    {
		log_i("read file failed,err:%d\r\n", err);
		return err;
    }
	log_i("\r\nread,err:%d,br:%d,%s\r\n", err,br,p_read_buf);
	err = f_close(&fd);
	log_i("\r\n,%d,f_close res=%d",__LINE__,err);
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
	log_i("\r\n,%d,f_open res=%d",__LINE__,err);
	
	if(err != FR_OK)
	{
		log_i("\r\n,%d,f_write res=%d",__LINE__,err);
        return err;
	}

	err = f_write(&fd, p_write_buf, write_len ,&bw);
	log_i("\r\n,%d,f_write res=%d len=%d bw=%d",__LINE__,err,strlen(p_write_buf),bw);

	err = f_close(&fd);
	log_i("\r\n,%d,f_close res=%d",__LINE__,err);
    return err;
}
	
FRESULT nv_init(void)
{
	FRESULT err = FR_OK;
	FATFS *pfs;
	memset(&UserFaftsParams,0,sizeof(UserFaftsParams));
	DWORD fre_clust, fre_size, tot_size;
	strcpy(UserFaftsParams.LDriveId,"1:");
	elog_i(ELOG_APP,"nv_init_1,UserFaftsParams.LDriveId:%s",UserFaftsParams.LDriveId);
	err = f_mount(&(UserFaftsParams.fs),(UserFaftsParams.LDriveId),1);
	elog_i(ELOG_APP,"nv_init_2,fmount err=%d",err);
	if(err == FR_NO_FILESYSTEM)
	{
		err = f_mkfs("1:",0,1024*1024*10);
		elog_i(ELOG_APP,"nv_init_3,f_mkfs res=%d",err);
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

        elog_i(ELOG_APP,"nv_init_4\r\ntot_size:%10lu KB\r\nfre_size:%10lu KB",tot_size *4, fre_size *4);
    }
	return err;
}
#ifdef USER_LEETTER_SHELL
char patch_name[32] = "1:";
void nv_read_test(char *str,int len)
{
	FRESULT err = FR_OK;
	char nv_read_buf[64]= {0};
	char buf[128]= {0};
	err = nv_read(str,nv_read_buf,len);
    
    sprintf(buf, "read string: %s\r\nlen:%d,err:%d\r\n",nv_read_buf,len,err);
	shellPrint(&shell,"%s",buf);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nvread, nv_read_test, nvread "1:xxxx.xx" len);

void nv_wirte_test(int argc, char *argv[])
{
	FRESULT err = FR_OK;
	DIR dir_info;
	int len = 0;
	char buf[128]= {0};
	elog_i(ELOG_APP,"cmd :%s,%s,%s,%s",argv[0],argv[1],argv[2],argv[3]);
	if(argv[1][0] == '.' && argv[1][1] == '.' )
	{
		//返回路径，不作处理
	}
	else if(argv[1][0] == '1' && argv[1][1] == ':' )
	{
		//绝对路径
		strcpy(buf,argv[1]);
		char *p = strchr(buf,'/');
		*p = '\0';
		if(f_opendir(&dir_info, buf) == FR_OK)
		{
			err = nv_write(argv[1],argv[2],strlen(argv[2]));
			f_closedir(&dir_info);
		}
		else
		{
			shellPrint(&shell,"no such file or directory\r\n");
		}
	}
	else
	{
		//相对路径

		if(f_opendir(&dir_info, patch_name) == FR_OK)
		{
			strcpy(buf,patch_name);
			strcat(buf,"/");
			strcat(buf, argv[1]);
			err = nv_write(buf,argv[2],strlen(argv[2]));
			f_closedir(&dir_info);
		}
		else
		{
			shellPrint(&shell,"no such file or directory\r\n");
		}
	}
	shellPrint(&shell,"write string:len:%d,err:%d",len,err);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), write, nv_wirte_test, fatfs file wirte);

int nv_size_test(int argc, char *argv[])
{
	FRESULT err = FR_OK;
	FATFS *pfs;
	char buf[64]= {0};

	DWORD fre_clust, fre_size, tot_size;

    err = f_getfree(argv[1], &fre_clust, &pfs);
    
    if( err == FR_OK )
    {
        // 总容量计算方法
        // pfs->csize 该参数代表一个簇占用几个 SD卡物理扇区，每个扇区512字节
        // pfs->n_fatent 簇的数量+2
        // 总容量 = 总簇数*一个簇占用大小
        // 剩余容量 = 剩余簇数*一个簇占用大小
        tot_size = (pfs->n_fatent - 2) * pfs->csize; // 总容量    单位Kbyte
        fre_size = fre_clust * pfs->csize;           // 可用容量  单位Kbyte

        elog_i(ELOG_APP,"nv_size_test_1\r\ntot_size:%10lu KB\r\nfre_size:%10lu KB",tot_size *4, fre_size *4);
    }
    
    sprintf(buf, "tot_size:%10lu KB\r\nfre_size:%10lu KB\r\n",tot_size *4, fre_size *4);
	shellPrint(&shell, buf);
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), fatfs_size, nv_size_test, fatfs size);

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
                elog_i(ELOG_APP,"%s/%s", path, fno.fname);
				// shellPrint(&shell,"%s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nvscanall, nv_scan_all_files_test, fatfs scan all files);


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
				
				elog_i(ELOG_APP,"/%s", fno.fname);
				sprintf(buf, "/%s\r\n",fno.fname);
				shellPrint(&shell,"%s",buf);
                if (res != FR_OK) break;
                patch_name[i] = 0;
            } else {                                       /* It is a file. */
                elog_i(ELOG_APP,"%s",fno.fname);
				sprintf(buf, "%s\r\n",fno.fname);
				shellPrint(&shell,"%s",buf);

            }
        }
        f_closedir(&dir);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), ls, nv_scan_files_test, fatfs scan files);

void nv_open_dir_test(int argc, char *argv[])
{
	char buf[64] = {0};
	DIR dir_info;
	FRESULT ret = FR_OK;
	log_i("argv[1]:%s\r\n",argv[1]);
	if(argv[1][0] == '.' && argv[1][1] == '.')
	{
		char *p = strrchr(patch_name,'/');
        if(p) *p = 0;
	}
	else if(argv[1][0] == '1' && argv[1][1] == ':')
	{
		log_i("argv[0]:%s\r\n",argv[1]);
		if(f_opendir(&dir_info, argv[1]) == FR_OK)
		{
			strcpy(patch_name,argv[1]);
			f_closedir(&dir_info);
		}
		else
		{
			shellPrint(&shell,"dir no such\r\n");
		}
	}
	else
	{
		strcat(buf,patch_name);
		strcat(buf,"/");
		strcat(buf,argv[1]);
		log_i("%s\r\n",buf);
		ret = f_opendir(&dir_info, buf);
		if(ret == FR_OK)
		{
			strcat(patch_name,"/");	
			strcat(patch_name,argv[1]);
			f_closedir(&dir_info);
		}
		else
		{
			shellPrint(&shell,"dir no such\r\n");
		}
		
	}
	log_i("%s,ret:%d\r\n",patch_name,ret);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), cd, nv_open_dir_test, fatfs open dir);


int nv_mkdir_dir_test(int argc, char *argv[])
{
	FRESULT res;
	DIR dir_info;
	char patch_mk[64] = {0};
	if(argc < 2)
	{
		shellPrint(&shell,"command no such\r\n");
	}
	else
	{
		log_i("argv[1]:%s\r\n",argv[1]);
		if(argv[1][0] == '.' && argv[1][1] == '.')
		{

		}
		else if(argv[1][0] == '1' && argv[1][1] == ':')
		{
			strcpy(patch_mk,argv[1]);
			char *p = strrchr(patch_mk,'/');
        	if(p) *p = 0;
			log_i("patch_mk:%s\r\n",patch_mk);
			if(f_opendir(&dir_info, patch_mk) == FR_OK)
			{
				f_closedir(&dir_info);
				res = f_mkdir(argv[1]);
				if(res != FR_OK)
				{
					shellPrint(&shell,"error\r\n");
				}
			}
			else
			{
				shellPrint(&shell,"dir no such\r\n");
			}
		}
		else
		{
			sprintf(patch_mk,"%s/%s",patch_name,argv[1]);
			log_i("%s\r\n",patch_mk);
			res = f_mkdir(patch_mk);
			if(res != FR_OK)
			{
				shellPrint(&shell,"error\r\n");
			}
		}
	}
	return res;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), mkdir, nv_mkdir_dir_test, fatfs mkdir dir);
void nv_rm_test(char *dir)
{
	FRESULT res;
	char patch_mk[32] = {0};
	strcat(patch_mk,patch_name);
	strcat(patch_mk,"/");
	strcat(patch_mk,dir);
	log_i("%s\r\n",patch_mk);
	res = f_unlink(patch_mk);
	if(res != FR_OK)
	{
		shellPrint(&shell,"error\r\n");
	}
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), rm, nv_rm_test, fatfs rm);
void nv_pwd_test(void)
{
	char patch_mk[64] = {0};
	log_i("%s\r\n",patch_name);
	sprintf(patch_mk,"%s",patch_name);
	shellPrint(&shell,patch_mk);

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


int nv_copy_test(int argc, char *argv[])
{
	shellPrint(&shell,"%s\r\n%s\r\n", argv[1], argv[2]);

	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),cp,nv_copy_test,file copy cmd);


int nv_rstset_test(int argc, char *argv[])
{
	FRESULT err = FR_OK;
	FATFS *pfs;
	memset(&UserFaftsParams,0,sizeof(UserFaftsParams));
	DWORD fre_clust, fre_size, tot_size;
	strcpy(UserFaftsParams.LDriveId,"1:");
		
	err = f_mkfs("1:",0,1024*1024*10);
	log_i("\r\nnv_init_3,f_mkfs res=%d\r\n",err);
	err = f_mount(NULL,(UserFaftsParams.LDriveId),1);
	err = f_mount(&(UserFaftsParams.fs),(UserFaftsParams.LDriveId),1);		

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

        log_i("\r\nnv_init_4\r\ntot_size:%10lu KB\r\nfre_size:%10lu KB\r\n",tot_size *4, fre_size *4);
    }
	shellPrint(&shell,"\r\ntot_size:%10lu KB\r\nfre_size:%10lu KB\r\n",tot_size *4, fre_size *4);
	return err;

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),fatfs_rstset,nv_rstset_test,file rstset cmd);
