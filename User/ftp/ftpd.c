/*************************** 基于LwIP raw API的FTP服务器*****************************
** 注意事项:
** 1. 若想要移动文件, 可在文件管理器中使用"xxx/", "../"这样的语法重命名文件
**    例如想要把"abc.txt"移动到当前目录的123目录下, 则可以将文件重命名为"123/abc.txt"
**    将"def.doc"移动到父目录的456文件夹下, 则应该重命名为"../456/def.doc"
** 2. 服务器使用FatFs读写磁盘文件
**    如果出现HardFault错误, 则可能是startup_stm32*.s启动文件里面的Stack_Size值太小
**    将其改大就可以解决问题
************************************************************************************/
#include <ff.h>
#include <lwip/tcp.h>
#include <string.h>
#include <time.h>
#include "ftpd.h"

#if !MEM_LIBC_MALLOC && !MEM_USE_POOLS && MEM_SIZE < 10240L
#error "MEM_SIZE is too small to run FTP server"
#endif

static err_t ftpd_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static void ftpd_change_user(struct ftpd_state *state, const char *newuser);
static int ftpd_copy_cmd(struct ftpd_state *state);
#if FTPD_PASV
static err_t ftpd_data_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
#endif
static void ftpd_data_check(struct ftpd_state *state);
static err_t ftpd_data_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
static void ftpd_data_err(void *arg, err_t err);
static err_t ftpd_data_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t ftpd_data_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static err_t ftpd_data_sent_list(void *arg, struct tcp_pcb *tpcb, u16_t len);
static err_t ftpd_data_sent_retr(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void ftpd_err(void *arg, err_t err);
static void ftpd_free(struct ftpd_state *state);
static err_t ftpd_free_data(struct ftpd_state *state, int option);
static int ftpd_is_valid_user(struct ftpd_user *user, int *pid);
static int ftpd_prepare_data(struct ftpd_state *state);
static void ftpd_process_cmd(struct ftpd_state *state);
static int ftpd_process_data_cmd(struct ftpd_state *state);
static int ftpd_process_directory_cmd(struct ftpd_state *state);
static int ftpd_process_file_cmd(struct ftpd_state *state);
static int ftpd_process_opt_cmd(struct ftpd_state *state);
static int ftpd_process_user_cmd(struct ftpd_state *state);
static err_t ftpd_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static int ftpd_send_msg(struct ftpd_state *state, const char *s);
static err_t ftpd_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);

// 用户列表以及对应的根目录
static const struct ftpd_account ftpd_users[] = {
  {{"anonymous", NULL}, "1:/public"}, // 匿名用户
  {{"admin", "123456"}, "1:/"},
  {{"test", "789123"}, "1:/test"}
};
// 盘符可在ffconf.h中的FF_VOLUME_STRS处指定

static struct tcp_pcb *ftpd_tpcb;
#if FTPD_SYSTEM_TIMEZONE == 2
static time_t ftpd_timezone;
#endif

/* 控制连接收到新请求 */
static err_t ftpd_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  struct ftpd_state *state;
  
  if (err != ERR_OK)
  {
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("FTPD failed to accept a client. err=%d\n", err));
    return err;
  }
  
  state = mem_malloc(sizeof(struct ftpd_state));
  if (state == NULL)
  {
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("FTPD failed to accept [%s]:%d\n", ipaddr_ntoa(&newpcb->remote_ip), newpcb->remote_port));
    tcp_abort(newpcb);
    return ERR_ABRT;
  }
  
  memset(state, 0, sizeof(struct ftpd_state));
  state->ctrlconn = newpcb;
  state->dataport = -1;
  state->type = 'A';
  strcpy(state->path, "/");
  state->userid = -1;
  
  LWIP_DEBUGF(FTPD_DEBUG, ("FTPD accepted [%s]:%d\n", ipaddr_ntoa(&newpcb->remote_ip), newpcb->remote_port));
  ftpd_send_msg(state, "220 LwIP FTP Service\r\n");
  
  tcp_arg(newpcb, state);
  tcp_err(newpcb, ftpd_err);
  tcp_recv(newpcb, ftpd_recv);
  tcp_sent(newpcb, ftpd_sent);
  
  return ERR_OK;
}

/* 改变用户名并清空密码 */
static void ftpd_change_user(struct ftpd_state *state, const char *newuser)
{
  if (state->user.name != NULL)
  {
    mem_free(state->user.name);
    state->user.name = NULL;
  }
  if (state->user.password != NULL)
  {
    mem_free(state->user.password);
    state->user.password = NULL;
  }

  if (newuser != NULL)
    state->user.name = ftpd_strdup(newuser);
}

/* 将文件夹和文件名连接在一起形成新路径 */
// 将buffer和filename连接起来, 保存到buffer中, 同时保证字符串末尾不带斜杠(根目录除外); buffer的最大容量为bufsize
// 成功时返回字符串的长度; 失败时返回-1且buffer中的内容不变
int ftpd_concat_path(char *buffer, int bufsize, const char *filename)
{
  char *p;
  int addslash, fileabs, folderlen, namelen, len;

  // 找出字符串的连接位置, 并去掉buffer的尾斜杠和filename的首斜杠
  if (filename != NULL && filename[0] == '/')
  {
    // 如果文件名是绝对路径, 则需要把文件夹路径改为根目录
    fileabs = 1;
    filename++; // 去掉首斜杠

    if (buffer[0] == '/')
      folderlen = 1; // 文件夹路径不带盘符时只保留根目录符号 (首斜杠)
    else
    {
      p = strchr(buffer, ':');
      if (p != NULL)
        folderlen = p + 1 - buffer; // 文件夹路径带盘符时只保留盘符
      else
        folderlen = 0; // 如果buffer是相对路径, 清空字符串
    }
  }
  else
  {
    // 如果文件名不是绝对路径, 则可以直接在文件夹路径末尾连接上文件名
    fileabs = 0;
    folderlen = strlen(buffer);
    if (folderlen > 1 && buffer[folderlen - 1] == '/')
      folderlen--;
  }

  // 去掉filename的尾斜杠
  if (filename != NULL)
    namelen = strlen(filename);
  else
    namelen = 0;
  if (namelen != 0 && filename[namelen - 1] == '/')
    namelen--;

  // 计算字符串连接在一起后需要的缓冲区大小
  if (folderlen == 0)
    addslash = fileabs; // 路径为空时加不加斜杠取决于文件名是不是绝对路径
  else if (folderlen == 1 && buffer[0] == '/')
    addslash = 0; // 路径为斜杠时不加斜杠
  else if (folderlen != 0 && buffer[folderlen - 1] == ':')
    addslash = 1; // 路径最后一个字符为冒号时要加斜杠
  else if (namelen == 0)
    addslash = 0; // 文件名为空时不加斜杠
  else
    addslash = 1; // 其他情况都要加斜杠
  len = folderlen + addslash + namelen; // 连接后的长度
  if (len >= bufsize)
    return -1; // 缓冲区不够

  // 连接字符串
  if (addslash)
    buffer[folderlen] = '/';
  if (namelen != 0)
    memcpy(buffer + folderlen + addslash, filename, namelen);
  buffer[len] = '\0';
  return len;
}

/* 将数据接收队列queue中的FTP命令字符串提取到state->cmd中, 并释放占用的pbuf内存 */
// 返回值: 0表示还没有收到完整命令; 1表示收到了完整命令; 2表示收到了完整命令, 但超过了缓冲区最大长度
// state->cmdlen表示已收到了当前命令多少个字符 (包括\r\n)
static int ftpd_copy_cmd(struct ftpd_state *state)
{
  char *c;
  int complete = 0; // 是否收到完整命令
  int cnt = 0; // 本次复制的字符数
  int i;
  struct pbuf *p;
  
  for (p = state->queue; p != NULL && complete == 0; p = p->next)
  {
    c = p->payload;
    for (i = 0; i < p->len && complete == 0; i++)
    {
      if (state->last == '\r' && *c == '\n')
      {
        if (state->cmdlen <= sizeof(state->cmd))
        {
          state->cmd[state->cmdlen - 1] = '\0'; // 把\r替换成\0
          complete = 1;
        }
        else
          complete = 2;
      }
      else
      {
        if (state->cmdlen < sizeof(state->cmd))
          state->cmd[state->cmdlen] = *c;
      }
      
      state->cmdlen++;
      state->last = *c;
      c++;
      cnt++;
    }
  }
  
  state->queue = pbuf_free_header(state->queue, cnt);
  return complete;
}

#if FTPD_PASV
/* 数据连接被动模式连接建立成功 */
static err_t ftpd_data_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  struct ftpd_state *state = arg;
  
  if (err != ERR_OK)
  {
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("FTPD data failed to accept a client. err=%d\n", err));
    return err;
  }
  
  if (!ip_addr_cmp(&newpcb->remote_ip, &state->ctrlconn->remote_ip))
  {
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_WARNING, ("%s: IP address mismatch\n", __FUNCTION__));
    tcp_abort(newpcb);
    return ERR_ABRT;
  }
  
  tcp_close(state->dataconn); // 关闭端口监听
  state->dataconn = newpcb;
  tcp_err(newpcb, ftpd_data_err);
  return ftpd_data_connected(arg, newpcb, err);
}
#endif

/* 检查数据连接是否未开始发送数据 */
// 这个函数应该在控制连接发送完开始信息后调用一次
static void ftpd_data_check(struct ftpd_state *state)
{
#if FTPD_PASV
  if (state->flags & FTPD_FLAG_PASSIVE)
  {
    // 在PASV模式下, 连接可能会在PASV命令执行完毕的时候就建立成功
    // 但必须要等到数据传输命令(如LIST命令)的响应(如150响应)发送完毕后, 才能开始发送数据
    if ((state->flags & FTPD_FLAG_NEWDATACONN) == 0)
      ftpd_data_sent(state, state->dataconn, 0);
    // PORT模式下不存在这个问题, 因为连接建立后就可以立即开始发送数据
  }
#endif
}

/* 数据连接主动模式建立成功 */
static err_t ftpd_data_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  struct ftpd_state *state = arg;
  
  LWIP_DEBUGF(FTPD_DEBUG, ("FTPD data connection to [%s]:%d is established\n", ipaddr_ntoa(&tpcb->remote_ip), tpcb->remote_port));
  state->flags &= ~FTPD_FLAG_NEWDATACONN;
  
  tcp_recv(tpcb, ftpd_data_recv);
  tcp_sent(tpcb, ftpd_data_sent);
  
  return ftpd_data_sent(arg, tpcb, 0);
}

/* 数据连接出错 */
static void ftpd_data_err(void *arg, err_t err)
{
  struct ftpd_state *state = arg;
  
  LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_WARNING, ("FTPD data error. err=%d\n", err));
  if (state != NULL)
  {
    state->dataconn = NULL; // 调用err回调函数时, tpcb已经被LwIP释放了, 所以不需要再次释放
    ftpd_free_data(state, FTPD_FREEDATA_ABORT);
    
    if (state->flags & FTPD_FLAG_NEWDATACONN)
    {
      state->flags &= ~FTPD_FLAG_NEWDATACONN;
      state->cmdstep |= FTPD_CMDSTEP_CONNFAILED;
      ftpd_send_msg(state, "425 Failed to establish connection.\r\n");
    }
    else
    {
      state->cmdstep |= FTPD_CMDSTEP_CONNABORTED;
      ftpd_process_cmd(state);
    }
  }
}

/* 数据连接收到数据 */
static err_t ftpd_data_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct ftpd_state *state = arg;
  struct pbuf *q;
  FRESULT fr;
  UINT bw;
  
  if (p != NULL)
  {
    if (state != NULL)
    {
      if (strcasecmp(state->cmd, "STOR") == 0 || strcasecmp(state->cmd, "APPE") == 0)
      {
        LWIP_DEBUGF(FTPD_DEBUG, ("%s: %d bytes received\n", __FUNCTION__, p->tot_len));
        for (q = p; q != NULL; q = q->next)
        {
          fr = f_write(state->fp, q->payload, q->len, &bw);
          if (bw != q->len)
          {
            LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: f_write() failed. fr=%d, q->len=%u, bw=%u\n", __FUNCTION__, fr, q->len, bw));
            pbuf_free(p);
            
            err = ftpd_free_data(state, FTPD_FREEDATA_ABORT);
            state->cmdstep = FTPD_CMDSTEP_CONNABORTED;
            ftpd_process_cmd(state);
            return err;
          }
        }
      }
    }
    
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
  }
  else
  {
    if (state != NULL)
    {
      LWIP_DEBUGF(FTPD_DEBUG, ("FTPD data connection [%s]:%d is shutdown by the client\n", ipaddr_ntoa(&tpcb->remote_ip), tpcb->remote_port));
      ftpd_free_data(state, FTPD_FREEDATA_CLOSE);
      
      // 通知命令处理函数, 数据连接已被客户端关闭
      state->cmdstep |= FTPD_CMDSTEP_CONNSHUTDOWN;
      ftpd_process_cmd(state);
    }
    else
      LWIP_DEBUGF(FTPD_DEBUG, ("FTPD data connection [%s]:%d is closed by the client\n", ipaddr_ntoa(&tpcb->remote_ip), tpcb->remote_port));
  }
  return ERR_OK;
}

static err_t ftpd_data_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  err_t err = ERR_OK;
  struct ftpd_state *state = arg;
  
  if (state != NULL)
  {
    if (strcasecmp(state->cmd, "LIST") == 0)
      err = ftpd_data_sent_list(arg, tpcb, len);
    else if (strcasecmp(state->cmd, "RETR") == 0)
      err = ftpd_data_sent_retr(arg, tpcb, len);
  }
  return err;
}

/* 发送文件列表 */
static err_t ftpd_data_sent_list(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  char buffer[MAX_PATH + 100];
  err_t err;
  int bufsize, loop, slen;
  struct ftpd_state *state = arg;
  struct tm tm;
  time_t t;
  FRESULT fr;
  
  if (state->finfo == NULL)
  {
    loop = 2;
    state->finfo = mem_malloc(sizeof(FILINFO));
    if (state->finfo == NULL)
    {
      LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: mem_malloc() failed\n", __FUNCTION__));
      goto err;
    }
  }
  else
    loop = 1;
  
  while (loop)
  {
    if (loop == 2)
    {
      // 读取下一个文件的信息
      fr = f_readdir(state->dp, state->finfo);
      if (fr != FR_OK || state->finfo->fname[0] == '\0')
      {
        if (fr != FR_OK)
          LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: f_readdir() failed. fr=%d\n", __FUNCTION__, fr)); // 读取文件信息失败
        
        // 列表发送完毕
        ftpd_free_data(state, FTPD_FREEDATA_SHUTDOWN);
        state->cmdstep = 2;
        ftpd_process_cmd(state);
        break;
      }
    }
    
    if (strcmp(state->finfo->fname, ".") == 0 || strcmp(state->finfo->fname, "..") == 0)
    {
      LWIP_DEBUGF(FTPD_DEBUG, ("%s: jumping over \"%s\"\n", __FUNCTION__, state->finfo->fname));
      continue;
    }
    
    // LIST命令必须输出带时区的时间 (如UTC+8)
    t = ftpd_filetime(state->finfo->fdate, state->finfo->ftime, &tm); // fdate+ftime(UTC) -> t(UTC), tm(UTC)
#if FTPD_SYSTEM_TIMEZONE == 0
    LWIP_UNUSED_ARG(t);
#elif FTPD_SYSTEM_TIMEZONE == 1
    localtime_r(&t, &tm); // t(UTC) -> tm(UTC+8)
#elif FTPD_SYSTEM_TIMEZONE == 2
    t += ftpd_get_timezone(); // t(UTC) -> t(UTC+8)
    gmtime_r(&t, &tm); // t(UTC+8) -> tm(UTC+8)
#endif
    slen = strftime(buffer, sizeof(buffer), "%m-%d-%Y  %I:%M%p       ", &tm);
    if (state->finfo->fattrib & AM_DIR)
      strcpy(buffer + slen, "<DIR>          ");
    else
      sprintf(buffer + slen, "%14llu ", (uint64_t)state->finfo->fsize);
    slen += 15;
    slen += sprintf(buffer + slen, "%s\r\n", state->finfo->fname);
    LWIP_ASSERT("slen < sizeof(buffer)", slen < sizeof(buffer));
    
    bufsize = tcp_sndbuf(tpcb);
    if (bufsize >= slen)
    {
      LWIP_DEBUGF(FTPD_DEBUG, ("%s", buffer));
      err = tcp_write(tpcb, buffer, slen, TCP_WRITE_FLAG_COPY);
      if (err != ERR_OK)
      {
        LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: tcp_write() failed. err=%d\n", __FUNCTION__, err));
        goto err;
      }
      loop = 2;
    }
    else
    {
      // TCP滑动窗口不够了, 暂时退出, 等待前面的数据发送完毕
      LWIP_DEBUGF(FTPD_DEBUG, ("%s: paused. sndbuf=%d, slen=%d\n", __FUNCTION__, bufsize, slen));
      loop = 0;
    }
  }
  return ERR_OK;

err:
  err = ftpd_free_data(state, FTPD_FREEDATA_ABORT);
  state->cmdstep = FTPD_CMDSTEP_CONNABORTED;
  ftpd_process_cmd(state);
  return err;
}

/* 发送文件内容 */
static err_t ftpd_data_sent_retr(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  char buffer[30];
  err_t err;
  struct ftpd_state *state = arg;
  unsigned int size;
  FRESULT fr;
  UINT br;
  
  // 等待上一段数据发送完毕
  state->dataout_len -= len;
  if (state->dataout_len != 0)
    return ERR_OK;
  
  // 释放上一段数据占用的内存
  if (state->dataout != NULL)
  {
    mem_free(state->dataout);
    state->dataout = NULL;
  }
  
  size = tcp_sndbuf(tpcb);
  LWIP_ASSERT("sndbuf != 0", size != 0);
  state->dataout = mem_malloc(size);
  if (state->dataout == NULL)
  {
    // 内存分配失败时改用buffer缓冲区
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: mem_malloc() failed\n", __FUNCTION__));
    if (size > sizeof(buffer))
      size = sizeof(buffer);
  }
  
  if (state->dataout != NULL)
    fr = f_read(state->fp, state->dataout, size, &br);
  else
    fr = f_read(state->fp, buffer, size, &br);
  if (fr != FR_OK)
  {
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: failed to read file. fr=%d\n", __FUNCTION__, fr));
    goto err;
  }
  if (br < size)
    size = br;
  
  if (size > 0)
  {
    state->dataout_len = size;
    if (state->dataout != NULL)
      err = tcp_write(tpcb, state->dataout, size, 0);
    else
      err = tcp_write(tpcb, buffer, size, TCP_WRITE_FLAG_COPY);
    
    if (err != ERR_OK)
    {
      LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: tcp_write() failed. err=%d\n", __FUNCTION__, err));
      goto err;
    }
  }
  
  if (f_eof(state->fp))
  {
    // 文件发送完毕
    ftpd_free_data(state, FTPD_FREEDATA_SHUTDOWN);
    state->cmdstep = 2;
    ftpd_process_cmd(state);
  }
  return ERR_OK;

err:
  err = ftpd_free_data(state, FTPD_FREEDATA_ABORT);
  state->cmdstep = FTPD_CMDSTEP_CONNABORTED;
  ftpd_process_cmd(state);
  return err;
}

/* 控制连接出错 */
static void ftpd_err(void *arg, err_t err)
{
  struct ftpd_state *state = arg;
  
  LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_WARNING, ("FTPD error. err=%d\n", err));
  if (state != NULL)
  {
    state->ctrlconn = NULL;
    ftpd_free(state);
  }
}

/* 判断指定文件是否存在 */
// 如果是判断文件夹是否存在, 则字符串末尾不能有斜杠
int ftpd_file_exists(const char *path)
{
  FRESULT fr;
  
  if (strcmp(path + 1, ":") == 0 || strcmp(path + 1, ":/") == 0)
    return 1;
  fr = f_stat(path, NULL);
  return fr == FR_OK;
}

/* 将文件时间转换为C标准格式 */
// 在STM32中, time_t是32位的无符号整数 (unsigned int)
// 因为没有符号位, 所以time_t支持超过2038年的年份, 可以放心使用
// 输出参数ptm和返回值time_t都是UTC时间
time_t ftpd_filetime(WORD fdate, WORD ftime, struct tm *ptm)
{
  time_t t;
  
  memset(ptm, 0, sizeof(struct tm));
  ptm->tm_year = ((fdate >> 9) & 0x7f) + 80;
  ptm->tm_mon = ((fdate >> 5) & 0x0f) - 1;
  ptm->tm_mday = fdate & 0x1f;
  ptm->tm_hour = (ftime >> 11) & 0x1f;
  ptm->tm_min = (ftime >> 5) & 0x3f;
  ptm->tm_sec = (ftime & 0x1f) << 1;
  
  // 如果输出了错误的日期, 那么Windows的文件管理器会错误显示快捷方式图标
  // mktime函数能自动修正ptm结构体中有误的值
  // mktime函数是把带时区的struct tm转换为不带时区的time_t，也就是mktime(UTC+8)=UTC
  t = mktime(ptm); // UTC-8=mktime(UTC)
  t += ftpd_get_timezone_libc(); // (UTC-8)+8=UTC
  return t;
}

/* 关闭FTP控制连接和数据连接, 释放state结构体以及里面的成员占用的内存 */
static void ftpd_free(struct ftpd_state *state)
{
  if (state == NULL)
    return;
  
  ftpd_free_data(state, FTPD_FREEDATA_ABORT); // 如果数据连接尚未关闭, 则强行中止
  if (state->ctrlconn != NULL)
  {
    if (state->flags & FTPD_FLAG_CLOSE)
      LWIP_DEBUGF(FTPD_DEBUG, ("FTPD connection [%s]:%d is closed by the server\n", ipaddr_ntoa(&state->ctrlconn->remote_ip), state->ctrlconn->remote_port));
    else
      LWIP_DEBUGF(FTPD_DEBUG, ("FTPD connection [%s]:%d is shutdown by the server\n", ipaddr_ntoa(&state->ctrlconn->remote_ip), state->ctrlconn->remote_port));
    
    tcp_arg(state->ctrlconn, NULL);
    tcp_close(state->ctrlconn);
    state->ctrlconn = NULL;
  }
  
  ftpd_change_user(state, NULL);
  mem_free(state);
}

/* 关闭FTP数据连接并释放相关内存 */
// 关闭连接时通常将option设为FTPD_FREEDATA_SHUTDOWN
// 只有在ftpd_data_recv(p=NULL)中才使用FTPD_FREEDATA_CLOSE
static err_t ftpd_free_data(struct ftpd_state *state, int option)
{
  err_t err = ERR_OK;
  
  if (state == NULL)
    return err;
  
  state->dataport = -1;
  if (state->dataconn != NULL)
  {
    tcp_arg(state->dataconn, NULL); // 连接关闭后, 回调函数仍有可能触发, 所以必须和state彻底脱离关系
#if FTPD_PASV
    if ((state->flags & FTPD_FLAG_PASSIVE) && (state->flags & FTPD_FLAG_NEWDATACONN))
    {
      LWIP_DEBUGF(FTPD_DEBUG, ("FTPD data pcb is removed\n"));
      tcp_close(state->dataconn);
    }
    else
    {
#endif
      if (option == FTPD_FREEDATA_ABORT)
      {
        LWIP_DEBUGF(FTPD_DEBUG, ("FTPD data connection [%s]:%d is aborted\n", ipaddr_ntoa(&state->dataconn->remote_ip), state->dataconn->remote_port));
        tcp_abort(state->dataconn);
        err = ERR_ABRT;
      }
      else
      {
        if (option == FTPD_FREEDATA_CLOSE)
          LWIP_DEBUGF(FTPD_DEBUG, ("FTPD data connection [%s]:%d is closed by the server\n", ipaddr_ntoa(&state->dataconn->remote_ip), state->dataconn->remote_port));
        else
          LWIP_DEBUGF(FTPD_DEBUG, ("FTPD data connection [%s]:%d is shutdown by the server\n", ipaddr_ntoa(&state->dataconn->remote_ip), state->dataconn->remote_port));
        tcp_close(state->dataconn);
      }
#if FTPD_PASV
    }
#endif
    state->dataconn = NULL;
  }
  
  if (state->dataout != NULL)
  {
    mem_free(state->dataout);
    state->dataout = NULL;
    state->dataout_len = 0;
  }
  
  // 只有文件夹成功打开了之后, 才可以将指针赋给state->dp
  if (state->dp != NULL)
  {
    f_closedir(state->dp);
    mem_free(state->dp);
    state->dp = NULL;
  }
  
  // 只有文件成功打开了之后, 才可以将指针赋给state->fp
  if (state->fp != NULL)
  {
    f_close(state->fp);
    mem_free(state->fp);
    state->fp = NULL;
  }
  
  if (state->finfo != NULL)
  {
    mem_free(state->finfo);
    state->finfo = NULL;
  }
  return err;
}

/* 将用户根文件夹路径(rootpath)、当前文件夹路径(state->path)和文件名(filename)连接起来, 放入buffer缓冲区中 */
// buffer的原有内容会被忽略并清空, bufsize为缓冲区的大小
// puserpath为输出参数, 其内容是以用户文件夹为根目录的文件路径
int ftpd_fullpath(const struct ftpd_state *state, char *buffer, int bufsize, const char *filename, char **puserpath)
{
  int basepos, ret;

  // 在缓冲区中准备好用户文件夹的路径
  if (state->userid == -1)
    return -1; // 未登录, 连接失败
  basepos = strlen(ftpd_users[state->userid].rootpath);
  if (basepos + 1 > bufsize)
    return -1; // 缓冲区不够
  strcpy(buffer, ftpd_users[state->userid].rootpath);

  // 获取相对于用户文件夹的文件路径
  if (buffer[basepos - 1] == '/')
    basepos--; // 使userpath的第一个字符为斜杠
               // 如果没有斜杠, 则userpath指向\0, 下面连接路径后可能会变成斜杠
  if (puserpath != NULL)
    *puserpath = buffer + basepos;

  // 连接state->path字符串
  if (filename == NULL || filename[0] != '/')
  {
    // filename不是以用户文件夹为根目录的绝对路径, 而是相对于state->path的相对路径
    // 需要将rootpath, state->path和filename这三个字符串连在一起
    // filename == NULL的情况可视为空字符串, 是相对路径
    LWIP_ASSERT("state->path[0] == '/'", state->path[0] == '/'); // state->path的首字符始终为斜杠
    ret = ftpd_concat_path(buffer, bufsize, state->path + 1);
    if (ret == -1)
      return -1;
  }
  else
  {
    // filename是以用户文件夹为根目录的绝对路径
    // 跳过斜杠字符, 只将rootpath和不带首斜杠的filename连起来
    filename++;
  }

  // 连接filename字符串
  ret = ftpd_concat_path(buffer, bufsize, filename);
  if (ret == -1)
    return -1;
  ret = ftpd_simplify_path(buffer, basepos);
  if (puserpath != NULL && **puserpath == '\0')
    *puserpath = "/"; // 如果最终结果就是用户根目录, 那么应该用正斜杠表示, 而不是空字符串
  return ret;
}

#if FTPD_SYSTEM_TIMEZONE == 2
/* 获取自定义时区 */
time_t ftpd_get_timezone(void)
{
  return ftpd_timezone;
}
#endif

/* 获取C库设置的时区 */
// C库mktime函数的参数是带时区的struct tm结构, 返回值为不带时区的time_t时间戳
// 本函数的作用是抵消掉mktime函数带来的时差
// 本函数不考虑夏令时, 不然太复杂了
time_t ftpd_get_timezone_libc(void)
{
  struct tm tm;
  time_t t = 0;
  
  gmtime_r(&t, &tm);
  t = mktime(&tm);
  return -t;
}

/* 判断输入的用户名和密码是否正确 */
static int ftpd_is_valid_user(struct ftpd_user *user, int *pid)
{
  int i;
  int n = LWIP_ARRAYSIZE(ftpd_users);

  if (user->name == NULL)
    return 0; // 未输入用户名

  for (i = 0; i < n; i++)
  {
    if (strcasecmp(user->name, ftpd_users[i].user.name) == 0)
    {
      if (pid != NULL)
        *pid = i;
      if (ftpd_users[i].user.password == NULL)
        return 1; // 任何密码都可以
      else if (user->password != NULL && strcmp(user->password, ftpd_users[i].user.password) == 0)
        return 1; // 密码正确
      else
        return 0; // 密码错误
    }
  }
  return 0; // 用户名不存在
}

/* 启动ftpd服务器 */
int ftpd_init(void)
{
  err_t err;
  struct tcp_pcb *temp;
  
  if (ftpd_tpcb != NULL)
  {
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_WARNING, ("%s: FTPD server is already started\n", __FUNCTION__));
    return -1;
  }
  
  temp = tcp_new();
  if (temp == NULL)
  {
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: tcp_new() failed\n", __FUNCTION__));
    return -1;
  }
  
  err = tcp_bind(temp, IP_ANY_TYPE, FTPD_PORT);
  if (err != ERR_OK)
  {
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: tcp_bind() failed. err=%d\n", __FUNCTION__, err));
    tcp_close(temp);
    return -1;
  }
  
  ftpd_tpcb = tcp_listen(temp);
  if (ftpd_tpcb == NULL)
  {
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: tcp_listen() failed\n", __FUNCTION__));
    tcp_close(temp);
    return -1;
  }
  temp = NULL;
  
  tcp_accept(ftpd_tpcb, ftpd_accept);
  return 0;
}

/* memchr的反向版本 */
void *ftpd_memrchr(const void *s, int c, size_t n)
{
  const char *p = s;
  int i;

  for (i = n - 1; i >= 0; i--)
  {
    if (p[i] == c)
      return (void *)&p[i];
  }
  return NULL;
}

/* 准备好数据连接 */
// 若函数返回-1, 则表示连接建立失败, 此时已发送了425消息, 不用再发送其他错误消息
static int ftpd_prepare_data(struct ftpd_state *state)
{
  err_t err;
  int ret = -1;
  
  if (state->dataport == -1)
  {
#if LWIP_IPV6
    if (IP_IS_V4_VAL(state->ctrlconn->remote_ip))
#endif
      ftpd_send_msg(state, "425 Use PORT or PASV first.\r\n");
#if LWIP_IPV6
    else if (IP_IS_V6_VAL(state->ctrlconn->remote_ip))
      ftpd_send_msg(state, "425 Use EPRT or EPSV first.\r\n");
#endif
    return -1;
  }
  
#if FTPD_PASV
  if (state->flags & FTPD_FLAG_PASSIVE)
  {
    LWIP_ASSERT("state->dataconn != NULL", state->dataconn != NULL);
    ret = 0;
  }
  else
  {
#endif
    LWIP_ASSERT("state->dataconn == NULL", state->dataconn == NULL);
    state->dataconn = tcp_new();
    if (state->dataconn == NULL)
    {
      LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: tcp_new() failed\n", __FUNCTION__));
      goto end;
    }
    
    tcp_arg(state->dataconn, state);
    err = tcp_connect(state->dataconn, &state->ctrlconn->remote_ip, state->dataport, ftpd_data_connected);
    if (err == ERR_OK)
    {
      // 使用PORT模式时, 最好将电脑的防火墙关闭, 以免板子连不上电脑而出错
      LWIP_DEBUGF(FTPD_DEBUG, ("FTPD is connecting to [%s]:%d...\n", ipaddr_ntoa(&state->ctrlconn->remote_ip), state->dataport));
      tcp_err(state->dataconn, ftpd_data_err);
      state->flags |= FTPD_FLAG_NEWDATACONN;
      ret = 0;
    }
    else
      LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: tcp_connect() failed. err=%d\n", __FUNCTION__, err));
#if FTPD_PASV
  }
#endif

end:
  if (ret == -1)
  {
    ftpd_send_msg(state, "425 Failed to establish connection.\r\n");
    if (state->dataconn != NULL)
    {
      tcp_arg(state->dataconn, NULL);
      tcp_close(state->dataconn);
      state->dataconn = NULL;
      state->dataport = -1;
    }
  }
  return ret;
}

/* 处理命令 */
static void ftpd_process_cmd(struct ftpd_state *state)
{
  int ret;
  
  // 只有当上一个命令的所有回应都发送完毕时, 才开始处理下一条命令
  if (state->sent != 0)
    return;
  else if (state->flags & FTPD_FLAG_TCPERROR)
    goto end;
  
  if ((state->flags & FTPD_FLAG_AGAIN) == 0)
  {
    // 上一条命令已执行完毕
    if (state->flags & (FTPD_FLAG_CLOSE | FTPD_FLAG_SHUTDOWN))
    {
      ftpd_free(state);
      return;
    }
    
    // 从接收队列中取出一条新命令
    ret = ftpd_copy_cmd(state);
    if (ret == 0)
      return; // 命令不完整
    LWIP_DEBUGF(FTPD_DEBUG, ("%s\n", state->cmd));
    state->cmdstep = 0; // 当前是命令的第一步操作
    
    if (ret == 2)
    {
      ftpd_send_msg(state, "500 Syntax error, command unrecognized.\r\n");
      goto end;
    }
    
    // 提取出命令参数
    state->cmdarg = strchr(state->cmd, ' ');
    if (state->cmdarg != NULL)
      *state->cmdarg++ = '\0';
    else
      state->cmdarg = "";
  }
  else
  {
    // 上一条命令未执行完毕, 虽然state->sent==0, 但还需要继续发送更多数据
    // cmd和cmdarg不变, 命令可根据cmdstep的值决定当前是第几步操作
    state->flags &= ~FTPD_FLAG_AGAIN;
  }
  
  // 处理各种命令
  if (ftpd_process_user_cmd(state)) // 这个必须第一个处理
    ;
  else if (ftpd_process_data_cmd(state))
    ;
  else if (ftpd_process_directory_cmd(state))
    ;
  else if (ftpd_process_file_cmd(state))
    ;
  else if (ftpd_process_opt_cmd(state))
    ;
  else
    ftpd_send_msg(state, "500 Unknown command.\r\n");

end:
  // TCP无法发送数据时, 强制关闭连接
  if (state->sent == 0 && (state->flags & FTPD_FLAG_TCPERROR))
  {
    state->flags = (state->flags & ~FTPD_FLAG_AGAIN) | FTPD_FLAG_SHUTDOWN;
    ftpd_free(state);
  }
}

/* 处理与数据连接有关的命令 */
static int ftpd_process_data_cmd(struct ftpd_state *state)
{
  char ip[IPADDR_STRLEN_MAX];
  int i, j, ret;
  int isport = 0;
  ip_addr_t ipaddr;
#if FTPD_PASV
  char buffer[100];
  err_t err;
  int ispasv = 0;
  struct tcp_pcb *newpcb;
#endif
  
#if LWIP_IPV6
  if (IP_IS_V4_VAL(state->ctrlconn->remote_ip))
  {
#endif
    if (strcasecmp(state->cmd, "PORT") == 0)
      isport = 4;
#if FTPD_PASV
    else if (strcasecmp(state->cmd, "PASV") == 0)
      ispasv = 4;
#endif
#if LWIP_IPV6
  }
  else if (IP_IS_V6_VAL(state->ctrlconn->remote_ip))
  {
    if (strcasecmp(state->cmd, "EPRT") == 0)
      isport = 6;
#if FTPD_PASV
    else if (strcasecmp(state->cmd, "EPSV") == 0)
      ispasv = 6;
#endif
  }
#endif
  
  if (isport)
  {
    // 如果之前启动了PASV模式, 则关闭创建的监听连接
    state->dataport = -1;
#if FTPD_PASV
    if (state->flags & FTPD_FLAG_PASSIVE)
    {
      state->flags &= ~FTPD_FLAG_PASSIVE;
      if (state->dataconn != NULL)
      {
        tcp_close(state->dataconn);
        state->dataconn = NULL;
      }
    }
#endif
    
    // 提取出IP地址
#if LWIP_IPV6
    if (isport == 4)
    {
#endif
      for (i = j = 0; i < sizeof(ip) && j < 4; i++)
      {
        if (isdigit(state->cmdarg[i]))
          ip[i] = state->cmdarg[i];
        else if (state->cmdarg[i] == ',')
        {
          ip[i] = '.';
          j++;
        }
        else
          break;
      }
      if (j != 4)
        goto porterr;
      ip[i - 1] = '\0';
#if LWIP_IPV6
    }
    else
    {
      if (memcmp(state->cmdarg, "|2|", 3) != 0)
        goto porterr;
      
      for (i = 0; i < sizeof(ip); i++)
      {
        if (state->cmdarg[3 + i] == '|')
          break;
        ip[i] = state->cmdarg[3 + i];
      }
      if (i == sizeof(ip))
        goto porterr;
      ip[i] = '\0';
    }
#endif
    
    ret = ipaddr_aton(ip, &ipaddr);
    if (ret == 0 || !ip_addr_cmp(&ipaddr, &state->ctrlconn->remote_ip))
      goto porterr;
    
    // 提取出端口号
#if LWIP_IPV6
    if (isport == 4)
    {
#endif
      ret = sscanf(state->cmdarg + i, "%d,%d", &i, &j);
      if (ret != 2)
        goto porterr;
      ret = i * 256 + j;
#if LWIP_IPV6
    }
    else
    {
      i = sscanf(state->cmdarg + 4 + i, "%d", &ret);
      if (i != 1)
        goto porterr;
    }
#endif
    
    if (ret != 0 && ret < 65536)
    {
      state->dataport = ret;
#if LWIP_IPV6
      if (isport == 4)
      {
#endif
#if FTPD_PASV
        ftpd_send_msg(state, "200 PORT command successful. Consider using PASV.\r\n");
#else
        ftpd_send_msg(state, "200 PORT command successful.\r\n");
#endif
#if LWIP_IPV6
      }
      else
        ftpd_send_msg(state, "200 EPRT command successful.\r\n");
#endif
      return 1;
    }
porterr:
#if LWIP_IPV6
    if (isport == 4)
#endif
      ftpd_send_msg(state, "500 Illegal PORT command.\r\n");
#if LWIP_IPV6
    else
      ftpd_send_msg(state, "500 Illegal EPRT command.\r\n");
#endif
  }
#if FTPD_PASV
  else if (ispasv)
  {
    if (state->dataconn == NULL)
    {
      state->dataconn = tcp_new();
      if (state->dataconn == NULL)
        goto pasverr;
      
#if LWIP_IPV6
      if (ispasv == 4)
#endif
        err = tcp_bind(state->dataconn, IP_ADDR_ANY, 0);
#if LWIP_IPV6
      else
        err = tcp_bind(state->dataconn, IP6_ADDR_ANY, 0);
#endif
      if (err != ERR_OK)
        goto pasverr;
      
      newpcb = tcp_listen(state->dataconn);
      if (newpcb == NULL)
        goto pasverr;
      
      state->dataconn = newpcb;
      tcp_arg(state->dataconn, state);
      tcp_accept(state->dataconn, ftpd_data_accept);
      
      state->dataport = state->dataconn->local_port;
      state->flags |= FTPD_FLAG_NEWDATACONN | FTPD_FLAG_PASSIVE;
    }
    
#if LWIP_IPV6
    if (ispasv == 4)
    {
#endif
      ipaddr_ntoa_r(&state->ctrlconn->local_ip, ip, sizeof(ip));
      for (i = 0; ip[i] != '\0'; i++)
      {
        if (ip[i] == '.')
          ip[i] = ',';
      }
      sprintf(buffer, "227 Entering Passive Mode (%s,%d,%d).\r\n", ip, (state->dataport >> 8) & 0xff, state->dataport & 0xff);
#if LWIP_IPV6
    }
    else
      sprintf(buffer, "229 Entering Extended Passive Mode (|||%d|).\r\n", state->dataport);
#endif
    ftpd_send_msg(state, buffer);
    return 1;
pasverr:
#if LWIP_IPV6
    if (ispasv == 4)
#endif
      ftpd_send_msg(state, "500 PASV command failed.\r\n");
#if LWIP_IPV6
    else
      ftpd_send_msg(state, "500 EPSV command failed.\r\n");
#endif
    if (state->dataconn != NULL)
    {
      tcp_close(state->dataconn);
      state->dataconn = NULL;
    }
  }
#endif
  else
    return 0;
  
  return 1;
}

static int ftpd_process_directory_cmd(struct ftpd_state *state)
{
  char buffer[MAX_PATH];
  char *path;
  int ret;
  DIR *dp = NULL;
  FRESULT fr;

  if (strcasecmp(state->cmd, "PWD") == 0)
  {
    ftpd_send_msg(state, "257 \"");
    ftpd_send_msg(state, state->path);
    ftpd_send_msg(state, "\" is the current directory.\r\n");
  }
  else if (strcasecmp(state->cmd, "CWD") == 0 || strcasecmp(state->cmd, "CDUP") == 0)
  {
    if (strcasecmp(state->cmd, "CWD") == 0)
      ret = ftpd_fullpath(state, buffer, MAX_PATH, state->cmdarg, &path);
    else
      ret = ftpd_fullpath(state, buffer, MAX_PATH, "..", &path);
    if (ret == -1)
      goto cwderr;
    else if (!ftpd_file_exists(buffer))
      goto cwderr;
    
    strcpy(state->path, path);
    ftpd_send_msg(state, "250 Directory successfully changed.\r\n");
    return 1;
cwderr:
    ftpd_send_msg(state, "550 Failed to change directory.\r\n");
  }
  else if (strcasecmp(state->cmd, "LIST") == 0)
  {
    if (state->cmdstep == 0)
    {
      ret = ftpd_fullpath(state, buffer, MAX_PATH, NULL, NULL);
      if (ret == -1)
        goto listerr;
      
      LWIP_ASSERT("state->dp == NULL", state->dp == NULL);
      dp = mem_malloc(sizeof(DIR));
      if (dp == NULL)
      {
        LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: mem_malloc(sizeof(DIR)) failed\n", __FUNCTION__));
        goto listerr;
      }
      
      fr = f_opendir(dp, buffer);
      if (fr != FR_OK)
        goto listerr;
      state->dp = dp; // 文件夹打开了之后才能赋给state->dp
      
      ret = ftpd_prepare_data(state);
      if (ret == -1)
        goto listerr2;
      
      state->cmdstep = 1;
      state->flags |= FTPD_FLAG_AGAIN;
      ftpd_send_msg(state, "150 Here comes the directory listing.\r\n");
      return 1;
listerr:
      state->cmdstep = FTPD_CMDSTEP_CONNABORTED;
listerr2:
      // 这里涉及到两个不同的操作
      // 一个是关闭文件夹, 另一个是释放存储文件夹信息的内存
      if (state->dp != NULL)
      {
        f_closedir(state->dp);
        state->dp = NULL;
      }
      if (dp != NULL)
      {
        mem_free(dp);
        dp = NULL;
      }
    }
    else if (state->cmdstep == 1)
    {
      state->flags |= FTPD_FLAG_AGAIN;
      ftpd_data_check(state);
    }
    else if (state->cmdstep == 2)
      ftpd_send_msg(state, "226 Directory send OK.\r\n");
    
    if (state->cmdstep & (FTPD_CMDSTEP_CONNABORTED | FTPD_CMDSTEP_CONNSHUTDOWN))
      ftpd_send_msg(state, "450 Failed to list the folder.\r\n");
  }
  else if (strcasecmp(state->cmd, "MKD") == 0)
  {
    ret = ftpd_fullpath(state, buffer, MAX_PATH, state->cmdarg, &path);
    if (ret != -1)
    {
      fr = f_mkdir(buffer);
      if (fr == FR_OK)
      {
        ftpd_send_msg(state, "257 \"");
        ftpd_send_msg(state, path);
        ftpd_send_msg(state, "\" created.\r\n");
        return 1;
      }
    }
    ftpd_send_msg(state, "550 Create directory operation failed.\r\n");
  }
  else if (strcasecmp(state->cmd, "RMD") == 0)
  {
    ret = ftpd_fullpath(state, buffer, MAX_PATH, state->cmdarg, NULL);
    if (ret != -1)
    {
      fr = f_unlink(buffer);
      if (fr == FR_OK)
      {
        ftpd_send_msg(state, "250 Remove directory operation successful.\r\n");
        return 1;
      }
    }
    ftpd_send_msg(state, "550 Remove directory operation failed.\r\n");
  }
  else
    return 0;
  return 1;
}

/* 处理与文件有关的命令 */
static int ftpd_process_file_cmd(struct ftpd_state *state)
{
  char buffer[MAX_PATH];
  int ret;
  struct tm tm;
  uint64_t size;
  FIL *fp = NULL;
  FILINFO* finfo = NULL;
  FRESULT fr;

  if (strcasecmp(state->cmd, "SIZE") == 0)
  {
    ret = ftpd_fullpath(state, buffer, MAX_PATH, state->cmdarg, NULL);
    if (ret == -1)
      goto sizeerr;
    
    fp = mem_malloc(sizeof(FIL));
    if (fp == NULL)
    {
      LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: mem_malloc(sizeof(FIL)) failed\n", __FUNCTION__));
      goto sizeerr;
    }
    
    fr = f_open(fp, buffer, FA_READ);
    if (fr != FR_OK)
    {
      LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: f_open() failed. fr=%d\n", __FUNCTION__, fr));
      goto sizeerr;
    }
    
    size = f_size(fp);
    f_close(fp);
    mem_free(fp);
    
    sprintf(buffer, "213 %llu\r\n", size);
    ftpd_send_msg(state, buffer);
    return 1;
sizeerr:
    ftpd_send_msg(state, "550 Could not get file size.\r\n");
    if (fp != NULL)
      mem_free(fp);
  }
  else if (strcasecmp(state->cmd, "RETR") == 0)
  {
    if (state->cmdstep == 0)
    {
      ret = ftpd_fullpath(state, buffer, MAX_PATH, state->cmdarg, NULL);
      if (ret == -1)
        goto retrerr;
      
      LWIP_ASSERT("state->fp == NULL", state->fp == NULL);
      fp = mem_malloc(sizeof(FIL));
      if (fp == NULL)
      {
        LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: mem_malloc(sizeof(FIL)) failed\n", __FUNCTION__));
        goto retrerr;
      }
      
      fr = f_open(fp, buffer, FA_READ);
      if (fr != FR_OK)
      {
        LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: f_open() failed. fr=%d\n", __FUNCTION__, fr));
        goto retrerr;
      }
      state->fp = fp; // 文件打开了之后才能赋给state->fp
      
      ret = ftpd_prepare_data(state);
      if (ret == -1)
        goto retrerr2;
      
      state->cmdstep = 1;
      state->flags |= FTPD_FLAG_AGAIN;
      
      sprintf(buffer, "150 Opening %s mode data connection for ", (state->type == 'I') ? "BINARY" : "ASCII");
      ftpd_send_msg(state, buffer);
      ftpd_send_msg(state, state->cmdarg);
      
      size = f_size(state->fp);
      sprintf(buffer, " (%llu bytes).\r\n", size);
      ftpd_send_msg(state, buffer);
      
      if (state->rest != 0)
      {
        f_lseek(state->fp, state->rest);
        state->rest = 0;
      }
      return 1;
retrerr:
      ftpd_send_msg(state, "550 Failed to open file.\r\n");
retrerr2:
      if (state->fp != NULL)
      {
        f_close(state->fp);
        state->fp = NULL;
      }
      if (fp != NULL)
      {
        mem_free(fp);
        fp = NULL;
      }
      state->rest = 0;
    }
    else if (state->cmdstep == 1)
    {
      state->flags |= FTPD_FLAG_AGAIN;
      ftpd_data_check(state);
    }
    else if (state->cmdstep == 2)
      ftpd_send_msg(state, "226 Transfer complete.\r\n");
    else if (state->cmdstep & (FTPD_CMDSTEP_CONNSHUTDOWN | FTPD_CMDSTEP_CONNABORTED))
      ftpd_send_msg(state, "451 Requested action aborted: local error in processing.\r\n");
  }
  else if (strcasecmp(state->cmd, "STOR") == 0 || strcasecmp(state->cmd, "APPE") == 0)
  {
    if (state->cmdstep == 0)
    {
      ret = ftpd_fullpath(state, buffer, MAX_PATH, state->cmdarg, NULL);
      if (ret == -1)
        goto storerr;
      
      LWIP_ASSERT("state->fp == NULL", state->fp == NULL);
      fp = mem_malloc(sizeof(FIL));
      if (fp == NULL)
      {
        LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: mem_malloc(sizeof(FIL)) failed\n", __FUNCTION__));
        goto storerr;
      }
      
      if (strcasecmp(state->cmd, "STOR") == 0)
        fr = f_open(fp, buffer, FA_CREATE_ALWAYS | FA_WRITE);
      else
        fr = f_open(fp, buffer, FA_OPEN_ALWAYS | FA_WRITE);
      if (fr != FR_OK)
      {
        LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: f_open() failed. fr=%d, path=\"%s\"\n", __FUNCTION__, fr, buffer));
        goto storerr;
      }
      state->fp = fp; // 文件打开了之后才能赋给state->fp
      
      ret = ftpd_prepare_data(state);
      if (ret == -1)
        goto storerr2;
      
      state->cmdstep = 1;
      state->flags |= FTPD_FLAG_AGAIN;
      ftpd_send_msg(state, "150 Ok to send data.\r\n");
      
      if (strcasecmp(state->cmd, "APPE") == 0 && state->rest != 0)
      {
        f_lseek(state->fp, state->rest);
        state->rest = 0;
      }
      return 1;
storerr:
      ftpd_send_msg(state, "550 Failed to open file.\r\n");
storerr2:
      if (state->fp != NULL)
      {
        f_close(state->fp);
        state->fp = NULL;
      }
      if (fp != NULL)
      {
        mem_free(fp);
        fp = NULL;
      }
      state->rest = 0;
    }
    else if (state->cmdstep == 1)
    {
      state->flags |= FTPD_FLAG_AGAIN;
      ftpd_data_check(state);
    }
    else if (state->cmdstep & FTPD_CMDSTEP_CONNSHUTDOWN)
      ftpd_send_msg(state, "226 Transfer complete.\r\n");
    else if (state->cmdstep & FTPD_CMDSTEP_CONNABORTED)
      ftpd_send_msg(state, "451 Requested action aborted: local error in processing.\r\n");
  }
  else if (strcasecmp(state->cmd, "DELE") == 0)
  {
    ret = ftpd_fullpath(state, buffer, MAX_PATH, state->cmdarg, NULL);
    if (ret != -1)
    {
      ret = f_unlink(buffer);
      if (ret == 0)
      {
        ftpd_send_msg(state, "250 Delete operation successful.\r\n");
        return 1;
      }
    }
    ftpd_send_msg(state, "550 Delete operation failed.\r\n");
  }
  else if (strcasecmp(state->cmd, "RNFR") == 0)
  {
    ret = ftpd_fullpath(state, state->rename, sizeof(state->rename), state->cmdarg, NULL);
    if (ret != -1)
    {
      state->flags |= FTPD_FLAG_RENAME;
      ftpd_send_msg(state, "350 Ready for RNTO.\r\n");
    }
    else
    {
      state->flags &= ~FTPD_FLAG_RENAME;
      ftpd_send_msg(state, "550 RNFR command failed.\r\n");
    }
  }
  else if (strcasecmp(state->cmd, "RNTO") == 0)
  {
    if ((state->flags & FTPD_FLAG_RENAME) == 0)
    {
      ftpd_send_msg(state, "503 RNFR required first.\r\n");
      return 1;
    }

    state->flags &= ~FTPD_FLAG_RENAME;
    ret = ftpd_fullpath(state, buffer, MAX_PATH, state->cmdarg, NULL);
    if (ret != -1)
    {
      ret = f_rename(state->rename, buffer);
      if (ret == 0)
      {
        ftpd_send_msg(state, "250 Rename successful.\r\n");
        return 1;
      }
    }
    ftpd_send_msg(state, "550 Rename failed.\r\n");
  }
  else if (strcasecmp(state->cmd, "REST") == 0)
  {
    state->rest = strtoull(state->cmdarg, NULL, 10);
    sprintf(buffer, "350 Restart position accepted (%llu).\r\n", state->rest);
    ftpd_send_msg(state, buffer);
  }
  else if (strcasecmp(state->cmd, "MDTM") == 0)
  {
    ret = ftpd_fullpath(state, buffer, MAX_PATH, state->cmdarg, NULL);
    if (ret == -1)
      goto mdtmerr;
      
    finfo = mem_malloc(sizeof(FILINFO));
    if (finfo == NULL)
    {
      LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: mem_malloc(sizeof(FILINFO)) failed\n", __FUNCTION__));
      goto mdtmerr;
    }
    
    fr = f_stat(buffer, finfo);
    if (fr != FR_OK)
    {
      LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: f_stat() failed. fr=%d\n", __FUNCTION__, fr));
      goto mdtmerr;
    }
    
    ftpd_filetime(finfo->fdate, finfo->ftime, &tm);
    mem_free(finfo);
    
    strftime(buffer, sizeof(buffer), "213 %Y%m%d%H%M%S\r\n", &tm); // MDTM命令必须输出UTC时间
    ftpd_send_msg(state, buffer);
    return 1;
mdtmerr:
    ftpd_send_msg(state, "550 Could not get modification time.\r\n");
    if (finfo != NULL)
      mem_free(finfo);
  }
  else
    return 0;
  return 1;
}

/* 处理与服务器选项有关的命令 */
static int ftpd_process_opt_cmd(struct ftpd_state *state)
{
  if (strcasecmp(state->cmd, "opts") == 0)
  {
    if (strcasecmp(state->cmdarg, "utf8 on") == 0)
      ftpd_send_msg(state, "200 Always in UTF8 mode.\r\n");
    else
      ftpd_send_msg(state, "501 Option not understood.\r\n");
  }
  else if (strcasecmp(state->cmd, "TYPE") == 0)
  {
    if (strcasecmp(state->cmdarg, "A") == 0)
      ftpd_send_msg(state, "200 Switching to ASCII mode.\r\n");
    else if (strcasecmp(state->cmdarg, "I") == 0)
      ftpd_send_msg(state, "200 Switching to Binary mode.\r\n");
    else
    {
      ftpd_send_msg(state, "500 Unrecognised TYPE command.\r\n");
      return 1;
    }
    state->type = state->cmdarg[0];
  }
  else if (strcasecmp(state->cmd, "noop") == 0)
    ftpd_send_msg(state, "200 NOOP ok.\r\n");
  else if (strcasecmp(state->cmd, "FEAT") == 0)
  {
    ftpd_send_msg(state, "211-Features:\r\n");
    ftpd_send_msg(state, " EPRT\r\n");
    ftpd_send_msg(state, " EPSV\r\n");
    ftpd_send_msg(state, " MDTM\r\n");
    ftpd_send_msg(state, " PASV\r\n");
    ftpd_send_msg(state, " REST STREAM\r\n");
    ftpd_send_msg(state, " SIZE\r\n");
    ftpd_send_msg(state, " UTF8\r\n");
    ftpd_send_msg(state, "211 End\r\n");
  }
  else if (strcasecmp(state->cmd, "SYST") == 0)
    ftpd_send_msg(state, "215 Windows_NT\r\n");
  else
    return 0;
  return 1;
}

/* 处理与用户有关的命令 */
static int ftpd_process_user_cmd(struct ftpd_state *state)
{
  int userid;
  
  if (strcasecmp(state->cmd, "USER") == 0)
  {
    if (state->userid != -1)
      ftpd_send_msg(state, "530 Can't change to another user.\r\n");
    else
    {
      ftpd_change_user(state, state->cmdarg);
      if (strcasecmp(state->cmdarg, "ANONYMOUS") == 0 && ftpd_is_valid_user(&state->user, NULL))
        ftpd_send_msg(state, "331 Anonymous access allowed, send identity (e-mail name) as password.\r\n");
      else
        ftpd_send_msg(state, "331 Please specify the password.\r\n");
    }
  }
  else if (strcasecmp(state->cmd, "PASS") == 0)
  {
    if (state->userid != -1)
      ftpd_send_msg(state, "230 Already logged in.\r\n");
    else if (state->user.name == NULL)
      ftpd_send_msg(state, "503 Login with USER first.\r\n");
    else
    {
      state->user.password = ftpd_strdup(state->cmdarg);
      if (ftpd_is_valid_user(&state->user, &userid))
      {
        if (ftpd_file_exists(ftpd_users[userid].rootpath))
        {
          state->userid = userid;
          ftpd_send_msg(state, "230 Login successful.\r\n");
        }
        else
        {
          ftpd_change_user(state, NULL);
          ftpd_send_msg(state, "530 Please create the home directory \"");
          ftpd_send_msg(state, ftpd_users[userid].rootpath);
          ftpd_send_msg(state, "\" before logging in.\r\n");
        }
      }
      else
      {
        ftpd_change_user(state, NULL);
        ftpd_send_msg(state, "530 Login incorrect.\r\n");
      }
    }
  }
  else if (strcasecmp(state->cmd, "QUIT") == 0)
  {
    ftpd_send_msg(state, "221 Goodbye.\r\n");
    state->flags |= FTPD_FLAG_SHUTDOWN;
  }
  else if (state->userid == -1)
    ftpd_send_msg(state, "530 Please login with USER and PASS.\r\n");
  else
    return 0;
  return 1;
}

static err_t ftpd_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct ftpd_state *state = arg;
  
  if (p != NULL)
  {
    LWIP_DEBUGF(FTPD_DEBUG, ("%s: received %d bytes\n", __FUNCTION__, p->tot_len));
    if (state->queue == NULL)
      state->queue = p;
    else
      pbuf_cat(state->queue, p);
  }
  else
  {
    if (state != NULL)
    {
      LWIP_DEBUGF(FTPD_DEBUG, ("FTPD connection [%s]:%d is shutdown by the client\n", ipaddr_ntoa(&tpcb->remote_ip), tpcb->remote_port));
      state->flags |= FTPD_FLAG_CLOSE;
    }
    else
    {
      LWIP_DEBUGF(FTPD_DEBUG, ("FTPD connection [%s]:%d is closed by the client\n", ipaddr_ntoa(&tpcb->remote_ip), tpcb->remote_port));
      return ERR_OK;
    }
  }
  
  ftpd_process_cmd(state);
  return ERR_OK;
}

/* 发送回应 */
static int ftpd_send_msg(struct ftpd_state *state, const char *s)
{
  err_t err;
  int len;
  
  if (state->flags & FTPD_FLAG_TCPERROR)
    return -1;
  
  len = strlen(s);
  LWIP_DEBUGF(FTPD_DEBUG, ("%s", s));
  LWIP_ASSERT("sndbuf >= len", tcp_sndbuf(state->ctrlconn) >= len);
  
  err = tcp_write(state->ctrlconn, s, len, TCP_WRITE_FLAG_COPY);
  if (err != ERR_OK)
  {
    state->flags |= FTPD_FLAG_TCPERROR;
    LWIP_DEBUGF(FTPD_DEBUG | LWIP_DBG_LEVEL_SERIOUS, ("%s: tcp_write() failed. err=%d\n", __FUNCTION__, err));
    return -1;
  }
  
  state->sent += len;
  return len;
}

static err_t ftpd_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct ftpd_state *state = arg;
  
  LWIP_DEBUGF(FTPD_DEBUG, ("%s: %d bytes of response sent\n", __FUNCTION__, len));
  if (state != NULL)
  {
    state->sent -= len;
    if (state->sent == 0)
    {
      if (state->cmdlen != 0)
      {
        LWIP_DEBUGF(FTPD_DEBUG, ("%s: processed %d bytes\n", __FUNCTION__, state->cmdlen));
        tcp_recved(state->ctrlconn, state->cmdlen);
        state->cmdlen = 0;
      }
      ftpd_process_cmd(state);
    }
  }
  
  return ERR_OK;
}

#if FTPD_SYSTEM_TIMEZONE == 2
/* 设置自定义时区 */
void ftpd_set_timezone(time_t timezone)
{
  ftpd_timezone = timezone;
}
#endif

/* 去除路径中的"./"和"../"以及"//" */
// path必须为绝对路径 (可以带盘符也可以不带盘符), 不允许为相对路径
// basepos是字符串中用户根目录末尾的斜杠的位置, 用于保证"../"在后退时不会退到用户根目录以外
// 比如"C:/foo/bar", 如果用户根目录是C:/foo, 那么basepos应该为6
int ftpd_simplify_path(char *path, int basepos)
{
  char *base, *p, *pp, *q;
  int len;

  // 检查path是否为绝对路径
  if (*path != '/')
  {
    p = strchr(path, '/');
    if (p == NULL || *(p - 1) != ':')
      return -1; // path不允许为相对路径
  }

  // 检查并修正basepos参数
  len = strlen(path);
  if (basepos < 0)
    basepos = 0;
  else if (basepos > len)
    basepos = len;
  base = path + basepos;
  if (*base != '/' && *base != '\0')
  {
    base = strchr(base, '/');
    if (base == NULL)
      base = path + len;
    basepos = base - path;
  }

  p = base; // 当前目录
  pp = base; // 父目录
  do
  {
    q = strchr(p + 1, '/');
    if (q != NULL)
    {
      len = q - p;
      if (len == 1 || (len == 2 && memcmp(p, "/.", 2) == 0))
        memmove(p, q, strlen(q) + 1);
      else if (len == 3 && memcmp(p, "/..", 3) == 0)
      {
        memmove(pp, q, strlen(q) + 1);
        p = pp;
        pp = ftpd_memrchr(base, '/', pp - base);
        if (pp == NULL)
          pp = p;
      }
      else
      {
        pp = p;
        p = q;
      }
    }
    else
    {
      len = strlen(p);
      if (len == 1 || (len == 2 && memcmp(p, "/.", 2) == 0))
      {
        if (p == path || *(p - 1) == ':')
          p++;
        *p = '\0';
      }
      else if (len == 3 && memcmp(p, "/..", 3) == 0)
      {
        if (pp == path || *(pp - 1) == ':')
          pp++;
        *pp = '\0';
      }
    }
  } while (q != NULL);
  return 0;
}

/* 开辟一块内存空间, 用于长期保存局部变量中的字符串, 避免函数退出时局部变量失效 */
char *ftpd_strdup(const char *s)
{
  char *p;
  int len;
  
  len = strlen(s) + 1;
  p = mem_malloc(len);
  if (p != NULL)
    memcpy(p, s, len);
  return p;
}
