#include "my_gui.h"
#include "lvgl.h"
#include "bsp_rtc.h"
#include "gnss.h"
extern lv_indev_t * indev_keypad;

typedef struct
{
    int speed;//当前速度
    int avg_speed;//平均速度
    int distance;//距离
    long time_s;//耗时秒
    int cal_all;//消耗能量
    int satellite_num;
}GPS_Type_Date;
GPS_Type_Date My_Gps_Data = {1301,1204,223,33333,555};

struct
{
    lv_obj_t* top_label;
    lv_obj_t* main_obj;
    lv_obj_t* bottom_obj;
    lv_obj_t* app_btn;
    lv_obj_t* app_txt;
    lv_obj_t* start_btn;
    lv_obj_t* start_txt;
}LvglMainViewParm = {0};

struct
{
    lv_obj_t* top_label;
    lv_obj_t * satellite_label;
    lv_obj_t * rtc_label;
    lv_obj_t * bat_label;
    lv_timer_t * timer;
    int bat;
}LvglTopViewParm = {0};

struct
{
    lv_obj_t* home_label;
    lv_obj_t * speed_label;
    lv_timer_t * timer;

}LvglHomeViewParm = {0};

static lv_style_t main_style;

void home_sig_timer_cb(lv_timer_t *timer)
{
    lv_label_set_text_fmt(LvglHomeViewParm.speed_label,"%d.%d",get_gnss_speed()/10,get_gnss_speed()%10);
}
//首页信息
void home_table_view(lv_obj_t *home_tab)
{

    if(LvglHomeViewParm.speed_label == NULL)
    {
        LvglHomeViewParm.speed_label = lv_label_create(home_tab);
        lv_label_set_text_fmt(LvglHomeViewParm.speed_label,"%d.%d",get_gnss_speed()/10,get_gnss_speed()%10);
    }
   
    lv_obj_set_style_text_font( LvglHomeViewParm.speed_label,&lv_font_montserrat_40,0);
    //lv_obj_set_style_border_width(speed_label,1,0);
    lv_obj_set_size( LvglHomeViewParm.speed_label,214,60);
    lv_obj_align_to( LvglHomeViewParm.speed_label,home_tab,LV_ALIGN_TOP_MID,0,0);
    lv_obj_set_style_text_color( LvglHomeViewParm.speed_label, lv_color_make(255,255,255), 0);

    auto fontHeight = lv_font_get_line_height(&lv_font_montserrat_40);
    auto verPad = (40 - fontHeight)/2;
    lv_obj_set_style_pad_top( LvglHomeViewParm.speed_label, verPad, 0);
    auto textWidth = lv_txt_get_width(lv_label_get_text( LvglHomeViewParm.speed_label), strlen(lv_label_get_text( LvglHomeViewParm.speed_label)), &lv_font_montserrat_40, 0,0);
    auto horPad = (214 - textWidth)/2;
    lv_obj_set_style_pad_left( LvglHomeViewParm.speed_label, horPad, 0);


    lv_obj_t * speed_label_KM = lv_label_create( LvglHomeViewParm.speed_label);
    lv_label_set_text(speed_label_KM, "km/h");
    lv_obj_set_style_text_font(speed_label_KM,&lv_font_montserrat_14,0);
    lv_obj_align_to(speed_label_KM, LvglHomeViewParm.speed_label,LV_ALIGN_BOTTOM_MID,-30,0);


    //平均速度
    lv_obj_t * avg_speed_label = lv_label_create(home_tab);
    lv_label_set_text_fmt(avg_speed_label,"%d.%d  km/h",My_Gps_Data.avg_speed/10,My_Gps_Data.avg_speed%10);
    lv_obj_set_style_text_font(avg_speed_label,&lv_font_montserrat_16,0);
    //lv_obj_set_style_border_width(avg_speed_label,1,0);
    lv_obj_set_size(avg_speed_label,106,40);
    lv_obj_align_to(avg_speed_label,home_tab,LV_ALIGN_LEFT_MID,0,10);
    lv_obj_set_style_text_color(avg_speed_label, lv_color_make(255,255,255), 0);

    fontHeight = lv_font_get_line_height(&lv_font_montserrat_16);
    verPad = (20 - fontHeight)/2;
    lv_obj_set_style_pad_top(avg_speed_label, verPad, 0);
    textWidth = lv_txt_get_width(lv_label_get_text(avg_speed_label), strlen(lv_label_get_text(avg_speed_label)), &lv_font_montserrat_16, 0,0);
    horPad = (106 - textWidth)/2;
    lv_obj_set_style_pad_left(avg_speed_label, horPad, 0);

    lv_obj_t * avg_speed_label_avg = lv_label_create(avg_speed_label);
    lv_label_set_text(avg_speed_label_avg, "AVG");
    lv_obj_set_style_text_font(avg_speed_label_avg,&lv_font_montserrat_14,0);
    lv_obj_align_to(avg_speed_label_avg,avg_speed_label,LV_ALIGN_BOTTOM_MID,0,0);



    //耗时
    lv_obj_t * time_label = lv_label_create(home_tab);

    int h = 0,m=0,s=0;
    lv_label_set_text_fmt(time_label,"%02d:%02d:%02d",h,m,s);

   // lv_obj_set_style_border_width(time_label,1,0);
    lv_obj_set_size(time_label,106,40);
    lv_obj_align_to(time_label,home_tab,LV_ALIGN_RIGHT_MID,0,10);
    lv_obj_set_style_text_font(time_label,&lv_font_montserrat_16,0);
    lv_obj_set_style_text_color(time_label, lv_color_make(255,255,255), 0);

    fontHeight = lv_font_get_line_height(&lv_font_montserrat_16);
    verPad = (20 - fontHeight)/2;
    lv_obj_set_style_pad_top(time_label, verPad, 0);
    textWidth = lv_txt_get_width(lv_label_get_text(time_label), strlen(lv_label_get_text(time_label)), &lv_font_montserrat_16, 0,0);
    horPad = (106 - textWidth)/2;
    lv_obj_set_style_pad_left(time_label, horPad, 0);

    lv_obj_t * time_label_time = lv_label_create(time_label);
    lv_label_set_text(time_label_time, "Time");
    lv_obj_set_style_text_font(time_label_time,&lv_font_montserrat_14,0);
    lv_obj_align_to(time_label_time,time_label,LV_ALIGN_BOTTOM_MID,-5,0);




    //距离
     lv_obj_t * distance_label = lv_label_create(home_tab);
    lv_label_set_text_fmt(distance_label, "%d.%d  km",My_Gps_Data.distance/10,My_Gps_Data.distance%10);
    //lv_obj_set_style_border_width(distance_label,1,0);
    lv_obj_set_size(distance_label,106,40);
    lv_obj_align_to(distance_label,home_tab,LV_ALIGN_BOTTOM_LEFT,0,0);

    lv_obj_set_style_text_font(distance_label,&lv_font_montserrat_16,0);
    lv_obj_set_style_text_color(distance_label, lv_color_make(255,255,255), 0);


    fontHeight = lv_font_get_line_height(&lv_font_montserrat_16);
    verPad = (20 - fontHeight)/2;
    lv_obj_set_style_pad_top(distance_label, verPad, 0);
    textWidth = lv_txt_get_width(lv_label_get_text(distance_label), strlen(lv_label_get_text(distance_label)), &lv_font_montserrat_16, 0,0);
    horPad = (106 - textWidth)/2;
    lv_obj_set_style_pad_left(distance_label, horPad, 0);

    lv_obj_t * distance_label_dist = lv_label_create(distance_label);
    lv_label_set_text(distance_label_dist, "Dist");
    lv_obj_set_style_text_font(distance_label_dist,&lv_font_montserrat_14,0);
    lv_obj_align_to(distance_label_dist,distance_label,LV_ALIGN_BOTTOM_MID,-10,0);


    //热量
    lv_obj_t * calorie_label = lv_label_create(home_tab);
    lv_label_set_text_fmt(calorie_label, "%d  cal",My_Gps_Data.cal_all);
    //lv_obj_set_style_border_width(calorie_label,1,0);
    lv_obj_set_size(calorie_label,106,40);
    lv_obj_align_to(calorie_label,home_tab,LV_ALIGN_BOTTOM_RIGHT,0,0);

    lv_obj_set_style_text_font(calorie_label,&lv_font_montserrat_16,0);
    lv_obj_set_style_text_color(calorie_label, lv_color_make(255,255,255), 0);


    fontHeight = lv_font_get_line_height(&lv_font_montserrat_16);
    verPad = (20 - fontHeight)/2;
    lv_obj_set_style_pad_top(calorie_label, verPad, 0);
    textWidth = lv_txt_get_width(lv_label_get_text(calorie_label), strlen(lv_label_get_text(calorie_label)), &lv_font_montserrat_16, 0,0);
    horPad = (106 - textWidth)/2;
    lv_obj_set_style_pad_left(calorie_label, horPad, 0);

    lv_obj_t * calorie_label_cal = lv_label_create(calorie_label);
    lv_label_set_text(calorie_label_cal, "Calorie");
    lv_obj_set_style_text_font(calorie_label_cal,&lv_font_montserrat_14,0);
    lv_obj_align_to(calorie_label_cal,calorie_label,LV_ALIGN_BOTTOM_MID,-10,0);

    //创建定时器
    if(LvglHomeViewParm.timer == NULL)
    {
        LvglHomeViewParm.timer = lv_timer_create(home_sig_timer_cb,1000,NULL);

    }
}

static void return_but_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
        my_gui_xtrack();
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");

    }
}
void app_table_view(void)
{
 /*创建Tab view 组件，在活动页面，底部，创建一个高度为50*/
    lv_obj_t *app_obj = lv_obj_create(lv_scr_act());
    //配置标签的颜色
    lv_obj_set_style_bg_color(app_obj,lv_color_make(30,35,45),LV_PART_MAIN);
    lv_obj_align(app_obj, LV_ALIGN_BOTTOM_MID, 0, 00);
    //设置选项卡的大小，为横240，高220
    lv_obj_set_size(app_obj,240,220);
    //设置对象为无边框
    lv_obj_set_style_border_width(app_obj,0,LV_BORDER_SIDE_NONE );
    lv_obj_set_style_radius(app_obj,0,0);

     lv_obj_t *return_btn = lv_btn_create(app_obj);
    //设置选项卡的大小，为横240，高220
    lv_obj_set_size(return_btn,30,30);
    lv_obj_align(return_btn, LV_ALIGN_BOTTOM_RIGHT, -40, -5);
    //按钮的点击回调
     lv_obj_add_event_cb(return_btn, return_but_event_handler, LV_EVENT_ALL, NULL);

     lv_obj_t *return_txt = lv_label_create(return_btn);
     lv_label_set_text(return_txt, LV_SYMBOL_BACKSPACE);
    lv_obj_center(return_txt);
    lv_obj_set_style_bg_color(return_btn,lv_color_make(255,127,39),LV_PART_MAIN);

}



void top_sig_timer_cb(lv_timer_t *timer)
{
    LvglTopViewParm.bat++;
     if(LvglTopViewParm.bat > 100)
     {
         LvglTopViewParm.bat = 0;

     }
    if(LvglTopViewParm.bat >80 && LvglTopViewParm.bat <= 100)
    {
        lv_label_set_text(LvglTopViewParm.bat_label,LV_SYMBOL_BATTERY_FULL);
    }
    else if(LvglTopViewParm.bat >60 && LvglTopViewParm.bat <= 80)
    {
        lv_label_set_text(LvglTopViewParm.bat_label,LV_SYMBOL_BATTERY_3);
    }
    else if(LvglTopViewParm.bat >40 && LvglTopViewParm.bat <= 60)
    {
        lv_label_set_text(LvglTopViewParm.bat_label,LV_SYMBOL_BATTERY_1);
    }
    else if(LvglTopViewParm.bat >20 && LvglTopViewParm.bat <= 40)
    {
        lv_label_set_text(LvglTopViewParm.bat_label,LV_SYMBOL_BATTERY_1);
    }
    else
    {
        lv_label_set_text(LvglTopViewParm.bat_label,LV_SYMBOL_BATTERY_EMPTY);
    }

    RTC_TimeTypeDef RTC_TimeStructure_lvgl;
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure_lvgl);
    lv_label_set_text_fmt(LvglTopViewParm.rtc_label,"%02d:%02d:%02d",RTC_TimeStructure_lvgl.RTC_Hours,RTC_TimeStructure_lvgl.RTC_Minutes,RTC_TimeStructure_lvgl.RTC_Seconds);

    lv_label_set_text_fmt(LvglTopViewParm.satellite_label,"%s %d",LV_SYMBOL_GPS,get_gnss_satellite_num());
    //printf("top_sig_timer_cb，top_sig_timer_cb: %d\r\n",LvglTopViewParm.bat);


}
void top_label_sig(lv_obj_t *top_label)
{

    lv_obj_set_style_bg_color(top_label,lv_color_make(30,35,45),LV_PART_MAIN);
    lv_obj_set_style_bg_opa(top_label,LV_OPA_COVER,LV_PART_MAIN);

    if(LvglTopViewParm.satellite_label == NULL)
    {
        LvglTopViewParm.satellite_label = lv_label_create(top_label);
        lv_label_set_text_fmt(LvglTopViewParm.satellite_label,"%s %d",LV_SYMBOL_GPS,get_gnss_satellite_num());
    }
    lv_obj_set_style_text_font(LvglTopViewParm.satellite_label,&lv_font_montserrat_14,0);
    lv_obj_set_size(LvglTopViewParm.satellite_label,30,20);
    lv_obj_align_to(LvglTopViewParm.satellite_label,top_label,LV_ALIGN_TOP_LEFT,0,0);
    lv_obj_set_style_text_color(LvglTopViewParm.satellite_label, lv_color_make(255,255,255), 0);

    RTC_TimeTypeDef RTC_TimeStructure_lvgl;
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure_lvgl);
    if(LvglTopViewParm.rtc_label == NULL)
    {
        LvglTopViewParm.rtc_label = lv_label_create(top_label);
        lv_label_set_text_fmt(LvglTopViewParm.rtc_label,"%02d:%02d:%02d",RTC_TimeStructure_lvgl.RTC_Hours,RTC_TimeStructure_lvgl.RTC_Minutes,RTC_TimeStructure_lvgl.RTC_Seconds);
    }
    lv_obj_set_style_text_font(LvglTopViewParm.rtc_label,&lv_font_montserrat_14,0);
    lv_obj_set_size(LvglTopViewParm.rtc_label,60,20);
    lv_obj_align(LvglTopViewParm.rtc_label, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_color(LvglTopViewParm.rtc_label, lv_color_make(255,255,255), 0);



    LvglTopViewParm.bat_label = lv_label_create(top_label);
    if(LvglTopViewParm.bat_label == NULL)
    {
        lv_label_set_text(LvglTopViewParm.bat_label,LV_SYMBOL_BATTERY_FULL);
    }
    lv_obj_set_style_text_font(LvglTopViewParm.bat_label,&lv_font_montserrat_14,0);
    lv_obj_set_size(LvglTopViewParm.bat_label,20,20);
    lv_obj_align(LvglTopViewParm.bat_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_text_color(LvglTopViewParm.bat_label, lv_color_make(255,255,255), 0);

    //创建定时器
    if(LvglTopViewParm.timer == NULL)
    {
        LvglTopViewParm.timer = lv_timer_create(top_sig_timer_cb,1000,NULL);

    }
}

static void app_but_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
          //app_table_view();

    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");

    }
}

static void start_but_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    static bool count=0;
    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
        if(count == 0)
        {
            lv_label_set_text(LvglMainViewParm.start_txt, LV_SYMBOL_PAUSE);
            count = 1;
        }
        else
        {

            lv_label_set_text(LvglMainViewParm.start_txt, LV_SYMBOL_PLAY);
             count = 0;
        }
        
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");

    }
}

void my_gui_xtrack(void)
{

    //创建顶部信息框

    LvglMainViewParm.top_label = lv_label_create(lv_scr_act());


    lv_obj_set_size(LvglMainViewParm.top_label,240,20);

    /*创建Tab view 组件，在活动页面，底部，创建一个高度为50*/
    LvglMainViewParm.main_obj = lv_obj_create(lv_scr_act());
    //配置标签的颜色
    lv_obj_set_style_bg_color(LvglMainViewParm.main_obj,lv_color_make(30,35,45),LV_PART_MAIN);
    lv_obj_align(LvglMainViewParm.main_obj, LV_ALIGN_TOP_MID, 0, 20);
    //设置选项卡的大小，为横240，高220
    lv_obj_set_size(LvglMainViewParm.main_obj,240,170);
    //设置对象为无边框
    lv_obj_set_style_border_width(LvglMainViewParm.main_obj,0,LV_BORDER_SIDE_NONE );
    lv_obj_set_style_radius(LvglMainViewParm.main_obj,0,0);


    LvglMainViewParm.bottom_obj = lv_obj_create(lv_scr_act());

    lv_obj_set_style_bg_color(LvglMainViewParm.bottom_obj,lv_color_make(30,35,45),LV_PART_MAIN);
    lv_obj_align(LvglMainViewParm.bottom_obj, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_size(LvglMainViewParm.bottom_obj,240,50);
    lv_obj_set_style_border_width(LvglMainViewParm.bottom_obj,0,LV_BORDER_SIDE_NONE );
    lv_obj_set_style_radius(LvglMainViewParm.bottom_obj,0,0);


    LvglMainViewParm.app_btn = lv_btn_create(LvglMainViewParm.bottom_obj);

    //设置选项卡的大小，为横240，高220
    lv_obj_set_size(LvglMainViewParm.app_btn,30,30);
    lv_obj_align(LvglMainViewParm.app_btn, LV_ALIGN_BOTTOM_RIGHT, -40, -5);
    //按钮的点击回调
    lv_obj_add_event_cb(LvglMainViewParm.app_btn, app_but_event_handler, LV_EVENT_ALL, NULL);



    LvglMainViewParm.app_txt = lv_label_create(LvglMainViewParm.app_btn);

    lv_label_set_text(LvglMainViewParm.app_txt, LV_SYMBOL_LIST);
    lv_obj_center(LvglMainViewParm.app_txt);
    lv_obj_set_style_bg_color(LvglMainViewParm.app_btn,lv_color_make(255,127,39),LV_PART_MAIN);


    LvglMainViewParm.start_btn = lv_btn_create(LvglMainViewParm.bottom_obj);

    //设置选项卡的大小，为横240，高220
    lv_obj_set_size(LvglMainViewParm.start_btn,30,30);
    lv_obj_align(LvglMainViewParm.start_btn, LV_ALIGN_BOTTOM_LEFT, 40, -5);
    //按钮的点击回调
    lv_obj_add_event_cb(LvglMainViewParm.start_btn, start_but_event_handler, LV_EVENT_ALL, NULL);


    LvglMainViewParm.start_txt = lv_label_create(LvglMainViewParm.start_btn);
    lv_label_set_text(LvglMainViewParm.start_txt, LV_SYMBOL_PLAY);
    lv_obj_center(LvglMainViewParm.start_txt);
    lv_obj_set_style_bg_color(LvglMainViewParm.start_btn,lv_color_make(255,127,39),LV_PART_MAIN);


    top_label_sig(LvglMainViewParm.top_label);
    home_table_view(LvglMainViewParm.main_obj);

    lv_group_t* g = lv_group_create();
    lv_group_add_obj(g,LvglMainViewParm.app_btn);
    lv_group_add_obj(g,LvglMainViewParm.start_btn);
    lv_indev_set_group(indev_keypad,g);
}





void my_gui(void)
{
    lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
    lv_obj_set_size(switch_obj, 120, 60);
    lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_t* switch_obj1 = lv_switch_create(lv_scr_act());
    lv_obj_set_size(switch_obj1, 120, 60);
    lv_obj_align(switch_obj1, LV_ALIGN_TOP_MID,0 ,20);


    lv_group_t* g = lv_group_create();
    lv_group_add_obj(g,switch_obj);
    lv_group_add_obj(g,switch_obj1);
    lv_indev_set_group(indev_keypad,g);

}