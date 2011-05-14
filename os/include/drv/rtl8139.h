/*************************************************************************************
 * 文件: rtl8139.h
 *
 * 描述: RTL8139网卡驱动
 *
 * 作者: Jun
 *
 * 说明: 此文件参考了Linux中的驱动, 甚至有的部分是直接拷过来的, 所以相应部分的版权
 *       要归原作者所有!
*************************************************************************************/
#ifndef FAMES_NIC_RTL8139_H
#define FAMES_NIC_RTL8139_H

/*------------------------------------------------------------------------------------
 *        常量定义
**----------------------------------------------------------------------------------*/
#define MAX_ETH_FRAME_SIZE    1536

#define RX_BUF_LEN    (8192)             /* Size of ring buffer for rtl8139 rx */
#define TX_BUF_LEN    MAX_ETH_FRAME_SIZE
#define NUM_TX_DESC    4                 /* Number of Tx descriptor registers. */

#define Rtl8139_ChipResetTimeOut 1000L  /* Rtl8139芯片重设定时器, 1秒         */


/*------------------------------------------------------------------------------------
 *        函数声明
**----------------------------------------------------------------------------------*/
BOOL init_rtl8139(void);
BOOL open_rtl8139(void);
BOOL stop_rtl8139(void);
BOOL xmit_rtl8139(void * srcbuf, INT16S buflen);
BOOL poll_rtl8139(void * dstbuf, INT16S * buflen);
BOOL ctrl_rtl8139(INT16S cmd, void * data);
BOOL get_status_rtl8139(void * status);
BOOL islink_rtl8139(void);
void watchdog_rtl8139(void * data, INT16S nr);


/*------------------------------------------------------------------------------------
 *        驱动信息块(RTL8139un)
**----------------------------------------------------------------------------------*/
#ifdef FAMES_NIC_RTL8139_C
NIC_DRIVER_BLOCK RTL8139_DRV=
       {            
          init_rtl8139,
          open_rtl8139,
          stop_rtl8139,
          xmit_rtl8139,
          poll_rtl8139,
          ctrl_rtl8139,
          get_status_rtl8139,
          islink_rtl8139,
          watchdog_rtl8139,
       };
#else
extern NIC_DRIVER_BLOCK RTL8139_DRV;
#endif



#endif                                      /* #ifndef FAMES_NIC_RTL8139_H         */

/*====================================================================================
 * 
 * 本文件结束: rtl8139.h
 * 
**==================================================================================*/

