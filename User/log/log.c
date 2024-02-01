 #include "log.h"

void LOG(uint8_t MODULE_ID,char *sub_name,uint8_t level,const char *format,...)
{
  char buff[LOG_MAX_BUFF];
  memset(buff,0,LOG_MAX_BUFF);
  sprintf(buff,"%d,%s,%d,%s",MODULE_ID,sub_name,level,format);
  printf("%s\r\n",buff);
}

char* subName1(char* file_name,char* fun_name,uint16_t line)
{
  char buff[LOG_MAX_BUFF];
  memset(buff,0,LOG_MAX_BUFF);
  sprintf(buff,"%s/%s/%d",file_name,fun_name,line);
  return buff;
}