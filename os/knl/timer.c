/*************************************************************************************
** �ļ�: timer.c
** ˵��: �ṩ��ʱ������
** ����: Jun
** ʱ��: 2010-7-7
*************************************************************************************/
#define  FAMES_TIMER_C
#include "includes.h"

/*------------------------------------------------------------------------------------
* ����: TimerInit()
* ˵��: ��ʼ����ʱ��
* ע��: �˺���Ӧ���û��ĵ�һ�������е���, ��ֻ�ܵ���һ��
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
* ����: TimerDefaultFunc()
* ˵��: ��ʱ���ص�����֮ȱʡ����
**----------------------------------------------------------------------------------*/
void __internal __TimerDefaultFunc(void * data, INT16S nr)
{
    data=data;
    nr=nr;
    return;
}

/*------------------------------------------------------------------------------------
* ����: TimerStop()
* ˵��: �رն�ʱ��
* ����: ��ʱ��Ӧ�����
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
* ����: TimerStart()
* ˵��: ��ʱ����ʼ(����)
* ����: ��ʱ��Ӧ�����
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
* ����: TimerSet()
* ˵��: ��ʱ������
* ����: timer       ��ʱ��Ӧ�����(0~TIMER_MAX_NUM-1)
*       InitValue   ��ʱ����ʼֵ(�����0)
*       TimerType   ��ʱ������(TIMER_TYPE_KEEP�������Ͳ���)
*       TimerFunc   ��ʱ���ص�����(NULL����ص���������)
*       TimerData   ��ʱ���ص������Ĳ���
* ���: ��
* ע��: �˺���������Ŀ�궨ʱ��
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
* ����: TimerGet()
* ˵��: ��ȡ��ʱ����ǰֵ
* ����: ��ʱ��Ӧ�����
* ���: ��ʱ����ǰֵ
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
* ����: TimerPulse()
* ˵��: ��ʱ�ı䶨ʱ���ĳ�ʱֵ(��ʱ������)
* ����: ��ʱ��Ӧ�����, �´γ�ʱʱ��
* ���: ok/fail
* ע��: �˺�����Ҫ��ʱ����������״̬
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
* ����: TimerForce()
* ˵��: ǿ��ʹ��ʱ����ʱ
* ����: ��ʱ��Ӧ�����
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
* ����: TimerTimeTick()
* ˵��: ��ʱ��֮ʱ�ӵδ������
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
            if(pt->timer_type&TIMER_TYPE_HARD){ /*Ӳʵʱ��־*/
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
* ����: PushTimerDPC()
* ˵��: ����һ��DPC����
* ����: ��ʱ��Ӧ�����
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
* ����: PopTimerDPC()
* ˵��: ȡ��һ��DPC����
* ���: ptimer  ������DPC����Ķ�ʱ�����
*       pnr     ��Ӧ��ʱ����ŵ�DPC�������
* ����ֵ: fail: û��DPC����
*         ok:   ��DPC�������ѽ���Ӧ�Ķ�ʱ����ŷ�����ptimer
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
* ����: TaskTimerDPC()
* ˵��: DPC����, ���ڴ���DPC����(DPC=Delayed Procedure Call)
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
    
    for(;;){  /* lint ��ϲ��while(1) */
        ENTER_CRITICAL();
        CurrentTCB->TaskTimer=10L;   /* ÿ��DPC�����ܳ���10���� */
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
            if(CurrentTCB->TaskTimer==(INT32U)0L)break;/* ʱ�䵽��, ���������ȥ�� */
        }
        TaskSleep(1L);
    }
}


/*
*���ļ�����: timer.c =================================================================
*/

