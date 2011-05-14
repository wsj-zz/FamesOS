/*******************************************************************************************
** 文件: message.h
** 说明: 任务间通讯之消息管理
** 作者: Jun
** 时间: 2010-11-9
*******************************************************************************************/
#ifndef FAMES_MESSAGE_H
#define FAMES_MESSAGE_H

/*
*开关设置 ----------------------------------------------------------------------------------
*/
#define TASK_MESSAGE_EN  1                  /* 消息处理模块致能                     */


/*------------------------------------------------------------------------------------------
 *                 消息控制块(MessageControlBlock, MSGCB)
 * 
 * 描述: 一个MsgCB就代表一个消息, 其中包含了消息的具体内容及状态:
 *
 *       1) state:    消息状态: FULL/NONE
 *       2) lock:     消息控制块读写锁
 *       3) msg:      消息内容
 *       4) wParam:   参数1 \
 *       5) lParam:   参数2 /\这两个参数的意义可自行定义
 *
 * 说明: 在发送一个消息的时候, 如果目标任务有用户指定的消息处理函数msg_handler,
 *       则调用这个函数, 当返回为ok时, 则认为此消息已处理完毕,
 *       否则, 就要把欲发送消息保存到目标任务的消息域中, 等待目标任务自己处理.
**----------------------------------------------------------------------------------------*/
typedef INT32S WPARAM;
typedef INT32S LPARAM;

typedef struct MessageControlBlock
{
    INT08S state;
    INT08S lock;
    INT16S msg;
    WPARAM wParam;
    LPARAM lParam;
}MSGCB;

typedef BOOL (*MSG_HDLR)(HANDLE task, MSGCB * msg); /* 消息处理函数原型 */


/*
*消息状态 ----------------------------------------------------------------------------------
*/
#define MSG_STATE_NONE     0x0         /* 表示没有消息         */
#define MSG_STATE_FULL     0x1         /* 表示有消息           */

/*
*系统消息 ----------------------------------------------------------------------------------
*/
#define  SYS_MSG_INIT        0x0001    /* 初始化消息        */
#define  SYS_MSG_KILL        0x0002    /* 销毁消息          */
#define  SYS_MSG_TIMER       0x0005    /* 定时器消息        */
#define  SYS_MSG_KEY         0x0006    /* 按键消息          */
#define  SYS_MSG_MOUSE       0x0007    /* 鼠标消息          */



/*
*函数声明 ----------------------------------------------------------------------------------
*/
BOOL apical InitMessage(MSGCB * msg);
BOOL apical PostMessage(HANDLE task, MSGCB * msg);
BOOL apical SendMessage(HANDLE task, MSGCB * msg);
BOOL apical GetMessage(MSGCB * msg_buf);
BOOL apical WaitMessage(MSGCB * msg_buf, INT32U tm);


#endif                                      /* #ifndef FAMES_MESSAGE_H                   */

/*=========================================================================================
 * 
 * 本文件结束: message.h
 * 
**=======================================================================================*/

