/***********************************************************************************************
 * 文件: ip.h
 *
 * 说明: IP协议
 *
 * 作者: Jun
 *
 * 特别: 此文件参考于Linux-1.0/ip.h
***********************************************************************************************/
#ifndef FAMES_IP_H
#define FAMES_IP_H

/*----------------------------------------------------------------------------------------------
 *
 * IP帧首部结构及常数
 *
**--------------------------------------------------------------------------------------------*/
#define IPOPT_END           0
#define IPOPT_NOOP          1
#define IPOPT_SEC           130
#define IPOPT_LSRR          131
#define IPOPT_SSRR          137
#define IPOPT_RR            7
#define IPOPT_SID           136
#define IPOPT_TIMESTAMP     68

#if 0
struct timestamp {
    INT08U  len;
    INT08U  ptr;
    union {
        INT08U flags:4, overflow:4;
        INT08U full_char;
    } x;
    INT32U  data[9];
};

#define MAX_ROUTE    16

struct route {
    INT08U  route_size;
    INT08U  pointer;
    INT32U  route[MAX_ROUTE];
};

struct options {
    struct route      record_route;
    struct route      loose_route;
    struct route      strict_route;
    struct timestamp  tstamp;
    INT16U            security;
    INT16U            compartment;
    INT16U            handling;
    INT16U            stream;
    INT16U            tcc;
};
#endif

struct iphdr {
    INT16U ihl:4;
    INT16U version:4;
    INT16U tos:8;
    /*
    INT08U ihl:4; 
    INT08U version:4;
    INT08U tos;
    */
    INT16U tot_len;
    INT16U id;
    INT16U flag_off;
    INT08U ttl;
    INT08U protocol;
    INT16U check;
    INT32U saddr;
    INT32U daddr;
  /*The options start here. */
};

/* IP flags. */
#define IP_CE       0x8000      /* Flag: "Congestion"               */
#define IP_DF       0x4000      /* Flag: "Don't Fragment"           */
#define IP_MF       0x2000      /* Flag: "More Fragments"           */
#define IP_OFFSET   0x1FFF      /* "Fragment Offset" part           */

/*----------------------------------------------------------------------------------------------
 *
 * 协议代码
 *
**--------------------------------------------------------------------------------------------*/
#define IP_P_ICMP   0x1         /* ICMP Internet Control Message    */
#define IP_P_TCP    0x6         /* TCP Transmission Control         */
#define IP_P_UDP    0x11        /* UDP User Datagram                */

/*----------------------------------------------------------------------------------------------
 *
 * 函数声明
 *
**--------------------------------------------------------------------------------------------*/
void     apical ip_dispatcher(INT08S * ip_pkt, INT16S pkt_len);
BOOL     apical ip_xmit(INT08S * ip_pkt);
INT08S * apical ip_build_header(INT08S * ip_pkt, INT32U dstip, 
                                INT08U protocol, INT16S proto_data_len );
void     apical set_local_ip(INT32U ip);
INT32U   apical get_local_ip(void);
INT32U   apical get_bcast_ip(void);

BOOL     apical check_ip_addr(INT32U ip);

BOOL     apical ip_string(INT08S * dst, INT32U ip);


#endif                          /* #ifndef FAMES_IP_H          */

/*==============================================================================================
 * 
 * 本文件结束: ip.h
 * 
**============================================================================================*/

