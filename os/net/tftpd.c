/***********************************************************************************************
** 文件: tftpd.c
** 说明: tftp-daemon
** 作者: Jun
***********************************************************************************************/
#define  FAMES_TFTPD_C
#include <includes.h>

#if TFTP_ENABLE == 1
/*----------------------------------------------------------------------------------------------
 * 常量定义
**--------------------------------------------------------------------------------------------*/
#define   TFTP_RX_BUF_SIZE       1024     /* 临时存放tftp数据包的缓冲大小 */
#define   TFTP_TX_BUF_SIZE       600      /* 临时存放tftp数据包的缓冲大小 */

#define   TFTP_MESSAGE_BUF_NUM   8        /* 存放tftp消息的缓冲个数       */

INT16S    tftp_packet_received = 0;       /* 接收到了tftp数据包           */

INT16S    tftp_packet_lock     = 0;       /* tftp报文锁                   */

INT16S    timeout_wait_ack     = 0;       /* 等待ACK超时                  */
INT16S    timeout_wait_rx      = 0;       /* 等待数据包超时               */

/*----------------------------------------------------------------------------------------------
 * 函数:    tftpd_dispatcher()
 *
 * 说明:    TFTP数据包处理(启动处理任务)
 *
 * 输入:    1) ip_pkt         IP数据包
 *          2) pkt_len        数据包大小
**--------------------------------------------------------------------------------------------*/
void apical tftpd_dispatcher(INT08S *ip_pkt, INT16S pkt_len)
{
    static HANDLE tftpd_task = InvalidHandle-1;
    static INT08S * rx_buf = NULL;
    void __daemon tftpd_daemon(void * data);

    if(!ip_pkt)return;

    if(!rx_buf)
        allocate_buffer(rx_buf, INT08S *, (INT32U)TFTP_RX_BUF_SIZE, return);

    if(pkt_len > TFTP_RX_BUF_SIZE){
        pkt_len = TFTP_RX_BUF_SIZE;
    }
    if(tftp_packet_lock){
        return;
    }    
    DispatchLock();
    MEMCPY(rx_buf, ip_pkt, pkt_len);
    tftp_packet_received = 1;
    DispatchUnlock();
    if(tftpd_task == InvalidHandle-1){
        tftpd_task=TaskCreate(tftpd_daemon, (void *)rx_buf, "tftpd", NULL, 2048, PRIO_NET, TASK_CREATE_OPT_NONE);
    } else {
        TaskAwake(tftpd_task);
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    tftpd_xmit()
 *
 * 说明:    发送tftp报文
 *
 * 输入:    1) target_ip     目标ip地址
 *          2) target_port   目标端口
 *          3) opcode        tftp操作码
 *          4) blkid         数据块号(或错误码)
 *          5) buf           数据(或错误信息)
 *          6) buf_len       数据长度
 *
 * 返回:    ok/fail
**--------------------------------------------------------------------------------------------*/
BOOL apical tftpd_xmit(INT32U target_ip, INT16U target_port, INT16U opcode, 
                       INT16U blkid, INT08S *buf, INT16U buf_len)
{
    static INT08S * tx_buf = NULL;
    struct tftphdr * hdr;

    if(!tx_buf)
        allocate_buffer(tx_buf, INT08S *, (INT32U)TFTP_TX_BUF_SIZE, return fail);

    hdr = (struct tftphdr *)tx_buf; /*lint !e826 */

    #if 0
    DispatchLock();
    printf("\ntftpd_xmit: target_ip=%08lX, port=%d, opcode=%d, blkid=%d, buf=%s, buf_len=%d",
        target_ip, target_port, opcode, blkid, buf, buf_len);
    DispatchUnlock();
    #endif

    switch(opcode){
        case TFTP_OP_RRQ:
        case TFTP_OP_WRQ:
            return fail;
        case TFTP_OP_DAT:
            FamesAssert(buf);
            hdr->opcode = INT16XCHG(opcode);
            hdr->un.id  = INT16XCHG(blkid);
            MEMCPY(&tx_buf[4], buf, (INT16S)buf_len);
            buf_len+=4;
            break;
        case TFTP_OP_ACK:
            hdr->opcode = INT16XCHG(opcode);
            hdr->un.id  = INT16XCHG(blkid);
            buf_len=4;
            break;
        case TFTP_OP_ERR:
            FamesAssert(buf);
            hdr->opcode = INT16XCHG(opcode);
            hdr->un.err.err  = INT16XCHG(blkid);
            MEMCPY(&tx_buf[4], buf, (INT16S)buf_len);
            buf_len+=4;
            break;
        default:
            return fail;
    }
    return send_udp(target_ip, target_port, UDP_P_TFTP, tx_buf, buf_len);

}

/*----------------------------------------------------------------------------------------------
 * 函数:    set_tftpd_message()
 *
 * 说明:    增加一个tftpd消息(用于显示)
 *
 * 输入:    1) type        icmp类型码
 *          2) src_ip      来源ip
 *
 * 返回:    none
**--------------------------------------------------------------------------------------------*/
typedef struct __tftpd_message_block {
    INT16S  in, out, number, lost;
    INT08S  buf[TFTP_MESSAGE_BUF_NUM][128];
}tftpd_message_block;
tftpd_message_block tftpd_message={0,0,0,0, }; /*lint !e785 */

void apical set_tftpd_message(INT16U opcode, INT16U blkid, 
                              INT08S *filename, 
                              INT08S *mode, 
                              INT32U src_ip)
{
    INT08S   ip_str[20];
    INT08S   t_mode[32] = "<NULL>";
    INT08S   t_flnm[128]= "<NULL>";

    if(tftpd_message.number >= TFTP_MESSAGE_BUF_NUM){
        tftpd_message.lost++;
        return;
    }

    ip_string(ip_str, src_ip);

    if(filename)
        STRCPY(t_flnm, filename);
    if(mode)
        STRCPY(t_mode, mode);

    switch(opcode)
    {
        case TFTP_OP_RRQ:
            DispatchLock();
            sprintf(tftpd_message.buf[tftpd_message.in], 
                 "tftp message: rrq: %s(%s) from %s", t_flnm, t_mode, ip_str);
            tftpd_message.in++;
            tftpd_message.number++;
            tftpd_message.in%=TFTP_MESSAGE_BUF_NUM;
            DispatchUnlock();
            break;
        case TFTP_OP_WRQ:
            DispatchLock();
            sprintf(tftpd_message.buf[tftpd_message.in], 
                 "tftp message: wrq: %s(%s) from %s", t_flnm, t_mode, ip_str);
            tftpd_message.in++;
            tftpd_message.number++;
            tftpd_message.in%=TFTP_MESSAGE_BUF_NUM;
            DispatchUnlock();
            break;
        case TFTP_OP_DAT:
            DispatchLock();
            sprintf(tftpd_message.buf[tftpd_message.in], 
                 "tftp message: dat: %-6d", blkid);
            tftpd_message.in++;
            tftpd_message.number++;
            tftpd_message.in%=TFTP_MESSAGE_BUF_NUM;
            DispatchUnlock();
            break;
        case TFTP_OP_ACK:
            DispatchLock();
            sprintf(tftpd_message.buf[tftpd_message.in], 
                 "tftp message: ack: %-6d", blkid);
            tftpd_message.in++;
            tftpd_message.number++;
            tftpd_message.in%=TFTP_MESSAGE_BUF_NUM;
            DispatchUnlock();
            break;
        case TFTP_OP_ERR:
            DispatchLock();
            sprintf(tftpd_message.buf[tftpd_message.in], 
                 "tftp message: err: %s(%d) from %s", t_flnm, blkid, ip_str);
            tftpd_message.in++;
            tftpd_message.number++;
            tftpd_message.in%=TFTP_MESSAGE_BUF_NUM;
            DispatchUnlock();
            break;
        default:
            DispatchLock();
            sprintf(tftpd_message.buf[tftpd_message.in], 
                 "tftp message: %s from %s", t_flnm, ip_str);
            tftpd_message.in++;
            tftpd_message.number++;
            tftpd_message.in%=TFTP_MESSAGE_BUF_NUM;
            DispatchUnlock();
            break;
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    get_tftpd_message()
 *
 * 说明:    读取一个tftpd消息
 *
 * 输入:    1) dst_message_buf  destination buffer
 *
 * 返回:    dst_message_buf/NULL for no messages
**--------------------------------------------------------------------------------------------*/
INT08S * apical get_tftpd_message(INT08S * dst_message_buf)
{
    static INT08S t_s[130];
    FamesAssert(dst_message_buf);

    if(!dst_message_buf){
        return NULL;
    }
    
    DispatchLock();
    if(tftpd_message.number <= 0){
        DispatchUnlock();
        return NULL;
    }
    STRCPY(t_s, tftpd_message.buf[tftpd_message.out]);
    STRCPY(dst_message_buf, t_s);
    tftpd_message.out++;
    tftpd_message.number--;
    tftpd_message.out%=TFTP_MESSAGE_BUF_NUM;
    DispatchUnlock();
    
    return dst_message_buf;    
}

/*----------------------------------------------------------------------------------------------
 * 函数:    tftpd_daemon()
 *
 * 描述:    tftp服务器的后台任务, 堆栈大小建议为2048
**--------------------------------------------------------------------------------------------*/
void __daemon tftpd_daemon(void * data)
{
    struct iphdr * ip_hdr;
    struct udphdr * u_hdr;
    struct tftphdr * hdr;
    INT16U tftp_len;
    INT16U binary_mode = 0;
    FILE * volatile fp=NULL;
    INT16U last_blk=0, bytes=0, blksn=0, blksn_old=0;
    INT32U ip_connect = 0L;
    INT16U remote_port = 0;
    INT16S timeout_resend = 0;
    INT08S filename[256], mode[10], buf[512];
    INT08S wbuf[512];
    void __internal __tx_timeout_for_tftpd(void *, INT16S);
    void __internal __rx_timeout_for_tftpd(void *, INT16S);

    FamesAssert(data);

    TimerSet(TimerTftpdTX, 1000L, TIMER_TYPE_AUTO, __tx_timeout_for_tftpd, (void *)CurrentTask);
    TimerStop(TimerTftpdTX);
    TimerSet(TimerTftpdRX, 30000L, TIMER_TYPE_AUTO, __rx_timeout_for_tftpd, (void *)CurrentTask);
    TimerStop(TimerTftpdRX);
    /*lint --e{613}*/
     
    do {
        if(timeout_wait_ack){ /* 超时重传 */
            timeout_wait_ack = 0;
            set_tftpd_message(TFTP_OP_XXX, 0, "wait ack timeout!", NULL, ip_connect);
            timeout_resend++;
            if(timeout_resend > 10){ /* 这么久还没收到,那就不管了 */
                TimerStop(TimerTftpdTX);
                timeout_wait_ack = 0;
                binary_mode = 0;
                if(fp){
                    DispatchLock();
                    fclose(fp);
                    fp=NULL;
                    DispatchUnlock();
                }
                ip_connect = 0L;
                timeout_resend = 0;
                goto i_will_take_a_nap;
            }
            if(!fp || ip_connect==0L){
                TimerStop(TimerTftpdTX);
                timeout_wait_ack = 0;
                goto i_will_take_a_nap;
            }
            tftpd_xmit(ip_connect, remote_port, TFTP_OP_DAT, blksn, buf, bytes);
            TimerReStart(TimerTftpdTX);
            goto i_will_take_a_nap;
        }

        if(timeout_wait_rx){   /* 等待客户端数据包超时 */
            TimerStop(TimerTftpdRX);
            timeout_wait_rx = 0;
            set_tftpd_message(TFTP_OP_XXX, 0, "wait client timeout(30sec)!", NULL, ip_connect);
            binary_mode = 0;
            if(fp){
                DispatchLock();
                fclose(fp);
                fp=NULL;
                DispatchUnlock();
            }
            ip_connect = 0L;                     
        }    
        
        if(tftp_packet_received==0)goto i_will_take_a_nap;
        tftp_packet_received=0;

        tftp_packet_lock = 1;
        
        ip_hdr = (struct iphdr *)data; /*lint !e826 */
        u_hdr  = (struct udphdr *)((INT08S *)data + (ip_hdr->ihl<<2)); /*lint !e826 !e613 */
        hdr    = (struct tftphdr *)((INT08S *)u_hdr+8); /*lint !e826 */

        tftp_len = INT16XCHG(u_hdr->len)-8; /* TFTP报文长度 */

        if(ip_hdr->daddr == get_bcast_ip()){ /*lint !e613 */
            goto i_will_take_a_nap;
        }

        if(ip_connect!=0L){
            if(ip_connect != ip_hdr->saddr){ /*lint !e613*/
                set_tftpd_message(TFTP_OP_XXX, 0, "not the client ip packet", NULL, ip_hdr->saddr); /*lint !e613*/
                tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ERR, 0, 
                                "I am busy, please wait a moment!!!", 35); /*lint !e613*/
                goto i_will_take_a_nap;
            }
        }

        TimerReStart(TimerTftpdRX); /* 接收到了客户端的数据包, 则重设定时器 */

        switch(INT16XCHG(hdr->opcode)){
            case TFTP_OP_RRQ:
                STRCPY(filename, hdr->un.filename);
                STRCPY(mode,    (hdr->un.filename+STRLEN(filename)+1));
                set_tftpd_message(TFTP_OP_RRQ, 0, filename, mode, ip_hdr->saddr); /*lint !e613*/
                if(ip_connect!=0){
                    TimerStop(TimerTftpdTX);
                    timeout_wait_ack = 0;
                    tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ERR, 1, 
                                    "rrq is not expected!!!", 23);/*lint !e613*/
                    binary_mode = 0;
                    if(fp){
                        DispatchLock();
                        fclose(fp);
                        fp=NULL;
                        DispatchUnlock();
                    }
                    ip_connect = 0L;
                    break;
                }
                ip_connect = ip_hdr->saddr; /*lint !e613*/
                remote_port = INT16XCHG(u_hdr->source);
                if(mode[0]=='o'||mode[0]=='O'){ /* octet */
                    binary_mode = 1;
                } else {
                    binary_mode = 0;
                }
                DispatchLock();
                fp=fopen(filename, binary_mode?("rb"):("rt"));
                DispatchUnlock();
                if(!fp){
                    tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ERR, 2, 
                                    "file not found!!!", 18); /*lint !e613*/
                    ip_connect = 0L;
                    break;
                }
                DispatchLock();
                bytes=fread(buf, 1, 512, fp);
                DispatchUnlock();
                if(bytes!=512) last_blk = 1;
                else           last_blk = 0;
                blksn = 1;
                tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_DAT, 
                                 blksn, buf, bytes); /*lint !e613*/
                TimerReStart(TimerTftpdTX);
                timeout_resend = 0;
                break;
            case TFTP_OP_WRQ:
                STRCPY(filename, hdr->un.filename);
                STRCPY(mode,    (hdr->un.filename+STRLEN(filename)+1));
                set_tftpd_message(TFTP_OP_WRQ, 0, filename, mode, ip_hdr->saddr); /*lint !e613*/
                if(ip_connect!=0){
                    tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ERR, 1, 
                                    "wrq is not expected!!!", 23); /*lint !e613*/
                    binary_mode = 0;
                    if(fp){
                        DispatchLock();
                        fclose(fp);
                        fp=NULL;
                        DispatchUnlock();
                    }
                    ip_connect = 0L;
                    break;
                }
                ip_connect = ip_hdr->saddr;
                remote_port = INT16XCHG(u_hdr->source);
                if(mode[0]=='o'||mode[0]=='O'){ /* octet */
                    binary_mode = 1;
                } else {
                    binary_mode = 0;
                }
                DispatchLock(); 
                fp = fopen(filename, binary_mode?"wb":"wt");
                DispatchUnlock();
                if(!fp){
                    tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ERR, 2, "file can not write!!!", 22);
                    ip_connect = 0L;
                    break;
                }
                blksn = 0;
                last_blk = 0;
                blksn_old  = 0;
                tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ACK, blksn, NULL, 0);
                break;
            case TFTP_OP_DAT:
                set_tftpd_message(TFTP_OP_DAT, INT16XCHG(hdr->un.id), filename, mode, ip_hdr->saddr);
                blksn = INT16XCHG(hdr->un.id);
                if(blksn_old !=0 && blksn_old==blksn){
                    tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ACK, blksn, NULL, 0);
                    break;
                }
                if(blksn - blksn_old > 1){
                    tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ACK, blksn_old, NULL, 0);
                    break;
                }
                blksn_old=blksn;
                if(!fp || ip_connect==0L){
                    tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ERR, 3, "file not open!!!", 17);
                    break;
                }
                bytes = tftp_len-4;
                DispatchLock();
                MEMCPY(wbuf, ((INT08S *)hdr+4), (INT16S)bytes);
                fwrite(wbuf, bytes, 1, fp);
                if(bytes!=512){
                    fclose(fp);
                    fp=NULL;
                    ip_connect=0L;
                    set_tftpd_message(TFTP_OP_XXX, 0, "=== receive end ===", 0, ip_hdr->saddr);                    
                }
                DispatchUnlock();
                tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ACK, blksn, NULL, 0);
                break;
            case TFTP_OP_ACK:
                timeout_resend = 0;
                set_tftpd_message(TFTP_OP_ACK, INT16XCHG(hdr->un.id), filename, mode, ip_hdr->saddr);
                if(!fp || ip_connect==0L){
                    tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_ERR, 3, "file not open!!!", 17);
                    TimerStop(TimerTftpdTX);
                    timeout_wait_ack = 0;
                    break;
                }
                if(last_blk){
                    TimerStop(TimerTftpdTX);
                    timeout_wait_ack = 0;
                    DispatchLock();
                    fclose(fp);
                    fp=NULL;
                    DispatchUnlock();
                    ip_connect = 0L;
                    set_tftpd_message(TFTP_OP_XXX, 0, "=== transmit end ===", 0, ip_hdr->saddr);                    
                    break;
                }
                blksn++;
                DispatchLock();
                bytes=fread(buf, 1, 512, fp);
                DispatchUnlock();
                if(bytes!=512) last_blk = 1;
                else           last_blk = 0;
                tftpd_xmit(ip_hdr->saddr, INT16XCHG(u_hdr->source), TFTP_OP_DAT, blksn, buf, bytes);
                TimerReStart(TimerTftpdTX);
                break;
            case TFTP_OP_ERR:
                TimerStop(TimerTftpdTX);
                timeout_wait_ack = 0;
                binary_mode = 0;
                if(fp){
                    DispatchLock();
                    fclose(fp);
                    fp=NULL;
                    DispatchUnlock();
                }
                ip_connect = 0L;
                set_tftpd_message(TFTP_OP_ERR, hdr->un.err.err, hdr->un.err.err_msg, NULL, ip_hdr->saddr);
                break;
            default:
                break;
        }
        
        if(ip_connect==0L){               /* 当前没有连接, 定时器应该关闭         */
            TimerStop(TimerTftpdRX);
        }
        
        i_will_take_a_nap:
        tftp_packet_lock = 0;
        if(tftp_packet_received==0){
            TaskSleep(1000L);
        }
        
    }while(1);/*lint !e506 */

}

/*----------------------------------------------------------------------------------------------
 * 函数:    tx_timeout_for_tftpd()
 *
 * 说明:    tftp服务器的DPC例程, 用于tftp超时重传机制的实现
**--------------------------------------------------------------------------------------------*/
void __internal __tx_timeout_for_tftpd(void * data, INT16S nr)
{
    nr = nr;

    timeout_wait_ack = 1;
    TaskAwake((HANDLE)data); /*lint !e507*/
}

/*----------------------------------------------------------------------------------------------
 * 函数:    rx_timeout_for_tftpd()
 *
 * 说明:    tftp服务器的DPC例程, 用于接收数据包时超时的处理
**--------------------------------------------------------------------------------------------*/
void __internal __rx_timeout_for_tftpd(void * data, INT16S nr)
{
    nr = nr;

    timeout_wait_rx = 1;
    TaskAwake((HANDLE)data); /*lint !e507*/
}


#endif /* TFTP_ENABLE == 1 */
/*==============================================================================================
 * 
 * 本文件结束: tftpd.c
 * 
**============================================================================================*/

