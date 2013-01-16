/******************************************************************************************
 * 文件:    gui/widget/dashed.c
 *
 * 描述:    画虚线控件
 *
 * 作者:    Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_DASHED_C
#include <includes.h>

/*-----------------------------------------------------------------------------------------
 * 
 *      DASHED私有结构
 * 
**---------------------------------------------------------------------------------------*/
struct gui_dashed_private_s {
    int len;
    int offset;
    INT08U dashed_mask;
};

typedef struct gui_dashed_private_s gui_dashed_private;

/*-----------------------------------------------------------------------------------------
 * 
 * 
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_dashedline_init_private(gui_widget * dashedline)
{
    int  bytes;
    INT08S * buf;
    gui_dashed_private * t, * t2;
    
    FamesAssert(dashedline);

    if(!dashedline)
        return fail;

    FamesAssert(dashedline->type == GUI_WIDGET_DASHEDLINE);

    if(dashedline->type != GUI_WIDGET_DASHEDLINE)
        return fail;

    bytes = (int)sizeof(gui_dashed_private);

    buf = (INT08S *)mem_alloc((INT32U)(INT32S)bytes);

    if(buf){
        MEMSET(buf, 0, bytes);
        t = (gui_dashed_private *)buf;/*lint !e826*/
        t->dashed_mask = 0xFF;
        if(dashedline->private_data){
            lock_kernel();
            t2 = dashedline->private_data;
            dashedline->private_data = NULL;
            unlock_kernel();
            mem_free(t2);
        }
        lock_kernel();
        dashedline->private_data = (void *)t;
        unlock_kernel();
        return ok;
    } else {
        return fail;
    }
}

BOOL guical gui_dashedline_set_param(gui_widget * dashedline, int len, int offset, INT08U dashed_mask)
{
    gui_dashed_private * t;

    FamesAssert(dashedline);

    if(!dashedline)
        return fail;

    if(dashedline->style & DASHED_STYLE_VERTICAL){
        if(len < 0 || len > dashedline->real_rect.height)
            len = dashedline->real_rect.height;
    } else {
        if(len < 0 || len > dashedline->real_rect.width)
            len = dashedline->real_rect.width;
    }
    
    lock_kernel();
    t = (gui_dashed_private *)dashedline->private_data;
    if(!t){
        unlock_kernel();
        return fail;
    }
    t->len = len;
    t->offset = offset;
    t->dashed_mask = dashed_mask;
    unlock_kernel();

    gui_refresh_widget(dashedline);

    return ok;
}

void gui_draw_dashedline(gui_widget * dashedline)
{
/*lint --e{534}*/
    int x, y, x1, y1, real_x, len, i;
    INT08U mask;
    COLOR bkcolor, color;
    gui_dashed_private * t;

    FamesAssert(dashedline);

    if(!dashedline)
        return;

    if(dashedline->flag & GUI_WIDGET_FLAG_NEED_REFRESH){
        t = (gui_dashed_private *)dashedline->private_data;
        if(!t)
            return;
        len = t->len;
        mask = t->dashed_mask;
        color = dashedline->color;
        bkcolor = dashedline->bkcolor;
        if(bkcolor==0)
            bkcolor = WIDGET_BKCOLOR;
        x  = dashedline->real_rect.x;
        y  = dashedline->real_rect.y;
        x1 = __gui_make_x2(x, dashedline->real_rect.width);
        y1 = __gui_make_y2(y, dashedline->real_rect.height);
        if(dashedline->style & DASHED_STYLE_VERTICAL){
            real_x = y;
            if(dashedline->style & DASHED_STYLE_CENTER){
                real_x += (dashedline->real_rect.height-len)/2;
            } else if(dashedline->style & DASHED_STYLE_RIGHT){
                real_x += (dashedline->real_rect.height-len);
            }
            gdi_draw_box(x, y, (x1), (y1), bkcolor);
            x += t->offset;
            i = ((INT16U)len>>3);
            while(i--){
                gdi_draw_v_bitmap(x, real_x, mask, color);
                real_x += 8;
            }
            i = (len&7);
            mask &= (0xFF<<(8-i));
            gdi_draw_v_bitmap(x, real_x, mask, color);
        } else {
            real_x = x;
            if(dashedline->style & DASHED_STYLE_CENTER){
                real_x += (dashedline->real_rect.width-len)/2;
            } else if(dashedline->style & DASHED_STYLE_RIGHT){
                real_x += (dashedline->real_rect.width-len);
            }
            gdi_draw_box(x, y, (x1), (y1), bkcolor);
            y += t->offset;
            i = ((INT16U)len>>3);
            while(i--){
                gdi_draw_h_bitmap(real_x, y, 8, mask, color);
                gdi_draw_h_bitmap(real_x, y, 8, mask, color);
                real_x += 8;
            }
            i = (len&7);
            gdi_draw_h_bitmap(real_x, y, i, mask, color);
        }
    } else {
        ; /* 只有在刷新时才会重画 */
    }
        
    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/dashed.c
 * 
**=======================================================================================*/


