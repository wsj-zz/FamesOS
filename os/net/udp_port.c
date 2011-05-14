/***********************************************************************************************
 * 文件: udp_port.c
 *
 * 说明: UDP端口管理
 *
 * 作者: Jun
***********************************************************************************************/
#define  FAMES_UDP_PORT_C
#include <includes.h>

/*----------------------------------------------------------------------------------------------
 *
 *      UDP端口管理结构
 *
**--------------------------------------------------------------------------------------------*/
struct udp_port_struct {
    INT16U    port;
    udp_dispatcher_func_type dispatcher;
    struct udp_port_struct * next;
};

typedef struct udp_port_struct udp_port_type;

#define  UDP_PORT_OPEN_MAX_NR  32 /* 可打开的UDP端口数 */

static udp_port_type   __udp_port_buf[UDP_PORT_OPEN_MAX_NR];

static udp_port_type * __udp_port_list = NULL;
static udp_port_type * __udp_port_free = NULL;

/*----------------------------------------------------------------------------------------------
 * 函数:    udp_port_initialize()
 *
 * 描述:    UDP端口管理之初始化
**--------------------------------------------------------------------------------------------*/
void apical __init udp_port_initialize(void)
{
    int i;

    CALLED_ONLY_ONCE();
    
    __udp_port_list = NULL;
    __udp_port_free = NULL;

    for(i=0; i<UDP_PORT_OPEN_MAX_NR; i++){
        __udp_port_buf[i].port = 0;
        __udp_port_buf[i].dispatcher = NULL;
        __udp_port_buf[i].next = __udp_port_free;
        __udp_port_free = &__udp_port_buf[i];
    }

    return;
}

void apical __exit udp_port_onexit(void)
{
    ;/* 目前不需要, 留待将来再看 */
}

/*----------------------------------------------------------------------------------------------
 * 函数:    open_udp_port()
 *
 * 描述:    打开一个UDP端口
 *
 * 参数:    port       要监听的目标端口
 *          dispatcher 目标端口数据包的处理函数
 *
 * 返回:    ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical open_udp_port(INT16U port, udp_dispatcher_func_type dispatcher)
{
    udp_port_type *t;
    BOOL retval;

    FamesAssert(port > 0);
    FamesAssert(dispatcher);

    if(port==0 || !dispatcher)
        return 0;

    retval = fail;

    lock_kernel();
    
    if(!__udp_port_free)
        goto out;

    t = __udp_port_list;
    while(t){ 
        if(t->port == port)
            goto out; /* get out if the port already opened */
        t = t->next;
    }
    
    t = __udp_port_free;             /* allocate */
    __udp_port_free = t->next;

    t->port       = port;
    t->dispatcher = dispatcher;

    t->next       = __udp_port_list; /* link */
    __udp_port_list = t;

    retval = ok;
    
out:
    unlock_kernel();

    return retval;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    close_udp_port()
 *
 * 描述:    关闭一个UDP端口
 *
 * 参数:    port 要关闭的目标端口
 *
 * 返回:    ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical close_udp_port(INT16U port)
{
    udp_port_type **list, *t;
    BOOL retval;

    FamesAssert(port > 0);
    
    if(0 == port)
        return fail;

    lock_kernel();

    retval = fail;
    
    list = &__udp_port_list;
    while(*list){
        if((*list)->port == port){/* found */
            t = *list;       /* save to t */
            *list = t->next;  /* unlink */
            t->port       = 0; /* reset */
            t->dispatcher = NULL;
            t->next = __udp_port_free; /* link to free */
            __udp_port_free = t;
            retval = ok;
            break;
        }
        list = &(*list)->next;
    }

    unlock_kernel();

    return retval;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    search_udp_port()
 *
 * 描述:    传递数据包到某个UDP端口
 *
 * 参数:    port    要传递到的目标端口
 *          ip_pkt  要传递的IP数据包
 *          pkt_len 数据包长度
 *
 * 返回:    ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical __sysonly search_udp_port(INT16U port, INT08S *ip_pkt, INT16S pkt_len)
{
    udp_dispatcher_func_type func;
    udp_port_type * t;
    BOOL  retval;

    FamesAssert(port > 0);
    FamesAssert(ip_pkt);
    FamesAssert(pkt_len > 0);

    if(0==port || !ip_pkt || pkt_len<=0)
        return 0;

    retval = fail;
    func   = NULL;
    
    lock_kernel();
    t = __udp_port_list; 
    while(t){
        if(t->port == port){/* found */
            func = t->dispatcher;
            break;
        }
        t = t->next;
    }
    unlock_kernel();

    if(func){
        retval = ok;
        (*func)(ip_pkt, pkt_len);
    }
    
    return retval;
}

/*==============================================================================================
 * 
 * 本文件结束: udp_port.c
 * 
**============================================================================================*/


