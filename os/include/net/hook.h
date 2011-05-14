/*************************************************************************************
 * �ļ�: net\hook.h
 *
 * ˵��: �������ݰ�����
 *
 * ����: Jun
 *
 * ����: 
*************************************************************************************/
#ifndef FAMES_NET_HOOK_H
#define FAMES_NET_HOOK_H

/*------------------------------------------------------------------------------------
 *      ����������ѡ��
**----------------------------------------------------------------------------------*/
#define FAMES_NETHOOK_EN   1  /* ���繳��֧�ֿ���, 1=֧��, 0=��֧�� */

/*------------------------------------------------------------------------------------
 *      ���繳����ض���
**----------------------------------------------------------------------------------*/
#define NETWORK_HOOK_RECV  1
#define NETWORK_HOOK_XMIT  2
#define NETWORK_HOOK_OPEN  3
#define NETWORK_HOOK_STOP  4

#define NETHOOK_NUM        8
typedef void (*NETHOOK)(void * data, INT16U flag);


/*------------------------------------------------------------------------------------
 *      ��������
**----------------------------------------------------------------------------------*/
#if  FAMES_NETWORK_EN == 1 && FAMES_NETHOOK_EN == 1
void apical InitNetworkHook(void);                 /* Initialize network hook    */
BOOL apical RegisterNetworkHook(NETHOOK nhook);    /* Register network hook      */
BOOL apical DeregisterNetworkHook(NETHOOK nhook);  /* Deregister network hook    */
BOOL apical ExecuteNetworkHooks(void * data, INT16U flag); 
                                                   /* ִ��network hooks          */
#else
#define InitNetworkHook()
#define RegisterNetworkHook(nethook)
#define DeregisterNetworkHook(nethook)
#define ExecuteNetworkHooks(data, flag)
#endif /* FAMES_NETWORK_EN==1 && FAMES_NETHOOK_EN==1 */

#endif /* #ifndef FAMES_NET_HOOK_H                   */
/*====================================================================================
 * 
 * ���ļ�����: net\hook.h
 * 
**==================================================================================*/

