/***********************************************************************************************
 * �ļ�:    init.c
 *
 * ����:    FamesOS��ʼ��
 *
 * ����:    Jun
 *
 * ʱ��:    2010-06-28 (����)
 *          2010-12-14 (�޸�)
 *
 * ˵��:    1) ���ڴ��ļ��������̶���main()������ʵ��, ������Ӧ�ó�����
 *             ��������main()Ϊ�������.
 *          2) FamesOS�ڴ˴���ʼ��, ��������һ���û�����(start)
***********************************************************************************************/
#define  FAMES_INIT_C
#define  FAMES_GLOBAL
#include "includes.h"

/*----------------------------------------------------------------------------------------------
 * ����:    InitFamesOS()
 *
 * ˵��:    ��ʼ��FamesOS
 *
 * ��ע:    ����3����������main()��������, Ӧ�ó��򲻿ɵ���
**--------------------------------------------------------------------------------------------*/
void apical InitFamesOS(void)
{
    CALLED_ONLY_ONCE();
    FamesOSStarted=NO;
    InitializeOnExit(); /* should call this before others */
    InitMemService();
    InitIrqManager();
    InitTasks(); 
    InitSwitch();
    InitTimeTick();
    InitApplication();
}

/*----------------------------------------------------------------------------------------------
 * ����:    StartOS()
 *
 * ˵��:    ����FamesOS��ִ�е�һ������
 *
 * ��ע:    Ӧ�ó��򲻿ɵ���
**--------------------------------------------------------------------------------------------*/
void apical StartOS(void)
{
    CALLED_ONLY_ONCE();  
    outportbyte(0xA0,0x20);
    outportbyte(0x20,0x20);  /* ����һ��EOIָ��                  */
    ExitAppFlag=NO;
    setjmp(JumpBuf);
    if(ExitAppFlag){
        return;
    }
    DISABLE_INT();
    GetHighestTask();
    FamesOSStarted=YES;
    FIRST_CONTEXT();
}

/*----------------------------------------------------------------------------------------------
 * ����:    ExitFamesOS()
 *
 * ˵��:    ��ȫ�˳�FamesOS������,���ص�...[DOS?]
 *
 * ��ע:    Ӧ�ó��򲻿ɵ���
**--------------------------------------------------------------------------------------------*/
void apical ExitFamesOS(void)
{
    StopTimeTick();
    StopSwitch();
    FreeTasks();
    ProcessOnExit();    
}

/*----------------------------------------------------------------------------------------------
 * ����:    ExitApplication()
 *
 * ˵��:    �˳�Ӧ�ó��������,�˺����Ὣ����Ȩ����ExitFamesOS(),�����շ���DOS
 *
 * ��ע:    ��ʹ�ô˺������˳�Ӧ�ó���
**--------------------------------------------------------------------------------------------*/
void apical ExitApplication(void)
{
    if(FamesOSStarted){
        DispatchLock();
        FamesOSStarted=NO;
        ExitAppFlag=YES;
        longjmp(JumpBuf, 1);
    }
}

/*----------------------------------------------------------------------------------------------
 * ����:    InitTimeTick()
 *
 * ˵��:    ��װʱ�ӵδ�(���뷢����)
 *
 * ��ע:    ��TimeTick�ҵ�IRQ0֮��, ����ʼ��8253��ʱ��(Channel-0)
**--------------------------------------------------------------------------------------------*/
void apical InitTimeTick(void)
{
    INT16S hz,hzl,hzh; 

    SecondsFromStart = 0L;
    hz=1193;
    hzl=hz%256;
    hzh=hz/256;
    DISABLE_INT();
    DOS_TimeTick=FamesGetVect(TickIntNo);
    outportb(0x43,(INT08U)0x36);
    outportb(0x40,(INT08U)hzl);
    outportb(0x40,(INT08U)hzh); 
    FamesSetVect(TickIntNo,TimeTick);
    ENABLE_INT(); 
}

/*----------------------------------------------------------------------------------------------
 * ����:    StopTimeTick()
 *
 * ˵��:    �ر�ʱ�ӵδ�(���뷢����)
**--------------------------------------------------------------------------------------------*/
void apical StopTimeTick(void)
{
    DISABLE_INT();
    FamesSetVect(TickIntNo,DOS_TimeTick);
    outportb(0x43,0x36);
    outportb(0x40,0x00);
    outportb(0x40,0x00);
    ENABLE_INT();
}

/*----------------------------------------------------------------------------------------------
 * ����:    TimeTick()
 *
 * ˵��:    ʱ�ӵδ��������(ʱ���ж�֮ISR)
 *
 * ��ע:    1����ִ��1��
**--------------------------------------------------------------------------------------------*/
void interrupt TimeTick(void)
{
    static INT16S dos_timer=0;
    
    #if DISPATCH_PREEMPTIVE_EN == 1 && DISPATCH_ROUNDROBIN_EN == 1
    static INT16S * pTimeCount;
    #endif
    
    IntNesting++;
    if (dos_timer++>=54) {
       asm push ds;
       asm push es;
       DOS_TimeTick();
       asm pop  es;
       asm pop  ds;
       dos_timer=0;
    } else {
       outportb(0x20,0x20);
    }
    if(FamesOSStarted==YES){
        SecondsFromStart++;
        TaskTimeTick();                     /* ˢ�¸�����֮�ڲ���ʱ��         */
        TimerTimeTick();                    /* ˢ�¶�ʱ����ʱֵ�������ص����� */ 
    }
    IntNesting--;
    #if DISPATCH_PREEMPTIVE_EN == 1
    if(FamesOSStarted!=YES)return;
    #if DISPATCH_ROUNDROBIN_EN == 1
    if(TimeSlice>0){                        /* ʱ��Ƭ��ת����                 */
        pTimeCount = &CurrentTCB->TimeCount;
        (*pTimeCount)++;
        if((*pTimeCount) >= TimeSlice){     /* ʱ��Ƭ�ѵ� */
            if(IntNesting || LockNesting){
                (*pTimeCount) = TimeSlice;
                return;
            } else {
                *pTimeCount=0;
                RoundRobinTask();
            }
        }
    }
    #endif /* #if DISPATCH_ROUNDROBIN_EN == 1 */
    if(IntNesting!=0)return;
    if(LockNesting!=0)return;
    SWITCH_CONTEXT();
    #endif /* #if DISPATCH_PREEMPTIVE_EN == 1 */
}

/*----------------------------------------------------------------------------------------------
 * ����:    ElapsedInit()
 *
 * ˵��:    ��������ʱ���ʼ��, ʵ����ֻ�����ڼ���ElapsedOverhead
 *
 * ��ע:    �����������������������ο���uC/OS-II(Jean J. Labrosse)��PC.C�ļ�
**--------------------------------------------------------------------------------------------*/
void apical ElapsedInit(void)
{
    CRITICAL_DEFINE()
    
    ElapsedOverhead = 0;
    ENTER_CRITICAL();
    ElapsedStart();
    ElapsedOverhead = ElapsedStop();
    EXIT_CRITICAL();
}

/*----------------------------------------------------------------------------------------------
 * ����:    ElapsedStart()
 *
 * ˵��:    ��������ʱ�俪ʼ
 *
 * ��ע:    ��������ο���uC/OS-II(Jean J. Labrosse)��PC.C�ļ�
**--------------------------------------------------------------------------------------------*/
void apical ElapsedStart(void)
{
    INT08U data;
    CRITICAL_DEFINE()

    ENTER_CRITICAL();
    data  = (INT08U)inp(0x61);              /* Disable timer #2                    */
    data &= 0xFE;
    outp(0x61,  data);
    outp(0x43,  0xB0);                      /* Mode 0 (B0=1011 0000)               */
    outp(0x42,  0xFF);
    outp(0x42,  0xFF);
    data |= 0x01;                           /* Start the timer                     */
    outp(0x61, data);
    EXIT_CRITICAL();
}

/*----------------------------------------------------------------------------------------------
 * ����:    ElapsedStop()
 *
 * ˵��:    ��������ʱ�����
 *
 * ����:    ��
 *
 * ���:    ����ʱ��(΢��), �����е�54926L=(1000000*65536/1193180)
 *
 * ��ע:    ��������ο���uC/OS-II(Jean J. Labrosse)��PC.C�ļ�
**--------------------------------------------------------------------------------------------*/
INT16U apical ElapsedStop(void)
{
    INT08U data;
    INT08U low;
    INT08U high;
    INT16U cnts;
    CRITICAL_DEFINE()

    ENTER_CRITICAL();
    data  = inp(0x61);                      /* Disable the timer                   */
    data &= 0xFE;
    outp(0x61, data);
    outp(0x43, 0x80);                       /* Latch the timer value               */
    low  = inp(0x42);
    high = inp(0x42);
    EXIT_CRITICAL();
    cnts = (((INT16U)high<<8)+(INT16U)low);
    cnts = (INT16U)0xFFFF - cnts;           /* Compute the time                    */
    return ((INT16U)(((INT32U)cnts * 54926L) >> 16) - ElapsedOverhead);
}

/*----------------------------------------------------------------------------------------------
 * ����:    FamesDelay()
 *
 * ˵��:    Ӳ��ʱ(����FamesOS����ʱ,�����л�����������!)
 *
 * ����:    1) us     ��ʱ��΢����(ֻ�Ǵ��ʱ��)
 *
 * ���:    ��
 *
 * ��ע:    �˺�����ʹ��ǰ���ʼ��(��һ�����л�ִ�г�ʼ������)
**--------------------------------------------------------------------------------------------*/
void apical FamesDelay(INT32U us)
{
    static BOOL   inited=NO;
    static INT32U loops_per_us=1L;
    CRITICAL_DEFINE()

    ENTER_CRITICAL();
    if(inited){
        INT32S loops;
        while(us>0){
            loops=(INT32S)loops_per_us;
            while(loops>0L){
                loops--;
            }
            us--;
        }
    } else {                                  /* ��һ������ʱ������1΢���ѭ������  */
        INT16U us_of_loops;
        INT32U loops;
        loops=(INT32U)1000000L;
        ElapsedStart();
        while(loops>0L){
           loops--;
        }
        us_of_loops=ElapsedStop();
        if(us_of_loops<=1u){
            us_of_loops=1u;
        }
        loops_per_us=1000000L/(INT32U)us_of_loops;
        loops_per_us-=2L;
        inited=YES;
    }
    EXIT_CRITICAL();
}

#if HW_TYPE == HW_PC
/*----------------------------------------------------------------------------------------------
 * ����:    FamesSetVect()
 *
 * ˵��:    �����ж�����
 *
 * ����:    1) interruptno   �ж�������
 *          2) isr           �жϷ�������
 *
 * ���:    ��
**--------------------------------------------------------------------------------------------*/
void apical FamesSetVect(INT16S interruptno, void interrupt (far *isr)())
{
/*lint --e{413} */
    INT32U * IntVectTbl=NULL; 
    CRITICAL_DEFINE()

    ENTER_CRITICAL();
    IntVectTbl[interruptno] = (INT32U)isr; 
    EXIT_CRITICAL();
}

/*----------------------------------------------------------------------------------------------
 * ����:    __FamesGetVect()
 *
 * ˵��:    ��ȡ�ж�����
 *
 * ����:    1) interruptno   �ж�������
 *
 * ���:    ��ȡ��������ָ��(������һ��INT32U)
 *
 * NOTE:    Ӧ�ó���Ӧ��ֱ�ӵ����������, ��Ӧ��ʹ��FamesGetVect()
**--------------------------------------------------------------------------------------------*/
INT32U apical __FamesGetVect(INT16S interruptno)
{
/*lint --e{413} */
    INT32U * IntVectTbl=NULL;
    INT32U   isr = NULL;
    CRITICAL_DEFINE()

    FamesAssert(interruptno >= 0);
    FamesAssert(interruptno < 256);

    ENTER_CRITICAL();
    isr = IntVectTbl[interruptno];
    EXIT_CRITICAL();
    
    return isr;
}
#endif

/*----------------------------------------------------------------------------------------------
 * ����:    main()
 *
 * ˵��:    C����������
 *
 * ����:    1) argc      �����в�������,  ������������ϵͳ����(DOS?)
 *          2) argv      �����в���ָ��
 *          3) env       �����л���ָ��
 *
 * ���:    ��
**--------------------------------------------------------------------------------------------*/
void main(INT16S argc, INT08S **argv, INT08S **envs)
{
    _argc_ = argc;
    _argv_ = argv;
    _envs_ = envs; 
    InitFamesOS();
    StartOS();
    ExitFamesOS();
}

/*----------------------------------------------------------------------------------------------
 *          ��ʼ����ض���
**--------------------------------------------------------------------------------------------*/
static HANDLE  init_task  = InvalidHandle;
static HANDLE  start_task = InvalidHandle;

void __task init(void *data); /* ��ʼ������ */

void super_key_hook(KEYCODE key);

/*----------------------------------------------------------------------------------------------
 * ����:    InitApplication()
 *
 * ˵��:    Ӧ�ó����ʼ��, ������ʼ������init()
 *
**--------------------------------------------------------------------------------------------*/
void apical InitApplication(void)
{
    CALLED_ONLY_ONCE();
    init_task = TaskCreate(init, NULL, "#init", NULL, 1024, 
                           PRIO_STARTUP, TASK_CREATE_OPT_NONE);
    if(init_task == InvalidHandle){
        sys_print("Fatal: can not create the #init task!\n");
        sys_exit();
    }
}

/*----------------------------------------------------------------------------------------------
 * ����:    init()
 *
 * ˵��:    ��ʼ������, ���ڳ�ʼ����Ҫ�����񻷾���ģ��
 *          ����Ӧ�ó�����ʼ����start
 *
 * ****     start��������Ӧ�ó����ṩ
**--------------------------------------------------------------------------------------------*/
void __task init(void *data)
{
    InitStat(); /* cpu */
    TimerInit();
    InitConsoleSvc();
    ElapsedInit();
    FamesDelay(0L);
    InitGUI();

    data = data;

    if(!RegisterKeyHook(super_key_hook)){
        sys_print("Error: super-key hook create failed!\n");
    }

    start_task = TaskCreate(start, NULL, "start", 
                            NULL, 10240, /* stack-size = 20K */
                            PRIO_STARTUP, TASK_CREATE_OPT_NONE);
    if(start_task == InvalidHandle){
        sys_print("Fatal: can not create the start task!\n");
        sys_exit();
    }
    for(;;){
        TaskSleep(100L); /* 10 times per second */
        /**
        *** do something here, like stack-checking/cpu-balance
        **/
    }
}

/*----------------------------------------------------------------------------------------------
 * ����:    super_key_hook()
 *
 * ˵��:    �������������, ��������ִ��ĳЩ����
 *          ��ϵͳ���涨��, ȫ�ֿ���
**--------------------------------------------------------------------------------------------*/
void __do_super_key(KEYCODE key)
{
    gui_widget * root;
    
    switch(key){
        case ALT_K:
            ExitApplication();
            break;
        case ALT_S:
            root = gui_get_root_widget();
            if(root)
                gui_refresh_widget(root);
            gui_put_root_widget();
            break;
        default:
            break;
    }
}

void super_key_hook(KEYCODE key)
{
    static BOOL super_key_leading = NO;

    switch(key){
        case ALT_X:
            super_key_leading = YES;
            break;
        case SHIFT:
        case CTRL:
        case ALT:
        case NONE_KEY:
        case ALT|SHIFT:
        case ALT|CTRL:
        case CTRL|SHIFT:
        case CTRL|SHIFT|ALT:
            break;
        default:
            if(super_key_leading){
                __do_super_key(key);
            }
            super_key_leading = NO;
            break;
    }

    return;
}

/*==============================================================================================
 * 
 * ���ļ�����: init.c
 * 
**============================================================================================*/

