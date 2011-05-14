/***********************************************************************************************
 * 文件: eth.c
 *
 * 说明: 以太网协议
 *
 * 作者: Jun
 *
 * 时间: 2010-8-18
***********************************************************************************************/
#define  FAMES_ETH_C
#include "includes.h"


#define  PROTOCOL_ARP_EN  1
#define  PROTOCOL_IP_EN   1
#define  PROTOCOL_IPX_EN  1

/*----------------------------------------------------------------------------------------------
 * 函数:    eth_dispatcher()
 *
 * 说明:    以太网帧分发器
 *
 * 输入:    1) buf            接收到的以太帧
 *          2) buf_len        帧大小
**--------------------------------------------------------------------------------------------*/
void apical eth_dispatcher(INT08S * buf, INT16S buf_len)
{
    struct ethhdr * hdr;

    FamesAssert(buf);
    FamesAssert(buf_len >= ETH_ZLEN);

    if(buf_len < ETH_ZLEN)
        return;

    eth_stat.rx_packets++;

    hdr = (struct ethhdr *)buf; /*lint !e826 :Suspicious conversion */

    buf     += sizeof(struct ethhdr); /*lint !e613 */ /* skip the eth header */
    buf_len -= sizeof(struct ethhdr); /*lint !e737 !e713 */

    /*
    printf("proto=%04X\r", INT16XCHG(hdr->h_proto));
    */

    switch(INT16XCHG(hdr->h_proto)){  /*lint !e613 */
        /*lint --e{553} */
        case ETH_P_FAMES:
            #if PROTOCOL_FAMES_EN == 1
            fames_net_dispatcher(buf, buf_len);
            #endif
            break;
        case ETH_P_IP:
            #if PROTOCOL_IP_EN == 1
            ip_dispatcher(buf, buf_len);
            #endif
            break;
        case ETH_P_ARP:
            #if PROTOCOL_ARP_EN == 1
            arp_dispatcher(buf, buf_len);
            #endif
            break;
        case ETH_P_IPX:
            #if PROTOCOL_IPX_EN == 1
            ipx_dispatcher(buf, buf_len);
            #endif
            break;
        case ETH_P_RARP:
            #if PROTOCOL_RARP_EN == 1
            rarp_dispatcher(buf, buf_len);
            #endif
            break;
        case ETH_P_X25:
            #if PROTOCOL_X25_EN == 1
            x25_dispatcher(buf, buf_len);
            #endif
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    send_eth()
 *
 * 说明:    发送一个以太网帧
 *
 * 输入:    1) buf            欲发送的缓冲区
 *          2) buf_len        缓冲区大小
 *
 * 输出:    ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical send_eth(INT08S * buf, INT16S buf_len)
{
    FamesAssert(buf);
    if(nic_xmit((void *)buf, buf_len)){
        eth_stat.tx_packets++;
        return ok;
    } else {
        return fail;
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    set_local_mac()
 *
 * 说明:    设定本地以太网地址
 *
 * 输入:    1) mac     欲设定的以太网地址
 *
 * 输出:    ok/fail
**--------------------------------------------------------------------------------------------*/
static INT08U ethernet_local_mac_addr[ETH_ALEN];
static INT08U ethernet_bcast_mac_addr[]="\xFF\xFF\xFF\xFF\xFF\xFF";

BOOL apical set_local_mac(INT08U * mac)
{
    if(!mac){
        return fail;
    }
    CopyMacAddr(ethernet_local_mac_addr, mac); /*lint !e717 */
    
    return ok;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    get_local_mac()
 *
 * 说明:    取本地以太网地址
 *
 * 输入:    无
 *
 * 输出:    本地以太网地址
**--------------------------------------------------------------------------------------------*/
INT08U * apical get_local_mac(void)
{    
    return ethernet_local_mac_addr;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    get_bcast_mac()
 *
 * 说明:    取以太网广播地址
 *
 * 输入:    无
 *
 * 输出:    以太网广播地址
**--------------------------------------------------------------------------------------------*/
INT08U * apical get_bcast_mac(void)
{    
    return ethernet_bcast_mac_addr;
}

/*==============================================================================================
 * 
 * 本文件结束: eth.c
 * 
**============================================================================================*/

