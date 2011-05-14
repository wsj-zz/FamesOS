/*************************************************************************************
** 文件: timer.h
** 说明: timer.c的头文件，其中包含了timer.c中各函数的声明及定时器相关数据结构定义
** 作者: Jun
** 时间: 2010-7-8
*************************************************************************************/
#ifndef  FAMES_TIMER_H
#define  FAMES_TIMER_H

/*------------------------------------------------------------------------------------
* 定时器控制块
**----------------------------------------------------------------------------------*/
typedef struct TimerControlBlock{
    INT32S   timer;                         /* 定时器当前值                         */    
    INT32S   timer_initialize;              /* 定时器初始值                         */
    INT16S   timer_type;                    /* 定时器类型                           */
    INT16S   timer_status;                  /* 定时器状态                           */
    DPC_FUNC timer_func;                    /* 定时器回调函数                       */
    void    *timer_data;                    /* timer_func的参数                     */
    INT16S   timer_dpc;                     /* 定时器服务延迟调用之计数值           */
}TimerCB;

#define TIMER_MAX_NUM         32           /* 定时器最大个数                       */
#define TIMER_DPC_NUM_MAX  10000           /* 定时器服务DPC请求的最大个数          */

/*------------------------------------------------------------------------------------
* 定时器类型
**----------------------------------------------------------------------------------*/
#define TIMER_TYPE_ONCE      0x0            /* 定时器类型: 只运行一次即关闭         */
#define TIMER_TYPE_AUTO      0x1            /* 定时器类型: 自动加载初始值并持续运行 */
#define TIMER_TYPE_HARD      0x2            /* 定时器类型: 硬实时                   */

#define TIMER_TYPE_KEEP      0x4            /* 用于TIMER_SET, 表示类型不变          */

/*------------------------------------------------------------------------------------
* 定时器状态
**----------------------------------------------------------------------------------*/
#define TIMER_STATUS_CLOSE     0            /* 定时器状态: 关闭                     */
#define TIMER_STATUS_LAST      1            /* 定时器状态: 最后一次DPC请求(内部用)  */
#define TIMER_STATUS_RUN       2            /* 定时器状态: 运行                     */
#define TIMER_STATUS_BUSY      3            /* 定时器状态: 正忙(暂不用)             */


/*------------------------------------------------------------------------------------
* 定时器变量
**----------------------------------------------------------------------------------*/
#ifdef FAMES_TIMER_C
static TimerCB  TIMER[TIMER_MAX_NUM];       /* 定时器数组                           */                
static INT16S   NumberOfTimerDPC=0;         /* 定时器服务DPC请求的个数              */
static BOOL     TimerInitedFlag=NO;         /* 定时器是否已初始化                   */
INT16U          TaskTimerPrio  =PRIO_TIMER; /* 定时器任务的优先级                   */
HANDLE          TaskTimerHandle=0;          /* 定时器任务的句柄                     */
INT16U          TaskTimerStkSize=1024;      /* 定时器任务的堆栈大小                 */
#else
extern INT16U   TaskTimerPrio;
extern HANDLE   TaskTimerHandle;
extern INT16U   TaskTimerStkSize;
#endif

/*------------------------------------------------------------------------------------
* 函数声明
**----------------------------------------------------------------------------------*/
void   apical TimerInit(void);               /* 定时器初始化                        */
BOOL   apical TimerStop(INT16S timer);       /* 定时器停止                          */
BOOL   apical TimerStart(INT16S timer);      /* 定时器开始                          */
BOOL   apical TimerSet(INT16S timer,         /* 定时器设置(并开启)...               */
               INT32S InitValue,             
               INT16S TimerType,             
               void (*TimerFunc)(void *, INT16S),
               void  *TimerData);      
INT32S apical TimerGet(INT16S timer);        /* 读取定时器当前值                    */
BOOL   apical TimerForce(INT16S timer);      /* 强迫定时器超时                      */
void   apical TimerTimeTick(void);           /* 定时器时钟滴答                      */
BOOL   apical PushTimerDPC(INT16S timer);    /* 增加一个定时器DPC请求               */
BOOL   apical PopTimerDPC(INT16S*, INT16S*); /* 读取一个定时器DPC请求               */

#define TimerReStart(tmr) TimerStart(tmr)    /* 定时器重启                          */

BOOL   apical TimerPulse(INT16S timer, INT32S value); /* 临时改变定时器的超时值     */

/*------------------------------------------------------------------------------------
* 定时器应用序号, 用户可在此处定义自己的定时器序号
**----------------------------------------------------------------------------------*/
enum {
    TimerSys = 5,                      /* 系统定时器                           */
    TimerGUI,                          /* GUI定时器                            */
    TimerNIC,                          /* 用于网卡                             */
    TimerLog,                          /* 用于LOG记录                          */
    TimerArp,                          /* 用于ARP协议                          */
    TimerTftpdTX,                      /* 用于tftp服务之超时重传               */
    TimerTftpdRX,                      /* 用于tftp服务之接收超时               */
    TimerPLC,                          /* 用于PLC通讯                          */

    Timer_for_test,
    TimerForMainScreen,                /* 主画面所用的定时器                   */
    LastTmrNo                          /* 最后的定时器编号                     */
};

#if    LastTmrNo >= TIMER_MAX_NUM /*lint !e553*/
#error LastTmrNo >= TIMER_MAX_NUM is not allowed, check it please!
#endif


#endif

/*
*本文件结束: timer.h =================================================================
*/

