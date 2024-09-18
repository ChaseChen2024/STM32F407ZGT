
#include "Http_Client_demo.h"
#include <string.h>
#include <time.h>

#ifdef USE_FATFS_CODE
#include "ff.h"
#endif
#include "lwip/opt.h"
#include <sys/time.h>
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/apps/sntp.h"
#include <lwip/sockets.h>
#include "sys_arch.h"
#include "lwip/ip.h"

#ifdef USE_FAL_CODE
#include "fal.h"
#endif
#include "user.h"

#ifdef USER_LEETTER_SHELL
#include "shell_port.h"
#endif



uint8_t location_flag_http = 1; 
static size_t update_file_cur_size_http = 0;
static const struct fal_partition *dl_part_http = NULL;

typedef struct http_client_tag
{
	
	int host_port; //端口
	char host_file[32]; // 要下载的文件在服务器上的路径
	char host_ip[32]; // ip
	char url[64];// url
	char new_file_name[32];//下载到本地文件系统后使用的名称
	char get_buf[256];// get 请求
}http_client_t;

http_client_t __EXRAM qt_http_client = {0};



static TaskHandle_t http_client_Task_Handle = NULL;
int parse_http_response(const char *data, size_t len);

void http_client_parm_init()
{
	qt_http_client.host_port = 80;
	memcpy(qt_http_client.host_file,"/fw/qt201_http_fota_fw.rbl",sizeof("/fw/qt201_http_fota_fw.rbl"));
	memcpy(qt_http_client.host_ip,"192.168.31.238",sizeof("192.168.31.238"));
	memcpy(qt_http_client.url,"http://192.168.31.238/fw/qt201_http_fota_fw.rbl",sizeof("http://192.168.31.238/fw/qt201_http_fota_fw.rbl"));
	memcpy(qt_http_client.new_file_name,"1:httpfw.rbl",sizeof("1:httpfw.rbl"));

	sprintf(qt_http_client.get_buf,
				"GET %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Connection: close\r\n"
                "\r\n",
				qt_http_client.host_file,qt_http_client.host_ip);

}
 static void http_client(void *thread_param)
 {
	char buffer[1024];
	int sock = -1,rece;
    struct sockaddr_in client_addr;
	FRESULT err = FR_OK;
	FIL fd;
	if(location_flag_http == 0)
    {

		err = f_open(&fd, qt_http_client.new_file_name, FA_CREATE_ALWAYS|FA_WRITE);
		if(err != FR_OK)
		{
			log_e("\r\n,%d,open file res=%d",__LINE__,err);
			goto exit;
		}
	}
	else
	{
		
		if ((dl_part_http = fal_partition_find("download")) == NULL)
		{
			elog_e(LOG_TAG,"Firmware download failed! Partition (%s) find error!", "download");
		}
		if (fal_partition_erase_all(dl_part_http) < 0)
		{
			elog_e(LOG_TAG,"Firmware download failed! Partition (%s) erase error!", dl_part_http->name);
		}
		update_file_cur_size_http = 0;
	}

   

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		log_i("Socket error\n");
		vTaskDelay(10);
		goto exit;
	}
	
	log_e("ip:%s",qt_http_client.host_ip);
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(80);
	client_addr.sin_addr.s_addr = inet_addr(qt_http_client.host_ip);
	memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));

	if (connect(sock,
				(struct sockaddr *)&client_addr,
				sizeof(struct sockaddr)) == -1)
	{
		log_i("Connect failed!\n");
		closesocket(sock);
		vTaskDelay(10);
		goto exit;
	}

	log_i("Connect to server successful!\n");
	write(sock,qt_http_client.get_buf,sizeof(qt_http_client.get_buf));

	int count = 0;
	UINT bw = 0;
	int headers_len = 0;
	while ((rece = recv(sock, buffer, 1024, 0)) > 0) {

		//第一包数据包含http头
		if(count == 0)
		{
			headers_len = parse_http_response(buffer,rece);
			if(headers_len < 0)
			{
				log_e("Incomplete request.");
			}
			rece -= headers_len;
			log_i("headers_len:%d,",headers_len);
			memmove(buffer, buffer + headers_len, rece);
		}

		if(location_flag_http == 0)
    	{
			f_write(&fd, buffer, rece ,&bw);
		}
		else
		{
			if (fal_partition_write(dl_part_http, update_file_cur_size_http,(uint8_t*)buffer, rece)< 0)
			{
				elog_e(LOG_TAG,"Firmware download failed! Partition (%s) write data error!", dl_part_http->name);
			}
        	update_file_cur_size_http += rece;
			memset(buffer, 0, 1024);
		}
		elog_e(LOG_TAG,"Received data packet from server:%d  Len:%d  bw:%d all_len:%d", ++count, rece, bw,update_file_cur_size_http);

	}
	if(location_flag_http == 0)
    {
		f_close(&fd);
	}
	else
	{
		update_file_cur_size_http = 0;
		if (fal_partition_read(dl_part_http, 0, buffer, 10) < 0)
        {
            elog_e(LOG_TAG,"Partition[%s] read error!", dl_part_http->name);
        }
	}

	closesocket(sock);
exit:
	vTaskDelete(NULL);
 }
 long HTTP_Demo_Task_Init(void)
{
		http_client_parm_init();
		BaseType_t xReturn = pdPASS;
		xReturn = xTaskCreate((TaskFunction_t )http_client,  /* 任务入口函数 */
													(const char*    )"http_client",/* 任务名字 */
													(uint16_t       )128*22,  /* 任务栈大小 */
													(void*          )NULL,/* 任务入口函数参数 */
													(UBaseType_t    )3, /* 任务的优先级 */
													(TaskHandle_t*  )&http_client_Task_Handle);/* 任务控制块指针 */ 
		return xReturn;
}

// 解析HTTP响应包头
// 解析成功返回头的长度
int parse_http_response(const char *data, size_t len) 
{
	int headers_length = 0;
	char data_buffer[1024] = {0};
	char headers_buffer[300] = {0};
	memcpy(data_buffer,data,len);
	const char *start_of_headers = strstr(data_buffer, "HTTP/");
	const char *end_of_headers = strstr(data_buffer, "\r\n\r\n");
	if (end_of_headers == NULL || start_of_headers == NULL || start_of_headers != data_buffer) 
	{
		log_e("Incomplete request.");
		return -1;
	}

	headers_length = end_of_headers - data_buffer + 4; // 包括"\r\n\r\n"
	log_i("headers_length:%d,",headers_length);
	memcpy(headers_buffer,data_buffer,headers_length);//复制头到headers_buffer
	log_i("headers_buffer:\r\n%s",headers_buffer);
	return headers_length;
}


#ifdef USER_LEETTER_SHELL

/*
格式： http_download <location>
*/
void http_download(int argc, char* argv[])
{
	if(!strcasecmp(argv[1], "0"))
	{
		location_flag_http = 0;
	}
	else
	{
		location_flag_http = 1;
	}


	HTTP_Demo_Task_Init();
	shellPrint(&shell,"http_download <%s> \r\n",qt_http_client.url);
	shellPrint(&shell,"http_download get <%s> \r\n",qt_http_client.get_buf);
	shellPrint(&shell,"http download location:%d",location_flag_http);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), http_download, http_download,http_download);
#endif // USER_LEETTER_SHELL