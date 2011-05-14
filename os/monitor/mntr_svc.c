/*************************************************************************************
 * 文件: mntr_svc.c
 *
 * 描述: 系统监测
 *
 * 作者: Jun
 *
 * 时间: 2010-12-6
 *
 * 版本: 第1版
 *
 * 说明: 系统监测是基于调试的目的, 它的功能由一个后台任务完成, 这个任务收集
 *       系统中的一些信息, 适当加工处理后输出到文件或者其它设备
*************************************************************************************/
#define  FAMES_MONITOR_SVC_C
#include "includes.h"


#if FAMES_MONITOR_EN == 1
/*------------------------------------------------------------------------------------
 * 
 * 数据结构及变量
 * 
**----------------------------------------------------------------------------------*/
struct monitor_svc_s {
         mntr_nm  name;            /* 名称             */
         mntr_f   func;            /* 函数             */
         INT32U   mseconds;        /* 执行间隔时间     */
         INT32U   timer;           /* 内部定时器, 0=关 */
         struct monitor_svc_s * next;
};

struct monitor_svc_s * monitor_svc_buffer;
struct monitor_svc_s * monitor_svc_free = NULL;
struct monitor_svc_s * monitor_svc_list = NULL;

HANDLE monitor_svc_handle = InvalidHandle; /* 后台任务句柄     */
INT16U monitor_svc_stack_size = 2048;      /* 后台任务堆栈尺寸 */

void   __exit     OnExit_MonitorService(void);
void   __daemon   MonitorServiceDaemon(void * data);
void   __internal __InitMonitorSvcStruct(struct monitor_svc_s * s);
struct monitor_svc_s ** __internal __FindMonitorSvcName(mntr_nm name);
    
/*------------------------------------------------------------------------------------
 * 函数: InitMonitorService()
 *
 * 说明: 系统监测服务初始化
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
 * 函数: OnExit_MonitorService()
 *
 * 说明: 系统监测模块退出例程
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
 * 函数:    RegisterMonitorService()
 *
 * 说明:    注册一个监测服务
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
 * 函数:    DeregisterMonitorService()
 *
 * 说明:    注销一个监测服务(由服务名svc_name标识)
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
 * 函数:    MonitorControl()
 *
 * 说明:    监测模块的控制
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
 * 函数: MonitorServiceDaemon()
 *
 * 说明: 系统监测服务后台任务, 建议堆栈大小设为2048
**----------------------------------------------------------------------------------*/
void __daemon MonitorServiceDaemon(void *data)
{
    static BOOL entered = NO;
    struct monitor_svc_s *ms;

    if(entered){
        task_return;
    }
    entered = YES; /* 只允许一个运行实例 */

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
 * 函数: __InitMonitorSvcStruct()
 *
 * 说明: 初始化一个monitor_svc_s结构
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
 * 函数: __FindMonitorSvcName()
 *
 * 说明: 查找一个监测服务名, 返回对应的结构指针
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
 * 本文件结束: mntr_svc.c
 * 
**==================================================================================*/

