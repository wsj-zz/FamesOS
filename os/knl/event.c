/*************************************************************************************
 * �ļ�:    event.c
 *
 * ˵��:    FamesOS�¼�����
 *
 * ����:    Jun
 *
 * ʱ��:    2010-6-28
*************************************************************************************/
#define  FAMES_EVENT_C
#include "includes.h"

/*lint --e{818}*/

/*------------------------------------------------------------------------------------
 * ����:     EventInit()
 * ˵��:     �¼������ʼ��
 * ����:     event �¼�����ָ��
 * ���:     ok/fail
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
 * ����:     EventSet()
 * ˵��:     �¼������趨
 * ����:     event �¼�����ָ��
 * ���:     ok/fail
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
 * ����:     EventReset()
 * ˵��:     �¼�����λ
 * ����:     event �¼�����ָ��
 * ���:     ok/fail
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
 * ����:     EventTest()
 * ˵��:     �¼��������
 * ����:     event �¼�����ָ��
 * ���:     ok/fail
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
 * ����:     EventTaskWait()
 * ˵��:     ��һ��������ӵ��¼�����ĵȴ�����
 * ����:     event �¼�����ָ��
 *           htask �����(���)
 * ���:     ok/fail
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
 * ����:     EventTaskReady()
 * ˵��:     ���ȴ��¼����������ȫ����Ϊ����̬
 * ����:     event �¼�����ָ��
 * ���:     ok/fail
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
 * ���ļ�����: event.c
 * 
**==================================================================================*/


