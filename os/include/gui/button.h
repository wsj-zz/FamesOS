/******************************************************************************************
 * �ļ�:    gui/widget/button.h
 *
 * ����:    ��ť�ؼ�
 *
 * ����:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_BUTTON_H
#define FAMES_GUI_WIDGET_BUTTON_H


/*-----------------------------------------------------------------------------------------
 * 
 *      BUTTON���
 * 
**---------------------------------------------------------------------------------------*/
#define BUTTON_STYLE_FOCUS_BORDER     0x0001       /* �߿�       */
#define BUTTON_STYLE_CLIENT_BDR       0x0002       /* CLIENT�߿� */
#define BUTTON_STYLE_MODAL_FRAME      0x0004       /* ģʽ��     */
#define BUTTON_STYLE_PRESSED          0x0010       /* ����״̬   */

/*-----------------------------------------------------------------------------------------
 * 
 *      ��������
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_button_init_private(gui_widget * button, int text_len);
BOOL guical gui_button_set_caption(gui_widget * button, INT08S * caption);
void        gui_draw_button(gui_widget * button);
INT16U      gui_button_get_property(gui_widget * button);




#endif /* #ifndef FAMES_GUI_WIDGET_BUTTON_H */

/*=========================================================================================
 * 
 * ���ļ�����: gui/widget/button.h
 * 
**=======================================================================================*/


