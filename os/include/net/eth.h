/*************************************************************************************
 * 文件: eth.h
 *
 * 说明: 以太网
 *
 * 作者: Jun
 *
 * 特别: 此文件参考于Linux-1.0/if_ether.h
*************************************************************************************/
#ifndef FAMES_ETH_H
#define FAMES_ETH_H

/* IEEE 802.3 Ethernet magic constants.  The frame sizes omit the preamble
   and FCS/CRC (frame check sequence). */
#define ETH_ALEN        6               /* Octets in one ethernet addr     */
#define ETH_HLEN        14              /* Total octets in header.     */
#define ETH_ZLEN        60              /* Min. octets in frame sans FCS */
#define ETH_DATA_LEN    1500            /* Max. octets in payload     */
#define ETH_FRAME_LEN   1514            /* Max. octets in frame sans FCS */


/* These are the defined Ethernet Protocol ID's. */
#define ETH_P_IP        0x0800          /* Internet Protocol packet    */
#define ETH_P_ARP       0x0806          /* Address Resolution packet    */
#define ETH_P_RARP      0x8035          /* Reverse Addr Res packet    */
#define ETH_P_X25       0x0805          /* CCITT X.25            */
#define ETH_P_IPX       0x8137          /* IPX over DIX            */

/* These are the self-defined Ethernet Protocol ID's for FamesOS */
#define ETH_P_FAMES     0x579D          /* 57='W', 9D='S'+'J': wsj      */


/* This is an Ethernet frame header. */
struct ethhdr {
    INT08U  h_dest[ETH_ALEN];           /* destination eth addr         */
    INT08U  h_source[ETH_ALEN];         /* source ether addr            */
    INT16U  h_proto;                    /* packet type ID field         */
};

/* Ethernet statistics collection data. */
struct enet_statistics{
    INT32S  rx_packets;                    /* total packets received       */
    INT32S  tx_packets;                    /* total packets transmitted    */
};

/* Variables definition */
#ifdef FAMES_ETH_C
struct enet_statistics  eth_stat={0L,0L};
const INT08U NullMacAddr[]="\x00\x00\x00\x00\x00\x00";
#else
extern struct enet_statistics  eth_stat;
extern const INT08U NullMacAddr[];
#endif

#define CopyMacAddr(dstbuf, macaddr)                \
                do {                                \
                    INT08U *srcmac;                 \
                    INT08U *dstmac;                 \
                    srcmac = (INT08U *)macaddr;     \
                    dstmac = (INT08U *)dstbuf;      \
                    dstmac[0]=srcmac[0];            \
                    dstmac[1]=srcmac[1];            \
                    dstmac[2]=srcmac[2];            \
                    dstmac[3]=srcmac[3];            \
                    dstmac[4]=srcmac[4];            \
                    dstmac[5]=srcmac[5];            \
                } while(0) 
                    

/* Functions prototypes */
void apical eth_dispatcher(INT08S * buf, INT16S buf_len);
BOOL apical send_eth(INT08S * buf, INT16S buf_len);
BOOL apical set_local_mac(INT08U * mac);
INT08U * apical get_local_mac(void);
INT08U * apical get_bcast_mac(void);

#endif                                  /* #ifndef FAMES_ETH_H          */

/*====================================================================================
 * 
 * 本文件结束: eth.h
 * 
**==================================================================================*/

