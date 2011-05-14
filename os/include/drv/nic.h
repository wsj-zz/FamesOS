/*************************************************************************************
 * 文件: nic.h
 *
 * 说明: 网卡驱动
 *
 * 作者: Jun
*************************************************************************************/
#ifndef FAMES_NIC_H
#define FAMES_NIC_H

/*------------------------------------------------------------------------------------
 *         网卡状态结构
**----------------------------------------------------------------------------------*/
typedef struct _NIC_STATUS_BLOCK {
    INT16S status;
    INT32S rx_packet;
    INT32S tx_packet;
}NIC_STATUS;

/*------------------------------------------------------------------------------------
 *         驱动信息块
 *
 * 每一个网卡驱动都必须实现的几个函数
**----------------------------------------------------------------------------------*/
typedef struct _NIC_DRIVER_BLOCK {
    BOOL  (*init)(void);                          /* 初始化                         */
    BOOL  (*open)(void);                          /* 打开                           */
    BOOL  (*stop)(void);                          /* 停止                           */
    BOOL  (*xmit)(void * srcbuf, INT16S  buflen); /* 发送                           */
    BOOL  (*poll)(void * dstbuf, INT16S *buflen); /* 接收                           */
    BOOL  (*control)(INT16S cmd, void * data);    /* 控制                           */
    BOOL  (*get_status)(void * status);           /* 读取状态                       */
    BOOL  (*islink)(void);                        /* 网线是否已连接                 */
    void  (*watchdog)(void *, INT16S);            /* 非活动状态超时处理             */
    NIC_STATUS status;                            /* 网卡状态及统计信息             */
    INT16U  VendorID;                             /* Vendor ID                      */
    INT16U  DeviceID;                             /* Device ID                      */
    INT16S  BusNo;                                /* Bus No.                        */
    INT16S  DeviceFuncNo;                         /* Device/Func No.                */
    STRING  NicName;                              /* 网卡名称                       */
}NIC_DRIVER_BLOCK;

#define NIC_CTRL_CMD_NONE      0
#define NIC_CTRL_CMD_RESET     1
#define NIC_CTRL_CMD_OPEN      2
#define NIC_CTRL_CMD_STOP      3
#define NIC_CTRL_CMD_SPEED     11
#define NIC_CTRL_CMD_DUPLEX    12
#define NIC_CTRL_CMD_FLOWCTRL  13

/*------------------------------------------------------------------------------------
 * 在这里包含各种不同网卡的头文件
**----------------------------------------------------------------------------------*/
#include <drv\rtl8139.h>        /* RTL-8139 Fast NIC - Jun */
#include <drv\pktdrv.h>

/*------------------------------------------------------------------------------------
 * 系统中的网卡列表: 在扫描时, 用于存储系统中的网卡位置信息
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

#define  NIC_MAX_NR  5                      /* 最多查找5个网卡                      */


/*------------------------------------------------------------------------------------
 *           网卡信息块
**----------------------------------------------------------------------------------*/
typedef struct _NIC_INFO_BLOCK {
    INT16U             VendorID;            /* 厂商ID                               */
    INT16U             DeviceID;            /* 设备ID                               */
    STRING             NicName;             /* 网卡名称                             */
    NIC_DRIVER_BLOCK * NicDriver;           /* 驱动信息块                           */
}NIC_INFO_BLOCK;

/*------------------------------------------------------------------------------------
 * 本系统支持的网卡列表
 * 
 * 如要添加新网卡驱动, 请按照下面格式
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
extern NIC_INFO_BLOCK   NICS[];             /* 可支持的网卡列表                     */
extern NIC_INFO_BLOCK * CurrentNIC;         /* 当前使用的网卡                       */
#endif

/*------------------------------------------------------------------------------------
 * 变量定义
**----------------------------------------------------------------------------------*/
#define  NIC_DRIVER_TYPE_NONE    0          
#define  NIC_DRIVER_TYPE_NATIVE  1          /* 网卡驱动类型: 本地支持        */
#define  NIC_DRIVER_TYPE_PKTDRV  2          /* 网卡驱动类型: Packet-Driver   */
#define  NIC_DRIVER_TYPE_NDIS    3          /* 网卡驱动类型: NDIS            */
#define  NIC_DRIVER_TYPE_OTHERS  4

#ifdef FAMES_NIC_C
NIC_IN_SYSTEM nic_in_system[NIC_MAX_NR];    /* 扫描时, 找到的所有网卡               */
INT16S        nr_of_nic=0;                  /* 系统中的网卡个数                     */
INT16S        nic_driver_type=NIC_DRIVER_TYPE_NONE; /* 网卡驱动类型                 */
#else
extern NIC_IN_SYSTEM nic_in_system[];
extern INT16S        nr_of_nic;
extern INT16S        nic_driver_type;
#endif

/*------------------------------------------------------------------------------------
 * 函数声明
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
 * 本文件结束: nic.h
 * 
**==================================================================================*/

