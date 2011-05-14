/*************************************************************************************
 * �ļ�: rtl8139.h
 *
 * ����: RTL8139��������
 *
 * ����: Jun
 *
 * ˵��: ���ļ��ο���Linux�е�����, �����еĲ�����ֱ�ӿ�������, ������Ӧ���ֵİ�Ȩ
 *       Ҫ��ԭ��������!
*************************************************************************************/
#ifndef FAMES_NIC_RTL8139_H
#define FAMES_NIC_RTL8139_H

/*------------------------------------------------------------------------------------
 *        ��������
**----------------------------------------------------------------------------------*/
#define MAX_ETH_FRAME_SIZE    1536

#define RX_BUF_LEN    (8192)             /* Size of ring buffer for rtl8139 rx */
#define TX_BUF_LEN    MAX_ETH_FRAME_SIZE
#define NUM_TX_DESC    4                 /* Number of Tx descriptor registers. */

#define Rtl8139_ChipResetTimeOut 1000L  /* Rtl8139оƬ���趨ʱ��, 1��         */


/*------------------------------------------------------------------------------------
 *        ��������
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
 *        ������Ϣ��(RTL8139un)
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
 * ���ļ�����: rtl8139.h
 * 
**==================================================================================*/

