/***********************************************************************************************
 * 文件:    init.c
 *
 * 描述:    FamesOS初始化
 *
 * 作者:    Jun
 *
 * 时间:    2010-06-28 (创建)
 *          2010-12-14 (修改)
 *
 * 说明:    1) 由于此文件包含并固定了main()函数的实现, 所以在应用程序中
 *             不可再以main()为程序入口.
 *          2) FamesOS在此处初始化, 并启动第一个用户任务(start)
***********************************************************************************************/
#define  FAMES_INIT_C
#define  FAMES_GLOBAL
#include "includes.h"

/*----------------------------------------------------------------------------------------------
 * 函数:    InitFamesOS()
 *
 * 说明:    初始化FamesOS
 *
 * 备注:    下面3个函数都由main()函数调用, 应用程序不可调用
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
 * 函数:    StartOS()
 *
 * 说明:    启动FamesOS并执行第一个任务
 *
 * 备注:    应用程序不可调用
**--------------------------------------------------------------------------------------------*/
void apical StartOS(void)
{
    CALLED_ONLY_ONCE();  
    outportbyte(0xA0,0x20);
    outportbyte(0x20,0x20);  /* 发送一次EOI指令                  */
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
 * 函数:    ExitFamesOS()
 *
 * 说明:    完全退出FamesOS的运行,返回到...[DOS?]
 *
 * 备注:    应用程序不可调用
**--------------------------------------------------------------------------------------------*/
void apical ExitFamesOS(void)
{
    StopTimeTick();
    StopSwitch();
    FreeTasks();
    ProcessOnExit();    
}

/*----------------------------------------------------------------------------------------------
 * 函数:    ExitApplication()
 *
 * 说明:    退出应用程序的运行,此函数会将控制权交与ExitFamesOS(),并最终返回DOS
 *
 * 备注:    须使用此函数来退出应用程序
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
 * 函数:    InitTimeTick()
 *
 * 说明:    安装时钟滴答(毫秒发生器)
 *
 * 备注:    将TimeTick挂到IRQ0之上, 并初始化8253定时器(Channel-0)
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
 * 函数:    StopTimeTick()
 *
 * 说明:    关闭时钟滴答(毫秒发生器)
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
 * 函数:    TimeTick()
 *
 * 说明:    时钟滴答服务例程(时钟中断之ISR)
 *
 * 备注:    1毫秒执行1次
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
        TaskTimeTick();                     /* 刷新各任务之内部定时器         */
        TimerTimeTick();                    /* 刷新定时器计时值并触发回调函数 */ 
    }
    IntNesting--;
    #if DISPATCH_PREEMPTIVE_EN == 1
    if(FamesOSStarted!=YES)return;
    #if DISPATCH_ROUNDROBIN_EN == 1
    if(TimeSlice>0){                        /* 时间片轮转调度                 */
        pTimeCount = &CurrentTCB->TimeCount;
        (*pTimeCount)++;
        if((*pTimeCount) >= TimeSlice){     /* 时间片已到 */
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
 * 函数:    ElapsedInit()
 *
 * 说明:    测算消耗时间初始化, 实际上只是用于计算ElapsedOverhead
 *
 * 备注:    这个函数及下面的两个函数参考自uC/OS-II(Jean J. Labrosse)的PC.C文件
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
 * 函数:    ElapsedStart()
 *
 * 说明:    测算消耗时间开始
 *
 * 备注:    这个函数参考自uC/OS-II(Jean J. Labrosse)的PC.C文件
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
 * 函数:    ElapsedStop()
 *
 * 说明:    测算消耗时间结束
 *
 * 输入:    无
 *
 * 输出:    消耗时间(微秒), 程序中的54926L=(1000000*65536/1193180)
 *
 * 备注:    这个函数参考自uC/OS-II(Jean J. Labrosse)的PC.C文件
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
 * 函数:    FamesDelay()
 *
 * 说明:    硬延时(整个FamesOS的延时,不会切换到其它任务!)
 *
 * 输入:    1) us     延时的微秒数(只是大概时间)
 *
 * 输出:    无
 *
 * 备注:    此函数在使用前须初始化(第一次运行会执行初始化动作)
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
    } else {                                  /* 第一次运行时，估算1微秒的循环次数  */
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
 * 函数:    FamesSetVect()
 *
 * 说明:    设置中断向量
 *
 * 输入:    1) interruptno   中断向量号
 *          2) isr           中断服务例程
 *
 * 输出:    无
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
 * 函数:    __FamesGetVect()
 *
 * 说明:    读取中断向量
 *
 * 输入:    1) interruptno   中断向量号
 *
 * 输出:    读取到的向量指针(这里是一个INT32U)
 *
 * NOTE:    应用程序不应该直接调用这个函数, 而应该使用FamesGetVect()
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
 * 函数:    main()
 *
 * 说明:    C语言主函数
 *
 * 输入:    1) argc      命令行参数个数,  这三个参数由系统传入(DOS?)
 *          2) argv      命令行参数指针
 *          3) env       命令行环境指针
 *
 * 输出:    无
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
 *          初始化相关定义
**--------------------------------------------------------------------------------------------*/
static HANDLE  init_task  = InvalidHandle;
static HANDLE  start_task = InvalidHandle;

void __task init(void *data); /* 初始化任务 */

void super_key_hook(KEYCODE key);

/*----------------------------------------------------------------------------------------------
 * 函数:    InitApplication()
 *
 * 说明:    应用程序初始化, 创建初始化任务init()
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
 * 函数:    init()
 *
 * 说明:    初始化任务, 用于初始化需要多任务环境的模块
 *          创建应用程序启始任务start
 *
 * ****     start任务须由应用程序提供
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
 * 函数:    super_key_hook()
 *
 * 说明:    超级命令键钩子, 可无条件执行某些命令
 *          在系统层面定义, 全局可用
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
 * 本文件结束: init.c
 * 
**============================================================================================*/

