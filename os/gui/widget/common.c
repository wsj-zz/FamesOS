/******************************************************************************************
 * 文件: gui/widget/common.c
 *
 * 描述: 控件公共例程
 *
 * 作者: Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_COMMON_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_draw_normal_bdr()
 * 
 * 描述:    为某个控件画常规边框
**---------------------------------------------------------------------------------------*/
int guical gui_widget_draw_normal_bdr(int x, int y, int x1, int y1)
{
/*lint --e{534}*/
    int move = 0;
    
    gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR1);
    gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR1);
    gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR5);
    gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR5);
    x++, y++, x1--; y1--, move++;
    gdi_draw_h_line(x,  y,  x1, COLOR_WHITE); /* WIDGET_COLOR_NORMAL_BDR2 */
    gdi_draw_v_line(x,  y,  y1, COLOR_WHITE);
    gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR4);
    gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR4);
    x++, y++, x1--; y1--, move++;
    
    return (move);
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_draw_static_bdr()
 * 
 * 描述:    为某个控件画静态边框
**---------------------------------------------------------------------------------------*/
int guical gui_widget_draw_static_bdr(int x, int y, int x1, int y1)
{
/*lint --e{534}*/
    int move = 0;
    
    gdi_draw_rect(x, y, x1, y1, COLOR_BLACK);
    move++;    
    
    return (move);
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_draw_static_bdr()
 * 
 * 描述:    为某个控件画下沉边框
**---------------------------------------------------------------------------------------*/
int guical gui_widget_draw_subside_bdr(int x, int y, int x1, int y1)
{
/*lint --e{534}*/
    int move = 0;
    
    gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR4);
    gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR4);
    gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR2);
    gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR2);
    move++;    
    
    return (move);
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_draw_xp_bdr()
 * 
 * 描述:    为某个控件画XP风格的边框
**---------------------------------------------------------------------------------------*/
int guical gui_widget_draw_xp_bdr(int x, int y, int x1, int y1)
{
/*lint --e{534}*/
    int move = 0;
    
    gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_BDR_XP_11);
    gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_BDR_XP_11);
    gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_BDR_XP_21);
    gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_BDR_XP_21);
    x++, y++, x1--; y1--, move++;
    gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_BDR_XP_12);
    gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_BDR_XP_12);
    gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_BDR_XP_22);
    gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_BDR_XP_22);
    x++, y++, x1--; y1--, move++;
    gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_BDR_XP_13);
    gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_BDR_XP_13);
    gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_BDR_XP_23);
    gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_BDR_XP_23);
    x++, y++, x1--; y1--, move++;
    
    return (move);
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_draw_client_bdr()
 * 
 * 描述:    为某个控件画客户机边框
**---------------------------------------------------------------------------------------*/
int guical gui_widget_draw_client_bdr(int x, int y, int x1, int y1)
{
/*lint --e{534}*/
    int move = 0;
    
    gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR3);
    gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR3);
    gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR3);
    gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR3);
    x++, y++, x1--; y1--, move++;
    gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR4);
    gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR4);
    gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR2);
    gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR2);
    x++, y++, x1--; y1--, move++;
    gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR5);
    gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR5);
    gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR1);
    gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR1);
    x++, y++, x1--; y1--, move++;
    
    return (move);
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_draw_modal_frame()
 * 
 * 描述:    为某个控件画模式边框
**---------------------------------------------------------------------------------------*/
int guical gui_widget_draw_modal_frame(int x, int y, int x1, int y1)
{
    return gui_widget_draw_normal_bdr(x, y, x1, y1);
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_draw_modal_frame()
 * 
 * 描述:    为某个控件画模式边框
**---------------------------------------------------------------------------------------*/
BOOL guical gui_init_rect(RECT * rect, int x, int y, int width, int height)
{
    if(!rect)
        return fail;

    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    draw_vertical_dashed_line()
 *
 * 描述:    画一条竖虚线
**---------------------------------------------------------------------------------------*/
void guical draw_vertical_dashed_line(int x, int y, int y1, COLOR color, INT08U mask)
{
    int  __len, v;

    __len = (y1-y)+1;

    v = (INT16U)__len >> 3;

    while(v--){
        gdi_draw_v_bitmap(x, y, mask, color);
        y += 8;
    }
    
    v = __len & 7;

    if(v){
        mask &= (0xFF << (8-v));
        gdi_draw_v_bitmap(x, y, mask, color);
    }
}

/*-----------------------------------------------------------------------------------------
 * 函数:    draw_horizontal_dashed_line()
 *
 * 描述:    画一条横虚线
**---------------------------------------------------------------------------------------*/
void guical draw_horizontal_dashed_line(int x, int y, int x1, COLOR color, INT08U mask)
{
    int  __len, v;

    __len = (x1-x)+1;

    v = (INT16U)__len >> 3;

    while(v--){
        gdi_draw_h_bitmap(x, y, 8, mask, color);
        x += 8;
    }
    
    v = __len & 7;

    if(v){
        gdi_draw_h_bitmap(x, y, v, mask, color);
    }
}

/*-----------------------------------------------------------------------------------------
 * 函数:    draw_dashed_rect()
 *
 * 描述:    画一个虚线框
**---------------------------------------------------------------------------------------*/
void guical draw_dashed_rect(int x, int y, int x1, int y1, COLOR color, INT08U mask)
{
    draw_horizontal_dashed_line(x, y, x1, color, mask);
    draw_vertical_dashed_line(x, y, y1, color, mask);
    draw_vertical_dashed_line(x1, y, y1, color, mask);
    draw_horizontal_dashed_line(x, y1, x1, color, mask);
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_draw_groupbox_bdr()
 *
 * 描述:    画一个控件组框
**---------------------------------------------------------------------------------------*/
int guical gui_widget_draw_groupbox_bdr(int x, int y, int x1, int y1)
{
/*lint --e{534}*/
    int move = 0;
    
    gdi_draw_rect(x, y, x1, y1, COLOR_BLACK);
    x++, y++, x1--; y1--, move++;
    gdi_draw_rect(x, y, x1, y1, COLOR_WHITE);
    x++, y++, x1--; y1--, move++;
    
    return (move);
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/common.c
 * 
**=======================================================================================*/


