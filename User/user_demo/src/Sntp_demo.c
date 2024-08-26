#if 1
#include "Rtc_demo.h"
#include "Sntp_demo.h"
#include "bsp_rtc.h"
#include "bsp_debug_usart.h"

#include <string.h>
#include <time.h>


#include "lwip/opt.h"
#include <sys/time.h>
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/apps/sntp.h"
#include <lwip/sockets.h>
#include "sys_arch.h"
#include "lwip/ip.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"


static TaskHandle_t SNTP_Demo_Task_Handle = NULL;/* RTC������ */
#if 0
uint32_t get_timestamp(void);
void sntp_set_time(uint32_t sntp_time);

/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void SNTP_Demo_Task(void* parameter)
{	
	LwIP_NW_Init();
	vTaskDelay(1000);
//	int a=0,b=0,c=0;
	Sntp_BSP_Init();
	// sntp_set_time(1);
	vTaskDelay(60000);
	sntp_stop();
	vTaskDelete(SNTP_Demo_Task_Handle); 
//   while (1)
//   {
//     /* ��ʾʱ������� */
// 		RTC_TimeAndDate_Show();
//   }
}
/*!
 * @brief ��ȡ��ǰʱ���
 *
 * @param [in] none
 *
 * @retval ��ǰʱ���
 */
uint32_t get_timestamp(void)
{
    struct tm stm;
    static RTC_DateTypeDef g_Date = {0};
    static RTC_TimeTypeDef g_Time = {0};

    ///��ȡʱ������ڻ�ȡ����ǰ
	RTC_GetTime(RTC_Format_BIN, &g_Time);
	RTC_GetDate(RTC_Format_BIN, &g_Date);

    stm.tm_year = g_Date.RTC_Year + 100;    //RTC_Year rang 0-99,but tm_year since 1900

    stm.tm_mon = g_Date.RTC_Month - 1;      //RTC_Month rang 1-12,but tm_mon rang 0-11

    stm.tm_mday = g_Date.RTC_Date;          //RTC_Date rang 1-31 and tm_mday rang 1-31

    stm.tm_hour = g_Time.RTC_Hours;         //RTC_Hours rang 0-23 and tm_hour rang 0-23

    stm.tm_min = g_Time.RTC_Minutes;        //RTC_Minutes rang 0-59 and tm_min rang 0-59

    stm.tm_sec = g_Time.RTC_Seconds;

	return (mktime(&stm) - (8 * 60 * 60));///����ʱ���ڶ���������8Сʱ����Ϊʱ��������ȥ
}

void sntp_set_time(uint32_t sntp_time)
{
	if(sntp_time == 0)
	{
		printf("sntp_set_time: wrong!@@\n");
		return;
	}
	
	printf("\r\nsntp_set_time: c00, enter!\r\n");
	printf("\r\nsntp_set_time: c01, get time = %u\r\n\n", sntp_time);

	struct tm *time;
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	sntp_time += (8 * 60 * 60); ///����ʱ���Ƕ�8����ƫ��8Сʱ

	time = localtime(&sntp_time);

	/*
	 * ���� RTC �� ʱ��
	 */
	sTime.RTC_Hours = time->tm_hour;
	sTime.RTC_Minutes = time->tm_min;
	sTime.RTC_Seconds = time->tm_sec;
	// sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	// sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (RTC_SetTime(RTC_Format_BINorBCD, &sTime) != SUCCESS)
	{
		// Error_Handler();
		printf("\r\nRTC_SetTime fail  \r\n");
	}
	
	/*
	 * ���� RTC �� ����
	 */
	sDate.RTC_WeekDay = time->tm_wday;
	sDate.RTC_Month = (time->tm_mon) + 1;
	sDate.RTC_Date = time->tm_mday;
	sDate.RTC_Year = (time->tm_year) + 1900 - 2000;
	if (RTC_SetDate(RTC_Format_BINorBCD, &sDate) != SUCCESS)
	{
		// Error_Handler();
		printf("\r\nRTC_SetDate fail  \r\n");
	}

	printf("\r\nsntp_set_time: c02, decode time: 20%d-%02d-%02d %d:%d:%d\n", \
				sDate.RTC_Year, sDate.RTC_Month, sDate.RTC_Date, sTime.RTC_Hours, sTime.RTC_Minutes, sTime.RTC_Seconds);
	
	printf("\r\nsntp_set_time: c03, test get = %u\n", get_timestamp());
	printf("\r\nsntp_set_time: c04, set rtc time done\n");
}

void set_sntp_server_list(void)
{
	uint32_t server_list[SNTP_MAX_SERVERS] =	{  
													0x279148D2,  //������ʱ����
													0x42041876,
													0x5F066CCA,
													0x0B6C1978,
													0x0B0C5CB6,
													0x58066BCB,
													0x14731978,
													0xC51F70CA,
													0x521D70CA,
													0x820176CA,
													0x510176CA,
												};
	ip_addr_t sntp_server;
												
	for(int i = 0; i < SNTP_MAX_SERVERS; i++)
	{
		sntp_server.addr = server_list[i];
		sntp_setserver(i, &sntp_server);  // ������ʱ����
	}
}

long SNTP_Demo_Task_Init(void)
{
			BaseType_t xReturn = pdPASS;
	
			/* ����RTC_Task���� */
		xReturn = xTaskCreate((TaskFunction_t )SNTP_Demo_Task,  /* ������ں��� */
													(const char*    )"SNTP_Demo_Task",/* �������� */
													(uint16_t       )512,  /* ����ջ��С */
													(void*          )NULL,/* ������ں������� */
													(UBaseType_t    )3, /* ��������ȼ� */
													(TaskHandle_t*  )&SNTP_Demo_Task_Handle);/* ������ƿ�ָ�� */ 
		return xReturn;
}
void Sntp_BSP_Init(void)
{
	printf("\r\n Sntp_BSP_Init \r\n");

	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	//  set_sntp_server_list();
  	sntp_setservername(0, "203.107.6.88" );
	sntp_init();
	printf("\r\nsntp_init \r\n");
	
}
#else
#define VERSION_3           3
#define VERSION_4           4
 
#define MODE_CLIENT         3
#define MODE_SERVER         4
 
#define NTP_LI              0
#define NTP_VN              VERSION_3   
#define NTP_MODE            MODE_CLIENT
#define NTP_STRATUM         0
#define NTP_POLL            0
#define NTP_PRECISION       0
 
#define NTP_HLEN            48
 
#define NTP_PORT            123
#define NTP_SERVER          "203.107.6.88"
 
#define TIMEOUT             10
 
#define BUFSIZE             1500
 
#define JAN_1970            0x83aa7e80
 
#define NTP_CONV_FRAC32(x)  (uint64_t) ((x) * ((uint64_t)1<<32))    
#define NTP_REVE_FRAC32(x)  ((double) ((double) (x) / ((uint64_t)1<<32)))   
 
#define NTP_CONV_FRAC16(x)  (uint32_t) ((x) * ((uint32_t)1<<16))    
#define NTP_REVE_FRAC16(x)  ((double)((double) (x) / ((uint32_t)1<<16)))    
#define NTP_TIMESTAMP_DELTA  2208988800UL
 
#define USEC2FRAC(x)        ((uint32_t) NTP_CONV_FRAC32( (x) / 1000000.0 )) 
#define FRAC2USEC(x)        ((uint32_t) NTP_REVE_FRAC32( (x) * 1000000.0 )) 
#define NTP_LFIXED2DOUBLE(x)    ((double) ( ntohl(((struct l_fixedpt *) (x))->intpart) - JAN_1970 + FRAC2USEC(ntohl(((struct l_fixedpt *) (x))->fracpart)) / 1000000.0 ))   


struct s_fixedpt {
    uint16_t    intpart;
    uint16_t    fracpart;
};
 
struct l_fixedpt {
    uint32_t    intpart;
    uint32_t    fracpart;
};
struct ntphdr {


    unsigned int    li:2;  			//����ʶ���� ��2 bit
    unsigned int    vn:3;  			//�汾�� , 3 bit ,��ʾNTP�İ汾�ţ���ǰΪ3
    unsigned int    mode:3;			//ģʽ�� 3 bit
    uint8_t         stratum;		//�㣬 8 bit
    uint8_t         poll;  			//���Լ���� 8 bit ����ʾ������Ϣ֮��������
    int8_t          precision; 		//���ȣ� 8 bit ����ʾ����ʱ��ľ���

    struct s_fixedpt    rootdelay;		//����ʱ�� 8 bit ����ʾ�����ο�Դ֮���������ܹ�ʱ��
    struct s_fixedpt    rtdispersion;	//����ɢ�� 8 bit , ��ʾ�����ο�Դ�йص��������
    uint32_t            reference_id;	//�ο�ʱ�ӱ�ʶ���� 8 bit , ������ʶ����Ĳο�Դ
    struct l_fixedpt    reference_time;	//�ο�ʱ���, 64 bits,����ʱ�ӱ��޸ĵ�����ʱ��
    struct l_fixedpt    original_time;	//ԭʼʱ���, 64 bits,�ͻ��˷��͵�ʱ��
    struct l_fixedpt    receive_time;	//����ʱ���, 64 bits,����˽��յ���ʱ��
    struct l_fixedpt    transfer_time;	//����ʱ���, 64 bits,������ͳ�Ӧ���ʱ��
};
#define NTPFRAC(x) (4294 * (x) + ((1981 * (x)) >> 11))
#define NTP_PACKET_SIZE 48
#define Data(i) ntohl(((uint32_t *)buf)[i])

uint8_t g_lwip_time_buf[100];
const char g_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
typedef struct _DateTime  /*�˽ṹ�嶨����NTPʱ��ͬ������ر���*/
{
    int  year;        /* �� */
    int  month;       /* �� */
    int  day;         /* �� */
    int  hour;        /* ʱ */
    int  minute;      /* �� */
    int  second;      /* �� */
} DateTime;
DateTime g_nowdate;   
uint32_t osKernelGetTickCount (void) {
  TickType_t ticks;

//   if (IS_IRQ()) {
//    ticks = xTaskGetTickCountFromISR();
//   } else {
     ticks = xTaskGetTickCount();
//   }

  return (ticks);
}
uint32_t  quec_get_time(void) 
{
    uint32_t currentMs = 0;

    currentMs = osKernelGetTickCount()/portTICK_PERIOD_MS;

    return currentMs;
}
uint32_t mktime_ue (unsigned int year, unsigned int mon,
					unsigned int day, unsigned int hour,
					unsigned int min, unsigned int sec)     
{
    if (0 >= (int) (mon -= 2)){    /**//* 1..12 -> 11,12,1..10 */
         mon += 12;      /**//* Puts Feb last since it has leap day */
         year -= 1;
    }
 
    return ((((unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +year*365 - 719499)*24 + hour)*60 + min )*60 + sec;
}
static void ql_ntp_gettime(uint32_t *time_coarse, uint32_t *time_fine)
{
    struct timeval now;
    uint32_t tm = quec_get_time();

    now.tv_sec = tm / 1000;
    now.tv_usec = tm % 1000;

    *time_coarse = now.tv_sec + JAN_1970;
    *time_fine = NTPFRAC(now.tv_usec);
}


int get_ntp_packet(uint32_t *data)  //����������NTP������
{
	uint32_t tv_sce = 0,tv_usec = 0;
	ql_ntp_gettime(&tv_sce, &tv_usec);

	data[0]= htonl((NTP_LI<<30)|(NTP_VN << 27)|(NTP_MODE << 24)|
	(NTP_STRATUM << 16)|(NTP_POLL << 8)|(NTP_PRECISION & 0xff ));
	data[1]= htonl(1 << 16);
	data[2]= htonl(1 << 16);
	
	data[10]= htonl(tv_sce);
	data[11]= htonl(tv_usec);
	printf("��NTP�����\r\n");
    return 0;
}

void lwip_calc_date_time(uint32_t time)
{
   unsigned int Pass4year;
    int hours_per_year;
    if (time <= 0)
    {
        time = 0;
    }

    g_nowdate.second = (int)(time % 60);   /* ȡ��ʱ�� */
    time /= 60;

    g_nowdate.minute = (int)(time % 60);   /* ȡ����ʱ�� */
    time /= 60;
    
    g_nowdate.hour = (int)(time % 24);     /* Сʱ�� */

    Pass4year = ((unsigned int)time / (1461L * 24L));/* ȡ��ȥ���ٸ����꣬ÿ������ 1461*24 Сʱ */

    g_nowdate.year = (Pass4year << 2) + 1970;    /* ������� */

    time %= 1461 * 24;     /* ������ʣ�µ�Сʱ�� */

    for (;;)               /* У������Ӱ�����ݣ�����һ����ʣ�µ�Сʱ�� */
    {
        hours_per_year = 365 * 24;         /* һ���Сʱ�� */

        if ((g_nowdate.year & 3) == 0) /* �ж����� */
        {

            hours_per_year += 24;          /* �����꣬һ�����24Сʱ����һ�� */
        }

        if (time < hours_per_year)
        {
            break;
        }

        g_nowdate.year++;
        time -= hours_per_year;
    }

    time /= 24;   /* һ����ʣ�µ����� */

    time++;       /* �ٶ�Ϊ���� */

    if ((g_nowdate.year & 3) == 0)      /* У��������������·ݣ����� */
    {
        if (time > 60)
        {
            time--;
        }
        else
        {
            if (time == 60)
            {
                g_nowdate.month = 1;
                g_nowdate.day = 29;
                return ;
            }
        }
    }

    for (g_nowdate.month = 0; g_days[g_nowdate.month] < time; g_nowdate.month++)   /* �������� */
    {
        time -= g_days[g_nowdate.month];
    }

    g_nowdate.day = (int)(time);

    return;

}
BaseType_t Dhcp_Client_Task_Init(void);

static void SNTP_Demo_Task(void* parameter)
{	
	
	printf("sntp init\r\n");
	
	LwIP_NW_Init();
	
	printf("sntp init111\r\n");
//	vTaskDelay(100);
	uint32_t data[12]; //�������ݰ�
	uint8_t buf[NTP_PACKET_SIZE];
    size_t nbytes = NTP_PACKET_SIZE;
    int socket_fd;
    struct sockaddr_in servaddr;
	uint32_t tv_sce = 0,tv_usec = 0;
	uint32_t T1,T2,T3,T4,T;
	uint32_t ip_info_size = 0;
	int recv_data_len = 0;
	int block = 1;
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	//�����׽���  socket_fd
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_fd < 0)
	{
		printf("socket creat fail \r\n");
		goto END;

	}
	//����ʹ�÷�����IO�������ķ��ͺͽ��պ�������������
	ioctlsocket(socket_fd,FIONBIO,&block);
	//���÷�������Ϣ��IP���ͣ��˿ںţ���������ַ
	servaddr.sin_family=AF_INET;
	servaddr.sin_port = htons(NTP_PORT);
	servaddr.sin_addr.s_addr = inet_addr(NTP_SERVER);
	memset(&(servaddr.sin_zero), 0, sizeof(servaddr.sin_zero));
	
	//��NTP�������ݰ�
	get_ntp_packet(data);

	//���͵�ָ��������
	sendto(socket_fd,data,nbytes,0,(struct sockaddr *) &servaddr,sizeof(struct sockaddr));
	printf("send\r\n");

	
	memset(buf, 0, NTP_PACKET_SIZE);
	
	ip_info_size = sizeof(struct sockaddr);
	for(int i = 0; i < 10; i++)
	{
		recv_data_len = recvfrom(socket_fd, buf, NTP_PACKET_SIZE, 0,(struct sockaddr *) &servaddr,&ip_info_size);
		for (int i = 0; i < recv_data_len; i++)
		{
			 printf("%x ", buf[i]);
		}
		
		printf("\r\nrecv_data_len=%d\r\n",recv_data_len);
		if(recv_data_len > 0)
		{
			break;
		}
		else
		{
			printf("\r\nSNTP_Demo_Task_0,try cnt:%d\r\n",i);
			vTaskDelay(200);
		}
	}
	printf("\r\nData(10) %x,JAN_1970 %x \r\n",Data(10),JAN_1970);
	
	T1 = Data(6);
	T3 = Data(10) - JAN_1970;
	//�㷨��Ҫ���䣬����û��
	
	//����ת��
	lwip_calc_date_time(T3);
	printf("����ʱ�䣺%02d-%02d-%02d %02d:%02d:%02d\r\n",  
                           g_nowdate.year, 
                           g_nowdate.month + 1,
                           g_nowdate.day,
                           g_nowdate.hour + 8,
                           g_nowdate.minute,
                           g_nowdate.second);
    /*
	 * ���� RTC �� ʱ��
	 */
	sTime.RTC_Hours = g_nowdate.hour + 8;
	sTime.RTC_Minutes = g_nowdate.minute;
	sTime.RTC_Seconds = g_nowdate.second;
	if (RTC_SetTime(RTC_Format_BINorBCD, &sTime) != SUCCESS)
	{
		printf("\r\nRTC_SetTime fail  \r\n");
	}
	
	/*
	 * ���� RTC �� ����
	 */
//	sDate.RTC_WeekDay = time->tm_wday;
	sDate.RTC_Month = g_nowdate.month + 1;
	sDate.RTC_Date = g_nowdate.day;
	sDate.RTC_Year = (g_nowdate.year) + 1900 - 2000;
	if (RTC_SetDate(RTC_Format_BINorBCD, &sDate) != SUCCESS)
	{
		// Error_Handler();
		printf("\r\nRTC_SetDate fail  \r\n");
	}

	vTaskDelay(1000);
	
	vStartMQTTTasks(1024*4,10);
	vTaskDelete(NULL); //ɾ������
END:
	printf("socket creat fail \r\n");
	close(socket_fd);
	//���� ����

	
}
long SNTP_Demo_Task_Init(void)
{
		BaseType_t xReturn = pdPASS;
		
			/* ����RTC_Task���� */
		xReturn = xTaskCreate((TaskFunction_t )SNTP_Demo_Task,  /* ������ں��� */
													(const char*    )"SNTP_Demo_Task",/* �������� */
													(uint16_t       )1024*1,  /* ����ջ��С */
													(void*          )NULL,/* ������ں������� */
													(UBaseType_t    )3, /* ��������ȼ� */
													(TaskHandle_t*  )&SNTP_Demo_Task_Handle);/* ������ƿ�ָ�� */ 
		return xReturn;
}
#endif
#endif
