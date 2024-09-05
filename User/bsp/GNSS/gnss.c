#if 1

#include "gnss.h"

#ifdef USE_GNSS_NMEA
NMEA0183 nmea0183; 
#endif // USE_GNSS_NMEA


char Usart6_Rx_Buf[UART6_BUFF_SIZE];
SemaphoreHandle_t Uart6_BinarySem_Handle =NULL;
static TaskHandle_t Gnss_Resolve_Task_Handle = NULL;

static void Gnss_Resolve_Task(void* parameter)
{
    uint16_t index = 0;
		uint8_t recv_data[UART6_BUFF_SIZE];
    uint32_t recv_data_len = 0;
    while (1)
    {
      xSemaphoreTake(Uart6_BinarySem_Handle, portMAX_DELAY);
      memset(recv_data,0,UART6_BUFF_SIZE);
      memcpy(recv_data,Usart6_Rx_Buf,sizeof(Usart6_Rx_Buf));
	    memset(Usart6_Rx_Buf,0,UART6_BUFF_SIZE);
      // printf("[%s],[%d],[%s]\r\n",__FUNCTION__,__LINE__,recv_data);

      #ifdef USE_GNSS_NMEA
      for(index=0; index<sizeof(recv_data); ++index)
      {
        
        if(nmea_decode(&nmea0183, recv_data[index])) 
        { 
          ///解析代码成功 
        }
        
      }
       printf("[%s],[%d],[%d/%d/%d:%d]\r\n",__FUNCTION__,__LINE__,nmea0183.gpsData.date_time.year,nmea0183.gpsData.date_time.month,nmea0183.gpsData.date_time.day,nmea0183.gpsParse.new_time);
       printf("[%s],[%d],[卫星数:%d]\r\n",__FUNCTION__,__LINE__,nmea0183.gpsParse.new_satellite_count);
       #endif // USE_GNSS_NMEA
    }
    
}


long Gnss_Resolve_Task_Init(void)
{
	BaseType_t xReturn = pdPASS;
	
	Uart6_BinarySem_Handle = xSemaphoreCreateBinary();	
  xReturn = xTaskCreate(Gnss_Resolve_Task,"Gnss_Resolve_Task",2048,NULL,2,&Gnss_Resolve_Task_Handle);
  if(pdPASS == xReturn)
    printf("[%s],[%d],[%s]\r\n",__FUNCTION__,__LINE__,"Gnss_Resolve_Task_Init succeed");
	return xReturn;
}



uint8_t get_gnss_satellite_num(void)
{
  return nmea0183.gpsData.num_sats;
}
int get_gnss_speed(void)
{
  return nmea0183.gpsData.ground_speed * 10;
}
int get_gnss_time(struct tm *time_info)
{
	unsigned int v = nmea0183.gpsParse.new_time;
    time_info->tm_sec  = v % 100; v /= 100;
    time_info->tm_min  = v % 100; v /= 100;
    time_info->tm_hour = v % 100; v /= 100;

	time_info->tm_mday = nmea0183.gpsData.date_time.day;
	time_info->tm_mon = nmea0183.gpsData.date_time.month;
	time_info->tm_year = nmea0183.gpsData.date_time.year;
  	return 0;
}
#endif