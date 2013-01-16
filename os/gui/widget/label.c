/******************************************************************************************
 * 文件:    gui/widget/label.c
 *
 * 描述:    标签控件
 *
 * 作者:    Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_LABEL_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * 
 *      LABEL私有结构
 * 
**---------------------------------------------------------------------------------------*/
struct gui_label_private_s {
    INT08S  * text;
    INT08S  * text_old;
    int       len;
};

typedef struct gui_label_private_s gui_label_private;

/*-----------------------------------------------------------------------------------------
 * 
 * 
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_label_init_private(gui_widget * label, int text_len)
{
    int  bytes;
    INT08S * buf;
    gui_label_private * t, * t2;
    
    FamesAssert(label);
    FamesAssert(text_len > 0);

    if(!label || text_len <= 0)
        return fail;

    FamesAssert(label->type == GUI_WIDGET_LABEL);

    if(label->type != GUI_WIDGET_LABEL)
        return fail;

    bytes = (text_len+2)*2 + (int)sizeof(gui_label_private);

    buf = (INT08S *)mem_alloc((INT32U)(INT32S)bytes);

    if(buf){
        MEMSET(buf, 0, bytes);
        t = (gui_label_private *)buf;/*lint !e826*/
        t->text = buf+sizeof(gui_label_private);
        t->text_old = t->text + text_len + 2;
        t->len  = text_len;
        t->text[0] = 0;
        t->text_old[0] = 0;
        if(label->private_data){
            lock_kernel();
            t2 = label->private_data;
            label->private_data = NULL;
            unlock_kernel();
            mem_free(t2);
        }
        lock_kernel();
        label->private_data = (void *)t;
        unlock_kernel();
        return ok;
    } else {
        return fail;
    }
}

BOOL guical gui_label_set_text(gui_widget * label, INT08S * text)
{
    gui_label_private * t;
    int len;

    FamesAssert(label);
    FamesAssert(text);

    if(!label || !text)
        return fail;

    lock_kernel();
    t = (gui_label_private *)label->private_data;
    unlock_kernel();
    if(!t)
        return fail;

    len = STRLEN(text);
    if(len > t->len)
        len = t->len-1;

    lock_kernel();
    MEMCPY(t->text, text, len);
    t->text[len] = 0;
    unlock_kernel();
    
    return ok;
}

void gui_draw_label(gui_widget * label)
{
/*lint --e{534}*/
    int x, y, x1, y1, move;
    COLOR bkcolor;
    gui_label_private * t;
    RECT * inner_rect;

    FamesAssert(label);

    if(!label)
        return;

    t = (gui_label_private *)label->private_data;
    if(!t)
        return;
    
    inner_rect = &label->inner_rect;
    bkcolor = label->bkcolor;
    if(bkcolor==0)
        bkcolor = WIDGET_BKCOLOR;
    
    if(label->flag & GUI_WIDGET_FLAG_NEED_REFRESH){
        x  = label->real_rect.x;
        y  = label->real_rect.y;
        x1 = __gui_make_x2(x, label->real_rect.width);
        y1 = __gui_make_y2(y, label->real_rect.height);
        if(label->style & LABEL_STYLE_XP_BORDER){
            move = gui_widget_draw_xp_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(label->style & LABEL_STYLE_MODAL_FRAME){
            move = gui_widget_draw_modal_frame(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(label->style & LABEL_STYLE_CLIENT_BDR){
            move = gui_widget_draw_client_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(label->style & LABEL_STYLE_BORDER){
            move = gui_widget_draw_static_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(label->style & LABEL_STYLE_SUBSIDE){
            move = gui_widget_draw_subside_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(label->style & LABEL_STYLE_TRANSPARENT){
            ;/* Transparent, do nothing here */
        } else if(!gdi_draw_box(x, y, x1, y1, bkcolor)){
            ;
        }
        x+=2; x1-=2; y+=1;
        gui_init_rect(inner_rect, x, y, (x1-x), (y1-y));
        t->text_old[0] = 0;
    } else {
        INT16U __opt;

        __opt = DRAW_OPT_FIL_BG;
        if(label->style & LABEL_ALIGN_CENTER)
            __opt |= DRAW_OPT_ALIGN_CENTER;
        if(label->style & LABEL_ALIGN_RIGHT)
            __opt |= DRAW_OPT_ALIGN_RIGHT;

        draw_font_for_widget(inner_rect->x, inner_rect->y, inner_rect->width, inner_rect->height, 
                             t->text, t->text_old, label->color, bkcolor, label->font, __opt);
    }

    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/label.c
 * 
**=======================================================================================*/


