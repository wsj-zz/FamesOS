/******************************************************************************************
 * 文件: gui/widget/progress.c
 *
 * 描述: 进度条控件
 *
 * 作者: Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_PROGRESS_C
#include "includes.h"


/*-----------------------------------------------------------------------------------------
 * 
 *      PROGRESS私有结构
 * 
**---------------------------------------------------------------------------------------*/
struct gui_progress_private_s {
    int   full_value;
    int   current;
    int   old_value;
};


typedef struct gui_progress_private_s gui_progress_private;

/*-----------------------------------------------------------------------------------------
 * 
 * 
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_progress_init_private(gui_widget * progress, int full_value)
{
    int  bytes;
    INT08S * buf;
    gui_progress_private * t, * t2;

    FamesAssert(progress);
    FamesAssert(full_value > 0);

    if(!progress || full_value <= 0)
        return fail;

    FamesAssert(progress->type == GUI_WIDGET_PROGRESS);

    if(progress->type != GUI_WIDGET_PROGRESS)
        return fail;

    bytes = sizeof(gui_progress_private);

    buf = (INT08S *)mem_alloc((INT32U)(INT32S)bytes);

    if(buf){
        MEMSET(buf, 0, bytes);
        t = (gui_progress_private *)buf;/*lint !e826*/
        t->full_value = full_value;
        t->current    = 0;
        t->old_value  = -1;
        if(progress->private_data){
            lock_kernel();
            t2 = progress->private_data;
            progress->private_data = NULL;
            unlock_kernel();
            mem_free(t2);
        }
        lock_kernel();
        progress->private_data = (void *)t;
        unlock_kernel();
        return ok;
    } else {
        return fail;
    }
}

BOOL guical gui_progress_set_value(gui_widget * progress, int value)
{
    gui_progress_private * t;

    FamesAssert(progress);
    FamesAssert(value >= 0);

    if(!progress || value < 0)
        return fail;

    lock_kernel();
    t = (gui_progress_private *)progress->private_data;
    unlock_kernel();
    if(!t)
        return fail;

    lock_kernel();
    t->current = value;
    unlock_kernel();

    gui_set_widget_changed(progress);

    return ok;
}

INT16U gui_progress_get_property(gui_widget * progress)
{
    if (gui_is_widget_changed(progress))
        return GUI_WIDGET_PROP_REFRESH_DIRTY;

    return GUI_WIDGET_PROP_NONE;
}

void gui_draw_progress(gui_widget * progress)
{
/*lint --e{534}*/
    int x, y, x1, y1, move;
    COLOR bkcolor, color;
    gui_progress_private * t;
    RECT * inner_rect;
    void __internal guical __gui_progress_draw_progress(int x, int y, int x1, int y1,
                                                 COLOR color, COLOR bkcolor, 
                                                 int full, int curr, int old, INT16U style);

    FamesAssert(progress);

    if(!progress)
        return;

    t = (gui_progress_private *)progress->private_data;
    if(!t)
        return;

    inner_rect = &progress->inner_rect;
    x  = progress->real_rect.x;
    y  = progress->real_rect.y;
    x1 = __gui_make_x2(x, progress->real_rect.width);
    y1 = __gui_make_y2(y, progress->real_rect.height);
    color   = progress->color;
    if(color==0)
        color = WIDGET_PROGRESS_COLOR;
    bkcolor = progress->bkcolor;
    if(bkcolor==0)
        bkcolor = WIDGET_BKCOLOR;

    if(progress->flag & GUI_WIDGET_FLAG_NEED_REFRESH){
        if(progress->style & PROGRESS_STYLE_XP_BORDER){
            move = gui_widget_draw_xp_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        }
        if(progress->style & PROGRESS_STYLE_MODAL_FRAME){
            move = gui_widget_draw_modal_frame(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        }
        if(progress->style & PROGRESS_STYLE_CLIENT_BDR){
            move = gui_widget_draw_client_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        }
        if(progress->style & PROGRESS_STYLE_BORDER){
            move = gui_widget_draw_static_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } else {
            move = gui_widget_draw_subside_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        }
        if(!gdi_draw_box(x, y, x1, y1, bkcolor)){
            ;
        }
        if(progress->style & PROGRESS_STYLE_SMOOTH)
            gui_init_rect(inner_rect, x+1, y+1, (x1-x)-2, (y1-y)-2);
        else
            gui_init_rect(inner_rect, x+1, y+1, (x1-x)-2, (y1-y)-2);
        t->old_value = -1;
    } else {
        x = inner_rect->x;
        y = inner_rect->y;
        if(t->old_value != t->current)
            __gui_progress_draw_progress(x, y, (x+inner_rect->width), (y+inner_rect->height), 
                                         color, bkcolor, t->full_value, t->current, t->old_value, progress->style);
        t->old_value = t->current;
    }

    return;
}

void __internal guical __gui_progress_draw_progress(int x, int y, int x1, int y1,
                                                 COLOR color, COLOR bkcolor, 
                                                 int full, int curr, int old,
                                                 INT16U style)
{
/*lint --e{534}*/
    int  x_dots, y_dots, width;
    int  __blocks, i;
    long __cur, __old;

    if(curr < 0)
        goto out;
    if(curr > full)
        goto out;
    
    x_dots = x1-x;
    y_dots = y1-y;

    if(x_dots<=0 || y_dots<=0 || full<=0)
        goto out;
    
    __cur  = (long)curr;
    __cur *= x_dots;
    __cur /= full;
    __old  = (long)old;
    __old *= x_dots;
    __old /= full;
    if(old<0)__old=-1L;
    
    if(style & PROGRESS_STYLE_SMOOTH){
        if(old < curr){
            gdi_draw_box(x, y, x+(int)__cur, y1, color);        
        } else if(old > curr){
            gdi_draw_box(x+(int)__cur, y, x1, y1, bkcolor);
        }
    } else {
        width  = y_dots*2/3; /* 进度条小颗粒的纵横比: 1.5 */
        __blocks = (int)(__cur/(width+2));/*lint !e776: Possible truncation of addition*/
        if(__blocks == (int)(__old/(width+2)))/*lint !e776*/
            return;
        for(i=0; i<__blocks; i++){
            gdi_draw_v_line(x, y, y1, bkcolor);
            x++;
            gdi_draw_box(x, y, x+width, y1, color);
            x+=width;
            gdi_draw_v_line(x, y, y1, bkcolor);
            x++;
        }
        if(x<x1)
            gdi_draw_box(x, y, x1, y1, bkcolor);
    }
    return;

out:
    return;
}
/*=========================================================================================
 * 
 * 本文件结束: gui/widget/progress.c
 * 
**=======================================================================================*/


