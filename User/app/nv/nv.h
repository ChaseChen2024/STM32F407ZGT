#ifndef _NV_H
#define _NV_H
#include <stdio.h>
#include "ff.h"

FRESULT nv_init(void);
FRESULT nv_write(char *file_name, char *p_write_buf, UINT write_len);
FRESULT nv_read(char *file_name,char *p_read_buf, UINT read_len);
#endif /* _NV_H */