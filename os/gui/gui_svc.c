/******************************************************************************************
 * �ļ�: gui_svc.c
 *
 * ˵��: gui������
 *
 * ����: Jun
 *
 * ʱ��: 2010-11-11
******************************************************************************************/
#define  FAMES_GUI_SERVICE_C
#include "includes.h"


/*-----------------------------------------------------------------------------------------
 * 
 * GUI���������
 * 
**---------------------------------------------------------------------------------------*/
static HANDLE gui_service_handle=InvalidHandle;


/*-----------------------------------------------------------------------------------------
 * ����:    gui_service_get_handle()
 *
 * ˵��:    ����GUI���������
**---------------------------------------------------------------------------------------*/
HANDLE gui_service_get_handle(void)
{
    return gui_service_handle;
}

/*-----------------------------------------------------------------------------------------
 * ����:    gui_service_timer()
 *
 * ˵��:    GUI��ʱ��
**---------------------------------------------------------------------------------------*/
void gui_service_timer(void * data, INT16S nr)
{
    data = data, nr = nr;
    
    if(gui_service_handle!=InvalidHandle)
        TaskAwake(gui_service_handle);
}
    
/*-----------------------------------------------------------------------------------------
 * ����:    gui_service_msg_handler()
 *
 * ˵��:    GUI��������Ϣ������, �ػ�������Ϣ������洢
**---------------------------------------------------------------------------------------*/
BOOL gui_service_msg_handler(HANDLE task, MSGCB * msg)
{    
    FamesAssert(msg);
    
    if(!msg)
        return ok;

    task = task;
    
    return ok;
}

/*-----------------------------------------------------------------------------------------
 * ����:    gui_service_daemon()
 *
 * ˵��:    GUI��������̨, ����������е�GUIԪ��
 *
 * �����ջ��С��Ϊ10240
**---------------------------------------------------------------------------------------*/
void __daemon gui_service_daemon(void * data)
{
    gui_window_t * w;
    INT32U sleep_ms;
    prepare_atomic()

    data = data;

    in_atomic();
    CurrentTCB->msg_handler = gui_service_msg_handler;
    CurrentTCB->wake_on_msg = NO;
    out_atomic();

    if(TimerSet(TimerGUI, 200L, TIMER_TYPE_AUTO, gui_service_timer, NULL)){
        ;
    } 

    for(;;){
        in_atomic();
        CurrentTCB->TaskTimer = 80L; /* set for calc run time */
        out_atomic();
        lock_kernel();
        gui_for_each_window(w)
            gui_window_action(w);
        unlock_kernel();
        in_atomic();
        sleep_ms = CurrentTCB->TaskTimer;
        out_atomic();
        if(sleep_ms > 80L)
            sleep_ms = 80L;
        if(sleep_ms > 40L)
            sleep_ms -= 20L; /* a loop for 80ms         */
        else
            sleep_ms = 20L;  /* sleep for 20ms at least */
        
        TaskSleep(sleep_ms);
    }
}

/*-----------------------------------------------------------------------------------------
 * ����:    gui_service_initialize()
 *
 * ˵��:    GUI��������ʼ��, ��������������, ��������
**---------------------------------------------------------------------------------------*/
BOOL gui_service_initialize(void)
{
    gui_service_handle = TaskCreate(gui_service_daemon, NULL, 
                                    "gui-service", /*lint !e1776*/
                                    NULL, 10240, PRIO_GUI, TASK_CREATE_OPT_NONE);
    
    if(gui_service_handle==InvalidHandle)
        return fail;

    TaskSuspend(gui_service_handle);
    
    return ok;
}

/*=========================================================================================
 * 
 * ���ļ�����: gui_svc.c
 * 
**=======================================================================================*/


