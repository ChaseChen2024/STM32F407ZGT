#ifndef _MY_GUI_H
#define _MY_GUI_H
#include <stdio.h>

typedef enum
{
    STARTUP_PAGE = 0,
    APPLIST_PAGE,
    STOPWATCH_PAGE,
    SETUP_PAGE,
    PAGE_MAX
}page_enum;

//void my_gui_xtrack(void);
void my_gui(void);
void xtrack_start(void);
#endif
