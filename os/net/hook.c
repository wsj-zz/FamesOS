/*************************************************************************************
 * 文件: net\hook.c
 *
 * 说明: 网络数据包钩子
 *
 * 作者: Jun
 *
 * 描述: 
*************************************************************************************/
#define  FAMES_NET_HOOK_C
#include <includes.h>

#if  FAMES_NETWORK_EN == 1 && FAMES_NETHOOK_EN == 1
/*----------------------------------------------------------------------------------------------
 *   NetHook数据结构及变量定义:
**--------------------------------------------------------------------------------------------*/
struct net_hook_s {
    NETHOOK    func;
    struct net_hook_s *next;
};

static struct net_hook_s  net_hook_buf[NETHOOK_NUM];

static struct net_hook_s *net_hook_free = NULL; /* 空闲列表 */
static struct net_hook_s *net_hook_list = NULL; /* 注册列表 */

/*------------------------------------------------------------------------------------
 * 函数:    InitNetworkHook()
 *
 * 描述:    初始化网络数据包钩子(截获数据包)
**----------------------------------------------------------------------------------*/
void apical InitNetworkHook(void)
{
    INT16S i;

    net_hook_free = NULL;
    net_hook_list = NULL;
    for(i=0; i<NETHOOK_NUM; i++){
        net_hook_buf[i].func =  NULL;
        net_hook_buf[i].next =  net_hook_free;
        net_hook_free        = &net_hook_buf[i];
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    RegisterNetworkHook()
 *
 * 描述:    注册一个数据包钩子
**----------------------------------------------------------------------------------*/
BOOL apical RegisterNetworkHook(NETHOOK nethook)
{
    BOOL   retval;
    struct net_hook_s *nh;
    
    FamesAssert(nethook);

    if(!nethook){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(net_hook_free){
        nh = net_hook_free;       /* allocate a free one   */
        net_hook_free = nh->next;

        nh->next = net_hook_list; /* link to net_hook_list */
        nh->func = nethook;
        net_hook_list = nh;      

        retval = ok;
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    DeregisterNetworkHook()
 *
 * 描述:    注销一个数据包钩子
**----------------------------------------------------------------------------------*/
BOOL apical DeregisterNetworkHook(NETHOOK nethook)
{
    BOOL   retval;
    struct net_hook_s **nh, *t;
    
    FamesAssert(nethook);

    if(!nethook){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(net_hook_list){
        for(nh = &net_hook_list; *nh;){
            if((*nh)->func == nethook){
                t = (*nh);
               (*nh) = (*nh)->next;     /* disconnect */
                t->next = net_hook_free; /* link to free list */
                t->func = NULL;
                net_hook_free = t;
                retval = ok;
                break;
            }
            nh = &(*nh)->next;
        }
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    ExecuteNetworkHooks()
 *
 * 描述:    执行数据包钩子例程
 *
 * 说明:    因为只有在网卡有数据流过时, 钩子例程才应该执行, 所以,
 *          应用程序不应该直接调用此函数
**----------------------------------------------------------------------------------*/
BOOL apical ExecuteNetworkHooks(void * data, INT16U flag)
{
    struct net_hook_s *nh;

    lock_kernel();
    nh = net_hook_list;
    while(nh){
        if(nh->func){
            (*nh->func)(data, flag);
        }
        nh = nh->next;
    }
    unlock_kernel();

    return ok;
}

#endif /* FAMES_NETWORK_EN==1 && FAMES_NETHOOK_EN==1 */

/*====================================================================================
 * 
 * 本文件结束: net\hook.c
 * 
**==================================================================================*/


