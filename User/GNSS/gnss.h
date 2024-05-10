#ifndef __GNSS_H
#define __GNSS_H


#include "user.h"
#include "NMEA0183.h"


long Gnss_Resolve_Task_Init(void);
uint8_t get_gnss_satellite_num(void);
int get_gnss_speed(void);
#endif
