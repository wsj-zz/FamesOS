/***********************************************************************************************
 * 文件:    ip.c
 *
 * 说明:    IP协议
 *
 * 作者:    Jun
 *
 * 特别:    此文件参考自Linux-1.0/ip.c
***********************************************************************************************/
#define   FAMES_IP_C
#include <includes.h>

/*----------------------------------------------------------------------------------------------
 * 函数make_ip_checksum()的选项及原型
**--------------------------------------------------------------------------------------------*/
#define   MAKE_IP_CSUM_OPT_MAKE   0
#define   MAKE_IP_CSUM_OPT_CHECK  1

BOOL apical make_ip_checksum(INT08U * ip_pkt, INT16U iphdr_len, INT16U opt);

/*----------------------------------------------------------------------------------------------
 * 函数:    ip_dispatcher()
 *
 * 说明:    IP数据包分发器
 *
 * 输入:    1) ip_pkt         接收到的IP数据包
 *          2) pkt_len        数据包大小
**--------------------------------------------------------------------------------------------*/
void apical ip_dispatcher(INT08S * ip_pkt, INT16S pkt_len)
{
    struct iphdr * hdr;
    INT16S total_len;

    if(!ip_pkt)return;
    pkt_len = pkt_len;

    hdr = (struct iphdr *)ip_pkt; /*lint !e826 */

    total_len = (INT16S)INT16XCHG(hdr->tot_len);

    if(hdr->version != 4)return;
    /* 好象不必要
    if(total_len > (INT16S)pkt_len)return;
    */
    if(!check_ip_addr(hdr->daddr))return;
    if(!make_ip_checksum((INT08U *)ip_pkt, (INT16U)(hdr->ihl<<2), MAKE_IP_CSUM_OPT_CHECK)){
        return;
    }
    switch(hdr->protocol){
        case IP_P_ICMP:
            icmp_dispatcher(ip_pkt, total_len);
            break;
        case IP_P_UDP:
            udp_dispatcher(ip_pkt, total_len);
            break;
        case IP_P_TCP:
            tcp_dispatcher(ip_pkt, total_len);
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    ip_xmit()
 *
 * 说明:    IP数据包发送
 *
 * 输入:    1) pkt         欲发送的数据包(包含了eth首部与ip首部)
 *
 * 特别:    在调用此函数之前, 须用ip_build_header()构造数据包首部
**--------------------------------------------------------------------------------------------*/
BOOL apical ip_xmit(INT08S * pkt)
{
    struct iphdr * hdr;

    FamesAssert(pkt);

    if(!pkt)return fail;

    hdr  = (struct iphdr *)(pkt + sizeof(struct ethhdr)); /*lint !e826 */

    make_ip_checksum((INT08U *)hdr, (INT16U)(hdr->ihl<<2), MAKE_IP_CSUM_OPT_MAKE);

    #if 0
    if("dump ip xmit"){
        INT16U i, len;
        len = (INT16XCHG(hdr->tot_len)+sizeof(struct ethhdr));
        DispatchLock();
        printf("\nip_xmit(): len=%d\n", len);
        for(i=0; i<len; i++)printf(" %02X ", pkt[i] & 0xff);
        printf("\n== end ==\n");
        DispatchUnlock();
    }
    #endif

    return send_eth((INT08S *)pkt, (INT16S)(INT16XCHG(hdr->tot_len)+sizeof(struct ethhdr)));
}

/*----------------------------------------------------------------------------------------------
 * 函数:    ip_build_header(eth首部与ip首部)
 *
 * 说明:    构造数据包首部()
 *
 * 输入:    1) pkt            目标数据包(包含了eth首部与ip首部)
 *          2) dstip          目标IP地址
 *          3) protocol       传输层协议代码
 *          4) proto_data_len 传输层数据包大小
 *
 * 返回:    1) IP数据指针     紧接IP首部的下一个字节的地址
 *                            传输层协议可据此构造其数据包
**--------------------------------------------------------------------------------------------*/
INT08S * apical ip_build_header(INT08S * pkt, INT32U dstip, 
                                INT08U protocol, INT16S proto_data_len )
{
    struct iphdr  * hdr;
    struct ethhdr * ehdr;
    INT08U        * dst_mac;
    static INT16U id = 0x1000;

    FamesAssert(pkt);

    if(!pkt)return NULL;

    ehdr = (struct ethhdr *)pkt; /*lint !e826 */
    hdr  = (struct iphdr *)(pkt + sizeof(struct ethhdr)); /*lint !e826 */

    hdr->ihl      = 5;
    hdr->version  = 4;
    hdr->tos      = 0;
    hdr->tot_len  = INT16XCHG((INT16U)(hdr->ihl<<2) + (INT16U)proto_data_len);
    hdr->id       = INT16XCHG(id++);
    hdr->flag_off = 0;
    hdr->ttl      = (INT08U)0x80;
    hdr->protocol = protocol;
    hdr->check    = 0x00;
    hdr->saddr    = get_local_ip();
    hdr->daddr    = dstip;

    dst_mac = get_mac(dstip);
    if(dst_mac == NULL){
        return NULL;
    }
    CopyMacAddr(ehdr->h_dest,   dst_mac); /*lint !e717 */
    CopyMacAddr(ehdr->h_source, get_local_mac());/*lint !e717 */
    ehdr->h_proto = INT16XCHG(ETH_P_IP);
    
    return (pkt + (hdr->ihl<<2) + sizeof(struct ethhdr));
}

/*----------------------------------------------------------------------------------------------
 * 函数:    make_ip_checksum()
 *
 * 说明:    计算(/检测)IP首部的校验和
 *
 * 输入:    1) ip_pkt         IP首部
 *          2) iphdr_len      IP首部长度
 *          3) opt            选项
 *
 * 返回:    1) ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical make_ip_checksum(INT08U * ip_pkt, INT16U iphdr_len, INT16U opt)
{
    INT16U * p, csum;
    INT32U  check=0;
    struct iphdr * hdr;

    FamesAssert(ip_pkt);

    if(!ip_pkt){
        return fail;
    }

    p = (INT16U *)ip_pkt; /*lint !e826 */

    while(iphdr_len>1) {
        check += (INT32U)*p++;
        if(check&0x80000000uL){
            check=(check&0xFFFFuL)+(check>>16);
        }
        iphdr_len-=2;
    }
    if(iphdr_len){
        check += (INT16U) *(INT08U *)p;
    }
    while(check>>16){
        check = (check&0xFFFFuL)+(check>>16);
    }

    csum = (INT16U) ~check;

    switch(opt){
        case MAKE_IP_CSUM_OPT_MAKE:
            hdr = (struct iphdr *)ip_pkt; /*lint !e826 */
            hdr->check = csum;
            return ok;
        case MAKE_IP_CSUM_OPT_CHECK:
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
 * 函数:    set_local_ip()
 *
 * 说明:    设定本地IP地址
 *
 * 输入:    1) ip  欲设定的IP地址
**--------------------------------------------------------------------------------------------*/
static INT32U ip_local_ip_addr=0x6400A8C0uL; /* 默认为 192.168.0.100 */
static INT32U ip_bcast_ip_addr=0xFFFFFFFFuL;

void apical set_local_ip(INT32U ip)
{
    lock_kernel();
    ip_local_ip_addr = ip;
    unlock_kernel();
}

/*----------------------------------------------------------------------------------------------
 * 函数:    get_local_ip()
 *
 * 说明:    取本地IP地址
 *
 * 输入:    无
 *
 * 输出:    本地IP地址
**--------------------------------------------------------------------------------------------*/
INT32U apical get_local_ip(void)
{
    INT32U __ip;

    lock_kernel();
    __ip = ip_local_ip_addr;
    unlock_kernel();
    
    return __ip;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    get_bcast_ip()
 *
 * 说明:    取IP广播地址
 *
 * 输入:    无
 *
 * 输出:    IP广播地址
**--------------------------------------------------------------------------------------------*/
INT32U apical get_bcast_ip(void)
{
    return ip_bcast_ip_addr;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    check_ip_addr()
 *
 * 说明:    检测目的IP是否是本机
 *
 * 输入:    ip   
 *
 * 输出:    ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical check_ip_addr(INT32U ip)
{
    #if 0
    fprintf(stderr,"ip=%08lX, local=%08lX\r", ip, get_local_ip());
    #endif
    if(ip == get_local_ip()){
        return ok;
    }
    if(ip == get_bcast_ip()){
        return ok;
    }
    return fail;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    ip_string()
 *
 * 说明:    将IP地址转化为对应的字符串
 *
 * 输入:    dst, ip
 *
 * 输出:    ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical ip_string(INT08S * dst, INT32U ip)
{
    INT08S * char_ip;
    INT08S t_s[20];

    if(!dst)return fail;
    
    char_ip = (INT08S *)&ip;

    DispatchLock();
    sprintf(t_s, "%d.%d.%d.%d", 0xff&char_ip[0], 0xff&char_ip[1], 0xff&char_ip[2], 0xff&char_ip[3]);
    STRCPY(dst, t_s);
    DispatchUnlock();

    return ok;
}

/*==============================================================================================
 * 
 * 本文件结束: ip.c
 * 
**============================================================================================*/


