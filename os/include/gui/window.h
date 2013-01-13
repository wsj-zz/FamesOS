/*******************************************************************************************
 * 文件:    gui/window.h
 *
 * 说明:    GUI之窗口管理
 *
 * 作者:    Jun
 *
 * 时间:    2012-12-20
*******************************************************************************************/
#ifndef FAMES_GUI_WINDOW_H
#define FAMES_GUI_WINDOW_H

/*------------------------------------------------------------------------------------------
 *
 * 相关定义
 *
**----------------------------------------------------------------------------------------*/
#define WINDOW_MAGIC   0x4D57       /* "WM" */
#define WINDOW_MAX_NR  32           /* 窗口最大个数 */

struct gui_window_struct {
    INT16U magic;
    INT16U flag;
    gui_widget * root_widget;
    RECT dirty_rect;
    int  has_dirty_rect;
    struct gui_window_struct * next;
};
typedef struct gui_window_struct gui_window_t;

#define INIT_WINDOW   {WINDOW_MAGIC, 0, NULL, {0,0,0,0}, NO, NULL}

#define WINDOW_FLAG_SHOW    0x0001      /* 显示出来 */

#define gui_for_each_window(w)  \
            for (w = gui_get_window_list(); w; w = w->next)

/*------------------------------------------------------------------------------------------
 *
 * 函数声明
 *
**----------------------------------------------------------------------------------------*/
gui_window_t * guical gui_create_window(gui_widget * widget);
void guical gui_destroy_window(gui_window_t * w);
void guical gui_show_window(gui_window_t * w);
void guical gui_hide_window(gui_window_t * w);
void guical gui_refresh_window(gui_window_t * w);
RECT * guical gui_window_get_realrect(gui_window_t * w);

gui_window_t * guical gui_get_top_window(void);
gui_window_t * guical gui_get_bottom_window(void);
void guical gui_get_window_usage(int * total, int * used);

gui_window_t * __sysonly gui_get_window_list(void);
void __sysonly InitWindowManager(void);
void __sysonly ExitWindowManager(void);
int  __sysonly gui_window_set_dirty(gui_window_t * w, RECT * rect);
int  __sysonly gui_window_clear_dirty(gui_window_t * w);
void __sysonly gui_window_action(gui_window_t * window);

void guical gui_window_move(gui_window_t * w, int x_move, int y_move);
void guical gui_window_moveto(gui_window_t * w, int x, int y);
void guical gui_window_resize(gui_window_t * w, int width_add, int height_add);
void guical gui_window_resize_to(gui_window_t * w, int width, int height);


#endif /* #ifndef FAMES_GUI_WINDOW_H */

/*==========================================================================================
 * 
 * 本文件结束: gui/window.h
 * 
**========================================================================================*/


