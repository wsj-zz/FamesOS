/***********************************************************************************************
 * 文件: arp.c
 * 说明: 地址解析协议(ARP/RARP)
 * 作者: Jun
 * 时间: 2010-8-18
***********************************************************************************************/
#define  FAMES_ARP_C
#include <includes.h>

/*----------------------------------------------------------------------------------------------
 * 
 * ARP解析事件, 用于唤醒等待ARP解析的任务(暂未用)
 * 
**--------------------------------------------------------------------------------------------*/
static ECB arp_wait_event;

/*----------------------------------------------------------------------------------------------
 * 函数:    arp_initialize()
 *
 * 说明:    ARP协议初始化
**--------------------------------------------------------------------------------------------*/
void apical arp_initialize(void)
{
    INT16S  i;
    void  __internal __arp_dpc(void *, INT16S);

    for(i=1; i<ARP_TABLE_SIZE; i++){
        arp_table[i].timeout = 0;
        arp_table[i].ip      = 0x0uL;
    }
    arp_table[0].timeout = ARP_TIMEOUT;
    arp_table[0].ip      = 0xFFFFFFFFuL;
    CopyMacAddr(arp_table[0].ha, get_bcast_mac()); /*lint !e717*/
    EventInit(&arp_wait_event);
    TimerSet(TimerArp, ARP_TIMER_VAL, TIMER_TYPE_AUTO, __arp_dpc, NULL);
}

/*----------------------------------------------------------------------------------------------
 * 函数:    arp_dispatcher()
 *
 * 说明:    ARP帧处理
 *
 * 输入:    1) buf          接收到的ARP帧
 *          2) buf_len      帧大小
**--------------------------------------------------------------------------------------------*/
void apical arp_dispatcher(INT08S * buf, INT16S buf_len)
{
    struct arphdr * hdr;
    /*
    char from[32], fo[32], local[32];
    */
    
    if(!buf)return;

    buf_len = buf_len;

    hdr = (struct arphdr *)buf; /*lint !e826 */

    if(hdr->ar_hrd != INT16XCHG(ARPHRD_ETHER) || hdr->ar_pro != INT16XCHG(0x0800)){
        return;
    }
    if(hdr->ar_hln != ARP_HARD_LEN || hdr->ar_pln != ARP_PROT_LEN){
        return;
    }
    switch(INT16XCHG(hdr->ar_op)){
        case ARPOP_REQUEST:
            /*
            ip_string(from, (INT32U)STRtoINT32((INT08S *)hdr->ar_spa, CHG_OPT_RAW) );
            ip_string(fo, (INT32U)STRtoINT32((INT08S *)hdr->ar_tpa, CHG_OPT_RAW) );
            ip_string(local, get_local_ip());
            printf("request from :%s   for %s,  local=%s\n",
                from, fo, local);
            */
            arp_insert((INT32U)STRtoINT32((INT08S *)hdr->ar_spa, CHG_OPT_RAW), hdr->ar_sha);
            if((INT32U)STRtoINT32((INT08S *)hdr->ar_tpa, CHG_OPT_RAW) == get_local_ip()){
                arp_send(hdr->ar_sha, (INT32U)STRtoINT32((INT08S *)hdr->ar_spa, CHG_OPT_RAW),
                         get_local_mac(), get_local_ip(), ARPOP_REPLY);
            }
            break;
        case ARPOP_REPLY:
            arp_insert((INT32U)STRtoINT32((INT08S *)hdr->ar_spa, CHG_OPT_RAW), hdr->ar_sha);
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    arp_insert()
 *
 * 说明:    增加一个ARP条目
 *
 * 输入:    1) ip      IP地址
 *          2) mac     MAC地址
 *
 * 备注:    1) 如果ARP高速缓冲中已经有了对应条目(IP), 则更新之.
 *          2) 如果缓冲中已经没有空间容纳新的条目, 则找一个最不常用的, 替换之.
**--------------------------------------------------------------------------------------------*/
void apical arp_insert(INT32U ip, INT08U * mac)
{
    INT16S  i, posi_nul, posi_oldest, min_timeout;
    struct arp_table_struct * arp;

    if( ip==0L || !mac )return;

    arp = arp_table;
    
    posi_nul = posi_oldest = 0;
    min_timeout = ARP_TIMEOUT+1;
    
    for(i=1, arp++; i<ARP_TABLE_SIZE; i++, arp++){ /* 从1开始, 跳过第0项 */
        if(arp->timeout == 0){
            if(posi_nul == 0){
                posi_nul = i; /* 找到一个空位 */
            }
            continue;
        }
        if( min_timeout > arp->timeout ){
            min_timeout = arp->timeout; /* 找到一个最不常用的 */
            posi_oldest = i;
        }
        DispatchLock();
        if(arp->ip == ip){   /* 条目已存在, 更新之 */
            CopyMacAddr(arp->ha, mac); /*lint !e717*/
            arp->timeout=ARP_TIMEOUT;
            DispatchUnlock();
            return;
        }
        DispatchUnlock();
    }
    /* 执行到这里, 说明缓冲中没有对应的条目, 继续... */
    if(posi_nul != 0){  /* 有空位 */
        arp = &arp_table[posi_nul];
    } else { /* 空位也没有??? 那只好替换一个最不常用的了, 其实, 我不想这样的, 但现实...... */
        arp = &arp_table[posi_oldest];
    }
    DispatchLock();
    arp->timeout = ARP_TIMEOUT;
    arp->ip      = ip;
    CopyMacAddr(arp->ha, mac); /*lint !e717*/
    DispatchUnlock();

    return;
}


/*----------------------------------------------------------------------------------------------
 * 函数:    arp_delete()
 *
 * 说明:    删除一个ARP条目
 *
 * 输入:    1) arp_item     ARP条目指针
 *
 * 备注:    arp_item->ha 是不需要清空的, 呵呵~~~
**--------------------------------------------------------------------------------------------*/
void apical arp_delete(struct arp_table_struct * arp_item)
{
    if(!arp_item)return;
    
    arp_item->timeout = 0;
    arp_item->ip      = 0x0L;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    arp_send()
 *
 * 说明:    发送ARP帧
 *
 * 输入:    1) dst_mac       目标MAC地址
 *          2) dst_ip        目标IP地址
 *          3) local_mac     本地MAC地址
 *          4) local_ip      本地IP地址
 *          5) arp_op        ARP操作码
**--------------------------------------------------------------------------------------------*/
void apical arp_send(INT08U * target_mac, INT32U target_ip, 
                     INT08U * local_mac, INT32U local_ip, 
                     INT16U arp_op)
{
    /*lint --e{613} */
    INT08U  arp_send_buf[ETH_ZLEN+16];
    struct ethhdr  * eth_hdr;
    struct arphdr  * arp_hdr;

    FamesAssert(target_mac); /*lint !e613*/
    FamesAssert(local_mac);  /*lint !e613*/

    DispatchLock();
    eth_hdr = (struct ethhdr *)&arp_send_buf[0];
    arp_hdr = (struct arphdr *)&arp_send_buf[sizeof(struct ethhdr)];
    CopyMacAddr(eth_hdr->h_dest, target_mac); /*lint !e717 !e613*/
    CopyMacAddr(eth_hdr->h_source, local_mac); /*lint !e717 !e613*/
    eth_hdr->h_proto = INT16XCHG(ETH_P_ARP);
    arp_hdr->ar_hrd  = INT16XCHG(ARPHRD_ETHER);
    arp_hdr->ar_pro  = INT16XCHG(0x0800);
    arp_hdr->ar_hln  = ARP_HARD_LEN;
    arp_hdr->ar_pln  = ARP_PROT_LEN;
    arp_hdr->ar_op   = INT16XCHG(arp_op);
    CopyMacAddr(arp_hdr->ar_sha, local_mac); /*lint !e717 !e613*/
    *(INT32U *)(arp_hdr->ar_spa) = local_ip;
    *(INT32U *)(arp_hdr->ar_tpa) = target_ip;
    if(arp_op == ARPOP_REQUEST){
        CopyMacAddr(arp_hdr->ar_tha, NullMacAddr); /*lint !e717 */
    } else {
        CopyMacAddr(arp_hdr->ar_tha, target_mac); /*lint !e717 !e613*/
    }
    send_eth((INT08S *)arp_send_buf, sizeof(struct ethhdr)+sizeof(struct arphdr));
    DispatchUnlock();
}

/*----------------------------------------------------------------------------------------------
 * 函数:    arp_dpc()
 *
 * 说明:    ARP协议的DPC例程, 用于管理ARP高速缓冲中各条目的定时器
**--------------------------------------------------------------------------------------------*/
void  __internal __arp_dpc(void * data, INT16S nr)
{
    INT16S  i;
    struct arp_table_struct * arp;

    data = data;

    arp = arp_table;
    
    for(i=1, arp++; i<ARP_TABLE_SIZE; i++, arp++){ /* 从1开始, 跳过第0项 */
        if(arp->timeout == 0)continue;
        arp->timeout -= nr;
        if(arp->timeout <= 0){
            arp_delete((struct arp_table_struct *)arp);
        }
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    get_mac()
 *
 * 说明:    解析IP地址为MAC地址
 *
 * 输入:    1) ip   欲解析的IP地址
 *
 * 输出:    对应的MAC地址
 *
 * 备注:    如果在高速缓冲中没有找到对应的条目, 
 *          则向网络发一条ARP请求, 最多重试3次
**--------------------------------------------------------------------------------------------*/
INT08U * apical get_mac(INT32U ip)
{
    INT16S  i, retries=0;
    INT32U  ms = 10L;
    struct arp_table_struct * arp;

    if(ip == 0xFFFFFFFFuL || ip == 0x0uL){ /* 广播 */
        return get_bcast_mac();
    }

    again:
    arp = arp_table;
    
    DispatchLock();
    for(i=0; i<ARP_TABLE_SIZE; i++, arp++){
        if(arp->timeout == 0)continue;
        if(arp->ip == ip){
            /* 
            arp->timeout = ARP_TIMEOUT;
            */
            #if 0
            INT08S ip_buf[32];
            ip_string(ip_buf, ip);
            printf("arp found: %s\n", ip_buf);
            #endif
            DispatchUnlock();
            return (INT08U *)arp->ha;
        }
    }
    DispatchUnlock();
    if(retries < ARP_MAX_TRIES){
        TaskSleep(ms);
        ms*=5L; /*  */ 
        arp_send(get_bcast_mac(), ip, get_local_mac(), get_local_ip(), ARPOP_REQUEST);
        TaskSleep(50L);
        retries++;
        goto again; /*lint !e801 */
    }

    return NULL;
}

/*==============================================================================================
 * 
 * 本文件结束: arp.c
 * 
**============================================================================================*/
