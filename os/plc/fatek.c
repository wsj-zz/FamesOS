/*************************************************************************************
 * 文件: fatek.c
 *
 * 描述: 永宏PLC服务函数定义
 *
 * 作者: Jun
 *
 * 注意: 此文件必需以大模式编译(如: TCC -1 -B -ml ###.C)
*************************************************************************************/
#define  FAMES_PLC_FATEK_C
#include <includes.h>


#if FAMES_PLC_SVC_EN == 1 && FAMES_PLC_FATEK_EN == 1
/*------------------------------------------------------------------------------------
 * 
 *       内部函数
 *
**----------------------------------------------------------------------------------*/
void    __internal INT_TO_HEX_STR(INT08S *dest, INT16S src); 
INT16S  __internal HEX_STR_TO_INT(INT08S *src_s);

#if 0
/*------------------------------------------------------------------------------------
 * 函数:    LONG_TO_HEX_STR()
 * 说明:    长整数转化为16进制字符串(8位)
 * 输入:    dest_s  目标字符串
 *          src_l   长整数
 * 输出:    无
 * 注意:    目标字符串(dest_s)必须有效且长度不能小于8位
**----------------------------------------------------------------------------------*/
void __internal LONG_TO_HEX_STR(INT08S * dest_s, INT32S src_l)
{
/*lint --e{613}*/
    FamesAssert(dest_s);
    dest_s[0]=HEXCHAR[((INT32U)src_l>>28)&0xfL];
    dest_s[1]=HEXCHAR[((INT32U)src_l>>24)&0xfL];
    dest_s[2]=HEXCHAR[((INT32U)src_l>>20)&0xfL];
    dest_s[3]=HEXCHAR[((INT32U)src_l>>16)&0xfL];
    dest_s[4]=HEXCHAR[((INT32U)src_l>>12)&0xfL];
    dest_s[5]=HEXCHAR[((INT32U)src_l>> 8)&0xfL];
    dest_s[6]=HEXCHAR[((INT32U)src_l>> 4)&0xfL];
    dest_s[7]=HEXCHAR[((INT32U)src_l    )&0xfL];
}
#endif

/*------------------------------------------------------------------------------------
 * 函数:    INT_TO_HEX_STR()
 * 说明:    整数转化为16进制字符串(4位)
 * 输入:    dest_s  目标字符串
 *          src_i   整数
 * 输出:    无
 * 注意:    目标字符串(dest_s)必须有效且长度不能小于4位
**----------------------------------------------------------------------------------*/
void __internal INT_TO_HEX_STR(INT08S * dest_s, INT16S src_i)
{
/*lint --e{613}*/
    FamesAssert(dest_s);
    dest_s[0]=HEXCHAR[((INT16U)src_i>>12)&0xf];
    dest_s[1]=HEXCHAR[((INT16U)src_i>> 8)&0xf];
    dest_s[2]=HEXCHAR[((INT16U)src_i>> 4)&0xf];
    dest_s[3]=HEXCHAR[((INT16U)src_i    )&0xf];
}

/*------------------------------------------------------------------------------------
 * 函数:    HEX_STR_TO_INT()
 * 说明:    16进制字符串(4位)转化为整数
 * 输入:    src_s  源字符串
 * 输出:    对应的整数
 * 注意:    源字符串(src_s)必须有效且长度不能小于4位
 *          这里共有两个实现,第一个速度快但容错性差,第二个容错性好但速度慢,
 *          这里用第一个.
**----------------------------------------------------------------------------------*/
#if 1
INT16S __internal HEX_STR_TO_INT(INT08S * src_s)
{
/*lint --e{613,701}*/
    #define DIFF_TO_HEX(c) (c>0x39?0x37:0x30) 
    INT16S  retval, t;
    FamesAssert(src_s);
    retval=0;
    t=(INT16S)src_s[0];
    t-=DIFF_TO_HEX(t);
    retval|=(t<<12);
    t=(INT16S)src_s[1];
    t-=DIFF_TO_HEX(t);
    retval|=(t<<8);
    t=(INT16S)src_s[2];
    t-=DIFF_TO_HEX(t);
    retval|=(t<<4);
    t=(INT16S)src_s[3];
    t-=DIFF_TO_HEX(t);
    retval|=(t);
    return retval;
    #undef DIFF_TO_HEX
}
#else
INT16S __internal HEX_STR_TO_INT(INT08S * src_s)
{
/*lint --e{613,701}*/
    INT16S i, retval;
    INT08S t;
    FamesAssert(src_s);
    retval=0;
    for(i=0;i<4;i++){
        retval<<=4;
        t=src_s[i];
        if(t>='0'&&t<='9'){
            retval+=(t-0x30);
        } else if(t>='a'&&t<='f'){
            retval+=(t-'a'+10);
        } else if(t>='A'&&t<='F'){
            retval+=(t-'A'+10);
        } else {
            retval+=0;
        }
    }
    return retval;
}
#endif

/*------------------------------------------------------------------------------------
 * 函数:    plc_common_isr_fatek()
 *
 * 描述:    永宏PLC所用串口的中断服务程序
**----------------------------------------------------------------------------------*/
void __sysonly plc_common_isr_fatek(PLC * plc)
{
    INT08U iir, rx_char;
    SERIAL_PORT * comx;

    comx = &(plc->rs232);
        
    if(comx->status!=COM_STATUS_OPEN){
        for(;;){
            iir=inportbyte(comx->base+UART_IIR);
            if(iir&0x1)break;
            switch(iir&UART_IIR_INT){
                case UART_IIR_RX:
                case UART_IIR_RXTO:
                    while(UART_RX_READY(comx->base)){
                        inportbyte(comx->base+UART_RDR);
                    }
                    break;
                default:
                    break;
            }
        }
        return;
    }
    
    for(;;){
        iir=inportbyte(comx->base+UART_IIR);
        #if 0
        textprintstr(1, 7, "plc_common_isr_fatek: UART_IIR_INT", 0);
        if(iir&1); else textprinthex16(1, 8, iir, 0);
        textprintdec16(6, 8, comx->rx_num, 0, 0);
        textprintdec16(12, 8, comx->rx_buf_len, 0, 0);
        #endif
        if(iir&1){                            /* No Interrupt Pending               */
            return;
        }
        switch(iir&UART_IIR_INT){
            case UART_IIR_THR:                /* Interrupt from TX(THR empty)       */
                while(UART_TX_READY(comx->base)){
                    if(comx->tx_num<comx->tx_buf_len){
                        outportbyte(comx->base+UART_THR, comx->tx_buf[comx->tx_num]);
                        comx->tx_num++;
                        if(comx->tx_num>=comx->tx_buf_len){
                            EventSet(&comx->tx_event);
                        }
                    } else {
                        break;
                    }
                }
                break;
            case UART_IIR_RX:                 /* Interrupt from RX(RDR ready/FIFO)  */
            case UART_IIR_RXTO:
                while(UART_RX_READY(comx->base)){
                    rx_char=inportbyte(comx->base+UART_RDR);
                    if(comx->rx_num>=comx->rx_buf_len){
                        comx->rx_num=0;
                    }
                    if(rx_char==FATEK_PROTOCOL_STX){
                        comx->rx_num=0;
                    }
                    comx->rx_buf[comx->rx_num]=rx_char;
                    if(rx_char==FATEK_PROTOCOL_ETX){
                        comx->rx_num++;
                        comx->rx_buf[comx->rx_num]=0;
                        comx->rx_packet++;
                        EventSet(&comx->rx_event);
                    }
                    comx->rx_num++;
                }
                break;
            default:                          /* Other interrupt                    */
                break;
        }
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    plc_control_fatek()
 *
 * 描述:    永宏PLC读写控制
 *
 * 输入:    plc     需读写控制的PLC
 *          action  PLC控制命令
 *
 * 输出:    ok/fail
**----------------------------------------------------------------------------------*/
BOOL __sysonly plc_control_fatek(PLC * plc, PLC_ACTION * action)
{
    INT16S nch, nch_rx, nch_rx_lrc, i, resend_times;
    INT08U lrc;
    INT08S station_no[4];
    SERIAL_PORT * comx;
    int      plc_cmd, num;
    INT08S * addr;
    INT16S * tdata;
    INT08S * plc_tx_buf;
    INT08S * plc_rx_buf;

    comx = &(plc->rs232);
    num  = action->number;
    plc_cmd = action->cmd;
    addr = action->addr;
    tdata = (INT16S *)action->associated;
    plc_tx_buf = (INT08S *)plc->tx_buf;
    plc_rx_buf = (INT08S *)plc->rx_buf;
    
    #if 0
    DispatchLock();
    printf("cmd=%d, addr=%s, data=%p, num=%d\n", 
            action->cmd, action->addr, action->associated, action->number);
    DispatchUnlock();
    #endif

    switch(plc_cmd){
        case FATEK_PLC_RUN:
        case FATEK_PLC_STOP:
            num=1;
        case FATEK_PLC_CONTROL_M: /*lint !e616 !e825*/
            break;
        default:
            if(addr==NULL){
                return fail;
            }
            if(tdata==NULL){
                return fail;
            }
            break;
    }
    #if 1
    if(plc_cmd == FATEK_PLC_READ_DR ||
       plc_cmd == FATEK_PLC_WRITE_DR){
       num*=2;
    }
    #else
    switch(plc_cmd){
        case FATEK_PLC_READ_DR:
        case FATEK_PLC_WRITE_DR:
            num*=2;
            break;
        case FATEK_PLC_CONTROL_M:
        case FATEK_PLC_READ_R:
        case FATEK_PLC_READ_M:
        case FATEK_PLC_WRITE_R:
        case FATEK_PLC_WRITE_M:
        case FATEK_PLC_RUN:
        case FATEK_PLC_STOP:
            break;
        default:
            return fail;
    }
    #endif
    
    FamesAssert(num>0 && num<=64);
    
    if((num<=0)||(num>64)){
        return fail;
    }

    if(comx->status!=COM_STATUS_OPEN){
        return fail;
    }

    INT08toSTR(station_no, (INT08S)plc->station_no, CHG_OPT_NUL);
    
    plc_tx_buf[0]=0x02;
    plc_tx_buf[1]=station_no[0];
    plc_tx_buf[2]=station_no[1];
    plc_tx_buf[3]='4';
    nch=0;
    nch_rx=0;
    switch(plc_cmd){
        case FATEK_PLC_READ_DR:
        case FATEK_PLC_READ_R:
            plc_tx_buf[4]='6';
            plc_tx_buf[5]=HEXCHAR[((INT16U)num>>4)&0xf];
            plc_tx_buf[6]=HEXCHAR[(        num   )&0xf];
            plc_tx_buf[7]=addr[0];
            plc_tx_buf[8]=addr[1];
            plc_tx_buf[9]=addr[2];
            plc_tx_buf[10]=addr[3];
            plc_tx_buf[11]=addr[4];
            plc_tx_buf[12]=addr[5];
            nch=13;
            nch_rx=9+num*4;
            break;
        case FATEK_PLC_WRITE_DR:
        case FATEK_PLC_WRITE_R:
            plc_tx_buf[4]='7';
            plc_tx_buf[5]=HEXCHAR[((INT16U)num>>4)&0xf];
            plc_tx_buf[6]=HEXCHAR[(        num   )&0xf];
            plc_tx_buf[7]=addr[0];
            plc_tx_buf[8]=addr[1];
            plc_tx_buf[9]=addr[2];
            plc_tx_buf[10]=addr[3];
            plc_tx_buf[11]=addr[4];
            plc_tx_buf[12]=addr[5];
            for(i=0; i<num; i++){
                INT_TO_HEX_STR(&plc_tx_buf[13+i*4], tdata[i]);/*lint !e679*/
            }
            nch=13+num*4;
            nch_rx=9;
            break;
        case FATEK_PLC_READ_M:
            plc_tx_buf[4]='4';
            plc_tx_buf[5]=HEXCHAR[((INT16U)num>>4)&0xf];
            plc_tx_buf[6]=HEXCHAR[(        num   )&0xf];
            plc_tx_buf[7]=addr[0];
            plc_tx_buf[8]=addr[1];
            plc_tx_buf[9]=addr[2];
            plc_tx_buf[10]=addr[3];
            plc_tx_buf[11]=addr[4];
            nch=12;
            nch_rx=9+num;
            break;
        case FATEK_PLC_WRITE_M:
            plc_tx_buf[4]='5';
            plc_tx_buf[5]=HEXCHAR[((INT16U)num>>4)&0xf];
            plc_tx_buf[6]=HEXCHAR[(        num   )&0xf];
            plc_tx_buf[7]=addr[0];
            plc_tx_buf[8]=addr[1];
            plc_tx_buf[9]=addr[2];
            plc_tx_buf[10]=addr[3];
            plc_tx_buf[11]=addr[4];
            for(i=0; i<num; i++){
                plc_tx_buf[12+i]=HEXCHAR[(tdata[i]==0)?0:1];/*lint !e679*/
            }
            nch=12+i;
            nch_rx=9;
            break;
        case FATEK_PLC_CONTROL_M:
            if(num<FATEK_PLC_M_DISABLE ||
               num>FATEK_PLC_M_RESET){
               return fail;
            }
            plc_tx_buf[4]='2';
            plc_tx_buf[5]=HEXCHAR[num];
            plc_tx_buf[6]=addr[0];
            plc_tx_buf[7]=addr[1];
            plc_tx_buf[8]=addr[2];
            plc_tx_buf[9]=addr[3];
            plc_tx_buf[10]=addr[4];
            nch=11;
            nch_rx=9;
            break;
        case FATEK_PLC_RUN:
            plc_tx_buf[4]='1';
            plc_tx_buf[5]='1';
            nch=6;
            nch_rx=9;
            break;
        case FATEK_PLC_STOP:
            plc_tx_buf[4]='1';
            plc_tx_buf[5]='0';
            nch=6;
            nch_rx=9;
            break;
        default:
            return fail;
    }
    if(nch<5||nch_rx<9){
        return fail;
    }
    lrc=(INT08U)0;
    for(i=0; i<nch; i++){
        lrc+=(INT08U)plc_tx_buf[i];
    }
    plc_tx_buf[nch]=HEXCHAR[(lrc>>4)&0xf];
    nch++;
    plc_tx_buf[nch]=HEXCHAR[(lrc   )&0xf];
    nch++;
    plc_tx_buf[nch]=0x03;
    nch++;
    plc_tx_buf[nch]=0;
    resend_times=0;
    plc_resend:
    if(resend_times>=PLC_RETRY_ERR){
        return fail;
    }
    resend_times++;
    i=0;
    do{
        if(i>=PLC_RETRY_RX_TO){ /* try 3 times if fail */
            SetErrorCode(ERROR_PLC_TIMEOUT_RX);
            return fail;
        }
        MEMSET(plc_rx_buf, 0, PLC_BUF_SIZE-20);
        PreRxSerialPort(comx, (INT08U *)plc_rx_buf, PLC_BUF_SIZE-20 );    
        SendSerialPort(comx, (INT08U *)plc_tx_buf, STRLEN(plc_tx_buf));
        i++;
    } while(!TaskWait(&(comx->rx_event), 20L+(INT32U)(INT32S)nch_rx*2L)); /* 按最小4800为准 */
                                    /* 到这里,已接收到PLC的响应 */
    /* TaskSleep(2L); */
    if(MEMCMP(plc_rx_buf, plc_tx_buf, 5)){
        if(resend_times>=PLC_RETRY_ERR){
            SetErrorCode(ERROR_PLC_RX_RESPONSE);
        }
        goto plc_resend;
    }
    if(plc_rx_buf[5]!='0'){
        if(resend_times>=PLC_RETRY_ERR){
            SetErrorCode(ERROR_PLC_RX_ERROR);
        }
        goto plc_resend;
    }
    nch_rx_lrc=nch_rx-3;
    lrc=(INT08U)0;
    for(i=0; i<nch_rx_lrc; i++){
        lrc+=(INT08U)plc_rx_buf[i];
    }
    if(plc_rx_buf[i]  !=HEXCHAR[(lrc>>4)&0xf] || 
       plc_rx_buf[i+1]!=HEXCHAR[(lrc&0xf)]){ /*lint !e679*/
        if(resend_times>=PLC_RETRY_ERR){
            SetErrorCode(ERROR_PLC_RX_LRC);
        }
        goto plc_resend;       
    }
    switch(plc_cmd){
        case FATEK_PLC_READ_R:
        case FATEK_PLC_READ_DR:
            for(i=0; i<num; i++){
                tdata[i]=HEX_STR_TO_INT(&plc_rx_buf[i*4+6]); /*lint !e679*/
            }
            break;
        case FATEK_PLC_READ_M:
            for(i=0; i<num; i++){
                tdata[i]=(INT16S)plc_rx_buf[i+6]-0x30; /*lint !e679*/
            }
            break;
        default:
            break;
    }
    return ok;
}

#endif /* FAMES_PLC_SVC_EN == 1 && FAMES_PLC_FATEK_EN == 1 */

/*====================================================================================
 * 
 * 本文件结束: fatek.c
 * 
**==================================================================================*/

