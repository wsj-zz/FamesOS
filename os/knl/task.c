/*************************************************************************************
 * �ļ�: task.c
 *
 * ˵��: �������
 *
 * ����: Jun
 *
 * ʱ��: 2010-7-5
*************************************************************************************/
#define  FAMES_TASK_C
#include "includes.h"

/*lint --e{818}*/
/*------------------------------------------------------------------------------------
 * ����:    TaskIdle()
 *
 * ˵��:    ϵͳ��������, �ͷű�ɾ���������Դ(��ջ)���л�����һ����
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
 * ����:    TaskStat()
 *
 * ˵��:    ϵͳͳ������, ����CPU��ʹ����
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
 * ����:    TaskStat()
 *
 * ˵��:    ͳ�������ʼ��
 *
 * ע��:    ������FamesOS����֮����ܱ�����
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
 * ����:    TaskReturnCapture()
 *
 * ˵��:    һ���������ִ����return���, ��ôϵͳ�������������, 
 *          ��ɾ����Ӧ������
 *
 * ע��:    Ӧ�ó��򲻿ɵ��ô˺���
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
 * ����:    InitTCB()
 *
 * ˵��:    ��ʼ��TCB
 *
 * ����:    tcb ���ʼ����TCBָ��
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
 * ����:    InitStack()
 *
 * ˵��:    ��ʼ����ջ
 *
 * ����:    TopOfStack  ջ��
 *          TaskEntry   �������
 *          TaskPara    �������
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

    *TopOfStack-- = (STACK_TYPE)0x1111;         /* ����ջ�׵���Щ���������ڵ��Ե� */
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
    *TopOfStack-- = (STACK_TYPE)0x0202;        /* ��ʼMSW, �ж����� */
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
 * ����:    InitTasks()
 *
 * ˵��:    ��ʼ��TCB���鼰�������, ������ϵͳ����
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
 * ����:    FreeTasks()
 *
 * ˵��:    �ͷ�Ϊ�����ջ��������ڴ�
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
 * ����:    TaskCreate()
 *
 * ����:    ����һ������, �������ɾ���ͬһ�����ȼ�
 *
 * ����:    1) TaskEntry   ������ڵ�ַ
 *          2) TaskPara    �������
 *          3) TaskName    ������
 *          4) TaskStack   ��ջ
 *          5) StackSize   ��ջ��С(��λΪSTACK_TYPE)
 *          6) Prio        ���ȼ�
 *          7) TaskOpt     ���񴴽�ѡ��(�ݲ���)
 *
 * ˵��:    1) �˺����������ж������е���
 *          2) ����TaskStack������һ����̬���������NULL, ������NULLʱ, 
 *             TaskCreate()����Ϊ��ջ�����ڴ�(StackSize����Ԫ����)
 *          3) ��TaskStack������ʱ, StackSizeӦ�������ֽ���, ��
 *             ��TaskStackΪNULLʱ, StackSizeӦ���Ƕ�ջ��Ԫ��(һ�㲻�����ֽ���)
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
    if(htask >= MAX_TASKS){ /* û�п����� */
        EXIT_CRITICAL();
        return InvalidHandle;
    }

    StackSize += STACK_SIZE_BASE;

    tp = (STACK_TYPE *)TaskStack;

    if(!tp){
        tp = (STACK_TYPE *)mem_alloc((INT32U)(StackSize)*(INT32U)(sizeof(STACK_TYPE)));
    } else { /* ����ջ�ɾ�̬���鶨��ʱ, StackSize�����ֽ���, ������Ҫ��֮ת��Ϊ��ջ��Ԫ�� */
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
    tp+=StackSize;   /* ָ��ջ�� */
    tp--;
    #if 0
    if((INT32U)tp&((INT32U)sizeof(STACK_TYPE)-1UL)){    /* У���߽� */
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
 * ����:    TaskDelete()
 *
 * ˵��:    ɾ��һ������
 *
 * ����:    hTask ��ɾ������ľ��
 *
 * ����:    ok/fail
 *
 * �ر�:    1) ��FamesOS��δ��ʼ����ֹͣʱ(FamesOSStarted==NO)������ɾ������
 *          2) ��ɾ��������ռ�е���Դ(�ź���,��Ϣ��)û�д���
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
    if(IntNesting>0){     /* ISR�в�����ɾ������ */
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
 * ����:    SwitchContextISR()
 * 
 * ˵��:    �������л����жϷ������
**----------------------------------------------------------------------------------*/
void interrupt SwitchContextISR(void)
{
    SWITCH_CONTEXT();
}

/*------------------------------------------------------------------------------------
 * ����:    InitSwitch()
 *
 * ˵��:    ��װSwitchContextISR()
**----------------------------------------------------------------------------------*/
void apical InitSwitch(void)
{
    DISABLE_INT();
    OldVectInDOS=FamesGetVect(FamesOS);
    FamesSetVect(FamesOS, SwitchContextISR);
    ENABLE_INT();
}

/*------------------------------------------------------------------------------------
 * ����:    StopSwitch()
 *
 * ˵��:    ��ԭFamesOS�ж�����
**----------------------------------------------------------------------------------*/
void apical StopSwitch(void)
{
    DISABLE_INT();
    FamesSetVect(FamesOS, OldVectInDOS);
    ENABLE_INT();
}

/*------------------------------------------------------------------------------------
 * ����:    TaskDispatch()
 *
 * ����:    �������(��ô��?)
 *
 * ˵��:    �������������������CPU, ʹ��ִ����������
**----------------------------------------------------------------------------------*/
void apical TaskDispatch(void)
{
    FamesAssert(!LockNesting); /* Assert for deadlock */
    FamesAssert(!IntNesting);
    
    #if DISPATCH_ROUNDROBIN_EN == 1
    CurrentTCB->TimeCount = 0; /* ʱ��Ƭ������λ */
    #endif
    DoTaskSwitch();
}

/*------------------------------------------------------------------------------------
 * ����:    LockDispatch()
 *
 * ˵��:    ������������,��ֹCPU����ռ
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
 * ����:    DispatchUnlock()
 *
 * ˵��:    ����������,����CPU����ռ
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
 * ����:    TaskSleep()
 *
 * ˵��:    ����˯��
 *
 * ����:    ms ˯��ʱ��, ��λ����, 0����һֱ˯��, ֱ��������
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
 * ����:    TaskAwake()
 *
 * ˵��:    ������
 *
 * ����:    htask ������
 *
 * ����һ��˯�ŵ�����(����˯����?)
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
 * ����:    TaskWait()
 *
 * ˵��:    ʹ����ȴ�һ���¼�
 *
 * ����:    event    Ҫ�ȴ����¼�ָ��
 *          timeout  �ȴ����ʱ��,��ʱʱ��,��λΪ����,0������Զ�ȴ�.
 *
 * ����ֵ:  ok/fail
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
 * ����:    TaskSuspend()
 *
 * ˵��:    ����һ������
 *
 * ����:    htask ������
 *
 * ����:    ok/fail
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
        ;  /* ����״̬ʱ, ֻ���Ϲ���ı�� */
    }
    SetTaskState(htask, TASK_STATE_SUSPEND);
    EXIT_CRITICAL();
    
    if(FamesOSStarted==YES && LockNesting==0){
        TaskDispatch();
    }
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    TaskResume()
 *
 * ˵��:    ʹһ�����ҵ�����ָ�����
 *
 * ����:    htask �����(���)
 *
 * ����:    ok/fail
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
 * ����:    SetTaskState()
 *
 * ˵��:    ��������״̬
 *
 * ����:    htask �������(����)
 *          state ״̬
 *
 * ���:    ok/fail
 *
 * ע��:    �˺���ֻ�����ĸı�����״̬,�����������Ľ����������(ֻ���п��ܱ�����)
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
 * ����:    TaskTimeTick()
 *
 * ˵��:    ˢ�¸�����֮�ڲ���ʱ��, ��TimeTick()���� 
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
 * ����:    TaskStackCheck()
 *
 * ˵��:    ����ջ��ʹ�����
 *
 * ����:    htask  ������
 *          opt    ��ջ���ѡ��
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
 * ����:    GetHighestTask()
 *
 * ˵��:    ȡ����һ����Ҫ���е�����
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
 * ����:    DelHighestFrom()
 *
 * ˵��:    ��ĳ�����������ȡ������Ȩ��ߵ��Ǹ�����
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
 * ����:    DelTaskFromList()
 *
 * ˵��:    ��ĳ�����������ȡ��ָ��������
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
 * ����:    AddTaskToList()
 *
 * ˵��:    ��һ������ӵ�ĳ���������֮��
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
 * ����:    SetTimeSlice()
 *
 * ����:    ����ʱ��Ƭ��С
 *
 * ����:    val   �µ�ʱ��Ƭ��С, 0=�ر�ʱ��Ƭ��ת����
 *
 * ˵��:    ʱ��Ƭ��ת������Ҫ�����ھ�����ͬ���ȼ�������֮��, ��ͬ���ȼ���
 *          ����֮�䰴���ȼ�����.
 *          Ҫʹ��ʱ��Ƭ��ת����, ��������ѡ��:
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
 * ����:    GetTimeSlice()
 *
 * ˵��:    ����ʱ��Ƭ��С
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
 * ����:    RoundRobinTask()
 *
 * ����:    ����ǰ�����Ƶ���ǰ���ȼ���ĩβ(������ת)
 *
 * ˵��:    �˺���ֻ��TimeTick()�е���, Ҳ����˵���ж���������ִ��
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
        return; /* �����ȼ�ֻ��һ������, ��ô�Ͳ���Ҫ��ת��. */
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
 * ���ļ�����: task.c
 * 
**==================================================================================*/

