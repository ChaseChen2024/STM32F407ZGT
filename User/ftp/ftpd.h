#ifndef _FTPD_H
#define _FTPD_H

#ifndef FTPD_DEBUG
#define FTPD_DEBUG LWIP_DBG_OFF
#endif

#define FTPD_PORT 21 // FTP服务器端口号
#define FTPD_PASV 1 // 是否允许使用PASV命令

// LIST命令使用的时区 (0: UTC时区, 1: C库时区--不考虑夏令时, 2: ftpd_set_timezone函数自定义的时区)
#define FTPD_SYSTEM_TIMEZONE 2

// 命令处理过程中的异常情况
#define FTPD_CMDSTEP_CONNFAILED 0x1000 // 连接建立失败
#define FTPD_CMDSTEP_CONNABORTED 0x2000 // 连接建立成功但异常中止
#define FTPD_CMDSTEP_CONNSHUTDOWN 0x4000 // 连接建立成功后被客户端关闭

// FTP客户端状态位
#if FTPD_PASV
#define FTPD_FLAG_PASSIVE 0x01 // 当前是否为被动模式
#endif
#define FTPD_FLAG_CLOSE 0x02 // 收到客户端的TCP第一次挥手后, 请求发送第三次挥手
#define FTPD_FLAG_SHUTDOWN 0x04 // 请求发送TCP第一次挥手, 然后接收客户端第三次挥手
#define FTPD_FLAG_RENAME 0x08 // 是否正在重命名文件
#define FTPD_FLAG_AGAIN 0x10 // 当前FTP命令还没有执行完毕, 控制连接上的数据发送完毕后应继续回来处理
#define FTPD_FLAG_NEWDATACONN 0x20 // 数据连接已创建但还未连接上
#define FTPD_FLAG_TCPERROR 0x40 // TCP发送数据出错

// 数据连接关闭方式
#define FTPD_FREEDATA_ABORT 0 // 强行中止数据连接
#define FTPD_FREEDATA_CLOSE 1 // 关闭数据连接 (客户端已关闭)
#define FTPD_FREEDATA_SHUTDOWN 2 // 关闭数据连接 (客户端未关闭)

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

struct ftpd_user
{
  char *name;
  char *password;
};

struct ftpd_account
{
  struct ftpd_user user;
  char *rootpath;
};

#ifdef FF_DEFINED
struct ftpd_state
{
  struct tcp_pcb *ctrlconn;
  struct tcp_pcb *dataconn;
  int dataport;
  
  char cmd[MAX_PATH + 20];
  int cmdlen;
  char *cmdarg;
  int cmdstep;
  char last;

  char type;
  char path[MAX_PATH];
  char rename[MAX_PATH];
  uint64_t rest;
  
  struct ftpd_user user;
  int userid;
  int flags;
  
  int sent; // 未收到确认的已发送字节数
  struct pbuf *queue; // 数据接收队列
  
  void *dataout;
  int dataout_len;
  DIR *dp;
  FIL *fp;
  FILINFO *finfo;
};
#else
struct ftpd_state;
#endif

int ftpd_concat_path(char *buffer, int bufsize, const char *filename);
int ftpd_file_exists(const char *path);
#ifdef FF_DEFINED
time_t ftpd_filetime(WORD fdate, WORD ftime, struct tm *ptm);
#endif
int ftpd_fullpath(const struct ftpd_state *state, char *buffer, int bufsize, const char *filename, char **puserpath);
#if FTPD_SYSTEM_TIMEZONE == 2
time_t ftpd_get_timezone(void);
#endif
time_t ftpd_get_timezone_libc(void);
int ftpd_init(void);
void *ftpd_memrchr(const void *s, int c, size_t n);
#if FTPD_SYSTEM_TIMEZONE == 2
void ftpd_set_timezone(time_t timezone);
#endif
int ftpd_simplify_path(char *path, int basepos);
char *ftpd_strdup(const char *s);

#endif
