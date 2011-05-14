/******************************************************************************************
** 文件: message.c
** 说明: 任务间通讯之消息管理
** 作者: Jun
** 时间: 2010-11-9
******************************************************************************************/
#define  FAMES_MESSAGE_C
#include "includes.h"

#define  handle_is_avail(handle) if((handle)<0||(handle)>=MAX_TASKS)return fail

/*-----------------------------------------------------------------------------------------
 * 函数:    InitMessage()
 *
 * 说明:    初始化一个消息块
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
 * 函数:    PostMessage()
 *
 * 说明:    发送一个消息, 直接发送到目标任务
 *
 * 输入:    1) task       目标任务句柄
 *          2) msg        消息
 *
 * 返回:    ok on success, otherwise fail
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

/*lint --e{530}, 暂时先这样写吧......*/
/*-----------------------------------------------------------------------------------------
 * 函数:    SendMessage()
 *
 * 说明:    发送一个消息并尝试处理
 *
 * 输入:    1) task       目标任务句柄
 *          2) msg        消息
 *
 * 返回:    ok on success, otherwise fail
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
 * 函数:    GetMessage()
 *
 * 说明:    读取收到的消息
 *
 * 输入:    1) msg_buf  读取到的消息存放到这里
 *
 * 返回:    ok on success, otherwise fail
 *
 * 注意:    此函数可能会因为消息锁而使任务进入睡眠状态, 所有绝对不能在中断中调用!
 *          此函数只可用在任务级上下文中
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
 * 函数:    WaitMessage()
 *
 * 说明:    等待消息
 *
 * 输入:    1) msg_buf  读取到的消息存放到这里
 *          2) tm       等待的时间, 0=永远等待
 *
 * 返回:    ok on success, fail on timeout
 *
 * 注意:    此函数可能会因为消息锁而使任务进入睡眠状态, 所有绝对不能在中断中调用!
 *          此函数只可用在任务级上下文中
 *
 *          当收到消息时(return ok), 仍需要检测msg_buf->state, 因为消息可能已经被
 *          msg_handler处理完了.
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
 * 本文件结束: message.c
 * 
**=======================================================================================*/


