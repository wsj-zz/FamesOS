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
#define gdi_get_screen_size(w, h)  \
                            X_GET_SCREEN_SIZE((w), (h))

/*----------------------------------------------------------------------------------------------
 * 点, 线
**--------------------------------------------------------------------------------------------*/
INT16S gdi_put_pixel(INT16S x, INT16S y, COLOR color);
COLOR  gdi_get_pixel(INT16S x, INT16S y);
INT16S gdi_draw_h_line(INT16S x, INT16S y, INT16S x2, COLOR color);
INT16S gdi_draw_v_line(INT16S x, INT16S y, INT16S y2, COLOR color);
INT16S gdi_draw_h_image(INT16S x, INT16S y, INT16S x2, COLOR colors[]);
INT16S gdi_draw_h_bitmap(INT16S x, INT16S y, INT16S dots, INT08U bitmap, COLOR color);
INT16S gdi_draw_h_bitmap_bg(INT16S x, INT16S y, INT16S dots, INT08U bitmap, COLOR color, COLOR  bgcolor);
INT16S gdi_draw_h_bitmap_bgmap(INT16S x, INT16S y, INT16S dots, INT08U bitmap, COLOR color, COLOR bgcolors[]);
INT16S gdi_draw_v_image(INT16S x, INT16S y, INT16S y2, COLOR colors[]);
INT16S gdi_draw_v_bitmap(INT16S x, INT16S y, INT08U bitmap, COLOR color);
INT16S gdi_draw_v_bitmap_bg(INT16S x, INT16S y, INT08U bitmap, COLOR color, COLOR bgcolor);
INT16S gdi_draw_v_bitmap_bgmap(INT16S x, INT16S y, INT08U bitmap, COLOR color, COLOR bgcolors[]);


/*----------------------------------------------------------------------------------------------
 * 基本图形: 矩形, 圆等
**--------------------------------------------------------------------------------------------*/
INT16S gdi_draw_rect(INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color);
INT16S gdi_draw_box(INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color);
INT16S gdi_draw_line(INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color);


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


