/***********************************************************************************************
** 文件: icmp.h
** 说明: ICMP协议(echo)
** 作者: Jun
***********************************************************************************************/
#ifndef FAMES_ICMP_H
#define FAMES_ICMP_H

/*----------------------------------------------------------------------------------------------
 *
 * ICMP帧首部结构及常数(只支持echo)
 *
**--------------------------------------------------------------------------------------------*/
#define ICMP_ECHOREPLY  0    /* Echo Reply              */
#define ICMP_ECHO       8    /* Echo Request            */

struct icmphdr {
    INT08U  type;
    INT08U  code;
    INT16U  checksum;
    union {
        struct {
            INT16U  id;
            INT16U  sequence;
        } echo;
        INT32U gateway;
    } un;
};

struct icmp_err {
    INT16U  errno;
    INT16U  atal:1;
};

/*----------------------------------------------------------------------------------------------
 *
 * 函数声明
 *
**--------------------------------------------------------------------------------------------*/
void apical icmp_dispatcher(INT08S *ip_pkt, INT16S pkt_len);
BOOL apical send_icmp(INT08U type, INT08U code, INT16U id, INT16U  sequence, 
                      INT32U target_ip, INT08S * data, INT16U data_len);
void apical set_icmp_message(INT08U type, INT32U src_ip);
INT08S * apical get_icmp_message(INT08S * dst_message_buf);

#define  send_echo(type, code, id, sequence, target_ip, data, data_len)  \
            send_icmp(type, code, id, sequence, target_ip, data, data_len)


  
#endif                          /* #ifndef FAMES_ICMP_H          */

/*
*本文件结束: icmp.h ============================================================================
*/

