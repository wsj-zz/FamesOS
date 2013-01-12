/******************************************************************************************
 * �ļ�:    gui/window.c
 *
 * ˵��:    GUI֮���ڹ���
 *
 * ����:    Jun
 *
 * ʱ��:    2012-12-20
******************************************************************************************/
#define  FAMES_GUI_WINDOW_C
#include "includes.h"


/*-----------------------------------------------------------------------------------------
 * 
 * ���ڳؼ�����
 * 
**---------------------------------------------------------------------------------------*/
static gui_window_t * gui_window_list = NULL;

static gui_window_t   windows_poll[WINDOW_MAX_NR];
static gui_window_t * window_free_list;

static gui_window_t   init_window = INIT_WINDOW;

static int window_total_nr, window_used_nr;


/*-----------------------------------------------------------------------------------------
 *
 * �ڲ�����: ���, ����, ����, �ͷ�
 *
**---------------------------------------------------------------------------------------*/
/**
 * Ĭ�ϼӵ�ĩβ
 */
static void __window_list_add(gui_window_t * w)
{
    gui_window_t ** ppw;

    FamesAssert(w);
    if (!w)
        return;

    lock_kernel();
    ppw = &gui_window_list;
    while (*ppw)
        ppw = &((*ppw)->next);

    w->next = NULL;
    *ppw = w;
    unlock_kernel();
}

static void __window_list_del(gui_window_t * w)
{
    gui_window_t ** ppw;

    FamesAssert(w);
    if (!w)
        return;

    lock_kernel();
    ppw = &gui_window_list;
    while (*ppw) {
        if (*ppw == w) { /* �ҵ��� */
            *ppw = w->next;
            w->next = NULL;
            break;
        }
        ppw = &((*ppw)->next);
    }
    unlock_kernel();
}

static gui_window_t * __alloc_window(void)
{
    gui_window_t * t;

    lock_kernel();
    t = window_free_list;

    if (t)
        window_free_list = t->next;

    if (t) {
        *t = init_window;
        window_used_nr++;
    }
    unlock_kernel();

    return t;
}

static void __free_window(gui_window_t * w)
{
    FamesAssert(window_used_nr > 0);

    lock_kernel();
    window_used_nr--;
    w->next = window_free_list;
    window_free_list = w;
    w->magic = 0;
    unlock_kernel();
}

/*-----------------------------------------------------------------------------------------
 *
 * ���ڹ�������ʼ�����˳�
 *
**---------------------------------------------------------------------------------------*/
void __sysonly InitWindowManager(void)
{
    int i;
    gui_window_t * t;

    gui_window_list = NULL;
    window_free_list = NULL;

    t = &windows_poll[0];
    for (i = 0; i < WINDOW_MAX_NR; i++, t++) {
        t->next = window_free_list;
        window_free_list = t;
    }

    window_used_nr = 0;
    window_total_nr = WINDOW_MAX_NR;
}

void __sysonly ExitWindowManager(void)
{
    gui_window_list = NULL;
}

/*-----------------------------------------------------------------------------------------
 *
 * ���ش���ע���б�
 *
**---------------------------------------------------------------------------------------*/
gui_window_t * __sysonly gui_get_window_list(void)
{
    gui_window_t * wl;

    lock_kernel();
    wl = gui_window_list;
    unlock_kernel();

    return wl;
}

/*-----------------------------------------------------------------------------------------
 *
 * ���ڵĴ���������
 *
**---------------------------------------------------------------------------------------*/
/**
 * ���ڸմ����������ص�, ��ͨ��gui_show_window()�Ż���ʾ����
 */
gui_window_t * guical gui_create_window(gui_widget * root_widget)
{
    gui_window_t * t;

    FamesAssert(root_widget);
    if (!root_widget)
        return NULL;

    if (root_widget->flag & GUI_WIDGET_FLAG_WINDOW)
        return NULL; /* ����˿ؼ��Ѱ�ĳ������, �������ٴΰ� */

    t = __alloc_window();
    if (!t)
        return NULL;

    t->root_widget = root_widget;
    root_widget->flag |= GUI_WIDGET_FLAG_WINDOW;

    __window_list_add(t);

    return t;
}

void guical gui_destroy_window(gui_window_t * w)
{
    gui_hide_window(w);
    if (w && w->root_widget)
        w->root_widget->flag &= ~GUI_WIDGET_FLAG_WINDOW;
    __window_list_del(w);
    __free_window(w);
}

/*-----------------------------------------------------------------------------------------
 *
 * ���ڵ���ʾ, ����, ˢ��
 *
**---------------------------------------------------------------------------------------*/
void guical gui_show_window(gui_window_t * w)
{
    FamesAssert(w);
    if (!w)
        return;

    lock_kernel();
    if (w->flag & WINDOW_FLAG_SHOW)
        goto out;

    __window_list_del(w);
    __window_list_add(w);
    w->flag |= WINDOW_FLAG_SHOW;
    gui_refresh_widget(w->root_widget);
    TimerForce(TimerGUI);

out:
    unlock_kernel();
    return;
}

void guical gui_hide_window(gui_window_t * w)
{
    gui_window_t * t;

    FamesAssert(w);
    if (!w)
        return;

    lock_kernel();
    if (!(w->flag & WINDOW_FLAG_SHOW))
        goto out;

    w->flag &= ~WINDOW_FLAG_SHOW;
    gui_for_each_window(t) {
        if (t == w)
            break;
        if (!(t->flag & WINDOW_FLAG_SHOW))
            continue; /* ��������ش��� */
        gui_refresh_widget(t->root_widget);
    }
    TimerForce(TimerGUI);

out:
    unlock_kernel();
    return;
}

void guical gui_refresh_window(gui_window_t * w)
{
    FamesAssert(w);
    if (!w)
        return;

    lock_kernel();
    if (w->flag & WINDOW_FLAG_SHOW) {/* ����ʾ�Ĳ���Ҫˢ�� */
        gui_refresh_widget(w->root_widget);
    }
    unlock_kernel();

    return;
}

/*-----------------------------------------------------------------------------------------
 *
 * ����������ײ㴰��
 *
**---------------------------------------------------------------------------------------*/
gui_window_t * guical gui_get_top_window(void)
{
    gui_window_t * t;

    lock_kernel();
    t = gui_window_list;
    if (t) {
        while (t->next)
            t = t->next;
    }
    unlock_kernel();

    return t;
}

gui_window_t * guical gui_get_bottom_window(void)
{
    return gui_get_window_list();
}

/*-----------------------------------------------------------------------------------------
 *
 * ���ش��ڵ�ʹ�����
 *
**---------------------------------------------------------------------------------------*/
void guical gui_get_window_usage(int * total, int * used)
{
    lock_kernel();

    if (total)
        *total = window_total_nr;
    if (used)
        *used = window_used_nr;

    unlock_kernel();
}

/*-----------------------------------------------------------------------------------------
 * ����:    gui_window_action()
 *
 * ����:    ִ�д����¼�(Ŀǰֻ����ʾ����)
 *
 * ˵��:    �˺�������lock_kernel()/unlock_kernel()����֮�µ���
**---------------------------------------------------------------------------------------*/
void __sysonly gui_window_action(gui_window_t * w)
{
/*lint --e{613}*/

    FamesAssert(w != NULL);
    FamesAssert(w->magic == WINDOW_MAGIC);
    FamesAssert(w->root_widget);

    if (!w || w->magic != WINDOW_MAGIC || !w->root_widget)
        return;

    if (!(w->flag & WINDOW_FLAG_SHOW))
        return;

    gdc_set_myself_window(w);
    gui_draw_widget(w->root_widget);
    gdc_set_myself_window(NULL);
}




/*=========================================================================================
 * 
 * ���ļ�����: gui/window.c
 * 
**=======================================================================================*/


