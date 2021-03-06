/***********************************************************************************************
 * 文件: udp.h
 *
 * 说明: UDP协议(User Datagram Protocol)
 *
 * 作者: Jun
 *
 * 特别: 此文件参考于Linux-1.0/udp.h
***********************************************************************************************/
#ifndef FAMES_UDP_H
#define FAMES_UDP_H

/*----------------------------------------------------------------------------------------------
 *
 * UDP帧首部结构及常数
 *
**--------------------------------------------------------------------------------------------*/
struct udphdr {
    INT16U  source;
    INT16U  dest;
    INT16U  len;
    INT16U  check;
};


#define  UDP_P_TFTP  69



/*----------------------------------------------------------------------------------------------
 *
 * 函数声明
 *
**--------------------------------------------------------------------------------------------*/
void apical udp_dispatcher(INT08S * ip_pkt, INT16S pkt_len);
BOOL apical send_udp(INT32U target_ip, INT16U target_port, INT16U local_port, 
                     INT08S * data_buf, INT16U data_len);


#endif                          /* #ifndef FAMES_UDP_H          */

/*==============================================================================================
 * 
 * 本文件结束: udp.h
 * 
**============================================================================================*/

