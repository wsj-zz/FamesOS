/*************************************************************************************
 * �ļ�: net\hook.c
 *
 * ˵��: �������ݰ�����
 *
 * ����: Jun
 *
 * ����: 
*************************************************************************************/
#define  FAMES_NET_HOOK_C
#include <includes.h>

#if  FAMES_NETWORK_EN == 1 && FAMES_NETHOOK_EN == 1
/*----------------------------------------------------------------------------------------------
 *   NetHook���ݽṹ����������:
**--------------------------------------------------------------------------------------------*/
struct net_hook_s {
    NETHOOK    func;
    struct net_hook_s *next;
};

static struct net_hook_s  net_hook_buf[NETHOOK_NUM];

static struct net_hook_s *net_hook_free = NULL; /* �����б� */
static struct net_hook_s *net_hook_list = NULL; /* ע���б� */

/*------------------------------------------------------------------------------------
 * ����:    InitNetworkHook()
 *
 * ����:    ��ʼ���������ݰ�����(�ػ����ݰ�)
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
 * ����:    RegisterNetworkHook()
 *
 * ����:    ע��һ�����ݰ�����
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
 * ����:    DeregisterNetworkHook()
 *
 * ����:    ע��һ�����ݰ�����
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
 * ����:    ExecuteNetworkHooks()
 *
 * ����:    ִ�����ݰ���������
 *
 * ˵��:    ��Ϊֻ������������������ʱ, �������̲�Ӧ��ִ��, ����,
 *          Ӧ�ó���Ӧ��ֱ�ӵ��ô˺���
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
 * ���ļ�����: net\hook.c
 * 
**==================================================================================*/


