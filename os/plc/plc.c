/**************************************************************************************
 * 文件:    plc.c
 *
 * 说明:    PLC通用例程
 *
 * 作者:    Jun
 *
 * 时间:    2010-9-2
**************************************************************************************/
#define  FAMES_PLC_C
#include <includes.h>

#if FAMES_PLC_SVC_EN == 1

/*------------------------------------------------------------------------------------
 * 
 *       内部函数
 *
**----------------------------------------------------------------------------------*/
void __internal __internal_link_action(PLC * plc, PLC_ACTION * action);
void __internal __internal_unlink_action(PLC * plc, PLC_ACTION * action);
void __internal __internal_do_plc_action(PLC * plc, PLC_ACTION * action);
int  __internal __plc_get_index(PLC * plc);


/*------------------------------------------------------------------------------------
 * 
 *       PLC的分配与释放等...
 *
**----------------------------------------------------------------------------------*/
#define  PLC_MAX_NUM   8             /* 系统支持的PLC最大个数 */

static   PLC PLC_POOL[PLC_MAX_NUM];  /* PLC缓冲池             */

__isr__ (far * __plc_isr[PLC_MAX_NUM])(void);

/*------------------------------------------------------------------------------------
 * 函数:    plc_alloc()
 *
 * 描述:    分配一个PLC控制块
 *
 * 返回:    成功时返回PLC控制块指针, 失败时返回NULL
 *
 * 说明:    返回的控制块已经过了初始化处理
**----------------------------------------------------------------------------------*/
PLC * apical plc_alloc(void)
{
    int i;
    PLC * retval;

    retval = NULL;

    lock_kernel();
    for(i=0; i<PLC_MAX_NUM; i++){
        if(PLC_POOL[i].flag == 0){
            MEMSET((INT08S *)&PLC_POOL[i], 0, sizeof(PLC));
            PLC_POOL[i].flag |= PLC_FLAG_USED;
            os_mutex_init(PLC_POOL[i].lock, 0);
            retval = &PLC_POOL[i];
            break;
        }
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    plc_free()
 *
 * 描述:    释放一个PLC控制块
**----------------------------------------------------------------------------------*/
BOOL apical plc_free(PLC * plc)
{
    BOOL  retval;
    int plc_index;
    
    FamesAssert(plc);
    
    if(!plc) /*lint !e774*/
        return fail;

    plc_index = __plc_get_index(plc);

    FamesAssert(plc_index >= 0);
    FamesAssert(plc_index < PLC_MAX_NUM);

    if(plc_index < 0 || plc_index >= PLC_MAX_NUM)
        return fail;

    retval = fail;

    lock_kernel();
    if(plc->flag & PLC_FLAG_USED){
        if(plc->flag & PLC_FLAG_OPEN){
            shut_plc(plc);
        }
        MEMSET((INT08S *)plc, 0, sizeof(PLC));
        os_mutex_init(plc->lock, 0);
        retval = ok;
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    open_plc()
 *
 * 说明:    打开一个PLC
**----------------------------------------------------------------------------------*/
BOOL apical open_plc(PLC * plc)
{
    void __daemon plc_action_daemon(void * data);
    #if FAMES_DEBUG_PLC == 1
    void __daemon plc_debug_daemon(void * dara);
    #endif
    BOOL retval;
    
    FamesAssert(plc);

    if(!plc)
        return fail;

    if(plc->flag & PLC_FLAG_OPEN){ 
        return fail;  /* already opened */
    }

    os_mutex_lock(plc->lock);

    plc->rx_buf = mem_alloc((INT32U)PLC_BUF_SIZE);
    if(!plc->rx_buf){
        goto out4;
    }
    MEMSET((INT08S *)plc->rx_buf, 0, PLC_BUF_SIZE);
    
    plc->tx_buf = mem_alloc((INT32U)PLC_BUF_SIZE);
    if(!plc->tx_buf){
        goto out3;
    }
    MEMSET((INT08S *)plc->tx_buf, 0, PLC_BUF_SIZE);    
    
    if(!OpenSerialPort(&plc->rs232)){
        goto out2;
    }
    plc->daemon = TaskCreate(plc_action_daemon, (void *)plc, 
                             plc->name, NULL, 1024, PRIO_PLC, 
                             TASK_CREATE_OPT_NONE);
    if(plc->daemon != InvalidHandle){
        plc->flag |= PLC_FLAG_OPEN;
    } else {
        goto out1;
    }
    #if FAMES_DEBUG_PLC == 1
    if(!TaskCreate(plc_debug_daemon, (void *)plc, "plc-debug", NULL, 2048, 
                     PRIO_DEBUG, TASK_CREATE_OPT_NONE)){
        ;
        /*
        * do something here... 
        */
    }
    #endif

    retval = ok;

out:
    os_mutex_unlock(plc->lock);
    return retval;

out1:
    CloseSerialPort(&plc->rs232);
out2:
    mem_free(plc->tx_buf);
out3:
    mem_free(plc->rx_buf);
out4:
    retval = fail;
    goto out;    
}

/*------------------------------------------------------------------------------------
 * 函数:    shut_plc()
 *
 * 说明:    关闭某个PLC
**----------------------------------------------------------------------------------*/
BOOL apical shut_plc(PLC * plc)
{
    FamesAssert(plc);

    if(!plc)
        return fail;

    if(!(plc->flag & PLC_FLAG_OPEN)){ 
        return fail;  /* not opened */
    }

    os_mutex_lock(plc->lock);
    
    if(!CloseSerialPort(&plc->rs232)){
        ; /* should do something here */
    }
    if(plc->daemon != InvalidHandle && plc->daemon != 0){
        TaskDelete(plc->daemon);
        plc->daemon = InvalidHandle;
    }
    plc->flag &= (~PLC_FLAG_OPEN);

    if(plc->rx_buf){
        mem_free(plc->rx_buf);
        plc->rx_buf = NULL;
    }
    if(plc->tx_buf){
        mem_free(plc->tx_buf);
        plc->tx_buf = NULL;
    }
    
    os_mutex_unlock(plc->lock);

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    do_plc_action()
 *
 * 说明:    执行PLC命令
**----------------------------------------------------------------------------------*/
BOOL apical do_plc_action(PLC * plc, PLC_ACTION * action, INT16U flag)
{
    BOOL retval;

    FamesAssert(plc);
    FamesAssert(action);

    if(!plc || !action)
        return fail;

    if(!(plc->flag & PLC_FLAG_OPEN)){ 
        return fail;  /* not opened */
    }

    retval = fail;
    
    os_mutex_lock(plc->lock);

    switch(flag & PLC_ACTION_FLAG_MASK) {
        case PLC_ACTION_FLAG_LINK:
        case PLC_ACTION_FLAG_ONCE_ASYC:
            __internal_link_action(plc, action);
            retval = ok;
            break;
        case PLC_ACTION_FLAG_UNLK:
            __internal_unlink_action(plc, action);
            retval = ok;
            break;
        case PLC_ACTION_FLAG_ONCE:
            __internal_do_plc_action(plc, action);
            retval = ok;
            break;
        default:
            retval = fail;
            break;
    }

    action->flag = flag;

    os_mutex_unlock(plc->lock);
 
    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    plc_set_param()
 *
 * 说明:    设置PLC参数
**----------------------------------------------------------------------------------*/
BOOL apical plc_set_param(PLC * plc, int type, INT08S * name,
                          void * private, int station_no,
                          INT16S base, INT16S irq, INT32S baudrate,
                          INT16S parity, INT16S databits, INT16S stopbits,
                          INT16S fifo_control
                         )
{
    int plc_index;

    FamesAssert(plc);
    FamesAssert(name);

    if(!plc || !name)
        return fail;

    plc_index = __plc_get_index(plc);

    FamesAssert(plc_index >= 0);
    FamesAssert(plc_index < PLC_MAX_NUM);

    if(plc_index < 0 || plc_index >= PLC_MAX_NUM)
        return fail;

    switch(type){
        case PLC_TYPE_FATEK:
        case PLC_TYPE_SIEMENS:
            break;
        default:
            return fail;
    }
    
    os_mutex_lock(plc->lock);
    plc->name = name;
    plc->type = type;
    plc->station_no = station_no;
    plc->private = private;    

    if(!InitSerialPort(&plc->rs232)){
        os_mutex_unlock(plc->lock);
        return fail;
    }
    if(!SetSerialPort(&plc->rs232, base, irq, baudrate, parity, 
              databits, stopbits, COM_INT_BOTH, fifo_control, __plc_isr[plc_index])){
        os_mutex_unlock(plc->lock);
        return fail;
    }

    os_mutex_unlock(plc->lock);
    
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    plc_set_action()
 *
 * 说明:    设置PLC命令(ACTION)
**----------------------------------------------------------------------------------*/
BOOL apical plc_set_action(PLC_ACTION * action, int id,
                           int cmd, INT08S * addr, 
                           void * associated, int number,
                           int timer, void (*finish)(int id, BOOL success),
                           int disabled
                         )
{
    FamesAssert(action);

    if(!action)
        return fail;

    action->flag = 0;
    action->id   = id;
    action->cmd  = cmd;
    action->addr = addr;
    action->associated = associated;
    action->number = number;
    action->timer = 0;
    action->timerinit = timer;
    action->count = 0;
    action->finish = finish;
    action->disabled = disabled;
    action->next = NULL;

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    plc_action_daemon()
 *
 * 说明:    PLC与上位机通讯的任务(其实, 主要用于读PLC)
**----------------------------------------------------------------------------------*/
void __daemon plc_action_daemon(void * data)
{
    PLC_ACTION * action, * ta;
    PLC        * plc;
    prepare_atomic()

    FamesAssert(data);

    if(!data){
        task_return;
    }

    plc = (PLC *)data;

    for(;;){
        if(0 == (plc->flag & PLC_FLAG_OPEN)){
            TaskSleep(200L);
            continue;
        }
        
        os_mutex_lock(plc->lock);
        action = plc->action_list;
        os_mutex_unlock(plc->lock);
        
        if(!action){
            TaskSleep(100L);
            continue;
        }
        
        while(action){
            os_mutex_lock(plc->lock);
            while(action->timer > 0 || action->disabled){ /* 查找一个可以执行的ACTION */
                action = action->next;
                if(!action)
                    goto next_time;
            }
            ta = action->next;
            in_atomic();
            action->timer = action->timerinit;
            out_atomic();
            __internal_do_plc_action(plc, action);
            if(action->flag == PLC_ACTION_FLAG_ONCE_ASYC){
                /* this action runs only once */
                __internal_unlink_action(plc, action);
            }
            action = ta;
        next_time:
            os_mutex_unlock(plc->lock);
            TaskSleep(5L);
        }
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    plc_action_dpc()
 *
 * 说明:    PLC通讯之DPC例程, 用于刷新PLC-ITEM的超时值(优先级)
**----------------------------------------------------------------------------------*/
void __internal plc_action_dpc(void * data, INT16S nr)
{
    PLC_ACTION * action;
    int        i;
    prepare_atomic()

    data = data;

    for(i=0; i<PLC_MAX_NUM; i++){
        if(!(PLC_POOL[i].flag & PLC_FLAG_OPEN))
            continue;
        action = PLC_POOL[i].action_list;
        while(action){
            if(action->timer > 0){
                in_atomic();
                action->timer -= (int)nr;
                out_atomic();
            }
            action = action->next;
        }
    }
    
    #if 0
    printf("plc_action_dpc end: %d\n", i);
    #endif
}

/*----------------------------------------------------------------------------------------------
 * 
 *      PLC所用中断服务程序的定义
 * 
**--------------------------------------------------------------------------------------------*/
__isr__ __internal __plc_common_isr(int plc_index)
{
    FamesAssert(plc_index >= 0);
    FamesAssert(plc_index < PLC_MAX_NUM);

    #if 0
    textprintstr(1, 5+plc_index, "__plc_common_isr: plc_index= ", 0);
    textprintdec16(30, 5+plc_index, plc_index, 0, 0);
    #endif
    
    enter_isr();

    if(plc_index < 0 || plc_index >= PLC_MAX_NUM)
        isr_return_false();
    
    switch(PLC_POOL[plc_index].type){
        #if  FAMES_PLC_FATEK_EN == 1
        case PLC_TYPE_FATEK:
            plc_common_isr_fatek(&PLC_POOL[plc_index]);
            isr_return_false();
        #endif
        #if  FAMES_PLC_SIEMENS_EN == 1
        case PLC_TYPE_SIEMENS:
            plc_common_isr_siemens(&PLC_POOL[plc_index]);
            isr_return_false();
        #endif
        default: 
            isr_return_false(); 
    }
    return fail; /*lint !e527; 其实执行不到这一句*/
}

/* 
 * PLC所用ISR的定义宏
*/
#define  define_plc_isr_name(x) __isr__ __plc_isr_##x(void)
#define  define_plc_isr_body(x) return __plc_common_isr((x))


#if PLC_MAX_NUM >= 1
define_plc_isr_name(0)
{
    define_plc_isr_body(0);
}
#endif
#if PLC_MAX_NUM >= 2
define_plc_isr_name(1)
{
    define_plc_isr_body(1);
}
#endif
#if PLC_MAX_NUM >= 3
define_plc_isr_name(2)
{
    define_plc_isr_body(2);
}
#endif
#if PLC_MAX_NUM >= 4
define_plc_isr_name(3)
{
    define_plc_isr_body(3);
}
#endif
#if PLC_MAX_NUM >= 5
define_plc_isr_name(4)
{
    define_plc_isr_body(4);
}
#endif
#if PLC_MAX_NUM >= 6
define_plc_isr_name(5)
{
    define_plc_isr_body(5);
}
#endif
#if PLC_MAX_NUM >= 7
define_plc_isr_name(6)
{
    define_plc_isr_body(6);
}
#endif
#if PLC_MAX_NUM >= 8
define_plc_isr_name(7)
{
    define_plc_isr_body(7);
}
#endif
#if PLC_MAX_NUM >= 9
define_plc_isr_name(8)
{
    define_plc_isr_body(8);
}
#endif
#if PLC_MAX_NUM >= 10
define_plc_isr_name(9)
{
    define_plc_isr_body(9);
}
#endif


/*------------------------------------------------------------------------------------
 * 函数:    __init_plc_common()
 *
 * 描述:    初始化PLC模块
**----------------------------------------------------------------------------------*/
void __internal __init __init_plc_common(void)
{
    int i;
    void __internal plc_action_dpc(void *, INT16S);

    #define __set_plc_isr(x)  (__plc_isr[(x)] = __plc_isr_##x)

    for(i=0; i<PLC_MAX_NUM; i++){
        MEMSET((INT08S *)&PLC_POOL[i], 0, sizeof(PLC));
        os_mutex_init(PLC_POOL[i].lock, 0);
        __plc_isr[i] = NULL;
    }
    if(!TimerSet(TimerPLC, 10L, TIMER_TYPE_AUTO, plc_action_dpc, NULL)){
        /*
        * do something here...
        */
    }
    #if PLC_MAX_NUM >= 1
    __set_plc_isr(0);
    #endif
    #if PLC_MAX_NUM >= 2
    __set_plc_isr(1);
    #endif
    #if PLC_MAX_NUM >= 3
    __set_plc_isr(2);
    #endif
    #if PLC_MAX_NUM >= 4
    __set_plc_isr(3);
    #endif
    #if PLC_MAX_NUM >= 5
    __set_plc_isr(4);
    #endif
    #if PLC_MAX_NUM >= 6
    __set_plc_isr(5);
    #endif
    #if PLC_MAX_NUM >= 7
    __set_plc_isr(6);
    #endif
    #if PLC_MAX_NUM >= 8
    __set_plc_isr(7);
    #endif
    #if PLC_MAX_NUM >= 9
    __set_plc_isr(8);
    #endif
    #if PLC_MAX_NUM >= 10
    __set_plc_isr(9);
    #endif
    #undef __set_plc_isr
}

/*------------------------------------------------------------------------------------
 * 函数:    __exit_plc_common()
 *
 * 描述:    PLC模块退出例程
**----------------------------------------------------------------------------------*/
void __internal __exit __exit_plc_common(void)
{
    int i;

    TimerStop(TimerPLC); 

    for(i=0; i<PLC_MAX_NUM; i++){
        if(PLC_POOL[i].flag & PLC_FLAG_OPEN){
            PLC * plc;
            plc = &PLC_POOL[i];
            if(!CloseSerialPort(&plc->rs232)){
                ;
            }
            if(plc->daemon != InvalidHandle && plc->daemon != 0){
                TaskDelete(plc->daemon);
                plc->daemon = InvalidHandle;
            }
            plc->flag &= (~PLC_FLAG_OPEN);

            if(plc->rx_buf){
                mem_free(plc->rx_buf);
                plc->rx_buf = NULL;
            }
            if(plc->tx_buf){
                mem_free(plc->tx_buf);
                plc->tx_buf = NULL;
            }
        }
        MEMSET((INT08S *)&PLC_POOL[i], 0, sizeof(PLC));
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    __plc_get_index()
 *
 * 说明:    计算某个PLC在PLC_POOL数组中的序号
**----------------------------------------------------------------------------------*/
int __internal __plc_get_index(PLC * plc)
{
    return (int)(plc-(PLC *)PLC_POOL);
}

/*------------------------------------------------------------------------------------
 * 函数:    __internal_link_action()
 *
 * 说明:    增加一个PLC_ACTION到PLC(注册)
**----------------------------------------------------------------------------------*/
void __internal __internal_link_action(PLC * plc, PLC_ACTION * action)
{
    PLC_ACTION * t;
    
    FamesAssert(plc);
    FamesAssert(action);

    if(!plc || !action)
        return;

    t = plc->action_list;
    
    while(t){/* whether linked yet? */
        if(t == action){
            return;
        }
        t = t->next;
    }
    
    action->next = plc->action_list;
    plc->action_list = action;

    return;
}

/*------------------------------------------------------------------------------------
 * 函数:    __internal_unlink_action()
 *
 * 说明:    从某个PLC删除一个PLC_ACTION(注销)
**----------------------------------------------------------------------------------*/
void __internal __internal_unlink_action(PLC * plc, PLC_ACTION * action)
{
    PLC_ACTION ** t;

    FamesAssert(plc);
    FamesAssert(action);

    if(!plc || !action)
        return;

    t = &(plc->action_list);
    while((*t)){
        if((*t) == action){
            (*t) = (*t)->next;
            break;
        }
        t = &((*t)->next);
    }

    return;
}

/*------------------------------------------------------------------------------------
 * 函数:    __internal_do_plc_action()
 *
 * 描述:    在某个PLC上执行一次PLC_ACTION
 *
 * 说明:    此函数不可对plc加互斥锁, 因为当前已经是上锁的状态了
**----------------------------------------------------------------------------------*/
void __internal __internal_do_plc_action(PLC * plc, PLC_ACTION * action)
{
    BOOL  retval;

    FamesAssert(plc);
    FamesAssert(action);

    if(!plc || !action)
        return;

    retval = fail;

    switch(plc->type){
        #if  FAMES_PLC_FATEK_EN == 1
        case PLC_TYPE_FATEK:
            retval = plc_control_fatek(plc, action);
            if(action->finish){
                (*action->finish)(action->id, retval);
            }
            action->count++;
            break;
        #endif
        #if  FAMES_PLC_SIEMENS_EN == 1
        case PLC_TYPE_SIEMENS:
            retval = plc_control_siemens(plc, action);
            if(action->finish){
                (*action->finish)(action->id, retval);
            }
            action->count++;
            break;
        #endif
        default:
            break;
    }
}


#if FAMES_DEBUG_PLC == 1
/*------------------------------------------------------------------------------------
 * 函数:    plc_debug_daemon()
 *
 * 描述:    用于调试PLC的后台任务
 *
 * 注意:    其中打开的文件没有关闭
 *          其执行不受PLC互斥锁的限制(否则, 此函数起不到作用)
**----------------------------------------------------------------------------------*/
void __daemon plc_debug_daemon(void * data)
{
    FILE   * err;
    INT08S filename[16];
    INT32U /*rx_pkt_old=-1,*/ tx_pkt_old=0;  
    PLC * plc;
    INT08S  t_str[PLC_BUF_SIZE+8];

    FamesAssert(data);

    if(!data){
        task_return;
    }

    plc = (PLC *)data;

    DispatchLock();
    sprintf(filename, "errplc%d.txt", __plc_get_index(plc));
    err = fopen(filename, "wa");
    DispatchUnlock();
    if(!err){
        DispatchLock();
        fprintf(stderr, "error: %s write error!\n", filename);
        DispatchUnlock();
        ExitApplication();
        return;
    }
    
    for(;;){
        #if 0
        textprintdec16(6, 18, plc->rs232.tx_num, 0, 0);
        textprintdec16(28, 18, plc->rs232.rx_num, 0, 0);
        textprintdec16(12, 18, plc->rs232.tx_buf_len, 0, 0);
        textprintdec16(18, 18, plc->rs232.tx_packet, 0, 0);
        textprinthex32(1, 20, (INT32S)&(plc->rs232), 0);
        #endif
        if(!(plc->flag & PLC_FLAG_OPEN)){
            fprintf(err,"PLC not opened\n");
            TaskSleep(1000L);
            continue;
        }
        if(plc->rs232.status==COM_STATUS_OPEN){
            if(plc->rs232.tx_num && tx_pkt_old!=plc->rs232.tx_packet){
                tx_pkt_old=plc->rs232.tx_packet;
                DispatchLock();
                MEMCPY(t_str, (INT08S *)plc->rs232.tx_buf, PLC_BUF_SIZE);
                fprintf(err,"TX: E=%-4d, SN=%-4ld, %02d,%s\n", FAMES_ERROR_CODE, plc->rs232.tx_packet,plc->rs232.tx_buf_len, t_str);
                DispatchUnlock();
            }
            if(plc->rs232.rx_num){
                DispatchLock();
                MEMCPY(t_str, (INT08S *)plc->rs232.rx_buf, PLC_BUF_SIZE);                
                fprintf(err,"RX: E=%-4d, SN=%-4ld, %02d,%s\n", FAMES_ERROR_CODE, plc->rs232.rx_packet,plc->rs232.rx_num, t_str);
                DispatchUnlock();
            } else {
                DispatchLock();
                fprintf(err,"RX: E=%-4d, SN=%-4ld, RxBuf is NULL\n", FAMES_ERROR_CODE, plc->rs232.rx_packet);
                DispatchUnlock();
            }
        }
        TaskSleep(5L); /* 先睡会儿, 等会再检测它 */
    }
}
#endif /* FAMES_DEBUG_PLC == 1 */

#endif /* FAMES_PLC_SVC_EN == 1 */

/*=====================================================================================
 * 
 * 本文件结束: plc.c
 * 
**===================================================================================*/

