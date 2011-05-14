/*************************************************************************************
 * 文件: task.c
 *
 * 说明: 任务管理
 *
 * 作者: Jun
 *
 * 时间: 2010-7-5
*************************************************************************************/
#define  FAMES_TASK_C
#include "includes.h"

/*lint --e{818}*/
/*------------------------------------------------------------------------------------
 * 函数:    TaskIdle()
 *
 * 说明:    系统空闲任务, 释放被删除任务的资源(堆栈)并切换到下一任务
**----------------------------------------------------------------------------------*/
void __task TaskIdle(void * data)
{
    HANDLE hTask;
    TCB *  ptcb;
    CRITICAL_DEFINE()
    
    data = data;
    
    for(;;){
        ENTER_CRITICAL();
        TaskIdleCnts++;
        EXIT_CRITICAL();
        ptcb = TCBS;
        for(hTask=0; hTask<MAX_TASKS; hTask++, ptcb++){
            DispatchLock();
            switch(ptcb->TcbUsed){
                case TCB_USED_DELETED:
                    mem_free((void *)ptcb->StackBuf);
                    InitTCB(ptcb);
                    ptcb->TaskState=TASK_STATE_DORMANT;
                    break;
                default:
                    break;
            }
            DispatchUnlock();
        }
        #if DISPATCH_PREEMPTIVE_EN!=1
        TaskDispatch();
        #endif
    }
}

#if TASK_STAT_EN == 1
/*------------------------------------------------------------------------------------
 * 函数:    TaskStat()
 *
 * 说明:    系统统计任务, 计算CPU的使用率
**----------------------------------------------------------------------------------*/
void __task TaskStat(void * data)
{
    INT32U  tmp;
    CRITICAL_DEFINE()

    CALLED_ONLY_ONCE();
    
    data = data; 
    while(TaskStatReady!=YES){
        TaskSleep(2000L);
    }
    if(TaskIdleCtMx==0L){
        for(;;)TaskSleep(0L);
    }
    for(;;){
        ENTER_CRITICAL();
        tmp=TaskIdleCnts;
        EXIT_CRITICAL();
        tmp*=100L;
        tmp/=TaskIdleCtMx;
        if(tmp>100L)tmp=100L;
        ENTER_CRITICAL();
        CPU_USED=100-(INT16U)tmp;
        TaskIdleCnts=0L;
        EXIT_CRITICAL();
        TaskSleep(1000L);
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskStat()
 *
 * 说明:    统计任务初始化
 *
 * 注意:    必须在FamesOS启动之后才能被调用
**----------------------------------------------------------------------------------*/
void apical __init InitStat(void)
{
    CRITICAL_DEFINE()
    
    CALLED_ONLY_ONCE();   

    if(FamesOSStarted!=YES){
        return;
    }
    ENTER_CRITICAL();
    TaskIdleCnts=0L;
    EXIT_CRITICAL();
    TaskSleep(1000L);
    ENTER_CRITICAL();
    TaskIdleCtMx=TaskIdleCnts;
    TaskIdleCnts=0L;
    TaskStatReady=YES;
    EXIT_CRITICAL();
}
#endif

/*------------------------------------------------------------------------------------
 * 函数:    TaskReturnCapture()
 *
 * 说明:    一个任务如果执行了return语句, 那么系统将调用这个函数, 
 *          以删除对应的任务
 *
 * 注意:    应用程序不可调用此函数
**----------------------------------------------------------------------------------*/
#if TASK_FORCE_INFINITE_LOOP == 0
void  apical TaskReturnCapture(void)
{
    HANDLE  self;
    CRITICAL_DEFINE()
    
    ENTER_CRITICAL();
    self=CurrentTask;
    EXIT_CRITICAL();
    TaskDelete(self);
}

#endif

/*------------------------------------------------------------------------------------
 * 函数:    InitTCB()
 *
 * 说明:    初始化TCB
 *
 * 输入:    tcb 需初始化的TCB指针
**----------------------------------------------------------------------------------*/
void apical InitTCB(TCB * tcb)
{
    FamesAssert(tcb);
    
    if(tcb==NULL){
        return;
    }
    tcb->TopOfStack  = NULL;
    tcb->Stack       = NULL;
    tcb->StackDepth  = 0;
    tcb->StackBuf    = NULL;
    tcb->TaskName    = "*none*";
    tcb->Priority    = 0;
    tcb->TaskState   = TASK_STATE_READY;
    tcb->TaskTimer   = 0L;
    tcb->TaskRunning = 0L;
    tcb->Prev        = NULL;
    tcb->Next        = NULL;
    tcb->Event       = NULL;
    tcb->TcbUsed     = TCB_USED_NO;
    InitMessage(&tcb->message);
    tcb->msg_handler = NULL;
    tcb->wake_on_msg = NO;
    tcb->console     = 0;
    #if DISPATCH_ROUNDROBIN_EN == 1
    tcb->TimeCount   = 0;
    #endif
}

/*------------------------------------------------------------------------------------
 * 函数:    InitStack()
 *
 * 说明:    初始化堆栈
 *
 * 输入:    TopOfStack  栈顶
 *          TaskEntry   任务入口
 *          TaskPara    任务参数
**----------------------------------------------------------------------------------*/
STACK_TYPE * apical __internal InitStack(STACK_TYPE *TopOfStack, 
                                    TASK_TYPE TaskEntry, void *TaskPara)
{
    STACK_TYPE SAVED_DS = 0;

    FamesAssert(TopOfStack);
    FamesAssert(TaskEntry);

    if(!TopOfStack || !TaskEntry){
        return NULL;
    }

    asm  MOV SAVED_DS, DS;

    *TopOfStack-- = (STACK_TYPE)0x1111;         /* 放在栈底的这些数字是用于调试的 */
    *TopOfStack-- = (STACK_TYPE)0x2222;
    *TopOfStack-- = (STACK_TYPE)0x3333;
    *TopOfStack-- = (STACK_TYPE)0x4444;
    *TopOfStack-- = (STACK_TYPE)0x5555;

    *TopOfStack-- = (STACK_TYPE)FP_SEG(TaskPara);
    *TopOfStack-- = (STACK_TYPE)FP_OFF(TaskPara);/*lint !e507 */
    #if TASK_FORCE_INFINITE_LOOP == 0
    *TopOfStack-- = (STACK_TYPE)FP_SEG(TaskReturnCapture);
    *TopOfStack-- = (STACK_TYPE)FP_OFF(TaskReturnCapture);/*lint !e507 */
    #else
    *TopOfStack-- = (STACK_TYPE)FP_SEG(TaskEntry);
    *TopOfStack-- = (STACK_TYPE)FP_OFF(TaskEntry);/*lint !e507 */
    #endif
    *TopOfStack-- = (STACK_TYPE)0x0202;        /* 初始MSW, 中断允许 */
    *TopOfStack-- = (STACK_TYPE)FP_SEG(TaskEntry);
    *TopOfStack-- = (STACK_TYPE)FP_OFF(TaskEntry);/*lint !e507 */

    *TopOfStack-- = (STACK_TYPE)0xAAAA;         /* AX */
    *TopOfStack-- = (STACK_TYPE)0xBBBB;         /* BX */
    *TopOfStack-- = (STACK_TYPE)0xCCCC;         /* CX */
    *TopOfStack-- = (STACK_TYPE)0xDCCD;         /* DX */
    *TopOfStack-- = (STACK_TYPE)0xEEEE;         /* ES */
    *TopOfStack-- =  SAVED_DS;                  /* DS */
    *TopOfStack-- = (STACK_TYPE)0x0123;         /* SI */
    *TopOfStack-- = (STACK_TYPE)0xD55D;         /* DI */
    *TopOfStack   = (STACK_TYPE)0xBBBB;         /* BP */

    return TopOfStack;
}

/*------------------------------------------------------------------------------------
 * 函数:    InitTasks()
 *
 * 说明:    初始化TCB数组及任务队列, 并创建系统任务
**----------------------------------------------------------------------------------*/
void apical InitTasks(void)
{
    INT16S i;
    
    CALLED_ONLY_ONCE();   

    CurrentTask    =  0;
    CurrentTCB     = &TCBS[0];
    TaskSwitches   =  0L;
    IntNesting     =  0;  
    TaskIdleCnts   =  0L;
    NumberOfTasks  =  0;
    TaskIdleCnts   =  0;
    TaskIdleCtMx   =  0;
    TaskIdleHandle =  0;
    TaskIdlePrio   =  (MAX_PRIORITY-1);
    TaskStatHandle =  0;
    TaskStatPrio   =  (MAX_PRIORITY-2);
    TaskStatReady  =  NO;
    InitTaskList(READYLIST);/*lint !e506 !e774*/
    InitTaskList(SLEEPLIST);/*lint !e506 !e774*/
    for(i=0; i<MAX_TASKS; i++){
        InitTCB(&TCBS[i]);
    }
    TaskIdleHandle=TaskCreate(TaskIdle, NULL, "#Idle", NULL, 512, TaskIdlePrio, TASK_CREATE_OPT_NONE);
#if TASK_STAT_EN == 1
    TaskStatHandle=TaskCreate(TaskStat, NULL, "#stat", NULL, 512, TaskStatPrio, TASK_CREATE_OPT_NONE);
#endif
}

/*------------------------------------------------------------------------------------
 * 函数:    FreeTasks()
 *
 * 说明:    释放为任务堆栈所申请的内存
**----------------------------------------------------------------------------------*/
void apical FreeTasks(void)
{
    HANDLE   hTask;
    
    CALLED_ONLY_ONCE();   

    for(hTask=0; hTask<MAX_TASKS; hTask++){
        switch(TCBS[hTask].TcbUsed){
            case TCB_USED_YES:
            case TCB_USED_DELETED:
                mem_free((void *)TCBS[hTask].StackBuf);
                break;
            default:
                break;
        }
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskCreate()
 *
 * 描述:    创建一个任务, 多个任务可具有同一个优先级
 *
 * 输入:    1) TaskEntry   任务入口地址
 *          2) TaskPara    任务参数
 *          3) TaskName    任务名
 *          4) TaskStack   堆栈
 *          5) StackSize   堆栈大小(单位为STACK_TYPE)
 *          6) Prio        优先级
 *          7) TaskOpt     任务创建选项(暂不用)
 *
 * 说明:    1) 此函数不能在中断例程中调用
 *          2) 参数TaskStack可以是一个静态数组或者是NULL, 当等于NULL时, 
 *             TaskCreate()将会为堆栈分配内存(StackSize个单元长度)
 *          3) 当TaskStack是数组时, StackSize应该是其字节数, 而
 *             当TaskStack为NULL时, StackSize应该是堆栈单元数(一般不等于字节数)
**----------------------------------------------------------------------------------*/
HANDLE apical TaskCreate(TASK_TYPE TaskEntry, void * TaskPara, INT08S * TaskName,
                         void * TaskStack, INT16U StackSize, INT16U Prio, INT16U TaskOpt)
{
    TCB         *ptcb;
    HANDLE       htask;
    STACK_TYPE  *tp=NULL;
    CRITICAL_DEFINE()

    FamesAssert(TaskEntry);
    FamesAssert(StackSize >= 16);
    FamesAssert(NumberOfTasks < MAX_TASKS);

    if(!TaskEntry)
        return InvalidHandle;

    if(StackSize<16)
        return InvalidHandle;

    if(Prio>=MAX_PRIORITY)
        return InvalidHandle;

    if(TaskName==NULL){
        TaskName="None";
    }

    TaskOpt=TaskOpt;
    
    ENTER_CRITICAL();
    if(IntNesting>0){
        EXIT_CRITICAL();
        return InvalidHandle;
    }
    if(NumberOfTasks>=MAX_TASKS){
        EXIT_CRITICAL();
        return InvalidHandle;
    }
    ptcb=TCBS;
    for(htask=0; htask<MAX_TASKS; htask++){
        if(ptcb->TcbUsed==TCB_USED_NO){
            break;
        } else if(ptcb->TcbUsed==TCB_USED_DELETED){
            mem_free((void *)ptcb->StackBuf);
            break;
        }
        ptcb++;
    }
    if(htask >= MAX_TASKS){ /* 没有空闲项 */
        EXIT_CRITICAL();
        return InvalidHandle;
    }

    StackSize += STACK_SIZE_BASE;

    tp = (STACK_TYPE *)TaskStack;

    if(!tp){
        tp = (STACK_TYPE *)mem_alloc((INT32U)(StackSize)*(INT32U)(sizeof(STACK_TYPE)));
    } else { /* 当堆栈由静态数组定义时, StackSize是其字节数, 所以需要将之转化为堆栈单元数 */
        StackSize /= sizeof(STACK_TYPE);
    }
    if(!tp){
        EXIT_CRITICAL();
        return InvalidHandle;
    }
    InitTCB(ptcb);
    ptcb->StackBuf    = tp;
    ptcb->StackDepth  = StackSize;
    ptcb->TaskName    = TaskName;
    ptcb->Priority    = Prio;
    ptcb->TaskState   = TASK_STATE_READY;
    ptcb->TcbUsed     = TCB_USED_YES;
    tp+=StackSize;   /* 指向栈底 */
    tp--;
    #if 0
    if((INT32U)tp&((INT32U)sizeof(STACK_TYPE)-1UL)){    /* 校正边界 */
        (INT32U)tp&=(~((INT32U)sizeof(STACK_TYPE)-1UL));
    }
    #endif
    ptcb->Stack      = tp;
    ptcb->TopOfStack = InitStack(tp, TaskEntry, TaskPara);
    if(!AddTaskToList(&READYLIST, ptcb)){
        EXIT_CRITICAL();
        mem_free(ptcb->StackBuf);
        InitTCB(ptcb);
        return InvalidHandle;
    }
    NumberOfTasks++;
    EXIT_CRITICAL();
    if(FamesOSStarted==YES && LockNesting==0){
        TaskDispatch();
    }
    return htask;
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskDelete()
 *
 * 说明:    删除一个任务
 *
 * 输入:    hTask 欲删除任务的句柄
 *
 * 返回:    ok/fail
 *
 * 特别:    1) 在FamesOS尚未开始或已停止时(FamesOSStarted==NO)不允许删除任务
 *          2) 被删除任务所占有的资源(信号量,消息等)没有处理
**----------------------------------------------------------------------------------*/
BOOL apical TaskDelete(HANDLE hTask)
{
    TCB  * ptcb;
    BOOL  deleteself;
    CRITICAL_DEFINE()

    if(hTask<0||hTask>=MAX_TASKS){
        return fail;
    }
    if(hTask==TaskIdleHandle){
        return fail;
    }
    ptcb=&TCBS[hTask];
    ENTER_CRITICAL();
    if(FamesOSStarted==NO){   
        EXIT_CRITICAL();
        return fail;
    }
    if(IntNesting>0){     /* ISR中不允许删除任务 */
        EXIT_CRITICAL();
        return fail;
    }
    if(NumberOfTasks<=0){
        EXIT_CRITICAL();
        return fail;
    }
    if(ptcb->TcbUsed!=YES){
        EXIT_CRITICAL();
        return fail;
    }
    if(hTask==CurrentTask){
        deleteself=YES;
    } else {
        deleteself=NO;
    }
    if(ptcb->TaskState==TASK_STATE_READY){
        if(!DelTaskFromList(&READYLIST, ptcb)){
            EXIT_CRITICAL();
            return fail;
        }
    } else if(ptcb->TaskState&TASK_STATE_WAIT){
        if(!DelTaskFromList(&((ECB *)ptcb->Event)->TaskWait, ptcb)){
            EXIT_CRITICAL();
            return fail;
        }
    } else {
        if(!DelTaskFromList(&SLEEPLIST, ptcb)){
            EXIT_CRITICAL();
            return fail;
        }
    }
    if(deleteself==YES){
        ptcb->TcbUsed=TCB_USED_DELETED;
    } else {
        ptcb->TcbUsed=TCB_USED_NO;
        mem_free((void *)ptcb->StackBuf);
        InitTCB(ptcb);
    }
    ptcb->TaskTimer=0L;
    ptcb->TaskState=TASK_STATE_DORMANT;
    NumberOfTasks--;
    EXIT_CRITICAL();
    if(deleteself==YES){
        TaskDispatch();
    }
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    SwitchContextISR()
 * 
 * 说明:    上下文切换的中断服务程序
**----------------------------------------------------------------------------------*/
void interrupt SwitchContextISR(void)
{
    SWITCH_CONTEXT();
}

/*------------------------------------------------------------------------------------
 * 函数:    InitSwitch()
 *
 * 说明:    安装SwitchContextISR()
**----------------------------------------------------------------------------------*/
void apical InitSwitch(void)
{
    DISABLE_INT();
    OldVectInDOS=FamesGetVect(FamesOS);
    FamesSetVect(FamesOS, SwitchContextISR);
    ENABLE_INT();
}

/*------------------------------------------------------------------------------------
 * 函数:    StopSwitch()
 *
 * 说明:    复原FamesOS中断向量
**----------------------------------------------------------------------------------*/
void apical StopSwitch(void)
{
    DISABLE_INT();
    FamesSetVect(FamesOS, OldVectInDOS);
    ENABLE_INT();
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskDispatch()
 *
 * 描述:    任务调度(这么简单?)
 *
 * 说明:    这个函数用于主动放弃CPU, 使其执行其它任务
**----------------------------------------------------------------------------------*/
void apical TaskDispatch(void)
{
    FamesAssert(!LockNesting); /* Assert for deadlock */
    FamesAssert(!IntNesting);
    
    #if DISPATCH_ROUNDROBIN_EN == 1
    CurrentTCB->TimeCount = 0; /* 时间片计数复位 */
    #endif
    DoTaskSwitch();
}

/*------------------------------------------------------------------------------------
 * 函数:    LockDispatch()
 *
 * 说明:    将调度器锁定,防止CPU被抢占
**----------------------------------------------------------------------------------*/
void apical DispatchLock(void)
{
    CRITICAL_DEFINE()
    
    FamesAssert(LockNesting>=0);

    ENTER_CRITICAL();
    LockNesting++;
    EXIT_CRITICAL();
}

/*------------------------------------------------------------------------------------
 * 函数:    DispatchUnlock()
 *
 * 说明:    调度器开锁,允许CPU被抢占
**----------------------------------------------------------------------------------*/
void apical DispatchUnlock(void)
{
    CRITICAL_DEFINE()
    
    FamesAssert(LockNesting>0);
    
    ENTER_CRITICAL();
    LockNesting--;
    EXIT_CRITICAL();
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskSleep()
 *
 * 说明:    任务睡眠
 *
 * 输入:    ms 睡眠时间, 单位毫秒, 0代表一直睡眠, 直到被唤醒
**----------------------------------------------------------------------------------*/
void apical TaskSleep(INT32U ms)
{
    CRITICAL_DEFINE()
    
    ENTER_CRITICAL();
    if(CurrentTCB->Priority==TaskIdlePrio){
        EXIT_CRITICAL();
        return;
    }
    CurrentTCB->TaskTimer=ms;
    SetTaskState(CurrentTask, TASK_STATE_SLEEP);
    DelTaskFromList(&READYLIST, CurrentTCB);
    AddTaskToList(&SLEEPLIST, CurrentTCB);
    EXIT_CRITICAL();
    TaskDispatch();
    return;
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskAwake()
 *
 * 说明:    任务唤醒
 *
 * 输入:    htask 任务句柄
 *
 * 唤醒一个睡着的任务(任务都睡着了?)
**----------------------------------------------------------------------------------*/
BOOL apical TaskAwake(HANDLE htask)
{
    TCB  * ptcb;
    BOOL retval;
    CRITICAL_DEFINE()

    if(htask<0||htask>=MAX_TASKS){
        return fail;
    }

    retval = fail;
    
    ENTER_CRITICAL();
    if(TCBS[htask].TaskState&TASK_STATE_SLEEP){
        TCBS[htask].TaskTimer=0L;
        SetTaskState(htask, TASK_STATE_SLEEP|TASK_STATE_UNSET);
        ptcb=&TCBS[htask];
        if(ptcb->TaskState==TASK_STATE_READY){
            DelTaskFromList(&SLEEPLIST, ptcb);
            AddTaskToList(&READYLIST, ptcb);
        } 
        retval = ok;
    }
    EXIT_CRITICAL();
    
    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskWait()
 *
 * 说明:    使任务等待一个事件
 *
 * 输入:    event    要等待的事件指针
 *          timeout  等待的最长时间,超时时间,单位为毫秒,0代表永远等待.
 *
 * 返回值:  ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical TaskWait(void * event, INT32U timeout)
{
    CRITICAL_DEFINE()
    
    if(event == NULL){
        return fail;
    }
    ENTER_CRITICAL();
    CurrentTCB->TaskTimer=timeout;
    SetTaskState(CurrentTask, TASK_STATE_WAIT);
    DelTaskFromList(&READYLIST, CurrentTCB);
    EventTaskWait((ECB *)event, CurrentTask);
    CurrentTCB->Event=event;
    EXIT_CRITICAL();
    TaskDispatch();
    if(EventTest((ECB *)event)){
        return ok;
    } else {
        return fail;
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskSuspend()
 *
 * 说明:    悬挂一个任务
 *
 * 输入:    htask 任务句柄
 *
 * 返回:    ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical TaskSuspend(HANDLE htask)
{
    TCB * ptcb;
    CRITICAL_DEFINE()
    
    if(htask<0||htask>=MAX_TASKS){
        return fail;
    }
    ptcb=&TCBS[htask];
    if(ptcb->Priority==TaskIdlePrio){
        return fail;
    }
    ENTER_CRITICAL();
    if(ptcb->TaskState==TASK_STATE_READY){
        DelTaskFromList(&READYLIST, ptcb);
        AddTaskToList(&SLEEPLIST, ptcb);
    } else {
        ;  /* 其它状态时, 只加上挂起的标记 */
    }
    SetTaskState(htask, TASK_STATE_SUSPEND);
    EXIT_CRITICAL();
    
    if(FamesOSStarted==YES && LockNesting==0){
        TaskDispatch();
    }
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskResume()
 *
 * 说明:    使一个悬挂的任务恢复运行
 *
 * 输入:    htask 任务号(句柄)
 *
 * 返回:    ok/fail
**----------------------------------------------------------------------------------*/
INT16S apical TaskResume(HANDLE htask)
{
    TCB * ptcb;
    BOOL  need_resched = NO;
    CRITICAL_DEFINE()
    
    if(htask<0||htask>=MAX_TASKS){
        return fail;
    }
    if(TCBS[htask].TaskState&TASK_STATE_SUSPEND){
        ENTER_CRITICAL();
        SetTaskState(htask, TASK_STATE_SUSPEND|TASK_STATE_UNSET);
        ptcb=&TCBS[htask];
        if(ptcb->TaskState==TASK_STATE_READY){
            DelTaskFromList(&SLEEPLIST, ptcb);
            AddTaskToList(&READYLIST, ptcb);
        } else {
            ;
        }
        if(CurrentTCB->Priority > ptcb->Priority){
            need_resched = YES;
        }
        EXIT_CRITICAL();
        if(FamesOSStarted==YES && LockNesting==0){
            if(need_resched == YES){
                TaskDispatch();
            }
        }
        return ok;
    }
    return fail;
}

/*------------------------------------------------------------------------------------
 * 函数:    SetTaskState()
 *
 * 说明:    设置任务状态
 *
 * 输入:    htask 任务序号(或句柄)
 *          state 状态
 *
 * 输出:    ok/fail
 *
 * 注意:    此函数只单纯的改变任务状态,并不会主动的进行任务调度(只是有可能被抢先)
**----------------------------------------------------------------------------------*/
BOOL apical SetTaskState(HANDLE htask, INT16U state)
{
    CRITICAL_DEFINE()
    
    if(htask<0||htask>=MAX_TASKS){
        return fail;
    }
    switch(state){
        case TASK_STATE_READY:
        case TASK_STATE_READY|TASK_STATE_UNSET:
            ENTER_CRITICAL();
            TCBS[htask].TaskState=TASK_STATE_READY;
            EXIT_CRITICAL();
            break;
        case TASK_STATE_SLEEP:
        case TASK_STATE_WAIT:
        case TASK_STATE_SUSPEND:
            ENTER_CRITICAL();
            TCBS[htask].TaskState|=state;
            EXIT_CRITICAL();
            break;
        case TASK_STATE_SLEEP|TASK_STATE_UNSET:
        case TASK_STATE_WAIT|TASK_STATE_UNSET:
        case TASK_STATE_SUSPEND|TASK_STATE_UNSET:
            ENTER_CRITICAL();
            TCBS[htask].TaskState&=(~state);
            EXIT_CRITICAL();
            break;
        default:
            return fail;
    }
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskTimeTick()
 *
 * 说明:    刷新各任务之内部定时器, 由TimeTick()调用 
**----------------------------------------------------------------------------------*/
void apical TaskTimeTick(void)
{
    HANDLE  ntask;
    TCB    *pt;
    pt=&TCBS[0];
    for(ntask=0; ntask<MAX_TASKS; ntask++, pt++){
        if(pt->TaskTimer>(INT32U)0L){
            pt->TaskTimer--;
            if(pt->TaskTimer==0L){
                switch(pt->TaskState&0x3){
                    case TASK_STATE_SLEEP:
                        SetTaskState(ntask, TASK_STATE_SLEEP|TASK_STATE_UNSET);
                        if(pt->TaskState!=TASK_STATE_READY){
                            break;
                        }
                        DelTaskFromList(&SLEEPLIST, pt);
                        AddTaskToList(&READYLIST, pt);
                        break;
                    case TASK_STATE_WAIT:
                        SetTaskState(ntask, TASK_STATE_WAIT|TASK_STATE_UNSET);
                        if(pt->TaskState!=TASK_STATE_READY){
                            break;
                        }
                        DelTaskFromList(&((ECB *)pt->Event)->TaskWait, pt);
                        AddTaskToList(&READYLIST, pt);
                        pt->Event=NULL;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    TaskStackCheck()
 *
 * 说明:    检查堆栈的使用情况
 *
 * 输入:    htask  任务句柄
 *          opt    堆栈检查选项
**----------------------------------------------------------------------------------*/
INT32U apical TaskStackCheck(HANDLE htask, INT16U opt)
{
    TCB   * ptcb;
    INT16U  v_sp, v_ss;
    INT32U  stack;
    STACK_TYPE * current_task_ptr;
    CRITICAL_DEFINE()
    
    if(htask<0||htask>=MAX_TASKS){
        return 0u;
    }
    
    ptcb=&TCBS[htask];
    v_sp=_SP; /*lint !e40 */
    v_ss=_SS; /*lint !e40 */

    current_task_ptr = (STACK_TYPE *)MK_FP(v_ss, v_sp);
    
    switch(opt){/*lint --e{571}, Suspicious cast */
        case STACK_CHECK_OPT_TOTAL:
             ENTER_CRITICAL();
             stack=((INT32U)(ptcb->Stack-ptcb->StackBuf)+1uL);
             EXIT_CRITICAL();
             break;
        case STACK_CHECK_OPT_USED:
             if(htask==CurrentTask){
                 ENTER_CRITICAL();
                 stack=((INT32U)(ptcb->Stack-current_task_ptr)+1uL);
                 EXIT_CRITICAL();
             } else {
                 ENTER_CRITICAL();
                 stack=((INT32U)(ptcb->Stack-ptcb->TopOfStack)+1uL);
                 EXIT_CRITICAL();
             }
             break;
        case STACK_CHECK_OPT_REMAIN:
             if(htask==CurrentTask){
                 ENTER_CRITICAL();
                 stack=(INT32U)(current_task_ptr-ptcb->StackBuf);
                 EXIT_CRITICAL();
             } else {
                 ENTER_CRITICAL();
                 stack=(INT32U)(ptcb->TopOfStack-ptcb->StackBuf);
                 EXIT_CRITICAL();
             }
             break;
        case STACK_CHECK_OPT_GET_BUF:
             ENTER_CRITICAL();
             stack=(INT32U)(ptcb->StackBuf);
             EXIT_CRITICAL();
             break;
        case STACK_CHECK_OPT_GET_BTM:
             ENTER_CRITICAL();
             stack=(INT32U)(ptcb->Stack);
             EXIT_CRITICAL();
             break;
        case STACK_CHECK_OPT_GET_PTR:
             if(htask==CurrentTask){
                 ENTER_CRITICAL();
                 stack=(INT32U)current_task_ptr;
                 EXIT_CRITICAL();
             } else {
                 ENTER_CRITICAL();
                 stack=(INT32U)(ptcb->TopOfStack);
                 EXIT_CRITICAL();
             }
             break;
        default:
             stack=0u;
             break;
    }
    return stack;
}

/*------------------------------------------------------------------------------------
 * 函数:    GetHighestTask()
 *
 * 说明:    取得下一个将要运行的任务
**----------------------------------------------------------------------------------*/
void apical GetHighestTask(void)
{
    PRIOMAP_TYPE  priomap;
    TCB          *ptcb;
    INT16U        highestprio;
    
    priomap = READYLIST.PrioMap;
    if(priomap==(PRIOMAP_TYPE)0){
        return;
    }
#if MAX_PRIORITY <= 8
    if(priomap&(PRIOMAP_TYPE)0xF){
        highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF];
    } else {
        priomap>>=4;
        highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+4;
    }
#elif MAX_PRIORITY <=16
    if(priomap&(PRIOMAP_TYPE)0xFF){
        if(priomap&(PRIOMAP_TYPE)0xF){
            highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF];
        } else {
            priomap>>=4;
            highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+4;
        }
    } else {
        priomap>>=8;
        if(priomap&(PRIOMAP_TYPE)0xF){
            highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+8;
        } else {
            priomap>>=4;
            highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+12;
        }
    }
#else    
    if(priomap&(PRIOMAP_TYPE)0xFFFF){
        if(priomap&(PRIOMAP_TYPE)0xFF){
            if(priomap&(PRIOMAP_TYPE)0xF){
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF];
            } else {
                priomap>>=4;
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+4;
            }
        } else {
            priomap>>=8;
            if(priomap&(PRIOMAP_TYPE)0xF){
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+8;
            } else {
                priomap>>=4;
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+12;
            }
        }
    } else {
        priomap>>=16;
        if(priomap&(PRIOMAP_TYPE)0xFF){
            if(priomap&(PRIOMAP_TYPE)0xF){
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+16;
            } else {
                priomap>>=4;
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+20;
            }
        } else {
            priomap>>=8;
            if(priomap&(PRIOMAP_TYPE)0xF){
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+24;
            } else {
                priomap>>=4;
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+28;
            }
        }
    }
#endif
    ptcb=READYLIST.PrioTCBS[highestprio].head;

    if(ptcb != CurrentTCB){
        TaskSwitches++;
        CurrentTCB=ptcb;
        CurrentTask=(INT16S)(CurrentTCB-(TCB *)TCBS);
#if SWITCH_HOOK_EN==1
        TaskSwitchHook();
#endif
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    DelHighestFrom()
 *
 * 说明:    从某个任务队列中取出优先权最高的那个任务
**----------------------------------------------------------------------------------*/
TCB * apical DelHighestFrom(TASK_LIST * tasklist)
{
    PRIOMAP_TYPE  priomap;
    TCB          *ptcb;
    INT16U        highestprio;
    CRITICAL_DEFINE()

    if(tasklist==NULL){
        return NULL;
    }
    ENTER_CRITICAL();
    priomap = tasklist->PrioMap;
    if(priomap==(PRIOMAP_TYPE)0){
        EXIT_CRITICAL();
        return NULL;
    }
#if MAX_PRIORITY <= 8
    if(priomap&(PRIOMAP_TYPE)0xF){
        highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF];
    } else {
        priomap>>=4;
        highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+4;
    }
#elif MAX_PRIORITY <=16
    if(priomap&(PRIOMAP_TYPE)0xFF){
        if(priomap&(PRIOMAP_TYPE)0xF){
            highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF];
        } else {
            priomap>>=4;
            highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+4;
        }
    } else {
        priomap>>=8;
        if(priomap&(PRIOMAP_TYPE)0xF){
            highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+8;
        } else {
            priomap>>=4;
            highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+12;
        }
    }
#else    
    if(priomap&(PRIOMAP_TYPE)0xFFFF){
        if(priomap&(PRIOMAP_TYPE)0xFF){
            if(priomap&(PRIOMAP_TYPE)0xF){
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF];
            } else {
                priomap>>=4;
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+4;
            }
        } else {
            priomap>>=8;
            if(priomap&(PRIOMAP_TYPE)0xF){
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+8;
            } else {
                priomap>>=4;
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+12;
            }
        }
    } else {
        priomap>>=16;
        if(priomap&(PRIOMAP_TYPE)0xFF){
            if(priomap&(PRIOMAP_TYPE)0xF){
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+16;
            } else {
                priomap>>=4;
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+20;
            }
        } else {
            priomap>>=8;
            if(priomap&(PRIOMAP_TYPE)0xF){
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+24;
            } else {
                priomap>>=4;
                highestprio=UnMapTbl[priomap&(PRIOMAP_TYPE)0xF]+28;
            }
        }
    }
#endif
    ptcb=tasklist->PrioTCBS[highestprio].head;
    if(ptcb==NULL){
        EXIT_CRITICAL();
        return NULL;
    }
    tasklist->PrioTCBS[highestprio].head=ptcb->Next;
    if(ptcb->Next==NULL){
        tasklist->PrioTCBS[highestprio].tail=NULL;
        #if   MAX_PRIORITY <= 8
        tasklist->PrioMap &= ~ PRIO_BITOFST(highestprio);
        #elif MAX_PRIORITY <= 16
        if(highestprio<8){
            tasklist->PrioMap &= ~ PRIO_BITOFST(highestprio);
        } else {
            tasklist->PrioMap &= ~(PRIO_BITOFST(highestprio-8)<<8);
        }
        #elif MAX_PRIORITY <= 24
        if(highestprio<8){
            tasklist->PrioMap &= ~ PRIO_BITOFST(highestprio);
        } else if(highestprio<16){
            tasklist->PrioMap &= ~(PRIO_BITOFST(highestprio-8)<<8);
        } else {
            tasklist->PrioMap &= ~(PRIO_BITOFST(highestprio-16)<<16);
        }
        #else
        if(highestprio<8){
            tasklist->PrioMap &= ~ PRIO_BITOFST(highestprio);
        } else if(highestprio<16){
            tasklist->PrioMap &= ~(PRIO_BITOFST(highestprio-8)<<8);
        } else if(highestprio<24){
            tasklist->PrioMap &= ~(PRIO_BITOFST(highestprio-16)<<16);
        } else {
            tasklist->PrioMap &= ~(PRIO_BITOFST(highestprio-24)<<24);
        }
        #endif        
    }
    EXIT_CRITICAL();

    return ptcb;
}

/*------------------------------------------------------------------------------------
 * 函数:    DelTaskFromList()
 *
 * 说明:    从某个任务队列中取出指定的任务
**----------------------------------------------------------------------------------*/
BOOL apical DelTaskFromList(TASK_LIST * tasklist, TCB * task)
{
    TCB       *ptcb, *ptcb2;
    INT16U     prio;
    CRITICAL_DEFINE()

    if(tasklist==NULL || task==NULL){
        return fail;
    }
    prio = task->Priority;
    if(prio >= TaskIdlePrio){
        return fail;
    }
    ENTER_CRITICAL();
    ptcb=tasklist->PrioTCBS[prio].head;
    ptcb2=ptcb;
    if(ptcb==NULL){
        EXIT_CRITICAL();
        return fail;
    }
    if(task==ptcb){
        tasklist->PrioTCBS[prio].head=ptcb->Next;        
    } else {
        while(ptcb){
            if(task!=ptcb){
                ptcb2=ptcb;
                ptcb=ptcb->Next;
            } else {
                ptcb2->Next=ptcb->Next;
                ptcb->Next=NULL;
                if(ptcb==tasklist->PrioTCBS[prio].tail){
                    tasklist->PrioTCBS[prio].tail=ptcb2;
                }
                break;
            }
        }
        if(ptcb==NULL){
            EXIT_CRITICAL();
            return fail;
        }
    }
    if(tasklist->PrioTCBS[prio].head==NULL){
        tasklist->PrioTCBS[prio].tail=NULL;
        #if   MAX_PRIORITY <= 8
        tasklist->PrioMap &= ~ PRIO_BITOFST(prio);
        #elif MAX_PRIORITY <= 16
        if(prio<8){
            tasklist->PrioMap &= ~ PRIO_BITOFST(prio);
        } else {
            tasklist->PrioMap &= ~(PRIO_BITOFST(prio-8)<<8);
        }
        #elif MAX_PRIORITY <= 24
        if(prio<8){
            tasklist->PrioMap &= ~ PRIO_BITOFST(prio);
        } else if(prio<16){
            tasklist->PrioMap &= ~(PRIO_BITOFST(prio-8)<<8);
        } else {
            tasklist->PrioMap &= ~(PRIO_BITOFST(prio-16)<<16);
        }
        #else
        if(prio<8){
            tasklist->PrioMap &= ~ PRIO_BITOFST(prio);
        } else if(prio<16){
            tasklist->PrioMap &= ~(PRIO_BITOFST(prio-8)<<8);
        } else if(prio<24){
            tasklist->PrioMap &= ~(PRIO_BITOFST(prio-16)<<16);
        } else {
            tasklist->PrioMap &= ~(PRIO_BITOFST(prio-24)<<24);
        }
        #endif        
    }
    EXIT_CRITICAL();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    AddTaskToList()
 *
 * 说明:    将一个任务加到某个任务队列之中
**----------------------------------------------------------------------------------*/
BOOL apical AddTaskToList(TASK_LIST * tasklist, TCB * task)
{
    INT16U  prio;
    CRITICAL_DEFINE()

    if(tasklist==NULL || task==NULL){
        return fail;
    }
    prio=task->Priority;
    if(prio > TaskIdlePrio){
        return fail;
    }
    ENTER_CRITICAL();
    if(tasklist->PrioTCBS[prio].tail==NULL){
        tasklist->PrioTCBS[prio].head=task;
        tasklist->PrioTCBS[prio].tail=task;
        task->Next=NULL;
        #if   MAX_PRIORITY <= 8
        tasklist->PrioMap |= PRIO_BITOFST(prio);
        #elif MAX_PRIORITY <= 16
        if(prio<8){
            tasklist->PrioMap |= PRIO_BITOFST(prio);
        } else {
            tasklist->PrioMap |= (PRIO_BITOFST(prio-8)<<8);
        }
        #elif MAX_PRIORITY <= 24
        if(prio<8){
            tasklist->PrioMap |= PRIO_BITOFST(prio);
        } else if(prio<16){
            tasklist->PrioMap |= (PRIO_BITOFST(prio-8)<<8);
        } else {
            tasklist->PrioMap |= (PRIO_BITOFST(prio-16)<<16);
        }
        #else
        if(prio<8){
            tasklist->PrioMap |= PRIO_BITOFST(prio);
        } else if(prio<16){
            tasklist->PrioMap |= (PRIO_BITOFST(prio-8)<<8);
        } else if(prio<24){
            tasklist->PrioMap |= (PRIO_BITOFST(prio-16)<<16);
        } else {
            tasklist->PrioMap |= (PRIO_BITOFST(prio-24)<<24);
        }
        #endif
    } else {
        tasklist->PrioTCBS[prio].tail->Next=task;
        tasklist->PrioTCBS[prio].tail=task;
        task->Next=NULL;
    }
    EXIT_CRITICAL();
    return ok;
}

#if DISPATCH_ROUNDROBIN_EN == 1
/*------------------------------------------------------------------------------------
 * 函数:    SetTimeSlice()
 *
 * 描述:    设置时间片大小
 *
 * 参数:    val   新的时间片大小, 0=关闭时间片轮转调度
 *
 * 说明:    时间片轮转调度主要是用于具有相同优先级的任务之间, 不同优先级的
 *          任务之间按优先级调度.
 *          要使用时间片轮转调度, 须有下面选项:
 *
 *          #define DISPATCH_PREEMPTIVE_EN  1
 *          #define DISPATCH_ROUNDROBIN_EN  1
 *
**----------------------------------------------------------------------------------*/
BOOL apical SetTimeSlice(INT16S val)
{
    prepare_atomic()
        
    if(val < 0)
        return fail;

    in_atomic();
    TimeSlice = val;
    out_atomic();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    GetTimeSlice()
 *
 * 说明:    返回时间片大小
**----------------------------------------------------------------------------------*/
INT16S apical GetTimeSlice(void)
{
    INT16S val;
    prepare_atomic()

    in_atomic();
    val = TimeSlice;
    out_atomic();

    return val;    
}

/*------------------------------------------------------------------------------------
 * 函数:    RoundRobinTask()
 *
 * 描述:    将当前任务移到当前优先级的末尾(任务轮转)
 *
 * 说明:    此函数只在TimeTick()中调用, 也就是说在中断上下文中执行
**----------------------------------------------------------------------------------*/
void apical RoundRobinTask(void)
{
    TCB       * ptcb;
    INT16U      prio;
    TASK_LIST * list;
    TCB  ** head;
    TCB  ** tail;

    ptcb = CurrentTCB;
    prio = ptcb->Priority;
    
    if(prio >= TaskIdlePrio){
        return;
    }

    list = &READYLIST;
    head = &list->PrioTCBS[prio].head;
    tail = &list->PrioTCBS[prio].tail;

    if((*head) == (*tail)){
        return; /* 本优先级只有一个任务, 那么就不需要轮转了. */
    }

    (*head)=ptcb->Next;
    (*tail)->Next=ptcb;
    (*tail)=ptcb;
    ptcb->Next=NULL;
    
    return;
}

#endif/* #if DISPATCH_ROUNDROBIN_EN == 1 */


/*====================================================================================
 * 
 * 本文件结束: task.c
 * 
**==================================================================================*/

