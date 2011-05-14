/*************************************************************************************
 * �ļ�: nic.h
 *
 * ˵��: ��������
 *
 * ����: Jun
*************************************************************************************/
#ifndef FAMES_NIC_H
#define FAMES_NIC_H

/*------------------------------------------------------------------------------------
 *         ����״̬�ṹ
**----------------------------------------------------------------------------------*/
typedef struct _NIC_STATUS_BLOCK {
    INT16S status;
    INT32S rx_packet;
    INT32S tx_packet;
}NIC_STATUS;

/*------------------------------------------------------------------------------------
 *         ������Ϣ��
 *
 * ÿһ����������������ʵ�ֵļ�������
**----------------------------------------------------------------------------------*/
typedef struct _NIC_DRIVER_BLOCK {
    BOOL  (*init)(void);                          /* ��ʼ��                         */
    BOOL  (*open)(void);                          /* ��                           */
    BOOL  (*stop)(void);                          /* ֹͣ                           */
    BOOL  (*xmit)(void * srcbuf, INT16S  buflen); /* ����                           */
    BOOL  (*poll)(void * dstbuf, INT16S *buflen); /* ����                           */
    BOOL  (*control)(INT16S cmd, void * data);    /* ����                           */
    BOOL  (*get_status)(void * status);           /* ��ȡ״̬                       */
    BOOL  (*islink)(void);                        /* �����Ƿ�������                 */
    void  (*watchdog)(void *, INT16S);            /* �ǻ״̬��ʱ����             */
    NIC_STATUS status;                            /* ����״̬��ͳ����Ϣ             */
    INT16U  VendorID;                             /* Vendor ID                      */
    INT16U  DeviceID;                             /* Device ID                      */
    INT16S  BusNo;                                /* Bus No.                        */
    INT16S  DeviceFuncNo;                         /* Device/Func No.                */
    STRING  NicName;                              /* ��������                       */
}NIC_DRIVER_BLOCK;

#define NIC_CTRL_CMD_NONE      0
#define NIC_CTRL_CMD_RESET     1
#define NIC_CTRL_CMD_OPEN      2
#define NIC_CTRL_CMD_STOP      3
#define NIC_CTRL_CMD_SPEED     11
#define NIC_CTRL_CMD_DUPLEX    12
#define NIC_CTRL_CMD_FLOWCTRL  13

/*------------------------------------------------------------------------------------
 * ������������ֲ�ͬ������ͷ�ļ�
**----------------------------------------------------------------------------------*/
#include <drv\rtl8139.h>        /* RTL-8139 Fast NIC - Jun */
#include <drv\pktdrv.h>

/*------------------------------------------------------------------------------------
 * ϵͳ�е������б�: ��ɨ��ʱ, ���ڴ洢ϵͳ�е�����λ����Ϣ
**----------------------------------------------------------------------------------*/
typedef struct _NIC_IN_SYSTEM {
    INT16U     VendorID;
    INT16U     DeviceID;
    INT16U     SubVendorID;
    INT16U     SubDeviceID;
    INT16S     BusNo;
    INT16S     DeviceFuncNo;
    BOOLEAN    Supported;
}NIC_IN_SYSTEM;

#define  NIC_MAX_NR  5                      /* ������5������                      */


/*------------------------------------------------------------------------------------
 *           ������Ϣ��
**----------------------------------------------------------------------------------*/
typedef struct _NIC_INFO_BLOCK {
    INT16U             VendorID;            /* ����ID                               */
    INT16U             DeviceID;            /* �豸ID                               */
    STRING             NicName;             /* ��������                             */
    NIC_DRIVER_BLOCK * NicDriver;           /* ������Ϣ��                           */
}NIC_INFO_BLOCK;

/*------------------------------------------------------------------------------------
 * ��ϵͳ֧�ֵ������б�
 * 
 * ��Ҫ�������������, �밴�������ʽ
**----------------------------------------------------------------------------------*/
#ifdef FAMES_NIC_C
/* Native Ethernet Interface Drivers */
NIC_INFO_BLOCK   NICS[] = {
    /* RealTek RTL-8139 Family Fast Ethernet driver, Jun made this */
    {0x10ec, 0x8139, "RTL8139C",          &RTL8139_DRV},
    {0x10ec, 0x8138, "RTL8138",           &RTL8139_DRV},
    {0x1186, 0x1300, "DFE530TX+ Ver-E1",  &RTL8139_DRV},
    {0x0, 0x0, NULL, NULL}     /* The End */
};
#if NIC_PKTDRV_EN==1
NIC_INFO_BLOCK   PKTDRV_NICS = {0, 0, "PKTDRV",  &PKTDRV_DRV};
#endif
#if 0
NIC_INFO_BLOCK   NDIS2_NICS  = {0, 0, "NDIS2",   &NDIS2_DRV};
#endif
NIC_INFO_BLOCK * CurrentNIC=NULL;
#else
extern NIC_INFO_BLOCK   NICS[];             /* ��֧�ֵ������б�                     */
extern NIC_INFO_BLOCK * CurrentNIC;         /* ��ǰʹ�õ�����                       */
#endif

/*------------------------------------------------------------------------------------
 * ��������
**----------------------------------------------------------------------------------*/
#define  NIC_DRIVER_TYPE_NONE    0          
#define  NIC_DRIVER_TYPE_NATIVE  1          /* ������������: ����֧��        */
#define  NIC_DRIVER_TYPE_PKTDRV  2          /* ������������: Packet-Driver   */
#define  NIC_DRIVER_TYPE_NDIS    3          /* ������������: NDIS            */
#define  NIC_DRIVER_TYPE_OTHERS  4

#ifdef FAMES_NIC_C
NIC_IN_SYSTEM nic_in_system[NIC_MAX_NR];    /* ɨ��ʱ, �ҵ�����������               */
INT16S        nr_of_nic=0;                  /* ϵͳ�е���������                     */
INT16S        nic_driver_type=NIC_DRIVER_TYPE_NONE; /* ������������                 */
#else
extern NIC_IN_SYSTEM nic_in_system[];
extern INT16S        nr_of_nic;
extern INT16S        nic_driver_type;
#endif

/*------------------------------------------------------------------------------------
 * ��������
**----------------------------------------------------------------------------------*/
BOOL scan_all_nic(void);
BOOL setup_working_nic_native(void);
#if NIC_PKTDRV_EN==1
BOOL setup_working_nic_pktdrv(void);
#endif
BOOL setup_working_nic_ndis(void);
BOOL setup_working_nic(void);
BOOL nic_init(void); 
BOOL nic_open(void);
BOOL nic_stop(void);
BOOL nic_xmit(void * srcbuf, INT16S buflen);
BOOL nic_poll(void * dstbuf, INT16S * buflen);
BOOL nic_control(INT16S cmd, void * data);
BOOL nic_get_status(void * status);
BOOL nic_islink(void);
void nic_watchdog(void * data, INT16S times);      

#endif                                      /* #ifndef FAMES_NIC_H                  */

/*====================================================================================
 * 
 * ���ļ�����: nic.h
 * 
**==================================================================================*/

