/******************************************************************************************
 * 文件: gui/widget/form.c
 *
 * 描述: 窗体控件
 *
 * 作者: Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_FORM_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * 
 *      FORM私有结构
 * 
**---------------------------------------------------------------------------------------*/
struct gui_form_private_s {
    INT08S  * caption;
    INT08S  * caption_old;
    int       cap_len;
    int       x_start;
    BMPINFO   icon;
    int       icon_y_start;
};

typedef struct gui_form_private_s gui_form_private;

/*-----------------------------------------------------------------------------------------
 * 
 * 
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_form_init_private(gui_widget * form, int cap_len)
{
    int  bytes;
    INT08S * buf;
    gui_form_private * t, * t2;
    
    FamesAssert(form);
    FamesAssert(cap_len > 0);

    if(!form || cap_len <= 0)
        return fail;

    FamesAssert(form->type == GUI_WIDGET_FORM);

    if(form->type != GUI_WIDGET_FORM)
        return fail;

    bytes = (cap_len+2)*2  + (int)sizeof(gui_form_private);

    buf = (INT08S *)mem_alloc((INT32U)(INT32S)bytes);

    if(buf){
        MEMSET(buf, 0, bytes);
        t = (gui_form_private *)buf;/*lint !e826*/
        t->caption = buf+sizeof(gui_form_private);
        t->caption_old = t->caption + cap_len + 2;
        t->cap_len = cap_len;
        t->caption[0] = 0;
        t->caption_old[0] = 0;
        InitBMPINFO(&t->icon);
        if(form->private_data){
            lock_kernel();
            t2 = form->private_data;
            form->private_data = NULL;
            unlock_kernel();
            mem_free(t2);
        }
        lock_kernel();
        form->private_data = (void *)t;
        unlock_kernel();
        return ok;
    } else {
        return fail;
    }
}

BOOL guical gui_form_set_icon(gui_widget * form, BMPINFO * icon)
{
    gui_form_private * t;
    BOOL retval;

    FamesAssert(form);
    FamesAssert(icon);

    if(!form || !icon)
        return fail;

    lock_kernel();
    t = (gui_form_private *)form->private_data;
    if(t){
        t->icon = *icon;
        t->x_start = icon->width;
        t->icon_y_start = (25-icon->height)/2+4; /* FIXME: 这里可能会有问题(直接用25去减???) */
        gui_refresh_widget(form);
        retval = ok;
    } else {
        retval = fail;
    }
    unlock_kernel();
    
    return retval;
}

BOOL guical gui_form_set_caption(gui_widget * form, INT08S * caption)
{
    gui_form_private * t;
    int  len;
    BOOL retval;

    FamesAssert(form);
    FamesAssert(caption);

    if(!form || !caption)
        return fail;

    lock_kernel();
    t = (gui_form_private *)form->private_data;
    if(t){
        len = STRLEN(caption);
        if(len >= t->cap_len)
            len = t->cap_len-1;
        MEMCPY(t->caption, caption, len);
        t->caption[len] = 0;
        retval = ok;
        gui_set_widget_changed(form);
    } else {
        retval = fail;
    }
    unlock_kernel();

    return retval;
}

INT16U gui_form_get_property(gui_widget * form)
{
    if (gui_is_widget_changed(form))
        return GUI_WIDGET_PROP_REFRESH_DIRTY;

    return GUI_WIDGET_PROP_NONE;
}

void gui_draw_form(gui_widget * form)
{
/*lint --e{534}*/
    int x, y, x1, y1, move;
    COLOR bkcolor;
    gui_form_private * t;
    RECT * inner_rect;

    FamesAssert(form);

    if(!form)
        return;
    
    t = (gui_form_private *)form->private_data;
    if(!t)
        return;
    
    inner_rect = &form->inner_rect;
    bkcolor = form->bkcolor;
    if(bkcolor==0)
        bkcolor = WIDGET_BKCOLOR;
    
    if(form->flag & GUI_WIDGET_FLAG_NEED_REFRESH){
        x  = form->real_rect.x;
        y  = form->real_rect.y;
        x1 = __gui_make_x2(x, form->real_rect.width);
        y1 = __gui_make_y2(y, form->real_rect.height);
        if(form->style & FORM_STYLE_XP_BORDER){
            move = gui_widget_draw_xp_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
            if(form->style & FORM_STYLE_TITLE){
                gdi_draw_box(x, y, x1-1, y+24, WIDGET_COLOR_TITLE_XP);
                gdi_draw_v_line(x1, y, y+25, WIDGET_COLOR_BDR_XP_23);
                y+=25;
                gdi_draw_h_line(x, y, x1-1, WIDGET_COLOR_BDR_XP_21);
                y++;
            }
        } else if(form->style & FORM_STYLE_NO_BORDER){
            ;/* 无边框 */
        } else if(form->style & FORM_STYLE_THIN_BDR){
            move = gui_widget_draw_static_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
            if(form->style & FORM_STYLE_CLIENT_BDR){
                move = gui_widget_draw_client_bdr(x, y, x1, y1);
                ___gui_widget_xy_move(move);
            }
        } else {/* 常规边框 */
            move = gui_widget_draw_normal_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
            if(form->style & FORM_STYLE_CLIENT_BDR){
                move = gui_widget_draw_client_bdr(x, y, x1, y1);
                ___gui_widget_xy_move(move);
            }            
            if(form->style & FORM_STYLE_TITLE){
                gdi_draw_box(x, y, x1, y+20, WIDGET_COLOR_TITLE_NORMAL);
                y+=21;
                gdi_draw_h_line(x, y++, x1, WIDGET_COLOR_NORMAL_BDR3);
                gdi_draw_h_line(x, y++, x1, WIDGET_COLOR_NORMAL_BDR4);
                gdi_draw_h_line(x, y++, x1, WIDGET_COLOR_NORMAL_BDR5);
            }
        }
        if(form->style & FORM_STYLE_TRANSPARENT){
            ;/* Transparent, do nothing here */
        } else if(!gdi_draw_box(x, y, x1, y1, bkcolor)){
            ;
        }
        gui_init_rect(inner_rect, x, y, (x1-x)+1, (y1-y)+1);
        t->caption_old[0] = 0;
        if(form->style & FORM_STYLE_TITLE){
            ShowBmp(x+2, form->real_rect.y+t->icon_y_start, &t->icon);
        }
    } else {
        if(form->style & FORM_STYLE_TITLE){
            if(form->style & FORM_STYLE_XP_BORDER)
                bkcolor = WIDGET_COLOR_TITLE_XP;
            else
                bkcolor = WIDGET_COLOR_TITLE_NORMAL;
            draw_font_for_widget((inner_rect->x+4)+t->x_start, form->real_rect.y+4, 
                                 (inner_rect->width-8)-t->x_start, 26, 
                                  t->caption, t->caption_old, COLOR_WHITE, bkcolor, form->font, DRAW_OPT_FIL_BG);
        }
    }

    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/form.c
 * 
**=======================================================================================*/


