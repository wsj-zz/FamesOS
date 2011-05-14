/*******************************************************************************************
 * 文件: gui/root_wid.h
 *
 * 说明: 根控件(控件树的根)
 *
 * 作者: Jun
 *
 * 时间: 2010-12-22
*******************************************************************************************/
#ifndef FAMES_GUI_ROOT_WIDGET_H
#define FAMES_GUI_ROOT_WIDGET_H

/*-----------------------------------------------------------------------------------------
 * 
 *      全局根控件(所有控件树的总根)
 * 
**---------------------------------------------------------------------------------------*/
#ifdef FAMES_GUI_ROOT_WIDGET_C
gui_widget * gui_global_root_widget = NULL;
#else
extern gui_widget * gui_global_root_widget;
#endif

/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
void guical  gui_root_widget_initialize(void);
gui_widget * guical gui_get_root_widget(void);
void guical  gui_put_root_widget(void);
BOOL guical  gui_root_widget_action(gui_widget * root_widget);
BOOL guical  gui_set_root_widget(gui_widget * root_widget);
BOOL guical  ___gui_set_root_widget(gui_widget * root_widget);


#endif /* #ifndef FAMES_GUI_ROOT_WIDGET_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/root_wid.h
 * 
**=======================================================================================*/



