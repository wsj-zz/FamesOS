/*************************************************************************************
 * 文件:    event.c
 *
 * 说明:    FamesOS事件服务
 *
 * 作者:    Jun
 *
 * 时间:    2010-6-28
*************************************************************************************/
#define  FAMES_EVENT_C
#include "includes.h"

/*lint --e{818}*/

/*------------------------------------------------------------------------------------
 * 函数:     EventInit()
 * 说明:     事件对象初始化
 * 输入:     event 事件对象指针
 * 输出:     ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical EventInit(ECB * event)
{
    CRITICAL_DEFINE()
    
    if(event == NULL){
        return fail;
    }
    ENTER_CRITICAL();
    event->Available=EVENT_PENDING;
    InitTaskList(event->TaskWait);/*lint !e506 !e774*/
    EXIT_CRITICAL();
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:     EventSet()
 * 说明:     事件对象设定
 * 输入:     event 事件对象指针
 * 输出:     ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical EventSet(ECB * event)
{
    CRITICAL_DEFINE()
    
    if(event == NULL){
        return fail;
    }
    ENTER_CRITICAL();
    event->Available=EVENT_AVAILABLE;
    EXIT_CRITICAL();
    EventTaskReady(event);
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:     EventReset()
 * 说明:     事件对象复位
 * 输入:     event 事件对象指针
 * 输出:     ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical EventReset(ECB * event)
{
    CRITICAL_DEFINE()
    
    if(event == NULL){
        return fail;
    }
    ENTER_CRITICAL();
    event->Available=EVENT_PENDING;
    EXIT_CRITICAL();
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:     EventTest()
 * 说明:     事件对象测试
 * 输入:     event 事件对象指针
 * 输出:     ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical EventTest(ECB * event)
{
    if(event == NULL){
        return fail;
    }
    if(event->Available==EVENT_AVAILABLE){
        return ok;
    } else {
        return fail;
    }
}

/*------------------------------------------------------------------------------------
 * 函数:     EventTaskWait()
 * 说明:     将一个任务添加到事件对象的等待域中
 * 输入:     event 事件对象指针
 *           htask 任务号(句柄)
 * 输出:     ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical EventTaskWait(ECB * event, HANDLE htask)
{
    CRITICAL_DEFINE()
    
    if(event == NULL){
        return fail;
    }
    if(htask<0||htask>=MAX_TASKS){
        return fail;
    }
    ENTER_CRITICAL();
    EventReset(event);
    AddTaskToList(&event->TaskWait, &TCBS[htask]);
    EXIT_CRITICAL();
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:     EventTaskReady()
 * 说明:     将等待事件对象的任务全部置为就绪态
 * 输入:     event 事件对象指针
 * 输出:     ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical EventTaskReady(ECB * event)
{    
    TCB * ptcb;
    CRITICAL_DEFINE()
    
    if(event == NULL){
        return fail;
    }
    for(;;){
        ENTER_CRITICAL();
        ptcb=DelHighestFrom(&event->TaskWait);
        if(ptcb){
            SetTaskState((HANDLE)(ptcb-(TCB *)TCBS), TASK_STATE_WAIT|TASK_STATE_UNSET);
            if(ptcb->TaskState==TASK_STATE_READY){
                AddTaskToList(&READYLIST, ptcb);
            } else {
                AddTaskToList(&SLEEPLIST, ptcb);
            }
            EXIT_CRITICAL();
        } else {
            EXIT_CRITICAL();
            break;
        }
    }
    return ok;
}

/*====================================================================================
 * 
 * 本文件结束: event.c
 * 
**==================================================================================*/


