/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "../../lvgl.h"
#include "lcd.h"
#include "user.h"
#include "bsp_debug_usart.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES] __EXRAM;            /*A screen sized buffer*/
static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES] __EXRAM;            /*Another screen sized buffer*/
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    // /* Example for 1) */
    // static lv_disp_draw_buf_t draw_buf_dsc_1;
    // static lv_color_t buf_1[MY_DISP_HOR_RES * 60];                          /*A buffer for 10 rows*/
    // lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 60);   /*Initialize the display buffer*/

    /* Example for 2) */
//     static lv_disp_draw_buf_t draw_buf_dsc_2;
//     static lv_color_t buf_2_1[MY_DISP_HOR_RES * 100];                        /*A buffer for 10 rows*/
//     static lv_color_t buf_2_2[MY_DISP_HOR_RES * 100];                        /*An other buffer for 10 rows*/
//     lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * MY_DISP_VER_RES);   /*Initialize the display buffer*/

    // /* Example for 3) also set disp_drv.full_refresh = 1 below*/
    static lv_disp_draw_buf_t draw_buf_dsc_3;

    printf("buf_3_1=0x%x\n\n",buf_3_1);
    printf("buf_3_2=0x%x\n\n",buf_3_2);
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, MY_DISP_VER_RES * MY_DISP_VER_RES);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_3;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
        Lcd_Init();	
   
   printf("LCD init\n\n");//��ʼ��OLED  
   
// 	 LCD_Clear(WHITE);
//    printf("[%s][%d]\r\n",__FILE__,__LINE__);
// 	 BACK_COLOR=WHITE;
//    	printf("[%s][%d]\r\n",__FILE__,__LINE__);
//      LCD_ShowChinese(10,0,0,32,RED);   //��
// 			LCD_ShowChinese(45,0,1,32,RED);   //��
// 			LCD_ShowChinese(80,0,2,32,RED);   //԰
// 			LCD_ShowChinese(115,0,3,32,RED);  //��
// 			LCD_ShowChinese(150,0,4,32,RED);  //��
		 
// 		 	LCD_ShowChinese(10,75,0,16,RED);   //��
// 			LCD_ShowChinese(45,75,1,16,RED);   //��
// 			LCD_ShowChinese(80,75,2,16,RED);   //԰
// 			LCD_ShowChinese(115,75,3,16,RED);  //��
// 			LCD_ShowChinese(150,75,4,16,RED);  //��
// 			LCD_ShowString(10,35,"2.4 TFT SPI 240*320",RED);
// 			LCD_ShowString(10,55,"LCD_W:",RED);	LCD_ShowNum(70,55,LCD_W,3,RED);
// 			LCD_ShowString(110,55,"LCD_H:",RED);LCD_ShowNum(160,55,LCD_H,3,RED);
    /*You code here*/
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
   
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    LCD_Fill3(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_p);
    //  *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);

}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
