/*************************************************************************************
** 文件: arp.h
** 说明: 地址解析协议(ARP/RARP)
** 作者: Jun
** 特别: 此文件参考于Linux-1.0/if_arp.h及arp.h
*************************************************************************************/
#ifndef FAMES_ARP_H
#define FAMES_ARP_H


/* constant defines.  */
#define ARP_TABLE_SIZE  32              /* size of ARP table            */
#define ARP_TIMEOUT     300             /* 5 minutes                    */
#define ARP_TIMER_VAL   1000L           /* arp.timeout的刷新间隔, 1秒   */


#define ARP_MAX_TRIES   5               /* max # of tries to send ARP   */

#define ARP_HARD_LEN    6               /* ethernet */
#define ARP_PROT_LEN    4               /* ip       */


/* This structure defines the ARP mapping cache. */
struct arp_table_struct {
    INT16S  timeout;
    INT32U  ip;
    INT08U  ha[ARP_HARD_LEN];
};

/*------------------------------------------------------------------------
 * ARP高速缓冲
**----------------------------------------------------------------------*/
#ifdef FAMES_ARP_C
struct arp_table_struct arp_table[ARP_TABLE_SIZE];
#else
extern struct arp_table_struct arp_table[];
#endif


/* ARP protocol HARDWARE identifiers. */
#define ARPHRD_NETROM   0       /* from KA9Q: NET/ROM pseudo    */
#define ARPHRD_ETHER    1       /* Ethernet 10Mbps              */
#define ARPHRD_EETHER   2       /* Experimental Ethernet        */
#define ARPHRD_AX25     3       /* AX.25 Level 2                */
#define ARPHRD_PRONET   4       /* PROnet token ring            */
#define ARPHRD_CHAOS    5       /* Chaosnet                     */
#define ARPHRD_IEEE802  6       /* IEEE 802.2 Ethernet- huh?    */
#define ARPHRD_ARCNET   7       /* ARCnet                       */
#define ARPHRD_APPLETLK 8       /* APPLEtalk                    */

/* ARP protocol opcodes. */
#define ARPOP_REQUEST   1       /* ARP request                  */
#define ARPOP_REPLY     2       /* ARP reply                    */
#define ARPOP_RREQUEST  3       /* RARP request                 */
#define ARPOP_RREPLY    4       /* RARP reply                   */


/*
 * Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  ARP packets are variable
 * in size; the arphdr structure defines the fixed-length portion.
 * Protocol type values are the same as those for 10 Mb/s Ethernet.
 * It is followed by the variable-sized fields ar_sha, arp_spa,
 * arp_tha and arp_tpa in that order, according to the lengths
 * specified.  Field names used correspond to RFC 826.
 */
struct arphdr {
    INT16U  ar_hrd;                    /* format of hardware address    */
    INT16U  ar_pro;                    /* format of protocol address    */
    INT08U  ar_hln;                    /* length of hardware address    */
    INT08U  ar_pln;                    /* length of protocol address    */
    INT16U  ar_op;                     /* ARP opcode (command)          */
    INT08U  ar_sha[ARP_HARD_LEN];      /* sender hardware address       */
    INT08U  ar_spa[ARP_PROT_LEN];      /* sender protocol address       */
    INT08U  ar_tha[ARP_HARD_LEN];      /* target hardware address       */
    INT08U  ar_tpa[ARP_PROT_LEN];      /* target protocol address       */
};

/* Functions prototypes */
void apical arp_initialize(void);
void apical arp_dispatcher(INT08S * buf, INT16S buf_len);
void apical arp_insert(INT32U ip, INT08U * mac);
void apical arp_delete(struct arp_table_struct * arp_item);
void apical arp_send(INT08U * target_mac, INT32U target_ip, 
                     INT08U * local_mac, INT32U local_ip, INT16U arp_op);
INT08U * apical get_mac(INT32U ip);


#endif                          /* #ifndef FAMES_ARP_H          */

/*====================================================================================
 * 
 * 本文件结束: arp.h
 * 
**==================================================================================*/

