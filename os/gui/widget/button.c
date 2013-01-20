/******************************************************************************************
 * 文件:    gui/widget/button.c
 *
 * 描述:    按钮控件
 *
 * 作者:    Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_BUTTON_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * 
 *      BUTTON私有结构
 * 
**---------------------------------------------------------------------------------------*/
struct gui_button_private_s {
    INT08S * caption;
    INT08S * caption_old;
    int      len;
};

typedef struct gui_button_private_s gui_button_private;

/*-----------------------------------------------------------------------------------------
 * 
 * 
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_button_init_private(gui_widget * button, int text_len)
{
    int  bytes;
    INT08S * buf;
    gui_button_private * t, * t2;
    
    FamesAssert(button);
    FamesAssert(text_len > 0);

    if(!button || text_len <= 0)
        return fail;

    FamesAssert(button->type == GUI_WIDGET_BUTTON);

    if(button->type != GUI_WIDGET_BUTTON)
        return fail;

    bytes = (text_len+2)*2 + (int)sizeof(gui_button_private);

    buf = (INT08S *)mem_alloc((INT32U)(INT32S)bytes);

    if(buf){
        MEMSET(buf, 0, bytes);
        t = (gui_button_private *)buf;/*lint !e826*/
        t->caption = buf+sizeof(gui_button_private);
        t->caption_old = t->caption + text_len + 2;
        t->len  = text_len;
        t->caption[0] = 0;
        t->caption_old[0] = 0;
        if(button->private_data){
            lock_kernel();
            t2 = button->private_data;
            button->private_data = NULL;
            unlock_kernel();
            mem_free(t2);
        }
        lock_kernel();
        button->private_data = (void *)t;
        unlock_kernel();
        return ok;
    } else {
        return fail;
    }
}

BOOL guical gui_button_set_caption(gui_widget * button, INT08S * caption)
{
    gui_button_private * t;
    int len;
    BOOL retval;

    FamesAssert(button);
    FamesAssert(caption);

    if(!button || !caption)
        return fail;

    retval = fail;

    lock_kernel();
    t = (gui_button_private *)button->private_data;
    if(!t)
        goto out;

    len = STRLEN(caption);
    if(len >= t->len)
        len = t->len-1;

    MEMCPY(t->caption, caption, len);
    t->caption[len] = 0;
    unlock_kernel();
    retval = ok;

    gui_set_widget_changed(button);

out:
    return retval;
}

INT16U gui_button_get_property(gui_widget * button)
{
    if (gui_is_widget_changed(button))
        return GUI_WIDGET_PROP_REFRESH_DIRTY;

    return GUI_WIDGET_PROP_NONE;
}

void gui_draw_button(gui_widget * button)
{
/*lint --e{534}*/
    int x, y, x1, y1, move;
    COLOR bkcolor;
    gui_button_private * t;
    RECT * inner_rect;

    FamesAssert(button);

    if(!button)
        return;

    t = (gui_button_private *)button->private_data;
    if(!t)
        return;
    
    inner_rect = &button->inner_rect;
    bkcolor = button->bkcolor;
    if(bkcolor==0)
        bkcolor = WIDGET_BKCOLOR;
    
    if(button->flag & GUI_WIDGET_FLAG_NEED_REFRESH){
        x  = button->real_rect.x;
        y  = button->real_rect.y;
        x1 = __gui_make_x2(x, button->real_rect.width);
        y1 = __gui_make_y2(y, button->real_rect.height);
        if(button->style & BUTTON_STYLE_CLIENT_BDR){
            move = gui_widget_draw_client_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
            gdi_draw_rect(x, y, x1, y1, WIDGET_BKCOLOR);
            ___gui_widget_xy_move(1);
            gdi_draw_rect(x, y, x1, y1, WIDGET_BKCOLOR);
            ___gui_widget_xy_move(1);
        } 
        if(button->style & BUTTON_STYLE_MODAL_FRAME){
            move = gui_widget_draw_modal_frame(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(button->style & BUTTON_STYLE_PRESSED){
            move = 0;  
            gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR3);
            gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR3);
            x++, y++;            
            gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR4);
            gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR4);
            gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR2);
            gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR2);
            x++, y++, x1--; y1--;
            gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR5);
            gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR5);
            gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR1);
            gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR1);
            x++, y++, x1--; y1--;
            ___gui_widget_xy_move(move);
            bkcolor = WIDGET_COLOR_NORMAL_BDR4;
            goto goto1;
        }
        move = gui_widget_draw_normal_bdr(x, y, x1, y1);
        ___gui_widget_xy_move(move);
        if(button->style & BUTTON_STYLE_FOCUS_BORDER){
            gdi_draw_rect(x, y, x1, y1, bkcolor);
            ___gui_widget_xy_move(1);
            #if 1
            move = gui_widget_draw_static_bdr(x, y, x1, y1);
            #else
            gdi_draw_rect(x, y, x1, y1, bkcolor);
            draw_dashed_rect(x, y, x1, y1, COLOR_BLACK, 0xAA);
            #endif
            ___gui_widget_xy_move(1);
        }
        goto1:
        if(!gdi_draw_box(x, y, x1, y1, bkcolor)){
            ;
        }
        gui_init_rect(inner_rect, x, y, (x1-x)+1, (y1-y)+1);
        t->caption_old[0] = 0;
    }

    if (maybe_second_step(button->flag)) {
        if(button->style & BUTTON_STYLE_PRESSED)
            bkcolor = WIDGET_COLOR_NORMAL_BDR4;
        draw_font_for_widget(inner_rect->x, inner_rect->y, inner_rect->width, inner_rect->height, 
                             t->caption, t->caption_old, button->color, bkcolor, button->font, DRAW_OPT_FIL_BG|DRAW_OPT_ALIGN_CENTER);

        gui_clr_widget_changed(button);
    }
        
    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/button.c
 * 
**=======================================================================================*/


