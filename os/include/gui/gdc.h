/***********************************************************************************************
 * 文件:    gdc.h
 *
 * 说明:    图形设备上下文(Graphics Device Context)
 *
 * 作者:    Jun
 *
 * 时间:    2012-12-17
***********************************************************************************************/
#ifndef FAMES_GDC_H
#define FAMES_GDC_H

/*----------------------------------------------------------------------------------------------
 *
 * 相关定义
 *
**--------------------------------------------------------------------------------------------*/

struct gui_widget_s;

/* 
 * 剪切域
 */
struct cliprect_struct {
    int x1, y1, x2, y2;
    struct cliprect_struct * prev;
};
typedef struct cliprect_struct cliprect_t;

/*
 * 图形设备上下文(GDC)
 */
struct gdc_struct {
    INT16U flag;
    /* others */
};
typedef struct gdc_struct gdc_t;

extern struct gdc_struct gdc_global;

enum GDC_value_enum {
    GDC_none = 0,   /* 不用显示了 */
    GDC_whole,      /* 可以全显示完 */
    GDC_has_more    /* 需要再次测试 */
};
typedef enum GDC_value_enum GDC_value;

/*----------------------------------------------------------------------------------------------
 *
 * GDC例程(声明/定义)
 *
**--------------------------------------------------------------------------------------------*/
int gdc_initialize(void);
int gdc_set_current(gdc_t * gdc);
gdc_t * gdc_get_current(void);
gui_window_t * __sysonly gdc_get_myself_window(void);
void __sysonly gdc_set_myself_window(gui_window_t * w);
void __sysonly gdc_set_myself_window_from_widget(gui_widget * wid);

GDC_value __sysonly gdc_is_xline_visible(int x, int x2, int y, int * o_x1, int * o_x2);
GDC_value __sysonly gdc_is_yline_visible(int y, int y2, int x, int * o_y1, int * o_y2);
GDC_value __sysonly gdc_is_point_visible(int x, int y);
BOOL      __sysonly gdc_is_rect_intersect(RECT * rect1, RECT * rect2);




#endif /* FAMES_GDC_H     */

/*==============================================================================================
 * 
 * 本文件结束: gdc.h
 * 
**============================================================================================*/


