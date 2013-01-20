/******************************************************************************************
 * 文件: gui/widget/form.h
 *
 * 描述: 窗体控件
 *
 * 作者: Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_FORM_H
#define FAMES_GUI_WIDGET_FORM_H


/*-----------------------------------------------------------------------------------------
 * 
 *      FORM风格
 * 
**---------------------------------------------------------------------------------------*/
#define FORM_STYLE_TITLE            0x0001       /* 标题栏     */
#define FORM_STYLE_NO_BORDER        0x0002       /* 无边框     */
#define FORM_STYLE_THIN_BDR         0x0004       /* 小边框     */
#define FORM_STYLE_CLIENT_BDR       0x0008       /* CLIENT边框 */
#define FORM_STYLE_XP_BORDER        0x0010       /* XP风格边框 */
#define FORM_STYLE_ROUND_ANGLE      0x0080       /* 圆角       */
#define FORM_STYLE_TRANSPARENT      0x0100       /* 透明       */

/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
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
 * 本文件结束: gui/widget/form.h
 * 
**=======================================================================================*/


