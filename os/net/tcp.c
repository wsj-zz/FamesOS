/***********************************************************************************************
** �ļ�: tcp.c
** ˵��: tcp protocol
** ����: Jun
** ��δ���, �Ժ���˵
***********************************************************************************************/
#define  FAMES_TCP_C
#include <includes.h>

/*----------------------------------------------------------------------------------------------
 * ����make_udp_checksum()��ѡ�ԭ��
**--------------------------------------------------------------------------------------------*/
#define   MAKE_TCP_CSUM_OPT_MAKE   0
#define   MAKE_TCP_CSUM_OPT_CHECK  1

BOOL apical make_tcp_checksum(INT08U * tcp_pkt, INT16U pkt_len, INT32U sip, INT32U dip, INT16U opt);

/*----------------------------------------------------------------------------------------------
 * ����:    tcp_dispatcher()
 *
 * ˵��:    TCP���ݰ�����
 *
 * ����:    1) ip_pkt         IP���ݰ�
 *          2) pkt_len        ���ݰ���С
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
 * ����:    make_tcp_checksum()
 *
 * ˵��:    ����(/���)TCP���ĵ�У���
 *
 * ����:    1) tcp_pkt        UDP���ݰ�
 *          2) pkt_len        UDP���ĳ���
 *          3) sip            ԴIP��ַ
 *          4) dip            Ŀ��IP��ַ
 *          5) opt            ѡ��(����)
 *
 * ����:    1) ok/fail
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

    check += (INT32U)(sip    ) & 0xFFFFuL; /* TCPα�ײ� */
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
 * ���ļ�����: tcp.c
 * 
**============================================================================================*/


