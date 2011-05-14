/*******************************************************************************************
 * �ļ�:    task.h
 *
 * ˵��:    �������
 *
 * ����:    Jun
 *
 * ʱ��:    2010-7-5
*******************************************************************************************/
#ifndef FAMES_TASK_H
#define FAMES_TASK_H

/*
*�������� ----------------------------------------------------------------------------------
*/
#define SWITCH_HOOK_EN  1                   /* �����л�����, 1=����                     */
#define TASK_STAT_EN    1                   /* ͳ������, 1=����                         */
#define TASK_FORCE_INFINITE_LOOP  0         /* �������ṹ: 1=����������ѭ��, 0=�ɷ��� */

/*------------------------------------------------------------------------------------------
 *                 ������ƿ�(TaskControlBlock, TCB)
 * 
 * ˵��: һ��TCB�ʹ���һ������, ��TCB�д洢�ж�Ӧ����Ķ�ջ��������Ϣ
 *
 *       1) TopOfStack:  ָ������ջ��, ���Ԫ�ر�����ڽṹ��ĵ�һ��λ����
 *       2) Stack:       ָ������ջ��(��ջ�Ŀ�ʼλ��)
 *       3) StackDepth:  ��ջ���, ��ʾ��ջ�������ɵ���������(STACK_TYPE), 
 *                       �������ֽ���(BYTES)
 *       4) StackBuf:    ���ڶ�ջ֮���������׵�ַ()
 *       5) TaskName:    ������
 *       6) Priority:    ���ȼ�
 *       7) TaskState:   ����״̬
 *       8) TaskTimer:   ����ʱ��
 *       9) TaskRunning: �������е�ʱ��(����)(�ݲ�֧��)
 *      10) TimeCount:   ʱ��Ƭ����, ����ʱ��Ƭ��ת����
 *      11) Prev, Next:  TCB����ָ��
 *      12) Event:       �������ȴ����¼�
 *      13) message:     ��Ϣ(�����յ�����Ϣ)
 *      14) msg_handler: ��Ϣ��������
 *      15) wake_on_msg: ������Ϣ����ʱ, �Ƿ�������(��Ҫ����WaitMessage())
 *                       YES=�յ���Ϣ��Ҫ����, NO=����Ҫ����, 
 *                       -YES=���յ���Ϣ, ���ѱ�msg_handler�����.
 *      16) console      ����̨��־, 1=�Ѵ�, 0=δ��(��ʱ���ܽ��ܼ�������)
 *      17) TcbUsed:     ���TCB�Ƿ�����(BOOLEAN)
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

#define  STACK_SIZE_BASE 0                  /* �����ջ����     */

/*
*����״̬ ----------------------------------------------------------------------------------
*/
#define TASK_STATE_READY    0x0000u         /* ����             */
#define TASK_STATE_SLEEP    0x0001u         /* ˯��             */
#define TASK_STATE_WAIT     0x0002u         /* �ȴ�һ���¼�     */
#define TASK_STATE_SUSPEND  0x0004u         /* ����             */

#define TASK_STATE_DORMANT  0x0008u         /* ��ɾ��           */

#define TASK_STATE_UNSET    0x8000u         /* ����״̬ȡ����־ */

/*
*TCB��USED��־ -----------------------------------------------------------------------------
*/
#define TCB_USED_NO          0      
#define TCB_USED_YES         1      
#define TCB_USED_DELETED     2      

/*------------------------------------------------------------------------------------------
 *                   ���ȼ���ض���(Priority)
 * 
 * ˵��: ��RTOS��, ����Ȩһ�����ھ����������Ҫ����, Խ����Ҫ��, ���Ƕ�ʱ�估��Ӧ�ٶ�Ҫ��
 *       Խ�ϸ��, ������Ȩ��Ӧ��Խ��. ��FamesOS��, ����Ȩ�Ĵ�С������������ȼ�������
 *       ��, ���ȼ�ԽС(0), ����Ȩ��Խ��.
 *       ��FamesOS��, һ����������Ӧһ�����ȼ�, ��һ�����ȼ����Զ�Ӧ�������
 *       һ������ֻ�ܱ���������Ȩ������ռ��, ͬ��֮�䲻���໥ռ��, ֻ������Ѳ�ķ�ʽ��
 *       �е���.
 *
 *       1) MAX_PRIORITY      ϵͳ֧�ֵ�������ȼ�����, Ϊ�����ִ��Ч�ʼ���С�ռ�
 *                            ����, ���ֵ��Ӧ�ô���32(sizeof(long))
 *
 *       2) PRIOMAP_TYPE      ���ȼ�λͼ����, ���ȼ�λͼ������ÿһλ����һ�����ȼ�
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
*������ر��� ------------------------------------------------------------------------------
*/
#ifdef  FAMES_TASK_C
TCB     TCBS[MAX_TASKS]= {{NULL,},};        /* ϵͳ���õ�TCB����                   */
TCB    *CurrentTCB  = NULL;                 /* ��ǰ�����TCBָ��                   */
HANDLE  CurrentTask    = 0;                 /* ��ǰ��������(���)                */
INT32U  TaskSwitches   = 0UL;               /* �����л��Ĵ���                      */
INT16S  NumberOfTasks  = 0;                 /* ϵͳ�е�ǰ��������                  */

INT32U  TaskIdleCnts   = 0UL;               /* Idle����ִ�еĴ���                  */
INT32U  TaskIdleCtMx   = 0UL;               /* Idle����ִ�е�������(��1����)     */
HANDLE  TaskIdleHandle = 0;                 /* Idle����ľ��                      */
INT16U  TaskIdlePrio   = (MAX_PRIORITY-1);  /* Idle��������ȼ�                    */
#if TASK_STAT_EN == 1
HANDLE  TaskStatHandle = 0;                 /* Stat����ľ��(ϵͳͳ������)        */
INT16U  TaskStatPrio   = (MAX_PRIORITY-2);  /* Stat��������ȼ�                    */
BOOL    TaskStatReady  = NO;                /* Stat�����Ƿ���׼����                */
INT16U  CPU_USED       = 0;                 /* CPUʹ����(�ٷֱ�)                   */
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
 *                   �����������
 * 
 * ˵��: ������нṹ���ڱ���ϵͳ�е�����
 *
 *       1) READYLIST    ���������
 *
 *       2) SLEEPLIST    ����˯�߱�
 *
 *       3) InitTaskList ��ʼ��һ�������
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
 *                  �ٽ�������(CRITICAL AREA PROTECTION)
 * 
 * ˵��: �ڳ��������, ��������ôһЩ�Ƚ�����Ĳ���, ��Щ���ֱ���Ҫһ����ִ�����, �м�
 *       �������κα���ϵĿ���(����Ӳ���ж�), ����������, ��ô���п��ܲ�������Ԥ�ϵ�
 *       ���. ����������Ĳ���, ���ǳ�֮Ϊ"�ٽ���"
 *
 *       �ٽ�����Ҫ����, �������������򵥵İ취: ���ж�!
 *
 *       1) ENTER_CRITICAL()    �����ٽ���֮ǰִ��(���ж�)
 *       2) EXIT_CRITICAL()     �˳��ٽ���֮��ִ��(���ж�)
 *       3) CRITICAL_DEFINE()   �ٽ������������ȶ���ı���
**----------------------------------------------------------------------------------------*/
#define CRITICAL_METHOD    1                /* �ٽ�����������                      */

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

#define NOP()             asm db 90h        /* �޶���ָ��                          */

#define DISABLE_INT()     asm cli           /* ��ֹ�ж�                            */
#define ENABLE_INT()      asm sti           /* ���ж�                            */

/*------------------------------------------------------------------------------------------
 *                  �������л�
 * 
 * ˵��: ������صĺ궨��: SWITCH_CONTEXT(), FIRST_CONTEXT()
 *
 * �ر�: �����˼������FreeRTOS��PC��ֲ����
 *
 * ԭ��: 1) SWITCH_CONTEXT():
 *         �Ƚ�һ������Ķ�ջָ�뱣�浽��TCB֮��, Ȼ�����GetHighestTask()
 *         �õ���һ����Ҫִ�е�����, �����������TCB�ָ���ջ, Ȼ�����ִ��
 *       2) FIRST_CONTEXT():
 *         �Ե�һ��ִ�е������TCB���ض�ջָ��, Ȼ����һ��IRETָ��ǿ������
 *
 * �����ı��漰�����Ļָ��Ĺ����ɱ������Զ�����(�ο�TC2.0֮interrupt�ؼ���)
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
*�����л� ----------------------------------------------------------------------------------
*/
#define DoTaskSwitch()  asm int FamesOS;    /* �����л���ʵ�ʶ���                  */

#ifdef FAMES_TASK_C
static void interrupt  (*OldVectInDOS)();   /* �ɵ��ж�����(0x80)                  */
#endif

/*
*��ջ���ѡ�� ------------------------------------------------------------------------------
*/
#define STACK_CHECK_OPT_TOTAL    0          /* ���ض�ջ���ܴ�С                    */
#define STACK_CHECK_OPT_USED     1          /* ���ض�ջ�����ô�С                  */
#define STACK_CHECK_OPT_REMAIN   2          /* ���ض�ջʣ��ռ�Ĵ�С              */
#define STACK_CHECK_OPT_GET_BUF  3          /* ���ض�ջ��ָ��                      */
#define STACK_CHECK_OPT_GET_BTM  4          /* ���ض�ջջ��ָ��                    */
#define STACK_CHECK_OPT_GET_PTR  5          /* ���ض�ջ��ǰָ��                    */

/*
*���񴴽�ѡ�� ------------------------------------------------------------------------------
*/
#define TASK_CREATE_OPT_NONE     0          

/*
*���� --------------------------------------------------------------------------------------
*/
#if TASK_FORCE_INFINITE_LOOP == 0
#define task_return return /* ��ǿ������Ϊ����ѭ��ʱ, ��ֱ�ӷ���(�൱��ɾ���Լ�) */
#else
#define task_return for(;;)TaskDelete(CurrentTask)
#endif

/*
*�������� ----------------------------------------------------------------------------------
*/
void     __task   TaskIdle(void * data);                    /* ϵͳ��������              */
#if TASK_STAT_EN == 1
void     __task   TaskStat(void * data);                    /* ϵͳͳ������              */                            
void     apical   InitStat(void);                           /* ��ʼ��ͳ������            */
#endif
#if TASK_FORCE_INFINITE_LOOP == 0
void     apical   TaskReturnCapture(void);                  /* �������񷵻�              */
#endif
void     apical   InitTCB(TCB *tcb);                        /* ��ʼ��TCB                 */
void     apical   InitTasks(void);                          /* ��ʼ�������ջ��TCB       */
void     apical   FreeTasks(void);                          /* �ͷ�InitTasks()������ڴ� */
HANDLE   apical   TaskCreate(TASK_TYPE  TaskEntry,          /* ����һ������              */
                             void      *TaskPara, 
                             INT08S    *TaskName,
                             void      *TaskStack,
                             INT16U     StackSize, 
                             INT16U     Prio, INT16U TaskOpt);
BOOL     apical   TaskDelete(HANDLE hTask);                 /* ɾ��һ������              */
void  interrupt   SwitchContextISR(void);                   /* �����ļ��л�ISR           */
void     apical   InitSwitch(void);                         /* ��װSwitchContextISR()    */
void     apical   StopSwitch(void);                         /* ȡ��InitSwitch()�Ķ���    */
void     apical   TaskDispatch(void);                       /* �뿪��ǰ����,�е���һ���� */
void     apical   DispatchLock(void);                       /* �����������              */
void     apical   DispatchUnlock(void);                     /* ������ȿ���              */
void     apical   TaskSleep(INT32U ms);                     /* ����˯��                  */
BOOL     apical   TaskAwake(HANDLE htask);                  /* ������                  */
BOOL     apical   TaskWait(void * event, INT32U timeout);   /* �ȴ�һ���¼�              */
BOOL     apical   TaskSuspend(HANDLE htask);                /* ����                      */
INT16S   apical   TaskResume(HANDLE htask);                 /* �ָ����������            */
BOOL     apical   SetTaskState(HANDLE htask, INT16U state); /* ��������״̬              */
void     apical   TaskTimeTick(void);                       /* ����Ķ�ʱʱ��            */
INT32U   apical   TaskStackCheck(HANDLE htask, INT16U opt); /* ����ջ��ʹ�����        */
void     apical   GetHighestTask(void);                     /* ȡ����һ����Ҫ���е�����  */
TCB    * apical   DelHighestFrom(TASK_LIST * tasklist);      
BOOL     apical   DelTaskFromList(TASK_LIST *, TCB *); 
BOOL     apical   AddTaskToList(TASK_LIST *, TCB *);

void     apical   TaskSwitchHook(void);                     /* �����л�����(���û�����)  */

#if DISPATCH_ROUNDROBIN_EN == 1
BOOL     apical   SetTimeSlice(INT16S new_value);
INT16S   apical   GetTimeSlice(void);
void     apical   RoundRobinTask(void);
#endif

#define  TaskDelay(ms) TaskSleep((INT32U)(ms))



#endif /* #ifndef FAMES_TASK_H */

/*==========================================================================================
 * 
 * ���ļ�����: task.h
 * 
**========================================================================================*/

