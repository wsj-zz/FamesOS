/******************************************************************************************
 * 文件:    gui/window.c
 *
 * 说明:    GUI之窗口管理
 *
 * 作者:    Jun
 *
 * 时间:    2012-12-20
******************************************************************************************/
#define  FAMES_GUI_WINDOW_C
#include "includes.h"


/*-----------------------------------------------------------------------------------------
 * 
 * 窗口池及队列
 * 
**---------------------------------------------------------------------------------------*/
static gui_window_t * gui_window_list = NULL;

static gui_window_t   windows_poll[WINDOW_MAX_NR];
static gui_window_t * window_free_list;

static gui_window_t   init_window = INIT_WINDOW;

static int window_total_nr, window_used_nr;


/*-----------------------------------------------------------------------------------------
 *
 * 内部函数: 入队, 出队, 申请, 释放
 *
**---------------------------------------------------------------------------------------*/
/**
 * 默认加到末尾
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
        if (*ppw == w) { /* 找到了 */
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
 * 窗口管理器初始化及退出
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
 * 返回窗口注册列表
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
 * 窗口的创建, 销毁, 查找
 *
**---------------------------------------------------------------------------------------*/
/**
 * 窗口刚创建后是隐藏的, 须通过gui_show_window()才会显示出来
 */
gui_window_t * guical gui_create_window(gui_widget * root_widget)
{
    gui_window_t * t;

    FamesAssert(root_widget);
    if (!root_widget)
        return NULL;

    if (root_widget->flag & GUI_WIDGET_FLAG_WINDOW)
        return NULL; /* 如果此控件已绑定某个窗口, 则不允许再次绑定 */

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

gui_window_t * guical gui_find_window_from_widget(gui_widget * widget)
{
    gui_window_t * win = NULL;

    FamesAssert(widget);
    if (!widget)
        return;

    lock_kernel();
    while (widget) {
        if (widget->flag & GUI_WIDGET_FLAG_WINDOW)
            break;
        widget = widget->father; /* 顺序向上找 */
    }
    if (widget) {
        gui_for_each_window(win) {
            if (win->root_widget == widget)
                break;
        }
    }
    unlock_kernel();

    return win;
}

/*-----------------------------------------------------------------------------------------
 *
 * 窗口的显示, 隐藏, 刷新
 *
**---------------------------------------------------------------------------------------*/
void guical gui_show_window(gui_window_t * w)
{
    FamesAssert(w);
    FamesAssert(w->magic == WINDOW_MAGIC);
    if (!w || w->magic != WINDOW_MAGIC || !w->root_widget)
        return;

    lock_kernel();
    if (w->flag & WINDOW_FLAG_SHOW)
        goto out;

    __window_list_del(w);
    __window_list_add(w);
    w->flag |= WINDOW_FLAG_SHOW;
    gui_window_clear_dirty(w);
    gui_refresh_widget(w->root_widget);
    TimerForce(TimerGUI);

out:
    unlock_kernel();
    TaskSleep(16uL);

    return;
}

void guical gui_hide_window(gui_window_t * w)
{
    gui_window_t * t;

    FamesAssert(w);
    FamesAssert(w->magic == WINDOW_MAGIC);
    if (!w || w->magic != WINDOW_MAGIC)
        return;

    TimerForce(TimerGUI);
    TaskSleep(35uL);    /* 让有需要的控件刷新一次, 35ms应该够了吧 */

    lock_kernel();
    if (!(w->flag & WINDOW_FLAG_SHOW))
        goto out;

    w->flag &= ~WINDOW_FLAG_SHOW;
    gui_for_each_window(t) {
        if (t == w)     /* 上面的窗口不需要刷新 */
            break;
        if (!(t->flag & WINDOW_FLAG_SHOW))
            continue;   /* 不理会隐藏窗口 */
        gui_window_set_dirty(t, gui_window_get_realrect(w));
        if (t->root_widget)
            gui_refresh_widget(t->root_widget);
    }
    TimerForce(TimerGUI);

out:
    unlock_kernel();
    TaskSleep(15uL);

    return;
}

void guical gui_refresh_window(gui_window_t * w)
{
    FamesAssert(w);
    FamesAssert(w->magic == WINDOW_MAGIC);
    if (!w || w->magic != WINDOW_MAGIC)
        return;

    lock_kernel();
    if (w->flag & WINDOW_FLAG_SHOW) {/* 不显示的不需要刷新 */
        gui_refresh_widget(w->root_widget);
    }
    unlock_kernel();

    return;
}

/*-----------------------------------------------------------------------------------------
 *
 * 窗口的移动及尺寸变化
 *
**---------------------------------------------------------------------------------------*/
enum __window_fn_xy_cmds {
    __window_fn_xy_move,
    __window_fn_xy_moveto,
    __window_fn_xy_resize,
    __window_fn_xy_resize_to
};

static void __do_window_relocation_resize(
                    gui_window_t * w,
                    int x, int y,
                    int width, int height,
                    int cmd)
{
    gui_window_t * t;

    FamesAssert(w);
    FamesAssert(w->magic == WINDOW_MAGIC);
    if (!w || w->magic != WINDOW_MAGIC || !w->root_widget)
        return;

    TimerForce(TimerGUI);
    TaskSleep(35uL);    /* 让有需要的控件刷新一次, 35ms应该够了吧 */

    lock_kernel();
    if (!(w->flag & WINDOW_FLAG_SHOW))
        goto do_set_it;

    gui_for_each_window(t) {
        if (t == w)     /* 上面的窗口不需要刷新 */
            break;
        if (!(t->flag & WINDOW_FLAG_SHOW))
            continue;   /* 不理会隐藏窗口 */
        gui_window_set_dirty(t, gui_window_get_realrect(w));
        gui_refresh_widget(t->root_widget);
    }

do_set_it:
    switch (cmd) {
        case __window_fn_xy_move:
            gui_move_widget_right(w->root_widget, x);
            gui_move_widget_down(w->root_widget, y);
            break;
        case __window_fn_xy_moveto:
            gui_set_widget_location(w->root_widget, x, y);
            break;
        case __window_fn_xy_resize:
            gui_set_widget_wider(w->root_widget, width);
            gui_set_widget_higher(w->root_widget, height);
            break;
        case __window_fn_xy_resize_to:
            gui_set_widget_dimension(w->root_widget, width, height);
            break;
        default:
            break;
    }
    gui_window_clear_dirty(w);

    unlock_kernel();

    TimerForce(TimerGUI);
    TaskSleep(15uL);

    return;
}

void guical gui_window_move(gui_window_t * w, int x_move, int y_move)
{
    __do_window_relocation_resize(w, x_move, y_move, 0, 0, __window_fn_xy_move);
}

void guical gui_window_moveto(gui_window_t * w, int x, int y)
{
    __do_window_relocation_resize(w, x, y, 0, 0, __window_fn_xy_moveto);
}

void guical gui_window_resize(gui_window_t * w, int width_add, int height_add)
{
    __do_window_relocation_resize(w, 0, 0, width_add, height_add, __window_fn_xy_resize);
}

void guical gui_window_resize_to(gui_window_t * w, int width, int height)
{
    __do_window_relocation_resize(w, 0, 0, width, height, __window_fn_xy_resize_to);
}

/*-----------------------------------------------------------------------------------------
 *
 * 返回最顶层或最底层窗口
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
 * 返回窗口的使用情况
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
 *
 * 设定与清除窗口的DIRTY_RECT
 *
 * 注意: gui_window_set_dirty()只应该由gui_hide_window()调用
 *
**---------------------------------------------------------------------------------------*/
int __sysonly gui_window_set_dirty(gui_window_t * w, RECT * rect)
{
    RECT * dirty_rect;
    int d_x1, d_y1, d_x2, d_y2;
    int r_x1, r_y1, r_x2, r_y2;

    FamesAssert(w);
    FamesAssert(w->magic == WINDOW_MAGIC);
    if (!w || w->magic != WINDOW_MAGIC)
        return fail;

    if (!rect)
        return gui_window_clear_dirty(w);

    lock_kernel();
    dirty_rect = &w->dirty_rect;
    if (w->has_dirty_rect == YES) { /* merge @rect to the dirty_rect */
        d_x1 = dirty_rect->x;
        d_x2 = __gui_make_x2(d_x1, dirty_rect->width);
        d_y1 = dirty_rect->y;
        d_y2 = __gui_make_y2(d_y1, dirty_rect->height);

        r_x1 = rect->x;
        r_x2 = __gui_make_x2(r_x1, rect->width);
        r_y1 = rect->y;
        r_y2 = __gui_make_y2(r_y1, rect->height);

        if (d_x1 > r_x1)
            d_x1 = r_x1;
        if (d_x2 < r_x2)
            d_x2 = r_x2;
        if (d_y1 > r_y1)
            d_y1 = r_y1;
        if (d_y2 < r_y2)
            d_y2 = r_y2;

        dirty_rect->x = d_x1;
        dirty_rect->y = d_y1;
        dirty_rect->width = __gui_make_width(d_x2, d_x1);
        dirty_rect->height = __gui_make_height(d_y2, d_y1);
    } else {
        *dirty_rect = *rect;
    }
    w->has_dirty_rect = YES;
    unlock_kernel();

    return ok;
}

int __sysonly gui_window_clear_dirty(gui_window_t * w)
{
    FamesAssert(w);
    FamesAssert(w->magic == WINDOW_MAGIC);
    if (!w || w->magic != WINDOW_MAGIC)
        return fail;

    lock_kernel();
    w->has_dirty_rect = NO;
    unlock_kernel();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 *
 * dirty rect: mask & unmask
 *
 * 这两函数的使用要非常小心, 因为会影响到dirty-rect的设定与计算
 *
**---------------------------------------------------------------------------------------*/
int __sysonly gui_window_dirty_mask(gui_window_t * w)
{
    int has = NO;

    FamesAssert(w);
    FamesAssert(w->magic == WINDOW_MAGIC);
    if (!w || w->magic != WINDOW_MAGIC)
        return fail;

    lock_kernel();
    has = w->has_dirty_rect;
    w->has_dirty_rect = NO;
    unlock_kernel();

    return has;
}

void __sysonly gui_window_dirty_unmask(gui_window_t * w, int has)
{
    FamesAssert(w);
    FamesAssert(w->magic == WINDOW_MAGIC);
    if (!w || w->magic != WINDOW_MAGIC)
        return;

    lock_kernel();
    w->has_dirty_rect = has;
    unlock_kernel();
}

/*-----------------------------------------------------------------------------------------
 *
 * 返回窗口的真实位置
 *
**---------------------------------------------------------------------------------------*/
RECT * guical gui_window_get_realrect(gui_window_t * w)
{
    gui_widget * root_widget;

    FamesAssert(w);
    FamesAssert(w->magic == WINDOW_MAGIC);
    if (!w || w->magic != WINDOW_MAGIC)
        return NULL;

    root_widget = w->root_widget;
    if (!root_widget)
        return NULL;

    return &root_widget->real_rect;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_window_action()
 *
 * 描述:    执行窗口事件(目前只是显示出来)
 *
 * 说明:    此函数须在lock_kernel()/unlock_kernel()保护之下调用
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
 * 本文件结束: gui/window.c
 * 
**=======================================================================================*/


