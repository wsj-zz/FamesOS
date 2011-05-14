/*************************************************************************************
** �ļ�: timer.h
** ˵��: timer.c��ͷ�ļ������а�����timer.c�и���������������ʱ��������ݽṹ����
** ����: Jun
** ʱ��: 2010-7-8
*************************************************************************************/
#ifndef  FAMES_TIMER_H
#define  FAMES_TIMER_H

/*------------------------------------------------------------------------------------
* ��ʱ�����ƿ�
**----------------------------------------------------------------------------------*/
typedef struct TimerControlBlock{
    INT32S   timer;                         /* ��ʱ����ǰֵ                         */    
    INT32S   timer_initialize;              /* ��ʱ����ʼֵ                         */
    INT16S   timer_type;                    /* ��ʱ������                           */
    INT16S   timer_status;                  /* ��ʱ��״̬                           */
    DPC_FUNC timer_func;                    /* ��ʱ���ص�����                       */
    void    *timer_data;                    /* timer_func�Ĳ���                     */
    INT16S   timer_dpc;                     /* ��ʱ�������ӳٵ���֮����ֵ           */
}TimerCB;

#define TIMER_MAX_NUM         32           /* ��ʱ��������                       */
#define TIMER_DPC_NUM_MAX  10000           /* ��ʱ������DPC�����������          */

/*------------------------------------------------------------------------------------
* ��ʱ������
**----------------------------------------------------------------------------------*/
#define TIMER_TYPE_ONCE      0x0            /* ��ʱ������: ֻ����һ�μ��ر�         */
#define TIMER_TYPE_AUTO      0x1            /* ��ʱ������: �Զ����س�ʼֵ���������� */
#define TIMER_TYPE_HARD      0x2            /* ��ʱ������: Ӳʵʱ                   */

#define TIMER_TYPE_KEEP      0x4            /* ����TIMER_SET, ��ʾ���Ͳ���          */

/*------------------------------------------------------------------------------------
* ��ʱ��״̬
**----------------------------------------------------------------------------------*/
#define TIMER_STATUS_CLOSE     0            /* ��ʱ��״̬: �ر�                     */
#define TIMER_STATUS_LAST      1            /* ��ʱ��״̬: ���һ��DPC����(�ڲ���)  */
#define TIMER_STATUS_RUN       2            /* ��ʱ��״̬: ����                     */
#define TIMER_STATUS_BUSY      3            /* ��ʱ��״̬: ��æ(�ݲ���)             */


/*------------------------------------------------------------------------------------
* ��ʱ������
**----------------------------------------------------------------------------------*/
#ifdef FAMES_TIMER_C
static TimerCB  TIMER[TIMER_MAX_NUM];       /* ��ʱ������                           */                
static INT16S   NumberOfTimerDPC=0;         /* ��ʱ������DPC����ĸ���              */
static BOOL     TimerInitedFlag=NO;         /* ��ʱ���Ƿ��ѳ�ʼ��                   */
INT16U          TaskTimerPrio  =PRIO_TIMER; /* ��ʱ����������ȼ�                   */
HANDLE          TaskTimerHandle=0;          /* ��ʱ������ľ��                     */
INT16U          TaskTimerStkSize=1024;      /* ��ʱ������Ķ�ջ��С                 */
#else
extern INT16U   TaskTimerPrio;
extern HANDLE   TaskTimerHandle;
extern INT16U   TaskTimerStkSize;
#endif

/*------------------------------------------------------------------------------------
* ��������
**----------------------------------------------------------------------------------*/
void   apical TimerInit(void);               /* ��ʱ����ʼ��                        */
BOOL   apical TimerStop(INT16S timer);       /* ��ʱ��ֹͣ                          */
BOOL   apical TimerStart(INT16S timer);      /* ��ʱ����ʼ                          */
BOOL   apical TimerSet(INT16S timer,         /* ��ʱ������(������)...               */
               INT32S InitValue,             
               INT16S TimerType,             
               void (*TimerFunc)(void *, INT16S),
               void  *TimerData);      
INT32S apical TimerGet(INT16S timer);        /* ��ȡ��ʱ����ǰֵ                    */
BOOL   apical TimerForce(INT16S timer);      /* ǿ�ȶ�ʱ����ʱ                      */
void   apical TimerTimeTick(void);           /* ��ʱ��ʱ�ӵδ�                      */
BOOL   apical PushTimerDPC(INT16S timer);    /* ����һ����ʱ��DPC����               */
BOOL   apical PopTimerDPC(INT16S*, INT16S*); /* ��ȡһ����ʱ��DPC����               */

#define TimerReStart(tmr) TimerStart(tmr)    /* ��ʱ������                          */

BOOL   apical TimerPulse(INT16S timer, INT32S value); /* ��ʱ�ı䶨ʱ���ĳ�ʱֵ     */

/*------------------------------------------------------------------------------------
* ��ʱ��Ӧ�����, �û����ڴ˴������Լ��Ķ�ʱ�����
**----------------------------------------------------------------------------------*/
enum {
    TimerSys = 5,                      /* ϵͳ��ʱ��                           */
    TimerGUI,                          /* GUI��ʱ��                            */
    TimerNIC,                          /* ��������                             */
    TimerLog,                          /* ����LOG��¼                          */
    TimerArp,                          /* ����ARPЭ��                          */
    TimerTftpdTX,                      /* ����tftp����֮��ʱ�ش�               */
    TimerTftpdRX,                      /* ����tftp����֮���ճ�ʱ               */
    TimerPLC,                          /* ����PLCͨѶ                          */

    Timer_for_test,
    TimerForMainScreen,                /* ���������õĶ�ʱ��                   */
    LastTmrNo                          /* ���Ķ�ʱ�����                     */
};

#if    LastTmrNo >= TIMER_MAX_NUM /*lint !e553*/
#error LastTmrNo >= TIMER_MAX_NUM is not allowed, check it please!
#endif


#endif

/*
*���ļ�����: timer.h =================================================================
*/

