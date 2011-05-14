/***********************************************************************************************
** 文件:    gdi.h
**
** 说明:    图形设备接口(GDI)
**
** 作者:    Jun
**
** 时间:    2010-03-03
***********************************************************************************************/
#ifndef FAMES_GDI_H
#define FAMES_GDI_H

/*----------------------------------------------------------------------------------------------
 * 
 * GDI启用开关
 * 
**--------------------------------------------------------------------------------------------*/
#define FAMES_GDI_EN         1           /* 是否要生成GDI代码, 1=是       */
#define FAMES_GDI_NOP()                  /* NON-OPERATION in GDI          */ 

/*----------------------------------------------------------------------------------------------
 * 
 * GDI例程(声明/定义)
 * 
**--------------------------------------------------------------------------------------------*/
#if     FAMES_GDI_EN == 1

/*----------------------------------------------------------------------------------------------
 * 图形打开与关闭, 调色板操作等
**--------------------------------------------------------------------------------------------*/
#define gdi_graph_opened() (SCREEN.opened==TRUE)
#define gdi_open_graph()    X_OPEN_GRAPH()
#define gdi_stop_graph()    X_CLOSE_GRAPH()
#define gdi_set_palette(index, red, green, blue)  \
                            X_SET_PALETTE((index), (red), (green), (blue))
#define gdi_set_palette_default()  \
                            X_SET_PALETTE_DEFAULT()
#define gdi_clr_screen(color)  \
                            X_CLEAR_SCREEN((COLOR)(color))
/*----------------------------------------------------------------------------------------------
 * 点, 线
**--------------------------------------------------------------------------------------------*/
#define gdi_put_pixel(x,y,color)  \
                            X_PUT_PIXEL((x), (y), (COLOR)(color))
#define gdi_get_pixel(x,y)  \
                            X_GET_PIXEL((x), (y))                            
#define gdi_draw_h_line(x,y,x2,color)  \
                            X_DRAW_H_LINE((x), (y), (x2), (COLOR)(color))
#define gdi_draw_v_line(x,y,y2,color)  \
                            X_DRAW_V_LINE((x), (y), (y2), (COLOR)(color))
#define gdi_draw_h_image(x,y,x2,colors)  \
                            X_DRAW_H_IMAGE((x), (y), (x2), (colors))
#define gdi_draw_h_bitmap(x,y,dots,bitmap,color)  \
                            X_DRAW_H_BITMAP((x), (y), (dots), (bitmap), (color))
#define gdi_draw_h_bitmap_bg(x,y,dots,bitmap,color,bgcolor)  \
                            X_DRAW_H_BITMAP_BG((x), (y), (dots), (bitmap), (color), (bgcolor)) 
#define gdi_draw_h_bitmap_bgmap(x,y,dots,bitmap,color,bgcolors)  \
                            X_DRAW_H_BITMAP_BGMAP((x), (y), (dots), (bitmap), (color), (bgcolors)) 
#define gdi_draw_v_image(x,y,y2,colors)  \
                            X_DRAW_V_IMAGE((x), (y), (y2), (colors))
#define gdi_draw_v_bitmap(x,y,bitmap,color)  \
                            X_DRAW_V_BITMAP((x), (y), (bitmap), (color))
#define gdi_draw_v_bitmap_bg(x,y,bitmap,color,bgcolor)  \
                            X_DRAW_V_BITMAP_BG((x), (y), (bitmap), (color), (bgcolor)) 
#define gdi_draw_v_bitmap_bgmap(x,y,bitmap,color,bgcolors)  \
                            X_DRAW_V_BITMAP_BGMAP((x), (y), (bitmap), (color), (bgcolors)) 

/*----------------------------------------------------------------------------------------------
 * 基本图形: 矩形, 圆等
**--------------------------------------------------------------------------------------------*/
#define gdi_draw_rect(x,y,x2,y2,color)  \
                            X_DRAW_RECT((x),(y),(x2),(y2),(color))
#define gdi_draw_box(x,y,x2,y2,color)  \
                            X_DRAW_BOX((x),(y),(x2),(y2),(color))

/*----------------------------------------------------------------------------------------------
 * 其它
**--------------------------------------------------------------------------------------------*/



#else
/*----------------------------------------------------------------------------------------------
 * 在这里使gdi类函数全部失效
**--------------------------------------------------------------------------------------------*/


#endif /* FAMES_GDI_EN==1 */

#endif /* FAMES_GDI_H     */


/*==============================================================================================
 * 
 * 本文件结束: gdi.h
 * 
**============================================================================================*/


