/***********************************************************************************************
 * �ļ�: arp.c
 * ˵��: ��ַ����Э��(ARP/RARP)
 * ����: Jun
 * ʱ��: 2010-8-18
***********************************************************************************************/
#define  FAMES_ARP_C
#include <includes.h>

/*----------------------------------------------------------------------------------------------
 * 
 * ARP�����¼�, ���ڻ��ѵȴ�ARP����������(��δ��)
 * 
**--------------------------------------------------------------------------------------------*/
static ECB arp_wait_event;

/*----------------------------------------------------------------------------------------------
 * ����:    arp_initialize()
 *
 * ˵��:    ARPЭ���ʼ��
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
 * ����:    arp_dispatcher()
 *
 * ˵��:    ARP֡����
 *
 * ����:    1) buf          ���յ���ARP֡
 *          2) buf_len      ֡��С
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
 * ����:    arp_insert()
 *
 * ˵��:    ����һ��ARP��Ŀ
 *
 * ����:    1) ip      IP��ַ
 *          2) mac     MAC��ַ
 *
 * ��ע:    1) ���ARP���ٻ������Ѿ����˶�Ӧ��Ŀ(IP), �����֮.
 *          2) ����������Ѿ�û�пռ������µ���Ŀ, ����һ������õ�, �滻֮.
**--------------------------------------------------------------------------------------------*/
void apical arp_insert(INT32U ip, INT08U * mac)
{
    INT16S  i, posi_nul, posi_oldest, min_timeout;
    struct arp_table_struct * arp;

    if( ip==0L || !mac )return;

    arp = arp_table;
    
    posi_nul = posi_oldest = 0;
    min_timeout = ARP_TIMEOUT+1;
    
    for(i=1, arp++; i<ARP_TABLE_SIZE; i++, arp++){ /* ��1��ʼ, ������0�� */
        if(arp->timeout == 0){
            if(posi_nul == 0){
                posi_nul = i; /* �ҵ�һ����λ */
            }
            continue;
        }
        if( min_timeout > arp->timeout ){
            min_timeout = arp->timeout; /* �ҵ�һ������õ� */
            posi_oldest = i;
        }
        DispatchLock();
        if(arp->ip == ip){   /* ��Ŀ�Ѵ���, ����֮ */
            CopyMacAddr(arp->ha, mac); /*lint !e717*/
            arp->timeout=ARP_TIMEOUT;
            DispatchUnlock();
            return;
        }
        DispatchUnlock();
    }
    /* ִ�е�����, ˵��������û�ж�Ӧ����Ŀ, ����... */
    if(posi_nul != 0){  /* �п�λ */
        arp = &arp_table[posi_nul];
    } else { /* ��λҲû��??? ��ֻ���滻һ������õ���, ��ʵ, �Ҳ���������, ����ʵ...... */
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
 * ����:    arp_delete()
 *
 * ˵��:    ɾ��һ��ARP��Ŀ
 *
 * ����:    1) arp_item     ARP��Ŀָ��
 *
 * ��ע:    arp_item->ha �ǲ���Ҫ��յ�, �Ǻ�~~~
**--------------------------------------------------------------------------------------------*/
void apical arp_delete(struct arp_table_struct * arp_item)
{
    if(!arp_item)return;
    
    arp_item->timeout = 0;
    arp_item->ip      = 0x0L;
}

/*----------------------------------------------------------------------------------------------
 * ����:    arp_send()
 *
 * ˵��:    ����ARP֡
 *
 * ����:    1) dst_mac       Ŀ��MAC��ַ
 *          2) dst_ip        Ŀ��IP��ַ
 *          3) local_mac     ����MAC��ַ
 *          4) local_ip      ����IP��ַ
 *          5) arp_op        ARP������
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
 * ����:    arp_dpc()
 *
 * ˵��:    ARPЭ���DPC����, ���ڹ���ARP���ٻ����и���Ŀ�Ķ�ʱ��
**--------------------------------------------------------------------------------------------*/
void  __internal __arp_dpc(void * data, INT16S nr)
{
    INT16S  i;
    struct arp_table_struct * arp;

    data = data;

    arp = arp_table;
    
    for(i=1, arp++; i<ARP_TABLE_SIZE; i++, arp++){ /* ��1��ʼ, ������0�� */
        if(arp->timeout == 0)continue;
        arp->timeout -= nr;
        if(arp->timeout <= 0){
            arp_delete((struct arp_table_struct *)arp);
        }
    }
}

/*----------------------------------------------------------------------------------------------
 * ����:    get_mac()
 *
 * ˵��:    ����IP��ַΪMAC��ַ
 *
 * ����:    1) ip   ��������IP��ַ
 *
 * ���:    ��Ӧ��MAC��ַ
 *
 * ��ע:    ����ڸ��ٻ�����û���ҵ���Ӧ����Ŀ, 
 *          �������緢һ��ARP����, �������3��
**--------------------------------------------------------------------------------------------*/
INT08U * apical get_mac(INT32U ip)
{
    INT16S  i, retries=0;
    INT32U  ms = 10L;
    struct arp_table_struct * arp;

    if(ip == 0xFFFFFFFFuL || ip == 0x0uL){ /* �㲥 */
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
 * ���ļ�����: arp.c
 * 
**============================================================================================*/
