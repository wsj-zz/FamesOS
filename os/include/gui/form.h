/******************************************************************************************
 * �ļ�: gui/widget/form.h
 *
 * ����: ����ؼ�
 *
 * ����: Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_FORM_H
#define FAMES_GUI_WIDGET_FORM_H


/*-----------------------------------------------------------------------------------------
 * 
 *      FORM���
 * 
**---------------------------------------------------------------------------------------*/
#define FORM_STYLE_TITLE            0x0001       /* ������     */
#define FORM_STYLE_NO_BORDER        0x0002       /* �ޱ߿�     */
#define FORM_STYLE_THIN_BDR         0x0004       /* С�߿�     */
#define FORM_STYLE_CLIENT_BDR       0x0008       /* CLIENT�߿� */
#define FORM_STYLE_XP_BORDER        0x0010       /* XP���߿� */
#define FORM_STYLE_ROUND_ANGLE      0x0080       /* Բ��       */
#define FORM_STYLE_TRANSPARENT      0x0100       /* ͸��       */

/*-----------------------------------------------------------------------------------------
 * 
 *      ��������
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_form_init_private(gui_widget * form, int cap_len);
BOOL  guical  gui_form_set_caption(gui_widget * form, INT08S * caption);
BOOL  guical  gui_form_set_icon(gui_widget * form, BMPINFO * icon);
void          gui_draw_form(gui_widget * form);
INT16U        gui_form_get_property(gui_widget * form);




#endif /* #ifndef FAMES_GUI_WIDGET_FORM_H */

/*=========================================================================================
 * 
 * ���ļ�����: gui/widget/form.h
 * 
**=======================================================================================*/


