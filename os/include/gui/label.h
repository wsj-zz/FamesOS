/******************************************************************************************
 * 文件:    gui/widget/label.h
 *
 * 描述:    标签控件
 *
 * 作者:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_LABEL_H
#define FAMES_GUI_WIDGET_LABEL_H


/*-----------------------------------------------------------------------------------------
 * 
 *      LABEL风格
 * 
**---------------------------------------------------------------------------------------*/
#define LABEL_STYLE_BORDER           0x0001       /* 边框       */
#define LABEL_STYLE_SUBSIDE          0x0002       /* 下沉       */
#define LABEL_STYLE_CLIENT_BDR       0x0004       /* CLIENT边框 */
#define LABEL_STYLE_MODAL_FRAME      0x0008       /* 模式框     */
#define LABEL_STYLE_XP_BORDER        0x0010       /* XP风格框   */
#define LABEL_STYLE_TRANSPARENT      0x0020       /* 透明       */
#define LABEL_ALIGN_LEFT             0x0100       /* 左对齐     */
#define LABEL_ALIGN_CENTER           0x0200       /* 居中       */
#define LABEL_ALIGN_RIGHT            0x0400       /* 右对齐     */


/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_label_init_private(gui_widget * label, int text_len);
BOOL  guical  gui_label_set_text(gui_widget * label, INT08S * text);
void          gui_draw_label(gui_widget * label);




#endif /* #ifndef FAMES_GUI_WIDGET_LABEL_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/widget/label.h
 * 
**=======================================================================================*/


