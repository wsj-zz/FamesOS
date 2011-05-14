/*******************************************************************************************
 * 文件:    task.h
 *
 * 说明:    任务管理
 *
 * 作者:    Jun
 *
 * 时间:    2010-7-5
*******************************************************************************************/
#ifndef FAMES_TASK_H
#define FAMES_TASK_H

/*
*开关设置 ----------------------------------------------------------------------------------
*/
#define SWITCH_HOOK_EN  1                   /* 任务切换钩子, 1=启用                     */
#define TASK_STAT_EN    1                   /* 统计任务, 1=启用                         */
#define TASK_FORCE_INFINITE_LOOP  0         /* 任务函数结构: 1=必须是无限循环, 0=可返回 */

/*------------------------------------------------------------------------------------------
 *                 任务控制块(TaskControlBlock, TCB)
 * 
 * 说明: 一个TCB就代表一个任务, 在TCB中存储有对应任务的堆栈及其它信息
 *
 *       1) TopOfStack:  指向任务栈顶, 这个元素必须放在结构体的第一个位置上
 *       2) Stack:       指向任务栈底(堆栈的开始位置)
 *       3) StackDepth:  堆栈深度, 表示堆栈可以容纳的最大变量数(STACK_TYPE), 
 *                       并不是字节数(BYTES)
 *       4) StackBuf:    用于堆栈之缓冲区的首地址()
 *       5) TaskName:    任务名
 *       6) Priority:    优先级
 *       7) TaskState:   任务状态
 *       8) TaskTimer:   任务定时器
 *       9) TaskRunning: 任务运行的时间(毫秒)(暂不支持)
 *      10) TimeCount:   时间片计数, 用于时间片轮转调度
 *      11) Prev, Next:  TCB链接指针
 *      12) Event:       任务所等待的事件
 *      13) message:     消息(任务收到的消息)
 *      14) msg_handler: 消息处理例程
 *      15) wake_on_msg: 当有消息发来时, 是否唤醒任务(主要用于WaitMessage())
 *                       YES=收到消息需要唤醒, NO=不需要唤醒, 
 *                       -YES=已收到消息, 但已被msg_handler处理过.
 *      16) console      控制台标志, 1=已打开, 0=未打开(这时不能接受键盘输入)
 *      17) TcbUsed:     这个TCB是否已用(BOOLEAN)
**----------------------------------------------------------------------------------------*/
typedef struct TaskControlBlock
{
    STACK_TYPE  * TopOfStack;                        
    STACK_TYPE  * Stack;                        
    INT16U        StackDepth;
    STACK_TYPE  * StackBuf;
    INT08S      * TaskName;    
    INT16U        Priority;    
    INT16U        TaskState;
    INT32U        TaskTimer;
    INT32U        TaskRunning;
    #if DISPATCH_ROUNDROBIN_EN == 1
    INT16S        TimeCount;
    #endif
    struct TaskControlBlock * Prev;
    struct TaskControlBlock * Next;
    void        * Event;
    MSGCB         message; 
    MSG_HDLR      msg_handler;
    INT16S        wake_on_msg;
    int           console;
    INT16U        TcbUsed;
}TCB;

#define  STACK_SIZE_BASE 0                  /* 任务堆栈基数     */

/*
*任务状态 ----------------------------------------------------------------------------------
*/
#define TASK_STATE_READY    0x0000u         /* 就绪             */
#define TASK_STATE_SLEEP    0x0001u         /* 睡眠             */
#define TASK_STATE_WAIT     0x0002u         /* 等待一个事件     */
#define TASK_STATE_SUSPEND  0x0004u         /* 挂起             */

#define TASK_STATE_DORMANT  0x0008u         /* 被删除           */

#define TASK_STATE_UNSET    0x8000u         /* 任务状态取消标志 */

/*
*TCB的USED标志 -----------------------------------------------------------------------------
*/
#define TCB_USED_NO          0      
#define TCB_USED_YES         1      
#define TCB_USED_DELETED     2      

/*------------------------------------------------------------------------------------------
 *                   优先级相关定义(Priority)
 * 
 * 说明: 在RTOS中, 优先权一般用于决定任务的重要级别, 越是重要的, 或是对时间及响应速度要求
 *       越严格的, 其优先权就应该越大. 在FamesOS中, 优先权的大小是由任务的优先级来体现
 *       的, 优先级越小(0), 优先权则越大.
 *       在FamesOS中, 一个任务必须对应一个优先级, 而一个优先级可以对应多个任务
 *       一个任务只能被更高优先权的任务占先, 同级之间不能相互占先, 只能以轮巡的方式进
 *       行调度.
 *
 *       1) MAX_PRIORITY      系统支持的最大优先级个数, 为了提高执行效率及减小空间
 *                            消耗, 这个值不应该大于32(sizeof(long))
 *
 *       2) PRIOMAP_TYPE      优先级位图类型, 优先级位图变量的每一位代表一个优先级
 *
 *       3) PRIO_BITOFST
 *       4) PRIO_BITMASK
**----------------------------------------------------------------------------------------*/
#if      MAX_PRIORITY <= 8
typedef  INT08U PRIOMAP_TYPE;
#elif    MAX_PRIORITY <=16
typedef  INT16U PRIOMAP_TYPE;
#else    
typedef  INT32U PRIOMAP_TYPE;
#endif

#define  PRIO_BITOFST(x)    ((PRIOMAP_TYPE)BITOFFSET[(x)])

/*
*任务相关变量 ------------------------------------------------------------------------------
*/
#ifdef  FAMES_TASK_C
TCB     TCBS[MAX_TASKS]= {{NULL,},};        /* 系统所用的TCB数组                   */
TCB    *CurrentTCB  = NULL;                 /* 当前任务的TCB指针                   */
HANDLE  CurrentTask    = 0;                 /* 当前任务的序号(句柄)                */
INT32U  TaskSwitches   = 0UL;               /* 任务切换的次数                      */
INT16S  NumberOfTasks  = 0;                 /* 系统中当前的任务数                  */

INT32U  TaskIdleCnts   = 0UL;               /* Idle任务执行的次数                  */
INT32U  TaskIdleCtMx   = 0UL;               /* Idle任务执行的最大次数(在1秒钟)     */
HANDLE  TaskIdleHandle = 0;                 /* Idle任务的句柄                      */
INT16U  TaskIdlePrio   = (MAX_PRIORITY-1);  /* Idle任务的优先级                    */
#if TASK_STAT_EN == 1
HANDLE  TaskStatHandle = 0;                 /* Stat任务的句柄(系统统计任务)        */
INT16U  TaskStatPrio   = (MAX_PRIORITY-2);  /* Stat任务的优先级                    */
BOOL    TaskStatReady  = NO;                /* Stat任务是否已准备好                */
INT16U  CPU_USED       = 0;                 /* CPU使用率(百分比)                   */
#endif
#else
extern  TCB     TCBS[];
extern  TCB    *CurrentTCB;
extern  HANDLE  CurrentTask; 
extern  INT32U  TaskSwitches;
extern  INT16S  NumberOfTasks;

extern  INT32U  TaskIdleCnts;
extern  INT32U  TaskIdleCtMx;
extern  HANDLE  TaskIdleHandle;
extern  INT16U  TaskIdlePrio;
#if TASK_STAT_EN == 1
extern  HANDLE  TaskStatHandle;
extern  INT16U  TaskStatPrio;
extern  BOOL    TaskStatReady;
extern  INT16U  CPU_USED;
#endif
#endif                                      /* #ifdef FAMES_TASK_C                 */

/*------------------------------------------------------------------------------------------
 *                   任务队列类型
 * 
 * 说明: 任务队列结构用于保存系统中的任务
 *
 *       1) READYLIST    任务就绪表
 *
 *       2) SLEEPLIST    任务睡眠表
 *
 *       3) InitTaskList 初始化一个任务表
**----------------------------------------------------------------------------------------*/
typedef struct _TASK_LIST_ {
    PRIOMAP_TYPE PrioMap;
    struct {
        TCB  * head;
        TCB  * tail;
    }PrioTCBS[MAX_PRIORITY];
}TASK_LIST;

#ifdef  FAMES_TASK_C
TASK_LIST  READYLIST;
TASK_LIST  SLEEPLIST;
#else
extern TASK_LIST  READYLIST;
extern TASK_LIST  SLEEPLIST;
#endif

/*lint -save -emacro(717, InitTaskList)*/
#define InitTaskList(tl)                                 \
         do {                                            \
            INT16S itl_i;                                \
            (tl).PrioMap=(PRIOMAP_TYPE)0;                \
            for(itl_i=0; itl_i<MAX_PRIORITY; itl_i++){   \
                (tl).PrioTCBS[itl_i].head=NULL;          \
                (tl).PrioTCBS[itl_i].tail=NULL;          \
            }                                            \
         } while(0)
/*lint -restore*/

/*------------------------------------------------------------------------------------------
 *                  临界区保护(CRITICAL AREA PROTECTION)
 * 
 * 说明: 在程序代码中, 总是有那么一些比较特殊的部分, 这些部分必须要一次性执行完毕, 中间
 *       不能有任何被打断的可能(例如硬件中断), 如果被打断了, 那么将有可能产生不可预料的
 *       结果. 像这种特殊的部分, 我们称之为"临界区"
 *
 *       临界区需要保护, 我们这里采用最简单的办法: 关中断!
 *
 *       1) ENTER_CRITICAL()    进入临界区之前执行(关中断)
 *       2) EXIT_CRITICAL()     退出临界区之后执行(开中断)
 *       3) CRITICAL_DEFINE()   临界区保护须事先定义的变量
**----------------------------------------------------------------------------------------*/
#define CRITICAL_METHOD    1                /* 临界区保护方法                      */

#if     CRITICAL_METHOD == 1
#define __CRITICAL_DEFINE()  INT16U __saved_psw;
#else
#define __CRITICAL_DEFINE()
#endif
/*lint -esym(529, __saved_psw) */
#define CRITICAL_DEFINE() __CRITICAL_DEFINE()
#if     CRITICAL_METHOD == 1
#define ENTER_CRITICAL()  asm pushf;asm pop __saved_psw;asm cli;
#define EXIT_CRITICAL()   asm push __saved_psw;asm popf;
#elif   CRITICAL_METHOD == 2
#define ENTER_CRITICAL()  asm pushf;asm cli
#define EXIT_CRITICAL()   asm popf
#elif   CRITICAL_METHOD == 3
#define ENTER_CRITICAL()  asm cli
#define EXIT_CRITICAL()   asm sti
#else
#define ENTER_CRITICAL()
#define EXIT_CRITICAL()
#endif

#define NOP()             asm db 90h        /* 无动作指令                          */

#define DISABLE_INT()     asm cli           /* 禁止中断                            */
#define ENABLE_INT()      asm sti           /* 打开中断                            */

/*------------------------------------------------------------------------------------------
 *                  上下文切换
 * 
 * 说明: 两个相关的宏定义: SWITCH_CONTEXT(), FIRST_CONTEXT()
 *
 * 特别: 其设计思想来自FreeRTOS的PC移植部分
 *
 * 原理: 1) SWITCH_CONTEXT():
 *         先将一个任务的堆栈指针保存到其TCB之中, 然后调用GetHighestTask()
 *         得到下一个将要执行的任务, 并用新任务的TCB恢复堆栈, 然后继续执行
 *       2) FIRST_CONTEXT():
 *         以第一个执行的任务的TCB加载堆栈指针, 然后用一条IRET指令强制运行
 *
 * 上下文保存及上下文恢复的过程由编译器自动生成(参考TC2.0之interrupt关键字)
**----------------------------------------------------------------------------------------*/
#define SWITCH_CONTEXT()                           \
        asm  mov    ax, seg CurrentTCB;            \
        asm  mov    ds, ax;                        \
        asm  les    bx, CurrentTCB;                \
        asm  mov    es:0x2[ bx ], ss;              \
        asm  mov    es:[ bx ], sp;                 \
        asm  call   far ptr GetHighestTask;        \
        asm  mov    ax, seg CurrentTCB;            \
        asm  mov    ds, ax;                        \
        asm  les    bx, dword ptr CurrentTCB;      \
        asm  mov    ss, es:[ bx + 2 ];             \
        asm  mov    sp, es:[ bx ];

#define FIRST_CONTEXT()                            \
        asm  mov    ax, seg CurrentTCB;            \
        asm  mov    ds, ax;                        \
        asm  les    bx, dword ptr CurrentTCB;      \
        asm  mov    ss, es:[ bx + 2 ];             \
        asm  mov    sp, es:[ bx ];                 \
        asm  pop    bp;                            \
        asm  pop    di;                            \
        asm  pop    si;                            \
        asm  pop    ds;                            \
        asm  pop    es;                            \
        asm  pop    dx;                            \
        asm  pop    cx;                            \
        asm  pop    bx;                            \
        asm  pop    ax;                            \
        asm  iret;

/*
*任务切换 ----------------------------------------------------------------------------------
*/
#define DoTaskSwitch()  asm int FamesOS;    /* 任务切换的实际动作                  */

#ifdef FAMES_TASK_C
static void interrupt  (*OldVectInDOS)();   /* 旧的中断向量(0x80)                  */
#endif

/*
*堆栈检测选项 ------------------------------------------------------------------------------
*/
#define STACK_CHECK_OPT_TOTAL    0          /* 返回堆栈的总大小                    */
#define STACK_CHECK_OPT_USED     1          /* 返回堆栈的已用大小                  */
#define STACK_CHECK_OPT_REMAIN   2          /* 返回堆栈剩余空间的大小              */
#define STACK_CHECK_OPT_GET_BUF  3          /* 返回堆栈首指针                      */
#define STACK_CHECK_OPT_GET_BTM  4          /* 返回堆栈栈底指针                    */
#define STACK_CHECK_OPT_GET_PTR  5          /* 返回堆栈当前指针                    */

/*
*任务创建选项 ------------------------------------------------------------------------------
*/
#define TASK_CREATE_OPT_NONE     0          

/*
*其它 --------------------------------------------------------------------------------------
*/
#if TASK_FORCE_INFINITE_LOOP == 0
#define task_return return /* 不强制任务为无限循环时, 可直接返回(相当于删除自己) */
#else
#define task_return for(;;)TaskDelete(CurrentTask)
#endif

/*
*函数声明 ----------------------------------------------------------------------------------
*/
void     __task   TaskIdle(void * data);                    /* 系统空闲任务              */
#if TASK_STAT_EN == 1
void     __task   TaskStat(void * data);                    /* 系统统计任务              */                            
void     apical   InitStat(void);                           /* 初始化统计任务            */
#endif
#if TASK_FORCE_INFINITE_LOOP == 0
void     apical   TaskReturnCapture(void);                  /* 捕获任务返回              */
#endif
void     apical   InitTCB(TCB *tcb);                        /* 初始化TCB                 */
void     apical   InitTasks(void);                          /* 初始化任务堆栈及TCB       */
void     apical   FreeTasks(void);                          /* 释放InitTasks()申请的内存 */
HANDLE   apical   TaskCreate(TASK_TYPE  TaskEntry,          /* 创建一个任务              */
                             void      *TaskPara, 
                             INT08S    *TaskName,
                             void      *TaskStack,
                             INT16U     StackSize, 
                             INT16U     Prio, INT16U TaskOpt);
BOOL     apical   TaskDelete(HANDLE hTask);                 /* 删除一个任务              */
void  interrupt   SwitchContextISR(void);                   /* 上下文件切换ISR           */
void     apical   InitSwitch(void);                         /* 安装SwitchContextISR()    */
void     apical   StopSwitch(void);                         /* 取消InitSwitch()的动作    */
void     apical   TaskDispatch(void);                       /* 离开当前任务,切到下一任务 */
void     apical   DispatchLock(void);                       /* 任务调度锁定              */
void     apical   DispatchUnlock(void);                     /* 任务调度开锁              */
void     apical   TaskSleep(INT32U ms);                     /* 任务睡眠                  */
BOOL     apical   TaskAwake(HANDLE htask);                  /* 任务唤醒                  */
BOOL     apical   TaskWait(void * event, INT32U timeout);   /* 等待一个事件              */
BOOL     apical   TaskSuspend(HANDLE htask);                /* 挂起                      */
INT16S   apical   TaskResume(HANDLE htask);                 /* 恢复挂起的任务            */
BOOL     apical   SetTaskState(HANDLE htask, INT16U state); /* 设置任务状态              */
void     apical   TaskTimeTick(void);                       /* 任务的定时时钟            */
INT32U   apical   TaskStackCheck(HANDLE htask, INT16U opt); /* 检查堆栈的使用情况        */
void     apical   GetHighestTask(void);                     /* 取得下一个将要运行的任务  */
TCB    * apical   DelHighestFrom(TASK_LIST * tasklist);      
BOOL     apical   DelTaskFromList(TASK_LIST *, TCB *); 
BOOL     apical   AddTaskToList(TASK_LIST *, TCB *);

void     apical   TaskSwitchHook(void);                     /* 任务切换钩子(须用户定义)  */

#if DISPATCH_ROUNDROBIN_EN == 1
BOOL     apical   SetTimeSlice(INT16S new_value);
INT16S   apical   GetTimeSlice(void);
void     apical   RoundRobinTask(void);
#endif

#define  TaskDelay(ms) TaskSleep((INT32U)(ms))



#endif /* #ifndef FAMES_TASK_H */

/*==========================================================================================
 * 
 * 本文件结束: task.h
 * 
**========================================================================================*/

