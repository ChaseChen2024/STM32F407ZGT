#ifndef __APP_LOG_H__
#define __APP_LOG_H__

#define LOG_ERR_LEVEL 0 // 
#define LOG_INFO_LEVEL 1 // 
#define LOG_DEBUG_LEVLE 2 // 

void printf_log(const char *file, int line, int level, const char *fmt, ...);

#define APP_PR_LOG(level, fmt, ...) do {\
    printf_log(__FILE__, __LINE__, level, fmt, ##__VA_ARGS__);\
}while(0);

#define PR_ERR(fmt, ...) APP_PR_LOG(LOG_ERR_LEVEL, fmt, ##__VA_ARGS__)
#define PR_INFO(fmt, ...) APP_PR_LOG(LOG_INFO_LEVEL, fmt, ##__VA_ARGS__)
#define PR_DEBUG(fmt, ...) APP_PR_LOG(LOG_DEBUG_LEVLE, fmt, ##__VA_ARGS__)

#define SYS_LOG_LEVEL LOG_DEBUG_LEVLE
void USART_Configuration(void);


#endif