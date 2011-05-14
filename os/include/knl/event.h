/*************************************************************************************
** 文件: event.h
** 说明: FamesOS事件定义
** 作者: Jun
** 时间: 2010-6-28
*************************************************************************************/
#ifndef FAMES_EVENT_H
#define FAMES_EVENT_H

/*------------------------------------------------------------------------------------
*                   事件控制块(EventControlBlock, ECB)
* 
* 说明: 一个ECB就代表一个事件
*
*       1) Available:  事件的有效标志
*       2) TaskWait:   等待此事件的任务
**----------------------------------------------------------------------------------*/
typedef struct EventControlBlock {          
    BOOL      Available;                       
    TASK_LIST TaskWait;              
}ECB;

#define EVENT_PENDING      0                  /* 事件状态: 等待                    */
#define EVENT_AVAILABLE    1                  /* 事件状态: 有效                    */

/*
*函数声明 ----------------------------------------------------------------------------
*/
BOOL apical EventInit(ECB * event);
BOOL apical EventSet(ECB * event);
BOOL apical EventReset(ECB * event);
BOOL apical EventTest(ECB * event);
BOOL apical EventTaskWait(ECB *, HANDLE htask);
BOOL apical EventTaskReady(ECB * event);


#endif                                        /* #ifndef FAMES_EVENT_H             */

/*
*本文件结束: event.h =================================================================
*/

