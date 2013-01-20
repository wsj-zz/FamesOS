/******************************************************************************************
 * 文件:    gui/widget/dashed.h
 *
 * 描述:    画虚线控件
 *
 * 作者:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_DASHED_H
#define FAMES_GUI_WIDGET_DASHED_H


/*-----------------------------------------------------------------------------------------
 * 
 *      DASHED风格
 * 
**---------------------------------------------------------------------------------------*/
#define DASHED_STYLE_VERTICAL     0x0001       /* 垂直画线     */
#define DASHED_STYLE_CENTER       0x0002       /* 居中         */
#define DASHED_STYLE_RIGHT        0x0004       /* 靠右         */

/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_dashedline_init_private(gui_widget * dashedline);
BOOL guical gui_dashedline_set_param(gui_widget * dashedline, int len, int offset, INT08U dashed_mask);
void        gui_draw_dashedline(gui_widget * dashedline);
INT16U      gui_dashedline_get_property(gui_widget * dashedline);




#endif /* #ifndef FAMES_GUI_WIDGET_DASHED_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/widget/dashed.h
 * 
**=======================================================================================*/


