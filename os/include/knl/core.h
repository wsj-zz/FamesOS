/*************************************************************************************
 * 文件:    knl/core.h
 *
 * 描述:    内核选项
 *
 * 作者:    Jun
 *
 * 时间:    2011-2-16
 *
 * 说明:    调度器选项及内核变量等
*************************************************************************************/
#ifndef FAMES_CORE_H
#define FAMES_CORE_H

/*------------------------------------------------------------------------------------
 * 
 *          调度器选项
 * 
**----------------------------------------------------------------------------------*/
#define DISPATCH_PREEMPTIVE_EN  1     /* 抢先式调度,     1=支持              */
#define DISPATCH_ROUNDROBIN_EN  1     /* 时间片轮转调度, 1=支持              */

/*------------------------------------------------------------------------------------
 * 
 *          内核相关定义
 * 
**----------------------------------------------------------------------------------*/
#define FamesOS         0x80          /* FamesOS系统中断号                   */
#define TickIntNo       0x08          /* 时钟中断号                          */
#define TicksPerSec     1000          /* 每秒滴答数(须固定为1000)            */

#define MAX_TASKS       16            /* 最大任务数                          */
#define MAX_PRIORITY    15            /* 最大优先级数[1,32]                  */
#if    (MAX_PRIORITY<1)||(MAX_PRIORITY>32)
#error  MAX_PRIORITY does not support this value, Please check it!
#endif

/*------------------------------------------------------------------------------------
 * 
 *          内核变量
 * 
**----------------------------------------------------------------------------------*/
#ifdef  FAMES_INIT_C
INT16S  IntNesting       = 0;         /* 中断嵌套层数                        */
INT16S  LockNesting      = 0;         /* 任务调度锁                          */
INT32U  SecondsFromStart = 0UL;       /* 从开机起程序运行的毫秒数            */
BOOL    FamesOSStarted   = NO;        /* FamesOS是否已运行                   */
#if     DISPATCH_ROUNDROBIN_EN == 1
INT16S  TimeSlice        = 0;         /* 时间片大小, 0=关闭时间片轮转调度    */
#endif
#else
extern  INT16S  IntNesting;
extern  INT16S  LockNesting;
extern  INT32U  SecondsFromStart;
extern  BOOL    FamesOSStarted;
#if     DISPATCH_ROUNDROBIN_EN == 1
extern  INT16S  TimeSlice;
#endif
#endif /* FAMES_INIT_C */


#endif /* #ifndef FAMES_CORE_H */
/*====================================================================================
 * 
 * 本文件结束: knl/core.h
 * 
**==================================================================================*/


