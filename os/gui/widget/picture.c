/******************************************************************************************
 * 文件: gui/widget/picture.c
 *
 * 描述: 图片控件
 *
 * 作者: Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_PICTURE_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 *
 *      PICTURE私有结构
 *
**---------------------------------------------------------------------------------------*/
struct gui_picture_private_s {
    BMPINFO bmpinfo;
};

typedef struct gui_picture_private_s gui_picture_private;


/*-----------------------------------------------------------------------------------------
 * 函数:    gui_picture_init_private()
 *
 * 描述:    图片控件私有结构初始化
**---------------------------------------------------------------------------------------*/
BOOL guical gui_picture_init_private(gui_widget * pic)
{
    gui_picture_private * t;
    INT08S * buf;
    int  nbytes;
    
    FamesAssert(pic);

    if(!pic)
        return fail;

    FamesAssert(pic->type == GUI_WIDGET_PICTURE);

    if(pic->type != GUI_WIDGET_PICTURE)
        return fail;

    nbytes = sizeof(gui_picture_private);

    buf = (INT08S *)mem_alloc((INT32U)(INT32S)nbytes);

    if(buf){
        MEMSET(buf, 0, nbytes);
        t = (gui_picture_private *)buf;/*lint !e826*/
        InitBMPINFO(&t->bmpinfo);
        lock_kernel();
        pic->private_data = (void *)t;
        unlock_kernel();
        return ok;
    } else {
        return fail;
    }
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_picture_set_picture()
 *
 * 描述:    设置图片控件的图片
**---------------------------------------------------------------------------------------*/
BOOL guical gui_picture_set_picture(gui_widget * pic, BMPINFO * bmpinfo)
{
    gui_picture_private * t;
    BOOL retval;

    FamesAssert(pic);
    FamesAssert(bmpinfo);

    if(!pic || !bmpinfo)
        return fail;

    lock_kernel();
    t = (gui_picture_private *)pic->private_data;
    if(t){
        t->bmpinfo = *bmpinfo;
        gui_refresh_widget(pic);
        retval = ok;
    } else {
        retval = fail;
    }
    unlock_kernel();
    
    gui_set_widget_changed(pic);

    return retval;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_picture_get_property()
 *
 * 描述:    返回PICTURE的控件特性
**---------------------------------------------------------------------------------------*/
INT16U gui_picture_get_property(gui_widget * pic)
{
    if (gui_is_widget_changed(pic))
        return GUI_WIDGET_PROP_REFRESH_DIRTY;

    return GUI_WIDGET_PROP_NONE;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_draw_picture()
 *
 * 描述:    画一个图片控件
**---------------------------------------------------------------------------------------*/
void gui_draw_picture(gui_widget * pic)
{
    int x, y, x1, y1, move;
    COLOR bkcolor;
    gui_picture_private * t;

    FamesAssert(pic);

    if(!pic)
        return;

    if(pic->flag & GUI_WIDGET_FLAG_NEED_REFRESH){
        x  = pic->real_rect.x;
        y  = pic->real_rect.y;
        x1 = __gui_make_x2(x, pic->real_rect.width);
        y1 = __gui_make_y2(y, pic->real_rect.height);
        bkcolor = pic->bkcolor;
        if(bkcolor==0)
            bkcolor = WIDGET_BKCOLOR;
        if(pic->style & PICTURE_STYLE_TRANSPARENT){
            goto goto1;
        }        
        if(pic->style & PICTURE_STYLE_MODAL_FRAME){
            move = gui_widget_draw_modal_frame(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(pic->style & PICTURE_STYLE_CLIENT_BDR){
            move = gui_widget_draw_client_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(pic->style & PICTURE_STYLE_STATIC_BDR){
            move = gui_widget_draw_static_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(pic->style & PICTURE_STYLE_NO_BORDER){
            ;/* NO_BORDER */
        } else if(pic->style & PICTURE_STYLE_BORDER2){
            move = gui_widget_draw_normal_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } else {
            move = gui_widget_draw_subside_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        
        }
        if(!gdi_draw_box(x, y, x1, y1, bkcolor)){
            ;
        }
    goto1:
        t = (gui_picture_private *)pic->private_data;
        if(t){
            SetBmpRect(&t->bmpinfo, (x1-x)+1, (y1-y)+1);
            ShowBmp(x, y, &t->bmpinfo);
        }
    } else {
        ;
    }

    gui_clr_widget_changed(pic);

    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/picture.c
 * 
**=======================================================================================*/


