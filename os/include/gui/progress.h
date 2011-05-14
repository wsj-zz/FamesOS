/******************************************************************************************
 * �ļ�: gui/widget/progress.h
 *
 * ����: �������ؼ�
 *
 * ����: Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_PROGRESS_H
#define FAMES_GUI_WIDGET_PROGRESS_H

/*-----------------------------------------------------------------------------------------
 * 
 *      PROGRESS�ؼ�������������
 * 
**---------------------------------------------------------------------------------------*/
struct progress_assoc_s {
    int  full;
    int  curr;
};

typedef struct progress_assoc_s progress_assoc_t;

/*-----------------------------------------------------------------------------------------
 * 
 *      PROGRESS���
 * 
**---------------------------------------------------------------------------------------*/
#define PROGRESS_STYLE_BORDER           0x0001       /* �߿�       */
#define PROGRESS_STYLE_CLIENT_BDR       0x0002       /* CLIENT�߿� */
#define PROGRESS_STYLE_MODAL_FRAME      0x0004       /* ģʽ��     */
#define PROGRESS_STYLE_XP_BORDER        0x0010       /* XP����   */
#define PROGRESS_STYLE_SMOOTH           0x0020       /* ����       */
#define PROGRESS_ALIGN_VERTICAL         0x0100       /* �����     */


/*-----------------------------------------------------------------------------------------
 * 
 *      ��������
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_progress_init_private(gui_widget * progress, int full_value);
BOOL guical gui_progress_set_value(gui_widget * progress, int value);
void        gui_draw_progress(gui_widget * progress);




#endif /* #ifndef FAMES_GUI_WIDGET_PROGRESS_H */

/*=========================================================================================
 * 
 * ���ļ�����: gui/widget/progress.h
 * 
**=======================================================================================*/


