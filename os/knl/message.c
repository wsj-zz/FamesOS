/******************************************************************************************
** �ļ�: message.c
** ˵��: �����ͨѶ֮��Ϣ����
** ����: Jun
** ʱ��: 2010-11-9
******************************************************************************************/
#define  FAMES_MESSAGE_C
#include "includes.h"

#define  handle_is_avail(handle) if((handle)<0||(handle)>=MAX_TASKS)return fail

/*-----------------------------------------------------------------------------------------
 * ����:    InitMessage()
 *
 * ˵��:    ��ʼ��һ����Ϣ��
**---------------------------------------------------------------------------------------*/
BOOL apical InitMessage(MSGCB * msg)
{
    FamesAssert(msg);

    if(!msg)
        return fail;

    msg->msg    =  0;
    msg->wParam = (INT32S)0;
    msg->lParam = (INT32S)0;
    msg->state  =  MSG_STATE_NONE;
    msg->lock   =  NO;

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * ����:    PostMessage()
 *
 * ˵��:    ����һ����Ϣ, ֱ�ӷ��͵�Ŀ������
 *
 * ����:    1) task       Ŀ��������
 *          2) msg        ��Ϣ
 *
 * ����:    ok on success, otherwise fail
**---------------------------------------------------------------------------------------*/
BOOL apical PostMessage(HANDLE task, MSGCB * msg)
{
    TCB   * ptask;
    BOOL  retval=fail;
    
    FamesAssert(msg);
    
    if(!msg)
        return fail;

    handle_is_avail(task);
    ptask = &TCBS[task];

    lock_kernel();
    if(ptask->message.state==MSG_STATE_FULL){ /* task'message is FULL, return fail */
        retval = fail;
    } else {
        ptask->message.msg    = msg->msg;
        ptask->message.wParam = msg->wParam;
        ptask->message.lParam = msg->lParam;
        ptask->message.state  = MSG_STATE_FULL;
        retval = ok;
    }
    unlock_kernel();

    if(ptask->wake_on_msg >= YES){
        TaskAwake(task);
    }
    
    return retval;
}

/*lint --e{530}, ��ʱ������д��......*/
/*-----------------------------------------------------------------------------------------
 * ����:    SendMessage()
 *
 * ˵��:    ����һ����Ϣ�����Դ���
 *
 * ����:    1) task       Ŀ��������
 *          2) msg        ��Ϣ
 *
 * ����:    ok on success, otherwise fail
**---------------------------------------------------------------------------------------*/
BOOL apical SendMessage(HANDLE task, MSGCB * msg)
{
    TCB   * ptask;
    BOOL  retval=fail;
    MSG_HDLR hdlr;
    prepare_atomic()
    
    FamesAssert(msg);

    if(!msg)
        return fail;

    handle_is_avail(task);
    ptask = &TCBS[task];

    atomic_do((hdlr=ptask->msg_handler));

    if(hdlr){          /* msg_handler exist              */
        retval = (*hdlr)(task, msg);
        if(!retval){   /* failed to handle, then Post it */
            retval = PostMessage(task, msg);
        } else {
            if(ptask->wake_on_msg >= YES){
                ptask->wake_on_msg = -YES;
                TaskAwake(task);
            }
        }
    } else {
        retval = PostMessage(task, msg);
    }
    
    return retval;
}

/*-----------------------------------------------------------------------------------------
 * ����:    GetMessage()
 *
 * ˵��:    ��ȡ�յ�����Ϣ
 *
 * ����:    1) msg_buf  ��ȡ������Ϣ��ŵ�����
 *
 * ����:    ok on success, otherwise fail
 *
 * ע��:    �˺������ܻ���Ϊ��Ϣ����ʹ�������˯��״̬, ���о��Բ������ж��е���!
 *          �˺���ֻ������������������
**---------------------------------------------------------------------------------------*/
BOOL apical GetMessage(MSGCB * msg_buf)
{
    INT16S state;
    BOOL   retval;
    prepare_atomic()

    FamesAssert(msg_buf);

    if(!msg_buf)
        return fail;

    os_mutex_lock(CurrentTCB->message.lock); 
    atomic_do((state=CurrentTCB->message.state));
    if(state==MSG_STATE_FULL){
        msg_buf->msg    = CurrentTCB->message.msg;
        msg_buf->wParam = CurrentTCB->message.wParam;
        msg_buf->lParam = CurrentTCB->message.lParam;
        msg_buf->state  = CurrentTCB->message.state;
        CurrentTCB->message.state=MSG_STATE_NONE;
        retval = ok;
    } else {
        retval = fail;
    }
    os_mutex_unlock(CurrentTCB->message.lock); 

    return retval;
}


/*-----------------------------------------------------------------------------------------
 * ����:    WaitMessage()
 *
 * ˵��:    �ȴ���Ϣ
 *
 * ����:    1) msg_buf  ��ȡ������Ϣ��ŵ�����
 *          2) tm       �ȴ���ʱ��, 0=��Զ�ȴ�
 *
 * ����:    ok on success, fail on timeout
 *
 * ע��:    �˺������ܻ���Ϊ��Ϣ����ʹ�������˯��״̬, ���о��Բ������ж��е���!
 *          �˺���ֻ������������������
 *
 *          ���յ���Ϣʱ(return ok), ����Ҫ���msg_buf->state, ��Ϊ��Ϣ�����Ѿ���
 *          msg_handler��������.
**---------------------------------------------------------------------------------------*/
BOOL apical WaitMessage(MSGCB * msg_buf, INT32U tm)
{
    INT16S state;
    BOOL   retval;
    prepare_atomic()

    FamesAssert(msg_buf);

    if(!msg_buf)
        return fail;

    os_mutex_lock(CurrentTCB->message.lock); 
    atomic_do((state=CurrentTCB->message.state));
    
    if(state!=MSG_STATE_FULL){ /* if none, wait */
        CurrentTCB->wake_on_msg=YES;
        os_mutex_unlock(CurrentTCB->message.lock); 
        TaskSleep(tm);
        os_mutex_lock(CurrentTCB->message.lock); 
        lock_kernel();
        if(CurrentTCB->wake_on_msg==-YES){ /* message received, but handled by msg_handler */
            CurrentTCB->message.state=MSG_STATE_NONE;
            state = MSG_STATE_FULL;
        } else {
            state=CurrentTCB->message.state;
        }
        CurrentTCB->wake_on_msg=NO;
        unlock_kernel();
    }

    if(state==MSG_STATE_FULL){
        msg_buf->msg    = CurrentTCB->message.msg;
        msg_buf->wParam = CurrentTCB->message.wParam;
        msg_buf->lParam = CurrentTCB->message.lParam;
        msg_buf->state  = CurrentTCB->message.state;
        CurrentTCB->message.state=MSG_STATE_NONE;
        os_mutex_unlock(CurrentTCB->message.lock); 
        retval = ok;
    } else {
        os_mutex_unlock(CurrentTCB->message.lock); 
        retval = fail;
    }

    return retval;
}


/*=========================================================================================
 * 
 * ���ļ�����: message.c
 * 
**=======================================================================================*/


