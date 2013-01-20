/******************************************************************************************
 * 文件:    gui/widget/usr_wid.h
 *
 * 说明:    自定义控件类型的注册
 *
 * 作者:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_USR_WIDGET_H
#define FAMES_GUI_USR_WIDGET_H


/*-----------------------------------------------------------------------------------------
 * 
 *      控件类型结构
 *
 * widget_id   控件的类型ID号(从256开始)
 * draw        画控件的方法
 *
**---------------------------------------------------------------------------------------*/
struct gui_widget_type_s {
    int    widget_id;
    void (*draw)(gui_widget * c);
    struct gui_widget_type_s *next;
};

typedef struct gui_widget_type_s gui_widget_type;

#define GUI_WIDGET_USR_START_ID  256  /* 用户自定义控件的起始ID号   */
#define GUI_WIDGET_USR_ENDED_ID  2560 /* 到达这个ID就不再允许注册了 */


/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
void  guical gui_initialize_usr_widget(void);
int   guical gui_register_usr_widget(gui_widget_type *);
BOOL  guical gui_deregister_usr_widget(int widget_id);

/*-----------------------------------------------------------------------------------------
 * 
 *      内部函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL __internal guical __gui_draw_usr_widget(gui_widget *);
INT16U  __gui_usr_widget_get_property(gui_widget * c);


#endif /* #ifndef FAMES_GUI_USR_WIDGET_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/widget/usr_wid.h
 * 
**=======================================================================================*/


