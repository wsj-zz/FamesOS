/*******************************************************************************************
 * �ļ�:    gui/window.h
 *
 * ˵��:    GUI֮���ڹ���
 *
 * ����:    Jun
 *
 * ʱ��:    2012-12-20
*******************************************************************************************/
#ifndef FAMES_GUI_WINDOW_H
#define FAMES_GUI_WINDOW_H

/*------------------------------------------------------------------------------------------
 *
 * ��ض���
 *
**----------------------------------------------------------------------------------------*/
#define WINDOW_MAGIC   0x4D57       /* "WM" */
#define WINDOW_MAX_NR  32           /* ���������� */

struct gui_window_struct {
    INT16U magic;
    INT16U flag;
    gui_widget * root_widget;
    struct gui_window_struct * next;
};
typedef struct gui_window_struct gui_window_t;

#define INIT_WINDOW   {WINDOW_MAGIC, 0, NULL, NULL}

#define WINDOW_FLAG_SHOW    0x0001      /* ��ʾ���� */

#define gui_for_each_window(w)  \
            for (w = gui_get_window_list(); w; w = w->next)

/*------------------------------------------------------------------------------------------
 *
 * ��������
 *
**----------------------------------------------------------------------------------------*/
gui_window_t * guical gui_create_window(gui_widget * widget);
void guical gui_destroy_window(gui_window_t * w);
void guical gui_show_window(gui_window_t * w);
void guical gui_hide_window(gui_window_t * w);
void guical gui_refresh_window(gui_window_t * w);
gui_window_t * guical gui_get_top_window(void);
gui_window_t * guical gui_get_bottom_window(void);
void guical gui_get_window_usage(int * total, int * used);

gui_window_t * __sysonly gui_get_window_list(void);
void __sysonly InitWindowManager(void);
void __sysonly ExitWindowManager(void);
void __sysonly gui_window_action(gui_window_t * window);


#endif /* #ifndef FAMES_GUI_WINDOW_H */

/*==========================================================================================
 * 
 * ���ļ�����: gui/window.h
 * 
**========================================================================================*/


