/******************************************************************************************
 * 文件:    gui/widget/groupbox.h
 *
 * 描述:    组控件
 *
 * 作者:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_GROUPBOX_H
#define FAMES_GUI_WIDGET_GROUPBOX_H


/*-----------------------------------------------------------------------------------------
 * 
 *      GROUPBOX风格
 * 
**---------------------------------------------------------------------------------------*/
#define GROUPBOX_STYLE_NO_BORDER        0x0001       /* 无边框     */
#define GROUPBOX_STYLE_FLAT_BDR         0x0002       /* 平坦       */
#define GROUPBOX_STYLE_CAPTION          0x0004       /* 标题       */
#define GROUPBOX_STYLE_TRANSPARENT      0x0008       /* 透明       */

/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_groupbox_init_private(gui_widget * groupbox, int caption_len);
BOOL  guical  gui_groupbox_set_caption(gui_widget * groupbox, INT08S * caption);
void          gui_draw_groupbox(gui_widget * groupbox);
INT16U        gui_groupbox_get_property(gui_widget * groupbox);


#endif /* #ifndef FAMES_GUI_WIDGET_GROUPBOX_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/widget/groupbox.h
 * 
**=======================================================================================*/


