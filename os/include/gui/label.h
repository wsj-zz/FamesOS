/******************************************************************************************
 * �ļ�:    gui/widget/label.h
 *
 * ����:    ��ǩ�ؼ�
 *
 * ����:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_LABEL_H
#define FAMES_GUI_WIDGET_LABEL_H


/*-----------------------------------------------------------------------------------------
 * 
 *      LABEL���
 * 
**---------------------------------------------------------------------------------------*/
#define LABEL_STYLE_BORDER           0x0001       /* �߿�       */
#define LABEL_STYLE_SUBSIDE          0x0002       /* �³�       */
#define LABEL_STYLE_CLIENT_BDR       0x0004       /* CLIENT�߿� */
#define LABEL_STYLE_MODAL_FRAME      0x0008       /* ģʽ��     */
#define LABEL_STYLE_XP_BORDER        0x0010       /* XP����   */
#define LABEL_STYLE_TRANSPARENT      0x0020       /* ͸��       */
#define LABEL_ALIGN_LEFT             0x0100       /* �����     */
#define LABEL_ALIGN_CENTER           0x0200       /* ����       */
#define LABEL_ALIGN_RIGHT            0x0400       /* �Ҷ���     */


/*-----------------------------------------------------------------------------------------
 * 
 *      ��������
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_label_init_private(gui_widget * label, int text_len);
BOOL  guical  gui_label_set_text(gui_widget * label, INT08S * text);
void          gui_draw_label(gui_widget * label);




#endif /* #ifndef FAMES_GUI_WIDGET_LABEL_H */

/*=========================================================================================
 * 
 * ���ļ�����: gui/widget/label.h
 * 
**=======================================================================================*/


