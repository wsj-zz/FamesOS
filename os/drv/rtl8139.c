/*************************************************************************************
 * 文件: rtl8139.c
 *
 * 描述: RTL8139网卡驱动
 *
 * 作者: Jun
 *
 * 说明: 此文件参考了Linux中的驱动, 甚至有的部分是直接拷过来, 所以相应部分的版权
 *       要归原作者所有!
*************************************************************************************/
#define FAMES_NIC_RTL8139_C
#include "includes.h"

/* Symbolic offsets to registers. */
enum RTL8139_registers {
    MAC0 = 0,        /* Ethernet hardware address. */
    MAR0 = 8,        /* Multicast filter. */
    TxStatus0 = 0x10,    /* Transmit status (Four 32bit registers). */
    TxAddr0 = 0x20,        /* Tx descriptor 0 (also four 32bit). */
    TxAddr1 = 0x24,        /* Tx descriptor 1 (also four 32bit). */
    TxAddr2 = 0x28,        /* Tx descriptor 2 (also four 32bit). */
    TxAddr3 = 0x2C,        /* Tx descriptor 3 (also four 32bit). */
    RxBuf = 0x30,
    ChipCmd = 0x37,
    RxBufPtr = 0x38,
    RxBufAddr = 0x3A,
    IntrMask = 0x3C,
    IntrStatus = 0x3E,
    TxConfig = 0x40,
    RxConfig = 0x44,
    Timer = 0x48,        /* A general-purpose counter. */
    RxMissed = 0x4C,    /* 24 bits valid, write clears. */
    Cfg9346 = 0x50,
    Config0 = 0x51,
    Config1 = 0x52,
    FlashReg = 0x54,
    MediaStatus = 0x58,
    Config3 = 0x59,
    Config4 = 0x5A,        /* absent on RTL-8139A */
    HltClk = 0x5B,
    MultiIntr = 0x5C,
    TxSummary = 0x60,
    BasicModeCtrl = 0x62,
    BasicModeStatus = 0x64,
    NWayAdvert = 0x66,
    NWayLPAR = 0x68,
    NWayExpansion = 0x6A,
    /* Undocumented registers, but required for proper operation. */
    FIFOTMS = 0x70,        /* FIFO Control and test. */
    CSCR = 0x74,        /* Chip Status and Configuration Register. */
    PARA78 = 0x78,
    PARA7c = 0x7c,        /* Magic transceiver parameter register. */
    Config5 = 0xD8        /* absent on RTL-8139A */
};

enum ClearBitMasks {
    MultiIntrClear = 0xF000,
    ChipCmdClear = 0xE2,
    Config1Clear = (1<<7)|(1<<6)|(1<<3)|(1<<2)|(1<<1)
};

enum ChipCmdBits {
    CmdReset = 0x10,
    CmdRxEnb = 0x08,
    CmdTxEnb = 0x04,
    RxBufEmpty = 0x01
};

/* Interrupt register bits, using my own meaningful names. */
enum IntrStatusBits {
    PCIErr = 0x8000,
    PCSTimeout = 0x4000,
    RxFIFOOver = 0x40,
    RxUnderrun = 0x20,
    RxOverflow = 0x10,
    TxErr = 0x08,
    TxOK = 0x04,
    RxErr = 0x02,
    RxOK = 0x01,

    RxAckBits = RxFIFOOver | RxOverflow | RxOK
};

enum TxStatusBits {
    TxHostOwns = 0x2000,
    TxUnderrun = 0x4000,
    TxStatOK = 0x8000
};

enum RxStatusBits {
    RxMulticast = 0x8000,
    RxPhysical = 0x4000,
    RxBroadcast = 0x2000,
    RxBadSymbol = 0x0020,
    RxRunt = 0x0010,
    RxTooLong = 0x0008,
    RxCRCErr = 0x0004,
    RxBadAlign = 0x0002,
    RxStatusOK = 0x0001
};

/* Bits in RxConfig. */
enum rx_mode_bits {
    AcceptErr = 0x20,
    AcceptRunt = 0x10,
    AcceptBroadcast = 0x08,
    AcceptMulticast = 0x04,
    AcceptMyPhys = 0x02,
    AcceptAllPhys = 0x01
};


#define VirtualAddr_PhysicalAddr(addr)   ((((INT32U)(addr)>>12)&0xFFFF0uL)+((INT32U)(addr)&0xFFFFuL))

struct rtl8139_private{
    INT16S   io_base;
    INT16S   irq;
    INT08U   mac_addr[6];
    INT08U  *poll_buf;  /* 接收缓冲 */
    INT08U  *xmit_buf[NUM_TX_DESC];
    INT16S   poll_buf_index;    /* 接收帧在poll_buf中的索引 */
    INT16S   xmit_buf_index;    /* 发送器索引, 8139共有4个发送器 */
    INT16S   state;    
};

#define RTL8139_STATE_NONE   0
#define RTL8139_STATE_INIT   1
#define RTL8139_STATE_OPEN   2
#define RTL8139_STATE_STOP   3

struct rtl8139_private rtl8139_data;


BOOL init_rtl8139(void)
{
    INT32U temp;
    static INT08U * rtl8139_rx_buf              =  NULL;
    static INT08U * rtl8139_tx_buf[NUM_TX_DESC] = {NULL};

    if(!rtl8139_rx_buf)
        allocate_buffer(rtl8139_rx_buf,    INT08U *, (INT32U)(RX_BUF_LEN+16), return fail);
    if(!rtl8139_tx_buf[0])
        allocate_buffer(rtl8139_tx_buf[0], INT08U *, (INT32U)(TX_BUF_LEN+16), return fail);
    if(!rtl8139_tx_buf[1])
        allocate_buffer(rtl8139_tx_buf[1], INT08U *, (INT32U)(TX_BUF_LEN+16), return fail);
    if(!rtl8139_tx_buf[2])
        allocate_buffer(rtl8139_tx_buf[2], INT08U *, (INT32U)(TX_BUF_LEN+16), return fail);
    if(!rtl8139_tx_buf[3])
        allocate_buffer(rtl8139_tx_buf[3], INT08U *, (INT32U)(TX_BUF_LEN+16), return fail);

    MEMSET((INT08S *)&rtl8139_data, 0, sizeof(rtl8139_data));
    MEMSET((INT08S *)&RTL8139_DRV.status, 0, sizeof(NIC_STATUS));
    pci_enable_device(RTL8139_DRV.BusNo, RTL8139_DRV.DeviceFuncNo);
    rtl8139_data.state       = RTL8139_STATE_NONE;
    rtl8139_data.io_base     = pci_get_base_addr_io(RTL8139_DRV.BusNo, RTL8139_DRV.DeviceFuncNo);
    temp = inportdword( rtl8139_data.io_base + MAC0); /* 读MAC地址 */
    rtl8139_data.mac_addr[0] = (INT08U)(temp & 0xffL); temp>>=8;
    rtl8139_data.mac_addr[1] = (INT08U)(temp & 0xffL); temp>>=8;
    rtl8139_data.mac_addr[2] = (INT08U)(temp & 0xffL); temp>>=8;
    rtl8139_data.mac_addr[3] = (INT08U)(temp & 0xffL);
    temp = inportdword( rtl8139_data.io_base + MAC0 + 4);
    rtl8139_data.mac_addr[4] = (INT08U)(temp & 0xffL); temp>>=8;
    rtl8139_data.mac_addr[5] = (INT08U)(temp & 0xffL);
    rtl8139_data.irq         = pci_get_irq_line(RTL8139_DRV.BusNo, RTL8139_DRV.DeviceFuncNo);
    rtl8139_data.poll_buf    = (INT08U *)rtl8139_rx_buf;
    rtl8139_data.xmit_buf[0] = (INT08U *)rtl8139_tx_buf[0];
    rtl8139_data.xmit_buf[1] = (INT08U *)rtl8139_tx_buf[1];
    rtl8139_data.xmit_buf[2] = (INT08U *)rtl8139_tx_buf[2];
    rtl8139_data.xmit_buf[3] = (INT08U *)rtl8139_tx_buf[3];
    /*lint --e{63}*/
    (INT32S)rtl8139_data.poll_buf +=   0xFL; /* 将指针对齐到32位边界 */
    (INT32S)rtl8139_data.poll_buf &= (~0xFL);/*lint !e63 */
    (INT32S)rtl8139_data.xmit_buf[0] += 0xFL;
    (INT32S)rtl8139_data.xmit_buf[0] &= (~0xFL);/*lint !e63 */
    (INT32S)rtl8139_data.xmit_buf[1] += 0xFL;
    (INT32S)rtl8139_data.xmit_buf[1] &= (~0xFL);/*lint !e63 */
    (INT32S)rtl8139_data.xmit_buf[2] += 0xFL;
    (INT32S)rtl8139_data.xmit_buf[2] &= (~0xFL);/*lint !e63 */
    (INT32S)rtl8139_data.xmit_buf[3] += 0xFL;
    (INT32S)rtl8139_data.xmit_buf[3] &= (~0xFL);/*lint !e63 */
    rtl8139_data.state       = RTL8139_STATE_INIT;
    set_local_mac(rtl8139_data.mac_addr);
    return ok;
}


BOOL open_rtl8139(void)
{
    DispatchLock();
    rtl8139_data.poll_buf_index = 0;
    rtl8139_data.xmit_buf_index = 0;
    pci_enable_device(RTL8139_DRV.BusNo, RTL8139_DRV.DeviceFuncNo);
    outportbyte(rtl8139_data.io_base + ChipCmd, CmdReset);
    outportbyte(rtl8139_data.io_base + ChipCmd, CmdRxEnb|CmdTxEnb); /*lint !e655 */
    outportdword(rtl8139_data.io_base + TxAddr0,  VirtualAddr_PhysicalAddr(rtl8139_data.xmit_buf[0]));
    outportdword(rtl8139_data.io_base + TxAddr1,  VirtualAddr_PhysicalAddr(rtl8139_data.xmit_buf[1]));
    outportdword(rtl8139_data.io_base + TxAddr2,  VirtualAddr_PhysicalAddr(rtl8139_data.xmit_buf[2]));
    outportdword(rtl8139_data.io_base + TxAddr3,  VirtualAddr_PhysicalAddr(rtl8139_data.xmit_buf[3]));
    outportdword(rtl8139_data.io_base + RxBuf,    VirtualAddr_PhysicalAddr(rtl8139_data.poll_buf));
    outportdword(rtl8139_data.io_base + TxConfig, 0x03000700UL);/* TCR: 32xDMA */
    outportdword(rtl8139_data.io_base + RxConfig, 0x0000212AUL);/* RCR: 8KB, 32xFIFO, 32xDMA, Error, BC */
    outportdword(rtl8139_data.io_base + MAR0,     0x0L);
    outportdword(rtl8139_data.io_base + MAR0 + 4, 0x0L);
    outportword(rtl8139_data.io_base + BasicModeCtrl, 0x100);/* 0x100=10Mb FullDPX, 0x2100=100M, 0x1100=Auto */
    set_local_mac(rtl8139_data.mac_addr);
    rtl8139_data.state = RTL8139_STATE_OPEN;
    TimerSet(TimerNIC, Rtl8139_ChipResetTimeOut, TIMER_TYPE_AUTO, watchdog_rtl8139, NULL);
    DispatchUnlock();
    return ok;
}

BOOL stop_rtl8139(void)
{
    DispatchLock();
    TimerStop(TimerNIC);
    outportbyte(rtl8139_data.io_base + ChipCmd, CmdReset);
    pci_disable_device(RTL8139_DRV.BusNo, RTL8139_DRV.DeviceFuncNo);
    rtl8139_data.state = RTL8139_STATE_STOP;
    DispatchUnlock();
    return ok;
}

BOOL xmit_rtl8139(void * srcbuf, INT16S buflen)
{
    INT08U * xmit_buf;
    INT16S xmit_buf_index;

    if(!islink_rtl8139()){
        return fail;
    }
    if(buflen > MAX_ETH_FRAME_SIZE){
       buflen = MAX_ETH_FRAME_SIZE;
    }
    DispatchLock();
    xmit_buf_index = rtl8139_data.xmit_buf_index;
    xmit_buf = rtl8139_data.xmit_buf[xmit_buf_index];
    /*
    printf("xmit_buf_index=%d, xmit_buf=%08lX, PhyAddr(xmit_buf)=%08lX\n",
        xmit_buf_index, xmit_buf, VirtualAddr_PhysicalAddr(xmit_buf));
    */
    for(;;){
        if(inportword(rtl8139_data.io_base + TxStatus0 + 4*xmit_buf_index) & 0x2000){
            break;
        }
        FamesDelay(200L);
    }
    rtl8139_data.xmit_buf_index = (1+xmit_buf_index)%4;
    MEMCPY((INT08S *)xmit_buf, (INT08S *)srcbuf, buflen);
    if(buflen < 64){
        MEMSET((INT08S *)&xmit_buf[buflen], 0, (64-buflen));
        buflen=64;
    }
    buflen = ((buflen+3)&(~3));
 /* outportdword(rtl8139_data.io_base + TxAddr0 + 4*xmit_buf_index, VirtualAddr_PhysicalAddr(xmit_buf)); */
    outportdword(rtl8139_data.io_base + TxStatus0 + 4*xmit_buf_index, (INT32U)((INT32S)buflen&0xFFFFL));
    DispatchUnlock();
    return ok;
}

BOOL poll_rtl8139(void * dstbuf, INT16S * buflen)
{
    INT08U  * Rx_Buf;
    INT16S    Rx_Idx;
    INT16S    RxBytes, i;
    
    #define ROLL_IDX(idx)  ((idx)&0x1FFF)

    FamesAssert(dstbuf);
    FamesAssert(buflen);

    if(!dstbuf || !buflen){
        return fail;
    }
    
    if(!islink_rtl8139()){
        return fail;
    }
    DispatchLock();
    if(!(inportword(rtl8139_data.io_base+ChipCmd)&RxBufEmpty)){
        Rx_Buf  = rtl8139_data.poll_buf;
        Rx_Idx  = rtl8139_data.poll_buf_index;
        RxBytes = *((INT16S *)&Rx_Buf[ROLL_IDX(Rx_Idx+2)]); /*lint !e826*/
        #if 0
            DispatchLock();
            for(i=0; i<RxBytes+4; i++)printf("%02X  ",
                Rx_Buf[ROLL_IDX(Rx_Idx+i)]);
                printf(" \n===== RxIndex=%04X, RxLen=%04X =====\n",
                Rx_Idx, RxBytes
              );
            DispatchUnlock();
        #endif
        rtl8139_data.poll_buf_index+=(RxBytes+7);
        rtl8139_data.poll_buf_index&=0x1FFC;
        outportbyte(rtl8139_data.io_base+0x36, 0x08);
        outportword(rtl8139_data.io_base+0x38, ((rtl8139_data.poll_buf_index-0x10)&0x1FFC));/*这里有点奇怪*/
        if((Rx_Buf[ROLL_IDX(Rx_Idx)]&0xfe)||((Rx_Buf[ROLL_IDX(Rx_Idx)]&0x1)==0)||(Rx_Buf[ROLL_IDX(Rx_Idx+1)]==0)||(RxBytes > MAX_ETH_FRAME_SIZE+4)){
            TimerForce(TimerNIC);
            DispatchUnlock();
            return fail;
        }
        RxBytes-=4; /* Ignore the 4-bytes crc tail */
        *buflen = RxBytes;
        for(i=0; i<RxBytes; i++, Rx_Idx++){
            ((INT08U *)dstbuf)[i] = Rx_Buf[ROLL_IDX(Rx_Idx+4)];
        }
        TimerReStart(TimerNIC);
        DispatchUnlock();
        return ok;
    } else {
        *buflen=0;
        DispatchUnlock();
        return fail;
    }
        
    #undef ROLL_IDX
}

BOOL ctrl_rtl8139(INT16S cmd, void * data)
{
    data = data;
    switch(cmd){
        case 0:
            break;
        default:
            break;
    }
    return ok;
}

BOOL get_status_rtl8139(void * status)
{
    status = status;
    return ok;
}

BOOL islink_rtl8139(void)
{
    if(inportbyte(rtl8139_data.io_base+MediaStatus)&0x4){
        TimerForce(TimerNIC);
        return fail;
    } else {
        return ok;
    }
}

void watchdog_rtl8139(void * data, INT16S nr)
{
    data = data;
    nr   = nr;
    stop_rtl8139();
    open_rtl8139();
}


/*====================================================================================
 * 
 * 本文件结束: rtl8139.c
 * 
**==================================================================================*/

