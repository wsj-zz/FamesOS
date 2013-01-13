/***********************************************************************************************
 * 文件:    gdc.c
 *
 * 说明:    图形设备上下文(Graphics Device Context)
 *
 * 作者:    Jun
 *
 * 时间:    2012-12-17
***********************************************************************************************/
#define  FAMES_GDI_C
#include <includes.h>

/*----------------------------------------------------------------------------------------------
 * 
 * 变量(或常量)定义
 * 
**--------------------------------------------------------------------------------------------*/
#define MAKE_X1_Y1_X2_Y2(rect)  int X1, Y1, X2, Y2;             \
                                X1 = (rect)->x;                 \
                                Y1 = (rect)->y;                 \
                                X2 = X1 + (rect)->width - 1;    \
                                Y2 = Y1 + (rect)->height - 1;


/*----------------------------------------------------------------------------------------------
 *
 * GDC API: gdc_is_xline_visible()
 *
**--------------------------------------------------------------------------------------------*/
static GDC_value gdc_is_xline_visible_1_rect(RECT * rect, int x, int x2, int y, int * o_x1, int * o_x2);
static GDC_value gdc_is_xline_visible_sub_window(gui_window_t * w, int x, int x2, int y, int * o_x1, int * o_x2);

GDC_value __sysonly gdc_is_xline_visible(int x, int x2, int y, int * o_x1, int * o_x2)
{
    gui_window_t * myself;
    gui_widget * myself_root;
    int min_x, max_x, min_y, max_y;
    GDC_value value;

    FamesAssert(o_x1 && o_x2);
    if (!o_x1 || !o_x2)
        return GDC_none;
    
    myself = gdc_get_myself_window();
    if (!myself)
        return GDC_none;

    myself_root = myself->root_widget;
    if (!myself_root)
        return GDC_none;

    min_x = myself_root->real_rect.x;
    max_x = __gui_make_x2(min_x, myself_root->real_rect.width);
    min_y = myself_root->real_rect.y;
    max_y = __gui_make_y2(min_y, myself_root->real_rect.height);

    if (y < min_y || y > max_y)
        return GDC_none;

    if (x < min_x)
        x = min_x;
    if (x2 > max_x)
        x2 = max_x;

    if (myself->has_dirty_rect == YES) { /* dirty rect is marked */
        min_x = myself->dirty_rect.x;
        max_x = __gui_make_x2(min_x, myself->dirty_rect.width);
        min_y = myself->dirty_rect.y;
        max_y = __gui_make_y2(min_y, myself->dirty_rect.height);

        if (y < min_y || y > max_y)
            return GDC_none;

        if (x < min_x)
            x = min_x;
        if (x2 > max_x)
            x2 = max_x;
    }

    if (x > x2)
        return GDC_none;

    value = gdc_is_xline_visible_sub_window(myself->next, x, x2, y, o_x1, o_x2);

    return value;
}

static GDC_value gdc_is_xline_visible_sub_window(gui_window_t * w, int x, int x2, int y, int * o_x1, int * o_x2)
{
    int __o_x1, __o_x2;
    GDC_value v, value;

    if (!w || !w->root_widget) {
        *o_x1 = x;
        *o_x2 = x2;
        return GDC_whole;
    }

    __o_x1 = x;
    __o_x2 = x2;

    for (;;) {
        if (w->flag & WINDOW_FLAG_SHOW)
            value = gdc_is_xline_visible_1_rect(&w->root_widget->real_rect,
                                                x, x2, y, 
                                                &__o_x1, &__o_x2);
        else
            value = GDC_whole;

        switch (value) {
            case GDC_none:
                v = GDC_none;
                break;
            case GDC_whole:
                v = gdc_is_xline_visible_sub_window(w->next,
                                                    x, x2, y, 
                                                    &__o_x1, &__o_x2);
                break;
            case GDC_has_more:
                v = gdc_is_xline_visible_sub_window(w->next,
                                                    __o_x1, __o_x2, y, 
                                                    &__o_x1, &__o_x2);
                if (v == GDC_none) {
                    x = __o_x2 + 1;
                    continue; /* try again */
                }
                v = GDC_has_more;
                break;

            default: /* BUG: 不应该有这种情况 */
                v = GDC_none;
                break;
        }
        break;
    }

    if (v != GDC_none) {
        *o_x1 = __o_x1;
        *o_x2 = __o_x2;
    }

    return v;
}

static GDC_value gdc_is_xline_visible_1_rect(RECT *rect, int x, int x2, int y, int * o_x1, int * o_x2)
{
    MAKE_X1_Y1_X2_Y2(rect);

    *o_x1 = x;
    *o_x2 = x2;

    if (y < Y1 || y > Y2) {
        return GDC_whole;
    }

    if (x > x2)
        return GDC_none;

    if (x > X2)
        return GDC_whole;
    if (x2 < X1)
        return GDC_whole;
    if (x >= X1 && x2 <= X2)
        return GDC_none;
    if (x < X1) {
        *o_x2 = X1-1;
        return GDC_has_more;
    }
    if (x2 > X2) {
        *o_x1 = X2+1;
        return GDC_has_more;
    }

    return GDC_none;
}


/*----------------------------------------------------------------------------------------------
 *
 * GDC API: gdc_is_yline_visible()
 *
**--------------------------------------------------------------------------------------------*/
static GDC_value gdc_is_yline_visible_1_rect(RECT *rect, int y, int y2, int x, int * o_y1, int * o_y2);
static GDC_value gdc_is_yline_visible_sub_window(gui_window_t * w, int y, int y2, int x, int * o_y1, int * o_y2);

GDC_value __sysonly gdc_is_yline_visible(int y, int y2, int x, int * o_y1, int * o_y2)
{
    gui_window_t * myself;
    gui_widget * myself_root;
    int min_x, max_x, min_y, max_y;
    GDC_value value;

    FamesAssert(o_y1 && o_y2);
    if (!o_y1 || !o_y2)
        return GDC_none;
    
    myself = gdc_get_myself_window();
    if (!myself)
        return GDC_none;

    myself_root = myself->root_widget;
    if (!myself_root)
        return GDC_none;

    min_x = myself_root->real_rect.x;
    max_x = __gui_make_x2(min_x, myself_root->real_rect.width);
    min_y = myself_root->real_rect.y;
    max_y = __gui_make_y2(min_y, myself_root->real_rect.height);

    if (x < min_x || x > max_x)
        return GDC_none;

    if (y < min_y)
        y = min_y;
    if (y2 > max_y)
        y2 = max_y;

    if (myself->has_dirty_rect == YES) { /* dirty rect is marked */
        min_x = myself->dirty_rect.x;
        max_x = __gui_make_x2(min_x, myself->dirty_rect.width);
        min_y = myself->dirty_rect.y;
        max_y = __gui_make_y2(min_y, myself->dirty_rect.height);

        if (x < min_x || x > max_x)
            return GDC_none;

        if (y < min_y)
            y = min_y;
        if (y2 > max_y)
            y2 = max_y;
    }

    if (y > y2)
        return GDC_none;

    value = gdc_is_yline_visible_sub_window(myself->next, y, y2, x, o_y1, o_y2);

    return value;
}

static GDC_value gdc_is_yline_visible_sub_window(gui_window_t * w, int y, int y2, int x, int * o_y1, int * o_y2)
{
    int __o_y1, __o_y2;
    GDC_value v, value;

    if (!w || !w->root_widget) {
        *o_y1 = y;
        *o_y2 = y2;
        return GDC_whole;
    }

    __o_y1 = y;
    __o_y2 = y2;

    for (;;) {
        if (w->flag & WINDOW_FLAG_SHOW)
            value = gdc_is_yline_visible_1_rect(&w->root_widget->real_rect,
                                                y, y2, x, 
                                                &__o_y1, &__o_y2);
        else
            value = GDC_whole;

        switch (value) {
            case GDC_none:
                v = GDC_none;
                break;
            case GDC_whole:
                v = gdc_is_yline_visible_sub_window(w->next,
                                                    y, y2, x, 
                                                    &__o_y1, &__o_y2);
                break;
            case GDC_has_more:
                v = gdc_is_yline_visible_sub_window(w->next,
                                                    __o_y1, __o_y2, x, 
                                                    &__o_y1, &__o_y2);
                if (v == GDC_none) {
                    y = __o_y2 + 1;
                    continue;
                }
                v = GDC_has_more;
                break;

            default: /* BUG: 不应该有这种情况 */
                v = GDC_none;
                break;
        }
        break;
    }

    if (v != GDC_none) {
        *o_y1 = __o_y1;
        *o_y2 = __o_y2;
    }

    return v;
}

static GDC_value gdc_is_yline_visible_1_rect(RECT *rect, int y, int y2, int x, int * o_y1, int * o_y2)
{
    MAKE_X1_Y1_X2_Y2(rect);

    *o_y1 = y;
    *o_y2 = y2;

    if (x < X1 || x > X2) {
        return GDC_whole;
    }

    if (y > y2)
        return GDC_none;

    if (y > Y2)
        return GDC_whole;
    if (y2 < Y1)
        return GDC_whole;
    if (y >= Y1 && y2 <= Y2)
        return GDC_none;
    if (y < Y1) {
        *o_y2 = Y1-1;
        return GDC_has_more;
    }
    if (y2 > Y2) {
        *o_y1 = Y2+1;
        return GDC_has_more;
    }

    return GDC_none;
}

/*----------------------------------------------------------------------------------------------
 *
 * GDC API: gdc_is_point_visible()
 *
**--------------------------------------------------------------------------------------------*/
static int gdc_is_point_visible_in_window(gui_window_t * w, int x, int y);

GDC_value __sysonly  gdc_is_point_visible(int x, int y)
{
    gui_window_t * myself, * w;
    gui_widget * myself_root;
    int min_x, max_x, min_y, max_y;
    
    myself = gdc_get_myself_window();
    if (!myself)
        return GDC_none;

    myself_root = myself->root_widget;
    if (!myself_root)
        return GDC_none;

    min_x = myself_root->real_rect.x;
    max_x = __gui_make_x2(min_x, myself_root->real_rect.width);
    min_y = myself_root->real_rect.y;
    max_y = __gui_make_y2(min_y, myself_root->real_rect.height);

    if (x < min_x || x > max_x)
        return GDC_none;
    if (y < min_y || y > max_y)
        return GDC_none;

    if (myself->has_dirty_rect == YES) { /* dirty rect is marked */
        min_x = myself->dirty_rect.x;
        max_x = __gui_make_x2(min_x, myself->dirty_rect.width);
        min_y = myself->dirty_rect.y;
        max_y = __gui_make_y2(min_y, myself->dirty_rect.height);

        if (x < min_x || x > max_x)
            return GDC_none;
        if (y < min_y || y > max_y)
            return GDC_none;
    }

    w = myself;
    while ((w = w->next) != NULL) {
        if (gdc_is_point_visible_in_window(w, x, y))
            return GDC_none;
    }

    return GDC_whole;
}

static int gdc_is_point_visible_in_window(gui_window_t * w, int x, int y)
{
    gui_widget * root;
    int min_x, max_x, min_y, max_y;

    root = w->root_widget;
    if (!root)
        return NO;

    min_x = root->real_rect.x;
    max_x = __gui_make_x2(min_x, root->real_rect.width);
    min_y = root->real_rect.y;
    max_y = __gui_make_y2(min_y, root->real_rect.height);

    if (x >= min_x && x <= max_x &&
        y >= min_y && y <= max_y)
        return YES;

    return NO;
}

/*----------------------------------------------------------------------------------------------
 *
 * myself_window: GUI-Server正在处理的窗口
 *
**--------------------------------------------------------------------------------------------*/
static gui_window_t * myself_window = NULL;

void __sysonly gdc_set_myself_window(gui_window_t * w)
{
    lock_kernel();
    myself_window = w;
    unlock_kernel();
}

gui_window_t * __sysonly gdc_get_myself_window(void)
{
    gui_window_t * w;

    lock_kernel();
    w = myself_window;
    unlock_kernel();
    
    return w;
}

void __sysonly gdc_set_myself_window_from_widget(gui_widget * widget)
{
    gui_window_t * win = NULL;

    FamesAssert(widget);
    if (!widget)
        return;

    lock_kernel();
    win = gui_find_window_from_widget(widget);
    if (win) { /* 找到了控件widget所属的窗口 */
        myself_window = win;
    }
    unlock_kernel();
}

/*----------------------------------------------------------------------------------------------
 *
 * GDC initialize, set current gdc, get current gdc,
 *
 * These functions are not used so far, which might be for future use.
 *
**--------------------------------------------------------------------------------------------*/
static gdc_t * current_gdc;

int gdc_initialize(void)
{
    return ok;
}

int gdc_set_current(gdc_t * gdc)
{
    current_gdc = gdc;
    return ok;
}

gdc_t * gdc_get_current(void)
{
    return current_gdc;
}


/*==============================================================================================
 * 
 * 本文件结束: gdc.c
 * 
**============================================================================================*/

