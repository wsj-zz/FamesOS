/***********************************************************************************************
 * 文件: icmp.c
 * 说明: ICMP协议
 * 作者: Jun
***********************************************************************************************/
#define  FAMES_ICMP_C
#include <includes.h>

/*----------------------------------------------------------------------------------------------
 * 函数make_icmp_checksum()的选项及原型
**--------------------------------------------------------------------------------------------*/
#define   MAKE_ICMP_CSUM_OPT_MAKE   0
#define   MAKE_ICMP_CSUM_OPT_CHECK  1

BOOL apical make_icmp_checksum(INT08U * icmp_pkt, INT16U pkt_len, INT16U opt);

#define   ICMP_DATAGRAM_MAX_LEN     256    /* 可接受的最大ICMP报文长度     */
#define   ICMP_MESSAGE_BUF_NUM      8      /* 存放ICMP消息的缓冲个数       */

INT16U    icmp_echo_request_id = 0;        /* 发送echo请求时的初始ID       */
INT16U    icmp_echo_request_sequence = 0;  /* 发送echo请求时的初始sequence */

INT16S    icmp_message_received = 0;       /* 接收到了icmp消息             */

/*----------------------------------------------------------------------------------------------
 * 函数:    icmp_dispatcher()
 *
 * 说明:    ICMP数据包处理(启动一个数据包处理任务)
 *
 * 输入:    1) ip_pkt         IP数据包
 *          2) pkt_len        数据包大小
**--------------------------------------------------------------------------------------------*/
void apical icmp_dispatcher(INT08S * ip_pkt, INT16S pkt_len)
{
    static HANDLE icmp_echo_task = InvalidHandle-1;
    static INT08S * rx_buf = NULL;
    void __daemon TaskICMP(void * data);

    if(!ip_pkt)return;

    if(rx_buf == NULL){
        allocate_buffer(rx_buf, INT08S *, (INT32U)2048, return);
    }

    if(pkt_len > 2048){
        pkt_len = 2048;
    }

    DispatchLock();
    MEMCPY(rx_buf, ip_pkt, pkt_len);
    icmp_message_received = 1;
    DispatchUnlock();
    if(icmp_echo_task == InvalidHandle-1){
        icmp_echo_task=TaskCreate(TaskICMP, (void *)rx_buf, "icmp-echo", 
                                  NULL, 512, PRIO_SYSTEM, TASK_CREATE_OPT_NONE);
        if(icmp_echo_task == InvalidHandle){
            /*
            * do something here, like sys_print();
            */
            sys_print("icmp_dispatcher: failed to create task: TaskICMP!\n");
        }
    } else {
        TaskAwake(icmp_echo_task);
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    make_icmp_checksum()
 *
 * 说明:    计算(/检测)ICMP数据包的校验和
 *
 * 输入:    1) icmp_pkt       ICMP数据包
 *          2) pkt_len        数据包长度
 *          3) opt            选项
 *
 * 返回:    1) ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical make_icmp_checksum(INT08U * icmp_pkt, INT16U pkt_len, INT16U opt)
{
    INT16U  * p, csum;
    INT32U   check=0;
    struct icmphdr * hdr;

    FamesAssert(icmp_pkt);

    if(!icmp_pkt){
        return fail;
    }

    p = (INT16U *)icmp_pkt; /*lint !e826 */

    while(pkt_len>1) {
        check += (INT32U) *p++;
        if(check&0x80000000uL){
            check=(check&0xFFFFuL)+(check>>16);
        }
        pkt_len-=2;
    }
    if(pkt_len){
        check += (INT16U) *(INT08U *)p;
    }
    while(check>>16){
         check=(check&0xFFFFuL)+(check>>16);
    }

    csum = (INT16U) ~check;

    switch(opt){
        case MAKE_ICMP_CSUM_OPT_MAKE:
            hdr = (struct icmphdr *)icmp_pkt; /*lint !e826 */
            hdr->checksum = csum;
            return ok;
        case MAKE_ICMP_CSUM_OPT_CHECK:
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
 * 函数:    send_icmp()
 *
 * 说明:    发送ICMP数据包
 *
 * 输入:    1) type        icmp类型字段
 *          2) code        icmp代码字段
 *          3) id          echo id
 *          4) sequence    echo sequence
 *          5) target_ip   目标IP
 *          6) data        icmp数据(这里为echo数据)
 *          7) data_len    数据长度(不包括icmphdr)
 *
 * 返回:    ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical send_icmp(INT08U type, INT08U code, INT16U id, INT16U  sequence, 
                      INT32U target_ip, INT08S * data, INT16U data_len)
{
    static INT08S * echo_pkt = NULL;
    static BOOL   echo_lock=NO;   
    struct icmphdr * hdr;
    INT16U i;
    INT08S * icmp_data;

    FamesAssert(data);

    if(!echo_pkt)
        allocate_buffer(echo_pkt, INT08S *, (INT32U)(ICMP_DATAGRAM_MAX_LEN+256), return fail);

    if(!data){
        return fail;
    }

    while(echo_lock == YES){
        TaskSleep(5L);
    }
    
    echo_lock = YES;
    hdr = (struct icmphdr *)ip_build_header(echo_pkt, target_ip, IP_P_ICMP, 
                                   (INT16S)(data_len+sizeof(struct icmphdr)) ); /*lint !e826 */
    if(hdr == NULL){
        echo_lock=NO;
        return fail;
    }
    if(id==0){
        id = icmp_echo_request_id++;
    }
    if(sequence==0){
        sequence = icmp_echo_request_sequence++;
    }
    hdr->type = type;
    hdr->code = code;
    hdr->un.echo.id = id;
    hdr->un.echo.sequence = sequence;
    hdr->checksum = 0;
    icmp_data = (INT08S *)hdr + sizeof(struct icmphdr);
    for(i=0; i<data_len; i++, icmp_data++){
        *icmp_data = data[i];
    }
    make_icmp_checksum((INT08U *)hdr, data_len + sizeof(struct icmphdr), MAKE_ICMP_CSUM_OPT_MAKE);
    ip_xmit(echo_pkt);
    echo_lock=NO;
    return ok;
}


/*----------------------------------------------------------------------------------------------
 * 函数:    set_icmp_message()
 *
 * 说明:    增加一个icmp消息
 *
 * 输入:    1) type        icmp类型码
 *          2) src_ip      来源ip
 *
 * 返回:    none
**--------------------------------------------------------------------------------------------*/
typedef struct __icmp_message_block {
    INT16S  in, out, number, lost;
    INT08S  buf[ICMP_MESSAGE_BUF_NUM][64];
}icmp_message_block;
icmp_message_block icmp_message={0,0,0,0, }; /*lint !e785 */

void apical set_icmp_message(INT08U type, INT32U src_ip)
{
    INT08S * echo_type;
    INT08S   ip_str[20];

    if(icmp_message.number >= ICMP_MESSAGE_BUF_NUM){
        icmp_message.lost++;
        return;
    }

    switch(type)
    {
        case ICMP_ECHO:
            echo_type = "echo request";
            break;
        case ICMP_ECHOREPLY:
            echo_type = "echo reply  ";
            break;
        default:
            return;
    }

    ip_string(ip_str, src_ip);
    DispatchLock();
    sprintf(icmp_message.buf[icmp_message.in], "icmp message: %s from %s", echo_type, ip_str);
    icmp_message.in++;
    icmp_message.number++;
    icmp_message.in%=ICMP_MESSAGE_BUF_NUM;
    DispatchUnlock();    
}

/*----------------------------------------------------------------------------------------------
 * 函数:    get_icmp_message()
 *
 * 说明:    读取一个icmp消息
 *
 * 输入:    1) dst_message_buf  destination buffer
 *
 * 返回:    dst_message_buf/NULL for no messages
**--------------------------------------------------------------------------------------------*/
INT08S * apical get_icmp_message(INT08S * dst_message_buf)
{
    FamesAssert(dst_message_buf);

    if(!dst_message_buf){
        return NULL;
    }
    
    DispatchLock();
    if(icmp_message.number <= 0){
        DispatchUnlock();
        return NULL;
    }
    STRCPY(dst_message_buf, icmp_message.buf[icmp_message.out]);
    icmp_message.out++;
    icmp_message.number--;
    icmp_message.out%=ICMP_MESSAGE_BUF_NUM;
    DispatchUnlock();
    
    return dst_message_buf;    
}

/*------------------------------------------------------------------------------------
 * 任务: ICMP报文处理
**----------------------------------------------------------------------------------*/
void __daemon TaskICMP(void * data)
{
    struct iphdr * ip_hdr;
    struct icmphdr * hdr;
    INT16U total_len, icmp_len;

    FamesAssert(data);

    if(!data){
        task_return;
    }

    do {
        if(icmp_message_received==0)goto i_will_take_a_nap;
        icmp_message_received=0;

        ip_hdr = (struct iphdr *)data; /*lint !e826 */
        hdr    = (struct icmphdr *)((INT08S *)data + (ip_hdr->ihl<<2)); /*lint !e826 !e613*/

        total_len = INT16XCHG(ip_hdr->tot_len); 

        icmp_len = total_len - (ip_hdr->ihl<<2); 

        if(icmp_len > ICMP_DATAGRAM_MAX_LEN){
            icmp_len = ICMP_DATAGRAM_MAX_LEN;
        }

        if(!make_icmp_checksum((INT08U *)hdr, icmp_len, MAKE_ICMP_CSUM_OPT_CHECK)){
            goto i_will_take_a_nap;
        }
        
        set_icmp_message(hdr->type, ip_hdr->saddr); 
        switch(hdr->type){
            case ICMP_ECHO:
                send_echo(ICMP_ECHOREPLY, 0, hdr->un.echo.id, hdr->un.echo.sequence, 
                          ip_hdr->saddr, 
                          (INT08S *)((INT08S *)hdr + sizeof(struct icmphdr)), 
                          icmp_len - sizeof(struct icmphdr)); 
                break;
            case ICMP_ECHOREPLY:
                break;
            default:
                break;
        } 
        i_will_take_a_nap:
        if(icmp_message_received==0){
            TaskSleep(100L);
        }
        
    }while(1);/*lint !e506 */
}

/*
*本文件结束: icmp.c ============================================================================
*/


