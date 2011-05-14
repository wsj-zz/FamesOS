/*************************************************************************************
 * �ļ�: mntr_svc.c
 *
 * ����: ϵͳ���
 *
 * ����: Jun
 *
 * ʱ��: 2010-12-6
 *
 * �汾: ��1��
 *
 * ˵��: ϵͳ����ǻ��ڵ��Ե�Ŀ��, ���Ĺ�����һ����̨�������, ��������ռ�
 *       ϵͳ�е�һЩ��Ϣ, �ʵ��ӹ������������ļ����������豸
*************************************************************************************/
#define  FAMES_MONITOR_SVC_C
#include "includes.h"


#if FAMES_MONITOR_EN == 1
/*------------------------------------------------------------------------------------
 * 
 * ���ݽṹ������
 * 
**----------------------------------------------------------------------------------*/
struct monitor_svc_s {
         mntr_nm  name;            /* ����             */
         mntr_f   func;            /* ����             */
         INT32U   mseconds;        /* ִ�м��ʱ��     */
         INT32U   timer;           /* �ڲ���ʱ��, 0=�� */
         struct monitor_svc_s * next;
};

struct monitor_svc_s * monitor_svc_buffer;
struct monitor_svc_s * monitor_svc_free = NULL;
struct monitor_svc_s * monitor_svc_list = NULL;

HANDLE monitor_svc_handle = InvalidHandle; /* ��̨������     */
INT16U monitor_svc_stack_size = 2048;      /* ��̨�����ջ�ߴ� */

void   __exit     OnExit_MonitorService(void);
void   __daemon   MonitorServiceDaemon(void * data);
void   __internal __InitMonitorSvcStruct(struct monitor_svc_s * s);
struct monitor_svc_s ** __internal __FindMonitorSvcName(mntr_nm name);
    
/*------------------------------------------------------------------------------------
 * ����: InitMonitorService()
 *
 * ˵��: ϵͳ�������ʼ��
**----------------------------------------------------------------------------------*/
void apical __init InitMonitorService(void)
{
    int i;
    
    CALLED_ONLY_ONCE();
    
    monitor_svc_handle = InvalidHandle;
    
    if(!RegisterOnExit(OnExit_MonitorService)){
        sys_print("InitMonitorService: failed to register OnExit routine!\n");
        return;
    }
    lock_kernel();
    monitor_svc_free = NULL;
    monitor_svc_list = NULL;
    monitor_svc_buffer = mem_alloc((INT32U)sizeof(struct monitor_svc_s) * MONITOR_MAX_NUM);
    unlock_kernel();
    if(!monitor_svc_buffer){
        SetErrorCode(ERROR_NO_MEM);
        return;
    }

    for(i=0; i<MONITOR_MAX_NUM; i++){
        __InitMonitorSvcStruct(&monitor_svc_buffer[i]);
        monitor_svc_buffer[i].next = monitor_svc_free;
        monitor_svc_free = &monitor_svc_buffer[i];
    }

    monitor_svc_handle = TaskCreate( MonitorServiceDaemon, NULL, 
                                     "MonitorService", 
                                     NULL, 
                                     monitor_svc_stack_size, 
                                     PRIO_MONITOR, TASK_CREATE_OPT_NONE
                                   );

    if(monitor_svc_handle == InvalidHandle){
        mem_free(monitor_svc_buffer);
        sys_print("InitMonitorService: failed to create Daemon!\n");
        return;
    }
    
    return;
}

/*------------------------------------------------------------------------------------
 * ����: OnExit_MonitorService()
 *
 * ˵��: ϵͳ���ģ���˳�����
**----------------------------------------------------------------------------------*/
void __exit OnExit_MonitorService(void)
{
    if(monitor_svc_handle != InvalidHandle){
        TaskDelete(monitor_svc_handle);
        monitor_svc_handle = InvalidHandle;
    }
    if(monitor_svc_buffer){
        mem_free(monitor_svc_buffer);
        monitor_svc_buffer = NULL;
    }
}


/*------------------------------------------------------------------------------------
 * ����:    RegisterMonitorService()
 *
 * ˵��:    ע��һ��������
**----------------------------------------------------------------------------------*/
BOOL apical RegisterMonitorService(mntr_nm svc_name, mntr_f func, mntr_opt opt)
{
    BOOL   retval;
    struct monitor_svc_s *ms;

    FamesAssert(svc_name);
    FamesAssert(func);
    FamesAssert(opt > 0uL);

    if(!func || !svc_name || 0uL==opt){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(monitor_svc_free){
        ms = monitor_svc_free;       /* allocate a free one      */
        monitor_svc_free = ms->next;
   
        __InitMonitorSvcStruct(ms);
        ms->next = monitor_svc_list; /* link to monitor_svc_list */
        ms->name = svc_name;
        ms->func = func;
        ms->mseconds = (INT32U)opt;
        ms->timer = 1uL;             /* now, opt acts as a timer */
        monitor_svc_list = ms;      

        retval = ok;
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * ����:    DeregisterMonitorService()
 *
 * ˵��:    ע��һ��������(�ɷ�����svc_name��ʶ)
**----------------------------------------------------------------------------------*/
BOOL apical DeregisterMonitorService(mntr_nm svc_name)
{
    BOOL   retval;
    struct monitor_svc_s **pp, *t;
    
    FamesAssert(svc_name);

    if(!svc_name){
        return fail;
    }

    retval = fail;

    lock_kernel();
    pp = __FindMonitorSvcName(svc_name);
    if(*pp){
        t    = (*pp);
       (*pp) = (*pp)->next; /* disconnect */
       __InitMonitorSvcStruct(t);
        t->next = monitor_svc_free;
        monitor_svc_free = t;
        retval = ok;
    }
    unlock_kernel();
    
    return retval;
}

/*------------------------------------------------------------------------------------
 * ����:    MonitorControl()
 *
 * ˵��:    ���ģ��Ŀ���
**----------------------------------------------------------------------------------*/
BOOL apical MonitorControl(mntr_nm svc_name, mntr_cmd cmd, void * data)
{
    BOOL   retval;
    struct monitor_svc_s **t;

    data = data;   /* prevent warning    */

    retval = fail;
    
    switch(cmd){
        case MNTR_CTRL_SVC_START:
            if(monitor_svc_handle != InvalidHandle){
                TaskResume(monitor_svc_handle);
                retval = ok;
            }
            break;
        case MNTR_CTRL_SVC_STOP:
            if(monitor_svc_handle != InvalidHandle){
                TaskSuspend(monitor_svc_handle);
                retval = ok;
            }
            break;
        case MNTR_CTRL_SVC_OPEN:
            if(svc_name){
                lock_kernel();
                t = __FindMonitorSvcName(svc_name);
                if(*t){
                   (*t)->timer = (*t)->mseconds;
                    retval = ok;
                }
                unlock_kernel();
            }
            break;
        case MNTR_CTRL_SVC_CLOSE:
            if(svc_name){
                lock_kernel();
                t = __FindMonitorSvcName(svc_name);
                if(*t){
                   (*t)->timer = 0uL;
                    retval = ok;
                }
                unlock_kernel();
            }
            break;
        default:
            break;
    }

    return retval;
}

/*------------------------------------------------------------------------------------
 * ����: MonitorServiceDaemon()
 *
 * ˵��: ϵͳ�������̨����, �����ջ��С��Ϊ2048
**----------------------------------------------------------------------------------*/
void __daemon MonitorServiceDaemon(void *data)
{
    static BOOL entered = NO;
    struct monitor_svc_s *ms;

    if(entered){
        task_return;
    }
    entered = YES; /* ֻ����һ������ʵ�� */

    data = data;   /* prevent warning    */

    for(;;){
        lock_kernel();
        ms = monitor_svc_list;
        while(ms){
            if(ms->timer>0uL){ 
                ms->timer--;
                if(0uL==ms->timer){
                    ms->timer=ms->mseconds;
                    if(ms->func){
                        (*ms->func)();
                    }
                }
            }
            ms = ms->next;
        }
        unlock_kernel();
        TaskSleep(1L);
    }
}

/*------------------------------------------------------------------------------------
 * ����: __InitMonitorSvcStruct()
 *
 * ˵��: ��ʼ��һ��monitor_svc_s�ṹ
**----------------------------------------------------------------------------------*/
void __internal __InitMonitorSvcStruct(struct monitor_svc_s * s)
{
    FamesAssert(s);

    if(!s)
        return;

    lock_kernel();
    s->name     = "none-name";
    s->func     = NULL;
    s->mseconds = 0uL;
    s->timer    = 0uL;
    s->next     = NULL;
    unlock_kernel();
}

/*------------------------------------------------------------------------------------
 * ����: __FindMonitorSvcName()
 *
 * ˵��: ����һ����������, ���ض�Ӧ�Ľṹָ��
**----------------------------------------------------------------------------------*/
struct monitor_svc_s **  __internal __FindMonitorSvcName(mntr_nm name)
{
    struct monitor_svc_s ** t;
    
    FamesAssert(name);

    t = &monitor_svc_list;

    while(*t){
        if(!(*t)->name)
            continue;
        if(!STRCMP(name, (*t)->name))
            break;
        t = &((struct monitor_svc_s *)((*t)->next));
    }

    return t;
}

#endif /* #if FAMES_MONITOR_EN == 1 */

/*====================================================================================
 * 
 * ���ļ�����: mntr_svc.c
 * 
**==================================================================================*/

