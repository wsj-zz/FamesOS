/*************************************************************************************
 * 文件: net\hook.h
 *
 * 说明: 网络数据包钩子
 *
 * 作者: Jun
 *
 * 描述: 
*************************************************************************************/
#ifndef FAMES_NET_HOOK_H
#define FAMES_NET_HOOK_H

/*------------------------------------------------------------------------------------
 *      网络服务编译选项
**----------------------------------------------------------------------------------*/
#define FAMES_NETHOOK_EN   1  /* 网络钩子支持开关, 1=支持, 0=不支持 */

/*------------------------------------------------------------------------------------
 *      网络钩子相关定义
**----------------------------------------------------------------------------------*/
#define NETWORK_HOOK_RECV  1
#define NETWORK_HOOK_XMIT  2
#define NETWORK_HOOK_OPEN  3
#define NETWORK_HOOK_STOP  4

#define NETHOOK_NUM        8
typedef void (*NETHOOK)(void * data, INT16U flag);


/*------------------------------------------------------------------------------------
 *      函数声明
**----------------------------------------------------------------------------------*/
#if  FAMES_NETWORK_EN == 1 && FAMES_NETHOOK_EN == 1
void apical InitNetworkHook(void);                 /* Initialize network hook    */
BOOL apical RegisterNetworkHook(NETHOOK nhook);    /* Register network hook      */
BOOL apical DeregisterNetworkHook(NETHOOK nhook);  /* Deregister network hook    */
BOOL apical ExecuteNetworkHooks(void * data, INT16U flag); 
                                                   /* 执行network hooks          */
#else
#define InitNetworkHook()
#define RegisterNetworkHook(nethook)
#define DeregisterNetworkHook(nethook)
#define ExecuteNetworkHooks(data, flag)
#endif /* FAMES_NETWORK_EN==1 && FAMES_NETHOOK_EN==1 */

#endif /* #ifndef FAMES_NET_HOOK_H                   */
/*====================================================================================
 * 
 * 本文件结束: net\hook.h
 * 
**==================================================================================*/

