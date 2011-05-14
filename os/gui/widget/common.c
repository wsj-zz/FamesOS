/******************************************************************************************
 * �ļ�: gui/widget/common.c
 *
 * ����: �ؼ���������
 *
 * ����: Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_COMMON_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * ����:    gui_widget_draw_normal_bdr()
 * 
 * ����:    Ϊĳ���ؼ�������߿�
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
 * ����:    gui_widget_draw_static_bdr()
 * 
 * ����:    Ϊĳ���ؼ�����̬�߿�
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
 * ����:    gui_widget_draw_static_bdr()
 * 
 * ����:    Ϊĳ���ؼ����³��߿�
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
 * ����:    gui_widget_draw_xp_bdr()
 * 
 * ����:    Ϊĳ���ؼ���XP���ı߿�
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
 * ����:    gui_widget_draw_client_bdr()
 * 
 * ����:    Ϊĳ���ؼ����ͻ����߿�
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
 * ����:    gui_widget_draw_modal_frame()
 * 
 * ����:    Ϊĳ���ؼ���ģʽ�߿�
**---------------------------------------------------------------------------------------*/
int guical gui_widget_draw_modal_frame(int x, int y, int x1, int y1)
{
    return gui_widget_draw_normal_bdr(x, y, x1, y1);
}

/*-----------------------------------------------------------------------------------------
 * ����:    gui_widget_draw_modal_frame()
 * 
 * ����:    Ϊĳ���ؼ���ģʽ�߿�
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
 * ����:    draw_vertical_dashed_line()
 *
 * ����:    ��һ��������
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
 * ����:    draw_horizontal_dashed_line()
 *
 * ����:    ��һ��������
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
 * ����:    draw_dashed_rect()
 *
 * ����:    ��һ�����߿�
**---------------------------------------------------------------------------------------*/
void guical draw_dashed_rect(int x, int y, int x1, int y1, COLOR color, INT08U mask)
{
    draw_horizontal_dashed_line(x, y, x1, color, mask);
    draw_vertical_dashed_line(x, y, y1, color, mask);
    draw_vertical_dashed_line(x1, y, y1, color, mask);
    draw_horizontal_dashed_line(x, y1, x1, color, mask);
}

/*-----------------------------------------------------------------------------------------
 * ����:    gui_widget_draw_groupbox_bdr()
 *
 * ����:    ��һ���ؼ����
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
 * ���ļ�����: gui/widget/common.c
 * 
**=======================================================================================*/


