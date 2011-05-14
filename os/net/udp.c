/***********************************************************************************************
 * 文件: udp.c
 *
 * 说明: udp protocol
 *
 * 作者: Jun
***********************************************************************************************/
#define  FAMES_UDP_C
#include <includes.h>

/*----------------------------------------------------------------------------------------------
 * 函数make_udp_checksum()的选项及原型
**--------------------------------------------------------------------------------------------*/
#define   MAKE_UDP_CSUM_OPT_MAKE   0
#define   MAKE_UDP_CSUM_OPT_CHECK  1

BOOL apical make_udp_checksum(INT08U * udp_pkt, INT16U pkt_len, INT32U sip, INT32U dip, INT16U opt);

/*----------------------------------------------------------------------------------------------
 * 函数:    udp_dispatcher()
 *
 * 说明:    UDP报文分发器
 *
 * 输入:    1) ip_pkt         IP数据包
 *          2) pkt_len        数据包大小
**--------------------------------------------------------------------------------------------*/
void apical udp_dispatcher(INT08S * ip_pkt, INT16S pkt_len)
{
    struct iphdr * ip_hdr;
    struct udphdr * u_hdr;
    INT16U udp_len;
    INT16U udp_port;

    FamesAssert(ip_pkt);

    if(!ip_pkt)return;

    ip_hdr    = (struct iphdr *)ip_pkt; /*lint !e826 */
    u_hdr     = (struct udphdr *)((INT08S *)ip_pkt + (ip_hdr->ihl<<2)); /*lint !e826 */

    udp_len   = INT16XCHG(u_hdr->len); /* UDP数据报长度 */

    if(!make_udp_checksum((INT08U *)u_hdr, udp_len, ip_hdr->saddr, ip_hdr->daddr, MAKE_UDP_CSUM_OPT_CHECK)){
        return;
    }

    udp_port = INT16XCHG(u_hdr->dest);

    if(search_udp_port(udp_port, ip_pkt, pkt_len)){
        return;
    }
    
    switch(udp_port){
        case UDP_P_TFTP:
            #if TFTP_ENABLE == 1
            tftpd_dispatcher(ip_pkt, pkt_len);
            #endif
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    make_udp_checksum()
 *
 * 说明:    计算(/检测)UDP报文的校验和
 *
 * 输入:    1) udp_pkt        UDP数据包
 *          2) pkt_len        UDP报文长度
 *          3) sip            源IP地址
 *          4) dip            目的IP地址
 *          5) opt            选项
 *
 * 返回:    1) ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical make_udp_checksum(INT08U * udp_pkt, INT16U pkt_len, INT32U sip, INT32U dip, INT16U opt)
{
    INT16U  * p, csum, i;
    INT32U   check=0;
    struct udphdr * hdr;

    FamesAssert(udp_pkt);

    if(!udp_pkt){
        return fail;
    }

    p = (INT16U *)udp_pkt; /*lint !e826 */

    i = pkt_len;

    while(i>1) {
        check += (INT32U) *p++;
        if(check&0x80000000uL){
            check=(check&0xFFFFuL)+(check>>16);
        }
        i-=2;
    }
    if(i){
        check += (INT32U)(0xFF & *(INT08U *)p);
    }

    check += (INT32U)(sip    ) & 0xFFFFuL; /* UDP伪首部 */
    check += (INT32U)(sip>>16) & 0xFFFFuL;
    check += (INT32U)(dip    ) & 0xFFFFuL;
    check += (INT32U)(dip>>16) & 0xFFFFuL;
    check += (INT32U)(IP_P_UDP<<8);
    check += (INT32U)INT16XCHG(pkt_len);

    
    while(check>>16){
         check=(check&0xFFFFuL)+(check>>16);

    }

    csum = (INT16U) ~check;

    switch(opt){
        case MAKE_UDP_CSUM_OPT_MAKE:
            hdr = (struct udphdr *)udp_pkt; /*lint !e826 */
            hdr->check = csum;
            return ok;
        case MAKE_UDP_CSUM_OPT_CHECK:
            if(csum == 0){
                return ok;
            } else {
                return fail;
            }
        default:
            return fail;
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    send_udp()
 *
 * 说明:    发送udp报文
 *
 * 输入:    1) target_ip     目标ip地址
 *          2) target_port   目标端口
 *          3) local_port    本地端口
 *          4) data_buf      udp数据
 *          5) data_len      数据长度
 *
 * 返回:    ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical send_udp(INT32U target_ip, INT16U target_port, INT16U local_port, 
                     INT08S * data_buf, INT16U data_len)
{
    static INT08S * tx_buf = NULL;
    struct udphdr * hdr;
    BOOL     retval;

    FamesAssert(data_buf);

    if(!tx_buf){
        allocate_buffer(tx_buf, INT08S *, (INT32U)2048, return fail);
    }

    hdr = (struct udphdr *)ip_build_header(tx_buf, target_ip, IP_P_UDP, (INT16S)data_len+8); /*lint !e826*/
    if(!hdr){
        return fail;
    }
    hdr->dest   = INT16XCHG(target_port);
    hdr->source = INT16XCHG(local_port);
    hdr->len    = INT16XCHG(data_len + 8);
    hdr->check  = 0x0;
    MEMCPY((INT08S *)((INT08S *)hdr+8), data_buf, (INT16S)data_len);
    make_udp_checksum((INT08U *)hdr, data_len+8, get_local_ip(), target_ip, MAKE_UDP_CSUM_OPT_MAKE);
    retval = ip_xmit(tx_buf);
    
    return retval;
 }


/*==============================================================================================
 * 
 * 本文件结束: udp.c
 * 
**============================================================================================*/


