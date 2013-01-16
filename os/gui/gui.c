/******************************************************************************************
 * �ļ�: gui.c
 *
 * ˵��: guiģ��, �ӿ�
 *
 * ����: Jun
 *
 * ʱ��: 2010-11-11
******************************************************************************************/
#define  FAMES_GUI_C
#include "includes.h"


void __exit OnExitGUI(void);

/*-----------------------------------------------------------------------------------------
 * ����:    InitGUI()
 *
 * ˵��:    GUIģ���ʼ��
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
 * ����:    OnExitGUI()
 *
 * ˵��:    GUIģ���˳�����
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
 * ����:    QuitGUI()
 *
 * ˵��:    �˳�GUIģ��
**---------------------------------------------------------------------------------------*/
void guical QuitGUI(void)
{
    if(DeregisterOnExit(OnExitGUI)){
        OnExitGUI();
    }
}

/*-----------------------------------------------------------------------------------------
 * ����:    StartGUI()
 *
 * ˵��:    ����GUI����
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
 * ���ļ�����: gui.c
 * 
**=======================================================================================*/


