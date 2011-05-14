/*************************************************************************************
** 文件: timer.c
** 说明: 提供定时器服务
** 作者: Jun
** 时间: 2010-7-7
*************************************************************************************/
#define  FAMES_TIMER_C
#include "includes.h"

/*------------------------------------------------------------------------------------
* 函数: TimerInit()
* 说明: 初始化定时器
* 注意: 此函数应在用户的第一个任务中调用, 且只能调用一次
**----------------------------------------------------------------------------------*/
void apical TimerInit(void)
{
    INT16S i;
    void __internal __TimerDefaultFunc(void *,INT16S);
    void __daemon TaskTimerDPC(void * data); 

    CALLED_ONLY_ONCE();

    NumberOfTimerDPC=0;
    for(i=0; i<TIMER_MAX_NUM; i++){
        TIMER[i].timer=0L;
        TIMER[i].timer_initialize=0L;
        TIMER[i].timer_type=TIMER_TYPE_ONCE;
        TIMER[i].timer_status=TIMER_STATUS_CLOSE;
        TIMER[i].timer_func=__TimerDefaultFunc;
        TIMER[i].timer_data=NULL;
        TIMER[i].timer_dpc=0;
    }
    TaskTimerHandle=TaskCreate(TaskTimerDPC, NULL, "#timer", NULL, TaskTimerStkSize, TaskTimerPrio, TASK_CREATE_OPT_NONE);
    TimerInitedFlag=YES;
}

/*------------------------------------------------------------------------------------
* 函数: TimerDefaultFunc()
* 说明: 定时器回调函数之缺省函数
**----------------------------------------------------------------------------------*/
void __internal __TimerDefaultFunc(void * data, INT16S nr)
{
    data=data;
    nr=nr;
    return;
}

/*------------------------------------------------------------------------------------
* 函数: TimerStop()
* 说明: 关闭定时器
* 输入: 定时器应用序号
**----------------------------------------------------------------------------------*/
BOOL apical TimerStop(INT16S timer)
{
    CRITICAL_DEFINE()
    
    if(timer<0||timer>=TIMER_MAX_NUM){
        return fail;
    }
    ENTER_CRITICAL();
    TIMER[timer].timer_status=TIMER_STATUS_CLOSE;
    TIMER[timer].timer=0L;;
    EXIT_CRITICAL();
    return ok;
}

/*------------------------------------------------------------------------------------
* 函数: TimerStart()
* 说明: 定时器开始(启动)
* 输入: 定时器应用序号
**----------------------------------------------------------------------------------*/
BOOL apical TimerStart(INT16S timer)
{
    CRITICAL_DEFINE()
    
    if(timer<0||timer>=TIMER_MAX_NUM){
        return fail;
    }
    ENTER_CRITICAL();
    TIMER[timer].timer=TIMER[timer].timer_initialize;
    TIMER[timer].timer_status=TIMER_STATUS_RUN;
    EXIT_CRITICAL();
    return ok;
}

/*------------------------------------------------------------------------------------
* 函数: TimerSet()
* 说明: 定时器设置
* 输入: timer       定时器应用序号(0~TIMER_MAX_NUM-1)
*       InitValue   定时器初始值(须大于0)
*       TimerType   定时器类型(TIMER_TYPE_KEEP代表类型不变)
*       TimerFunc   定时器回调函数(NULL代表回调函数不变)
*       TimerData   定时器回调函数的参数
* 输出: 无
* 注意: 此函数会启动目标定时器
**----------------------------------------------------------------------------------*/
BOOL apical TimerSet(INT16S timer, INT32S InitValue, INT16S TimerType, 
                     void (*TimerFunc)(void *, INT16S), void * TimerData)
{
    if(timer<0||timer>=TIMER_MAX_NUM){
        return fail;
    }
    if(InitValue<0L){
        return fail;
    }
    if(TimerType==TIMER_TYPE_KEEP){
        TimerType=TIMER[timer].timer_type;
    }
    switch(TimerType){
        case TIMER_TYPE_AUTO:
        case TIMER_TYPE_ONCE:
        case TIMER_TYPE_AUTO|TIMER_TYPE_HARD:
        case TIMER_TYPE_ONCE|TIMER_TYPE_HARD:
            break;
        default:
            return fail;
    }
    if(TimerFunc==NULL){
        TimerFunc=TIMER[timer].timer_func;
    }
    TimerStop(timer);
    TIMER[timer].timer=InitValue;
    TIMER[timer].timer_initialize=InitValue;
    TIMER[timer].timer_type=TimerType;
    TIMER[timer].timer_func=TimerFunc;
    TIMER[timer].timer_data=TimerData;
    TIMER[timer].timer_dpc=0;
    TimerStart(timer);
    return ok;
}

/*------------------------------------------------------------------------------------
* 函数: TimerGet()
* 说明: 读取定时器当前值
* 输入: 定时器应用序号
* 输出: 定时器当前值
**----------------------------------------------------------------------------------*/
INT32S apical TimerGet(INT16S timer)
{
    INT32S temp_timer;
    CRITICAL_DEFINE()
    
    if(timer<0||timer>=TIMER_MAX_NUM){
        return (INT32S)-1L;
    }
    ENTER_CRITICAL();
    temp_timer=TIMER[timer].timer;
    EXIT_CRITICAL();
    return temp_timer;
}

/*------------------------------------------------------------------------------------
* 函数: TimerPulse()
* 说明: 临时改变定时器的超时值(定时器脉冲)
* 输入: 定时器应用序号, 下次超时时间
* 输出: ok/fail
* 注意: 此函数需要定时器处于启动状态
**----------------------------------------------------------------------------------*/
BOOL apical TimerPulse(INT16S timer, INT32S value)
{
    CRITICAL_DEFINE()
    
    if(timer<0||timer>=TIMER_MAX_NUM){
        return fail;
    }
    if(TIMER[timer].timer_status!=TIMER_STATUS_RUN){
        return fail;
    }
    ENTER_CRITICAL();
    TIMER[timer].timer=value;
    EXIT_CRITICAL();
    return ok;
}

/*------------------------------------------------------------------------------------
* 函数: TimerForce()
* 说明: 强制使定时器超时
* 输入: 定时器应用序号
**----------------------------------------------------------------------------------*/
BOOL apical TimerForce(INT16S timer)
{
    CRITICAL_DEFINE()
    
    if(timer<0||timer>=TIMER_MAX_NUM){
        return fail;
    }
    ENTER_CRITICAL();
    TIMER[timer].timer=(INT32S)0L;
    EXIT_CRITICAL();
    return ok;
}

/*------------------------------------------------------------------------------------
* 函数: TimerTimeTick()
* 说明: 定时器之时钟滴答服务器
**----------------------------------------------------------------------------------*/
void apical TimerTimeTick(void)
{
    INT16S    tt;
    TimerCB  *pt;

    if(TimerInitedFlag!=YES){
        return;
    }
    pt=TIMER;
    for(tt=0; tt<TIMER_MAX_NUM; tt++, pt++){
        if(pt->timer_status==TIMER_STATUS_CLOSE)continue;
        if(pt->timer_status==TIMER_STATUS_LAST) continue;
        pt->timer--;
        if(pt->timer<=0L){
            if(pt->timer_type&TIMER_TYPE_HARD){ /*硬实时标志*/
                if(pt->timer_type==(TIMER_TYPE_AUTO|TIMER_TYPE_HARD)){
                    if(pt->timer_initialize>0L){
                        pt->timer=pt->timer_initialize;
                    } else {
                        pt->timer_status=TIMER_STATUS_LAST;
                    }
                } else {
                    pt->timer_status=TIMER_STATUS_LAST;
                }
                (*pt->timer_func)(pt->timer_data, 1);
            } else {                            /* DPC */
                if(pt->timer_type==TIMER_TYPE_AUTO){
                    if(pt->timer_initialize>0L){
                        pt->timer=pt->timer_initialize;
                    } else {
                        pt->timer_status=TIMER_STATUS_LAST;
                    }
                } else {
                    pt->timer_status=TIMER_STATUS_LAST;
                }
                PushTimerDPC(tt);
            }
        }
    }
    return;
}

/*------------------------------------------------------------------------------------
* 函数: PushTimerDPC()
* 说明: 增加一个DPC请求
* 输入: 定时器应用序号
**----------------------------------------------------------------------------------*/
BOOL apical PushTimerDPC(INT16S timer)
{
    if(timer<0||timer>=TIMER_MAX_NUM){
        return fail;
    }
    if(NumberOfTimerDPC >= TIMER_DPC_NUM_MAX ){
        return fail;
    }
    if(NumberOfTimerDPC < 0){
        NumberOfTimerDPC = 0;
    }
    TIMER[timer].timer_dpc++;
    NumberOfTimerDPC++;
    if(NumberOfTimerDPC==TIMER_DPC_NUM_MAX){
        return fail;
    }
    return ok;
}

/*------------------------------------------------------------------------------------
* 函数: PopTimerDPC()
* 说明: 取出一个DPC请求
* 输出: ptimer  发出了DPC请求的定时器序号
*       pnr     对应定时器序号的DPC请求个数
* 返回值: fail: 没有DPC请求
*         ok:   有DPC请求且已将对应的定时器序号放入了ptimer
**----------------------------------------------------------------------------------*/
BOOL apical PopTimerDPC(INT16S * ptimer, INT16S * pnr)
{/*lint --e{613}*/
    INT16S tt;
    CRITICAL_DEFINE()

    FamesAssert(ptimer);
    FamesAssert(pnr);

    ENTER_CRITICAL();
    if(NumberOfTimerDPC > 0){
        *ptimer=-1;
        *pnr=0;
        for(tt=0; tt<TIMER_MAX_NUM; tt++){
            if(TIMER[tt].timer_dpc>0){
                *ptimer=tt;
                *pnr   =TIMER[tt].timer_dpc;
                TIMER[tt].timer_dpc=0;
                NumberOfTimerDPC-=(*pnr);
                EXIT_CRITICAL();
                return ok;
            }
        }
    }
    *ptimer=(INT16S)-1;
    *pnr=0;
    NumberOfTimerDPC=0;
    EXIT_CRITICAL();
    return fail;
}

/*------------------------------------------------------------------------------------
* 函数: TaskTimerDPC()
* 说明: DPC任务, 用于处理DPC请求(DPC=Delayed Procedure Call)
**----------------------------------------------------------------------------------*/
void __daemon TaskTimerDPC(void * data)
{
    INT16S timer_dpc;
    INT16S dpc_nbr;
    CRITICAL_DEFINE()

    data = data;

    while(TimerInitedFlag!=YES){
        TaskSleep(10L);
    }
    
    for(;;){  /* lint 不喜欢while(1) */
        ENTER_CRITICAL();
        CurrentTCB->TaskTimer=10L;   /* 每次DPC处理不能超过10毫秒 */
        EXIT_CRITICAL();
        while(PopTimerDPC(&timer_dpc, &dpc_nbr)){
            if(timer_dpc<0 || timer_dpc >= TIMER_MAX_NUM)break;
            switch(TIMER[timer_dpc].timer_status){
                case TIMER_STATUS_CLOSE:
                    continue;
                case TIMER_STATUS_LAST:
                    ENTER_CRITICAL();
                    TIMER[timer_dpc].timer_status=TIMER_STATUS_CLOSE;
                    EXIT_CRITICAL();
                    break;
                default:
                    break;
            }
            if(TIMER[timer_dpc].timer_func==NULL)break;
            (*TIMER[timer_dpc].timer_func)(TIMER[timer_dpc].timer_data, dpc_nbr);
            if(CurrentTCB->TaskTimer==(INT32U)0L)break;/* 时间到了, 做别的事情去吧 */
        }
        TaskSleep(1L);
    }
}


/*
*本文件结束: timer.c =================================================================
*/

