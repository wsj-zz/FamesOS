/***********************************************************************************************
** 文件: tcp.c
** 说明: tcp protocol
** 作者: Jun
** 尚未完成, 以后再说
***********************************************************************************************/
#define  FAMES_TCP_C
#include <includes.h>

/*----------------------------------------------------------------------------------------------
 * 函数make_udp_checksum()的选项及原型
**--------------------------------------------------------------------------------------------*/
#define   MAKE_TCP_CSUM_OPT_MAKE   0
#define   MAKE_TCP_CSUM_OPT_CHECK  1

BOOL apical make_tcp_checksum(INT08U * tcp_pkt, INT16U pkt_len, INT32U sip, INT32U dip, INT16U opt);

/*----------------------------------------------------------------------------------------------
 * 函数:    tcp_dispatcher()
 *
 * 说明:    TCP数据包处理
 *
 * 输入:    1) ip_pkt         IP数据包
 *          2) pkt_len        数据包大小
**--------------------------------------------------------------------------------------------*/
void apical tcp_dispatcher(INT08S * buf, INT16S buf_len)
{
    FamesAssert(buf);

    buf_len = buf_len;
    
    #if 0
    static int i=0;
    DispatchLock();
    printf("tcp rxed %d\n", i++);
    DispatchUnlock();
    #endif
}

/*----------------------------------------------------------------------------------------------
 * 函数:    make_tcp_checksum()
 *
 * 说明:    计算(/检测)TCP报文的校验和
 *
 * 输入:    1) tcp_pkt        UDP数据包
 *          2) pkt_len        UDP报文长度
 *          3) sip            源IP地址
 *          4) dip            目的IP地址
 *          5) opt            选项(命令)
 *
 * 返回:    1) ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical make_tcp_checksum(INT08U * tcp_pkt, INT16U pkt_len, INT32U sip, INT32U dip, INT16U opt)
{
    INT16U  * p, csum, i;
    INT32U   check=0;
    struct tcphdr * hdr;

    FamesAssert(tcp_pkt);

    if(!tcp_pkt){
        return fail;
    }

    p = (INT16U *)tcp_pkt; /*lint !e826 */

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

    check += (INT32U)(sip    ) & 0xFFFFuL; /* TCP伪首部 */
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
        case MAKE_TCP_CSUM_OPT_MAKE:
            hdr = (struct tcphdr *)tcp_pkt; /*lint !e826 */
            hdr->check = csum;
            return ok;
        case MAKE_TCP_CSUM_OPT_CHECK:
            if(csum == 0){
                return ok;
            } else {
                return fail;
            }
        default:
            return fail;
    }
}


/*==============================================================================================
 * 
 * 本文件结束: tcp.c
 * 
**============================================================================================*/


