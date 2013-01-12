/***********************************************************************************************
** �ļ�:    gdi.h
**
** ˵��:    ͼ���豸�ӿ�(GDI)
**
** ����:    Jun
**
** ʱ��:    2010-03-03
***********************************************************************************************/
#ifndef FAMES_GDI_H
#define FAMES_GDI_H

/*----------------------------------------------------------------------------------------------
 *
 * GDI���ÿ���
 *
**--------------------------------------------------------------------------------------------*/
#define FAMES_GDI_EN         1           /* �Ƿ�Ҫ����GDI����, 1=��       */
#define FAMES_GDI_NOP()                  /* NON-OPERATION in GDI          */ 

/*----------------------------------------------------------------------------------------------
 *
 * GDI����(����/����)
 *
**--------------------------------------------------------------------------------------------*/
#if     FAMES_GDI_EN == 1

/*----------------------------------------------------------------------------------------------
 * ͼ�δ���ر�, ��ɫ�������
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
 * ��, ��
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
 * ����ͼ��: ����, Բ��
**--------------------------------------------------------------------------------------------*/
INT16S gdi_draw_rect(INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color);
INT16S gdi_draw_box(INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color);
INT16S gdi_draw_line(INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color);


/*----------------------------------------------------------------------------------------------
 * ����
**--------------------------------------------------------------------------------------------*/



#else
/*----------------------------------------------------------------------------------------------
 * ������ʹgdi�ຯ��ȫ��ʧЧ
**--------------------------------------------------------------------------------------------*/


#endif /* FAMES_GDI_EN==1 */

#endif /* FAMES_GDI_H     */


/*==============================================================================================
 * 
 * ���ļ�����: gdi.h
 * 
**============================================================================================*/


