/***********************************************************************************************
 * �ļ�: eth.c
 *
 * ˵��: ��̫��Э��
 *
 * ����: Jun
 *
 * ʱ��: 2010-8-18
***********************************************************************************************/
#define  FAMES_ETH_C
#include "includes.h"


#define  PROTOCOL_ARP_EN  1
#define  PROTOCOL_IP_EN   1
#define  PROTOCOL_IPX_EN  1

/*----------------------------------------------------------------------------------------------
 * ����:    eth_dispatcher()
 *
 * ˵��:    ��̫��֡�ַ���
 *
 * ����:    1) buf            ���յ�����̫֡
 *          2) buf_len        ֡��С
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
 * ����:    send_eth()
 *
 * ˵��:    ����һ����̫��֡
 *
 * ����:    1) buf            �����͵Ļ�����
 *          2) buf_len        ��������С
 *
 * ���:    ok/fail
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
 * ����:    set_local_mac()
 *
 * ˵��:    �趨������̫����ַ
 *
 * ����:    1) mac     ���趨����̫����ַ
 *
 * ���:    ok/fail
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
 * ����:    get_local_mac()
 *
 * ˵��:    ȡ������̫����ַ
 *
 * ����:    ��
 *
 * ���:    ������̫����ַ
**--------------------------------------------------------------------------------------------*/
INT08U * apical get_local_mac(void)
{    
    return ethernet_local_mac_addr;
}

/*----------------------------------------------------------------------------------------------
 * ����:    get_bcast_mac()
 *
 * ˵��:    ȡ��̫���㲥��ַ
 *
 * ����:    ��
 *
 * ���:    ��̫���㲥��ַ
**--------------------------------------------------------------------------------------------*/
INT08U * apical get_bcast_mac(void)
{    
    return ethernet_bcast_mac_addr;
}

/*==============================================================================================
 * 
 * ���ļ�����: eth.c
 * 
**============================================================================================*/

