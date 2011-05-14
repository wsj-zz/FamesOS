/*******************************************************************************************
 * �ļ�: gui/root_wid.c
 *
 * ˵��: ���ؼ�(�ؼ����ĸ�)
 *
 * ����: Jun
 *
 * ʱ��: 2010-12-22
*******************************************************************************************/
#define  FAMES_GUI_ROOT_WIDGET_C
#include "includes.h"


/*-----------------------------------------------------------------------------------------
 * 
 * ���ؼ�ָ�뼰������
 * 
**---------------------------------------------------------------------------------------*/
static gui_widget * __root_widget = NULL;
#if 0
static int __root_widget_lock = 0;
#endif

/*-----------------------------------------------------------------------------------------
 * ����:    gui_root_widget_initialize()
 *
 * ����:    ���ؼ���ʼ��
**---------------------------------------------------------------------------------------*/
void guical gui_root_widget_initialize(void)
{
    __root_widget = NULL;
    
    #if 0
    os_mutex_init(__root_widget_lock, 0);
    #endif
}

/*-----------------------------------------------------------------------------------------
 * ����:    gui_get_root_widget()
 *
 * ����:    ��ȡ���ؼ�ָ��
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
 * ����:    gui_put_root_widget()
 *
 * ����:    �ͷŸ��ؼ�ָ��
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
 * ����:    gui_set_root_widget()
 *
 * ����:    ���ø��ؼ�ָ��(Ҳ�е�ǰ�ؼ���)
 *
 * ˵��:    ���ĸ��ؼ�������������Ļ�ı仯ˢ��, ��Ļ�����µĿؼ���ʹ��
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
 * ����:    ___gui_set_root_widget()
 *
 * ����:    ���ø��ؼ�ָ��(Ҳ�е�ǰ�ؼ���)
 *
 * ˵��:    ���ĸ��ؼ�������������Ļ�ı仯ˢ��, ��Ļ�����µĿؼ���ʹ��
 *          �˺�����gui_set_root_widget()��ͬ, ֻ�ǲ��������Ļ.
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
 * ����:    gui_root_widget_action()
 *
 * ����:    ִ�и��ؼ�(����ʾ��ǰ�ؼ���)
**---------------------------------------------------------------------------------------*/
BOOL guical gui_root_widget_action(gui_widget * root_widget)
{
    if(!root_widget)
        return fail;

    return gui_draw_widget(root_widget);
}

/*=========================================================================================
 * 
 * ���ļ�����: gui/root_wid.c
 * 
**=======================================================================================*/



