/*******************************************************************************************
 * 文件: gui/root_wid.c
 *
 * 说明: 根控件(控件树的根)
 *
 * 作者: Jun
 *
 * 时间: 2010-12-22
*******************************************************************************************/
#define  FAMES_GUI_ROOT_WIDGET_C
#include "includes.h"


/*-----------------------------------------------------------------------------------------
 * 
 * 根控件指针及互斥锁
 * 
**---------------------------------------------------------------------------------------*/
static gui_widget * __root_widget = NULL;
#if 0
static int __root_widget_lock = 0;
#endif

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_root_widget_initialize()
 *
 * 描述:    根控件初始化
**---------------------------------------------------------------------------------------*/
void guical gui_root_widget_initialize(void)
{
    __root_widget = NULL;
    
    #if 0
    os_mutex_init(__root_widget_lock, 0);
    #endif
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_get_root_widget()
 *
 * 描述:    获取根控件指针
**---------------------------------------------------------------------------------------*/
gui_widget * guical gui_get_root_widget(void)
{
    #if 0
    os_mutex_lock(__root_widget_lock);
    #else
    lock_kernel();
    #endif
    return __root_widget;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_put_root_widget()
 *
 * 描述:    释放根控件指针
**---------------------------------------------------------------------------------------*/
void guical gui_put_root_widget(void)
{
    #if 0
    os_mutex_unlock(__root_widget_lock);
    #else
    unlock_kernel();
    #endif
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_set_root_widget()
 *
 * 描述:    设置根控件指针(也叫当前控件树)
 *
 * 说明:    更改根控件会引起整个屏幕的变化刷新, 屏幕会由新的控件树使用
**---------------------------------------------------------------------------------------*/
BOOL guical gui_set_root_widget(gui_widget * root_widget)
{
    FamesAssert(root_widget);

    if(!root_widget)
        return fail;
    
    gui_get_root_widget();
    __root_widget = root_widget;
    gdi_clr_screen(CLRSCR_COLOR);
    lock_kernel();
    root_widget->flag &= ~GUI_WIDGET_FLAG_REFRESH;
    unlock_kernel();
    gui_refresh_widget(root_widget);
    TimerForce(TimerGUI);
    gui_put_root_widget();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    ___gui_set_root_widget()
 *
 * 描述:    设置根控件指针(也叫当前控件树)
 *
 * 说明:    更改根控件会引起整个屏幕的变化刷新, 屏幕会由新的控件树使用
 *          此函数与gui_set_root_widget()相同, 只是并不清除屏幕.
**---------------------------------------------------------------------------------------*/
BOOL guical ___gui_set_root_widget(gui_widget * root_widget)
{
    FamesAssert(root_widget);

    if(!root_widget)
        return fail;
    
    gui_get_root_widget();
    __root_widget = root_widget;
    lock_kernel();
    root_widget->flag &= ~GUI_WIDGET_FLAG_REFRESH;
    unlock_kernel();
    gui_refresh_widget(root_widget);
    TimerForce(TimerGUI);
    gui_put_root_widget();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_root_widget_action()
 *
 * 描述:    执行根控件(即显示当前控件树)
**---------------------------------------------------------------------------------------*/
BOOL guical gui_root_widget_action(gui_widget * root_widget)
{
    if(!root_widget)
        return fail;

    return gui_draw_widget(root_widget);
}

/*=========================================================================================
 * 
 * 本文件结束: gui/root_wid.c
 * 
**=======================================================================================*/



