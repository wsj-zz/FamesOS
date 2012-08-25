/******************************************************************************************
 * �ļ�: gui/widget/edit.h
 *
 * ����: �ı��༭�ؼ�
 *
 * ����: Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_EDIT_H
#define FAMES_GUI_WIDGET_EDIT_H


/*-----------------------------------------------------------------------------------------
 * 
 *      EDIT���
 * 
**---------------------------------------------------------------------------------------*/
#define EDIT_STYLE_NO_BORDER        0x0001       /* �ޱ߿�     */
#define EDIT_STYLE_STATIC_BDR       0x0002       /* ��̬�߿�   */
#define EDIT_STYLE_CLIENT_BDR       0x0004       /* CLIENT�߿� */
#define EDIT_STYLE_MODAL_FRAME      0x0008       /* ģʽ��     */
#define EDIT_STYLE_XP_BORDER        0x0010       /* XP����   */
#define EDIT_STYLE_TRANSPARENT      0x0020       /* ͸��       */
#define EDIT_STYLE_EDITING          0x0040       /* ���ڱ༭   */
#define EDIT_ALIGN_LEFT             0x0100       /* �����     */
#define EDIT_ALIGN_CENTER           0x0200       /* ����       */
#define EDIT_ALIGN_RIGHT            0x0400       /* �Ҷ���     */
#define EDIT_STYLE_READONLY         0x1000       /* ֻ��       */
#define EDIT_STYLE_PASSWORD         0x2000       /* ��������� */
#define EDIT_STYLE_IN_VIEW          0x4000       /* VIEW�༭�� */

/*-----------------------------------------------------------------------------------------
 * 
 *      EDIT����������־
 * 
**---------------------------------------------------------------------------------------*/
#define EDIT_ASSOC_FLAG_INT08     0x00010000    /* ���������Ǹ� 8λ���� */
#define EDIT_ASSOC_FLAG_INT16     0x00020000    /* ���������Ǹ�16λ���� */
#define EDIT_ASSOC_FLAG_INT32     0x00030000    /* ���������Ǹ�32λ���� */
#define EDIT_ASSOC_FLAG_INT64     0x00040000    /* ���������Ǹ�64λ���� */

/*-----------------------------------------------------------------------------------------
 * 
 *      ��������
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_edit_init_private(gui_widget * edit, int text_len);
BOOL  guical  gui_edit_set_text(gui_widget * edit, INT08S * text);
BOOL  guical  gui_edit_get_text(gui_widget * edit, INT08S * text);
void          gui_draw_edit(gui_widget * edit);
BOOL  guical  gui_edit_blink_on(gui_widget * edit, int speed, int index, INT16U opt);
BOOL  guical  gui_edit_blink_off(gui_widget * edit, INT16U opt);
BOOL  guical  gui_edit_blink_set(gui_widget * edit, int index, INT16U opt);

/*-----------------------------------------------------------------------------------------
 * ����:    gui_edit_input()
 *
 * ����:    @edit    the edit widget which will perform input
 *          @buf     the output buffer, should be of sufficient size
 *          @bytes   the max number of bytes can be entered (<=128)
 *          @option  not used currently
 *
 * ����:    EDIT�ؼ�����, ���128���ַ�
**---------------------------------------------------------------------------------------*/
KEYCODE     gui_edit_input(gui_widget * edit, char * __BUF buf, int bytes, INT16U option);




#endif /* #ifndef FAMES_GUI_WIDGET_EDIT_H */

/*=========================================================================================
 * 
 * ���ļ�����: gui/widget/edit.h
 * 
**=======================================================================================*/


