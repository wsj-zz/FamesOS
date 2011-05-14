/******************************************************************************************
 * 文件:    gui/widget/groupbox.c
 *
 * 描述:    组控件
 *
 * 作者:    Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_GROUPBOX_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * 
 *      GROUPBOX私有结构
 * 
**---------------------------------------------------------------------------------------*/
struct gui_groupbox_private_s {
    INT08S  * caption;
    int       len;
};

typedef struct gui_groupbox_private_s gui_groupbox_private;

/*-----------------------------------------------------------------------------------------
 * 
 * 
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_groupbox_init_private(gui_widget * groupbox, int caption_len)
{
    int  bytes;
    INT08S * buf;
    gui_groupbox_private * t, * t2;
    
    FamesAssert(groupbox);
    FamesAssert(caption_len > 0);

    if(!groupbox || caption_len <= 0)
        return fail;

    FamesAssert(groupbox->type == GUI_WIDGET_GROUPBOX);

    if(groupbox->type != GUI_WIDGET_GROUPBOX)
        return fail;

    bytes = (caption_len+2) + (int)sizeof(gui_groupbox_private);

    buf = (INT08S *)mem_alloc((INT32U)(INT32S)bytes);

    if(buf){
        MEMSET(buf, 0, bytes);
        t = (gui_groupbox_private *)buf;/*lint !e826*/
        t->caption = buf+sizeof(gui_groupbox_private);
        t->len  = caption_len;
        t->caption[0] = 0;
        if(groupbox->private_data){
            lock_kernel();
            t2 = groupbox->private_data;
            groupbox->private_data = NULL;
            unlock_kernel();
            mem_free(t2);
        }
        lock_kernel();
        groupbox->private_data = (void *)t;
        unlock_kernel();
        return ok;
    } else {
        return fail;
    }
}

BOOL guical gui_groupbox_set_caption(gui_widget * groupbox, INT08S * caption)
{
    gui_groupbox_private * t;
    int len;

    FamesAssert(groupbox);
    FamesAssert(caption);

    if(!groupbox || !caption)
        return fail;

    lock_kernel();
    t = (gui_groupbox_private *)groupbox->private_data;
    unlock_kernel();
    if(!t)
        return fail;

    len = STRLEN(caption);
    if(len > t->len)
        len = t->len-1;

    lock_kernel();
    MEMCPY(t->caption, caption, len);
    t->caption[len] = 0;
    unlock_kernel();

    gui_refresh_widget(groupbox);
    
    return ok;
}

void gui_draw_groupbox(gui_widget * groupbox)
{
/*lint --e{534}*/
    int x, y, x1, y1, move;
    COLOR bkcolor;
    gui_groupbox_private * t;

    FamesAssert(groupbox);

    if(!groupbox)
        return;

    t = (gui_groupbox_private *)groupbox->private_data;
    if(!t)
        return;
    
    bkcolor = groupbox->bkcolor;
    if(bkcolor==0)
        bkcolor = WIDGET_BKCOLOR;
    
    if(groupbox->flag & GUI_WIDGET_FLAG_REFRESH){
        x  = groupbox->real_rect.x;
        y  = groupbox->real_rect.y;
        x1 = groupbox->real_rect.width + x;
        y1 = groupbox->real_rect.height + y;

        if(groupbox->style & GROUPBOX_STYLE_CAPTION)
            y += 8;

        if(groupbox->style & GROUPBOX_STYLE_TRANSPARENT){
            goto out;
        }
        if(groupbox->style & GROUPBOX_STYLE_FLAT_BDR){
            gdi_draw_rect(x, y, x1, y1, COLOR_BLACK);
            ___gui_widget_xy_move(1);
            gdi_draw_rect(x, y, x1, y1, COLOR_WHITE);
            ___gui_widget_xy_move(1);
        } else {
            move = gui_widget_draw_groupbox_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        }
        if(groupbox->style & GROUPBOX_STYLE_CAPTION){
            draw_font_ex(x+12, (y-8), 1, 16, t->caption, groupbox->color, bkcolor, groupbox->font, DRAW_OPT_FIL_BG);
        } 
    }

out:
    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/groupbox.c
 * 
**=======================================================================================*/


