/*************************************************************************************
 * 文件: net.h
 *
 * 描述: 网络模块(以太网)
 *
 * 作者: Jun
 *
 * 说明: 网络模块的初始化及退出, 网络数据包的接收与处理
 *       网络协议栈的处理
*************************************************************************************/
#ifndef FAMES_NET_H
#define FAMES_NET_H

/*------------------------------------------------------------------------------------
 *      网络服务编译选项
**----------------------------------------------------------------------------------*/
#define FAMES_NETWORK_EN   1  /* 网络服务支持开关, 1=支持, 0=不支持 */


/*------------------------------------------------------------------------------------
 *      函数声明
**----------------------------------------------------------------------------------*/
#if  FAMES_NETWORK_EN == 1
void apical __init InitNetService(void);           /* Initialize network service */
BOOL apical OpenNetwork(void);                     /* Open network service       */
BOOL apical StopNetwork(void);                     /* Stop network service       */
BOOL apical RestartNetwork(void);                  /* Restart network service    */
#else  /* FAMES_NETWORK_EN==1 */
#define InitNetService()
#define OpenNetwork()
#define StopNetwork();
#define RestartNetwork();
#endif /* FAMES_NETWORK_EN==1 */

#endif /* #ifndef FAMES_NET_H */
/*====================================================================================
 * 
 * 本文件结束: net.h
 * 
**==================================================================================*/

