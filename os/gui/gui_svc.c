/******************************************************************************************
 * 文件: gui_svc.c
 *
 * 说明: gui服务器
 *
 * 作者: Jun
 *
 * 时间: 2010-11-11
******************************************************************************************/
#define  FAMES_GUI_SERVICE_C
#include "includes.h"


/*-----------------------------------------------------------------------------------------
 * 
 * GUI服务器句柄
 * 
**---------------------------------------------------------------------------------------*/
static HANDLE gui_service_handle=InvalidHandle;


/*-----------------------------------------------------------------------------------------
 * 函数:    gui_service_get_handle()
 *
 * 说明:    返回GUI服务器句柄
**---------------------------------------------------------------------------------------*/
HANDLE gui_service_get_handle(void)
{
    return gui_service_handle;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_service_timer()
 *
 * 说明:    GUI定时器
**---------------------------------------------------------------------------------------*/
void gui_service_timer(void * data, INT16S nr)
{
    data = data, nr = nr;
    
    if(gui_service_handle!=InvalidHandle)
        TaskAwake(gui_service_handle);
}
    
/*-----------------------------------------------------------------------------------------
 * 函数:    gui_service_msg_handler()
 *
 * 说明:    GUI服务器消息处理器, 截获所有消息并缓冲存储
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
 * 函数:    gui_service_daemon()
 *
 * 说明:    GUI服务器后台, 负责管理所有的GUI元素
 *
 * 建议堆栈大小设为10240
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
 * 函数:    gui_service_initialize()
 *
 * 说明:    GUI服务器初始化, 创建服务器任务, 但不运行
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
 * 本文件结束: gui_svc.c
 * 
**=======================================================================================*/


