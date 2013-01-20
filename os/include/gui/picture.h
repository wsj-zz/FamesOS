/******************************************************************************************
 * �ļ�: gui/widget/picture.h
 *
 * ����: ͼƬ�ؼ�
 *
 * ����: Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_PICTURE_H
#define FAMES_GUI_WIDGET_PICTURE_H


/*-----------------------------------------------------------------------------------------
 * 
 *      PICTURE���
 * 
**---------------------------------------------------------------------------------------*/
#define PICTURE_STYLE_NO_BORDER        0x0001       /* �ޱ߿�     */
#define PICTURE_STYLE_BORDER2          0x0002       /* ��һ�ֱ߿� */
#define PICTURE_STYLE_STATIC_BDR       0x0004       /* ��̬�߿�   */
#define PICTURE_STYLE_CLIENT_BDR       0x0008       /* CLIENT�߿� */
#define PICTURE_STYLE_MODAL_FRAME      0x0010       /* ģʽ��     */
#define PICTURE_STYLE_TRANSPARENT      0x0020       /* ͸��       */


/*-----------------------------------------------------------------------------------------
 * 
 *      ��������
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_picture_init_private(gui_widget * pic);
BOOL  guical  gui_picture_set_picture(gui_widget * pic, BMPINFO * bmpinfo);
void          gui_draw_picture(gui_widget * pic);
INT16U        gui_picture_get_property(gui_widget * pic);




#endif /* #ifndef FAMES_GUI_WIDGET_PICTURE_H */

/*=========================================================================================
 * 
 * ���ļ�����: gui/widget/picture.h
 * 
**=======================================================================================*/


