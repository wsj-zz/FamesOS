/******************************************************************************************
 * �ļ�:    gui/widget/groupbox.h
 *
 * ����:    ��ؼ�
 *
 * ����:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_GROUPBOX_H
#define FAMES_GUI_WIDGET_GROUPBOX_H


/*-----------------------------------------------------------------------------------------
 * 
 *      GROUPBOX���
 * 
**---------------------------------------------------------------------------------------*/
#define GROUPBOX_STYLE_NO_BORDER        0x0001       /* �ޱ߿�     */
#define GROUPBOX_STYLE_FLAT_BDR         0x0002       /* ƽ̹       */
#define GROUPBOX_STYLE_CAPTION          0x0004       /* ����       */
#define GROUPBOX_STYLE_TRANSPARENT      0x0008       /* ͸��       */

/*-----------------------------------------------------------------------------------------
 * 
 *      ��������
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_groupbox_init_private(gui_widget * groupbox, int caption_len);
BOOL  guical  gui_groupbox_set_caption(gui_widget * groupbox, INT08S * caption);
void          gui_draw_groupbox(gui_widget * groupbox);
INT16U        gui_groupbox_get_property(gui_widget * groupbox);


#endif /* #ifndef FAMES_GUI_WIDGET_GROUPBOX_H */

/*=========================================================================================
 * 
 * ���ļ�����: gui/widget/groupbox.h
 * 
**=======================================================================================*/


