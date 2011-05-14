/**************************************************************************************
 * �ļ�:    plc.c
 *
 * ˵��:    PLCͨ������
 *
 * ����:    Jun
 *
 * ʱ��:    2010-9-2
**************************************************************************************/
#define  FAMES_PLC_C
#include <includes.h>

#if FAMES_PLC_SVC_EN == 1

/*------------------------------------------------------------------------------------
 * 
 *       �ڲ�����
 *
**----------------------------------------------------------------------------------*/
void __internal __internal_link_action(PLC * plc, PLC_ACTION * action);
void __internal __internal_unlink_action(PLC * plc, PLC_ACTION * action);
void __internal __internal_do_plc_action(PLC * plc, PLC_ACTION * action);
int  __internal __plc_get_index(PLC * plc);


/*------------------------------------------------------------------------------------
 * 
 *       PLC�ķ������ͷŵ�...
 *
**----------------------------------------------------------------------------------*/
#define  PLC_MAX_NUM   8             /* ϵͳ֧�ֵ�PLC������ */

static   PLC PLC_POOL[PLC_MAX_NUM];  /* PLC�����             */

__isr__ (far * __plc_isr[PLC_MAX_NUM])(void);

/*------------------------------------------------------------------------------------
 * ����:    plc_alloc()
 *
 * ����:    ����һ��PLC���ƿ�
 *
 * ����:    �ɹ�ʱ����PLC���ƿ�ָ��, ʧ��ʱ����NULL
 *
 * ˵��:    ���صĿ��ƿ��Ѿ����˳�ʼ������
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
 * ����:    plc_free()
 *
 * ����:    �ͷ�һ��PLC���ƿ�
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
 * ����:    open_plc()
 *
 * ˵��:    ��һ��PLC
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
 * ����:    shut_plc()
 *
 * ˵��:    �ر�ĳ��PLC
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
 * ����:    do_plc_action()
 *
 * ˵��:    ִ��PLC����
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
 * ����:    plc_set_param()
 *
 * ˵��:    ����PLC����
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
 * ����:    plc_set_action()
 *
 * ˵��:    ����PLC����(ACTION)
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
 * ����:    plc_action_daemon()
 *
 * ˵��:    PLC����λ��ͨѶ������(��ʵ, ��Ҫ���ڶ�PLC)
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
            while(action->timer > 0 || action->disabled){ /* ����һ������ִ�е�ACTION */
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
 * ����:    plc_action_dpc()
 *
 * ˵��:    PLCͨѶ֮DPC����, ����ˢ��PLC-ITEM�ĳ�ʱֵ(���ȼ�)
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
 *      PLC�����жϷ������Ķ���
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
    return fail; /*lint !e527; ��ʵִ�в�����һ��*/
}

/* 
 * PLC����ISR�Ķ����
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
 * ����:    __init_plc_common()
 *
 * ����:    ��ʼ��PLCģ��
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
 * ����:    __exit_plc_common()
 *
 * ����:    PLCģ���˳�����
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
 * ����:    __plc_get_index()
 *
 * ˵��:    ����ĳ��PLC��PLC_POOL�����е����
**----------------------------------------------------------------------------------*/
int __internal __plc_get_index(PLC * plc)
{
    return (int)(plc-(PLC *)PLC_POOL);
}

/*------------------------------------------------------------------------------------
 * ����:    __internal_link_action()
 *
 * ˵��:    ����һ��PLC_ACTION��PLC(ע��)
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
 * ����:    __internal_unlink_action()
 *
 * ˵��:    ��ĳ��PLCɾ��һ��PLC_ACTION(ע��)
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
 * ����:    __internal_do_plc_action()
 *
 * ����:    ��ĳ��PLC��ִ��һ��PLC_ACTION
 *
 * ˵��:    �˺������ɶ�plc�ӻ�����, ��Ϊ��ǰ�Ѿ���������״̬��
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
 * ����:    plc_debug_daemon()
 *
 * ����:    ���ڵ���PLC�ĺ�̨����
 *
 * ע��:    ���д򿪵��ļ�û�йر�
 *          ��ִ�в���PLC������������(����, �˺����𲻵�����)
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
        TaskSleep(5L); /* ��˯���, �Ȼ��ټ���� */
    }
}
#endif /* FAMES_DEBUG_PLC == 1 */

#endif /* FAMES_PLC_SVC_EN == 1 */

/*=====================================================================================
 * 
 * ���ļ�����: plc.c
 * 
**===================================================================================*/

