/******************************************************************************************
 * 文件: gui.c
 *
 * 说明: gui模块, 接口
 *
 * 作者: Jun
 *
 * 时间: 2010-11-11
******************************************************************************************/
#define  FAMES_GUI_C
#include "includes.h"


void __exit OnExitGUI(void);

/*-----------------------------------------------------------------------------------------
 * 函数:    InitGUI()
 *
 * 说明:    GUI模块初始化
**---------------------------------------------------------------------------------------*/
void guical __init InitGUI(void)
{
    BOOL retval;

    retval = RegisterOnExit(OnExitGUI);
    if(!retval)
        goto out;

    InitWindowManager();
    retval = gui_service_initialize();
    if(!retval)
        goto out2;

    retval = gui_widget_pool_init();
    if(!retval)
        goto out1;

    gui_initialize_usr_widget();

    if(!init_xms_service()){
        printf("InitGUI: init_xms_service failed\n");
        getch();
        ExitApplication();
    }
    load_sys_font();

    gui_desktop_init(); /* Setup the Desktop Window */
    
out:
    return;
    
out1:
    TaskDelete(gui_service_get_handle());
out2:
    DeregisterOnExit(OnExitGUI);
    goto out;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    OnExitGUI()
 *
 * 说明:    GUI模块退出例程
**---------------------------------------------------------------------------------------*/
void __exit OnExitGUI(void)
{
    HANDLE htask;
    CALLED_ONLY_ONCE();

    lock_kernel();
    htask = gui_service_get_handle();

    if(htask != InvalidHandle){
        TaskDelete(htask);
    }
    unlock_kernel();

    gui_widget_pool_free();

    ExitWindowManager();

    gdi_stop_graph();
}

/*-----------------------------------------------------------------------------------------
 * 函数:    QuitGUI()
 *
 * 说明:    退出GUI模块
**---------------------------------------------------------------------------------------*/
void guical QuitGUI(void)
{
    if(DeregisterOnExit(OnExitGUI)){
        OnExitGUI();
    }
}

/*-----------------------------------------------------------------------------------------
 * 函数:    StartGUI()
 *
 * 说明:    启动GUI服务
**---------------------------------------------------------------------------------------*/
void guical StartGUI(void)
{
    HANDLE htask;

    gdi_open_graph();

    htask = gui_service_get_handle();

    if(htask != InvalidHandle){
        TaskResume(htask);
    } else {
        sys_print("Fatal: There is no GUI-SERVICE daemon!\n");
        waitkey(0);
        ExitApplication();
    }
}


/*=========================================================================================
 * 
 * 本文件结束: gui.c
 * 
**=======================================================================================*/


