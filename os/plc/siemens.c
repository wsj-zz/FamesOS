/***********************************************************************************************
 * 文件:    siemens.c
 *
 * 描述:    西门子PLC通讯服务
 *
 * 作者:    Jun
***********************************************************************************************/
#define  FAMES_PLC_SIEMENS_C
#include <includes.h>


#if FAMES_PLC_SVC_EN == 1 && FAMES_PLC_SIEMENS_EN == 1
/*----------------------------------------------------------------------------------------------
 * 函数:    SIEMENS_PLC_RTU_CRC()
 * 说明:    西门子PLC之RTU协议的CRC校验码
 * 输入:    字符串及其长度
 * 输出:    CRC校验码(循环冗余校验)
**--------------------------------------------------------------------------------------------*/
INT16U __internal SIEMENS_PLC_RTU_CRC(INT08U * buf, INT16S len)
{
    INT16S i,j;
    INT16U crc, flag;

    FamesAssert(buf);

    if(!buf)
        return 0;

    crc=0xffff;
    for(i=0;i<len;i++){
        crc^=buf[i];
        for(j=0;j<8;j++){
            flag=crc&0x0001;
            crc>>=1;
            if(flag){
                crc&=0x7fff;
                crc^=0xa001;
            }
        }
    }
    return crc;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    plc_common_isr_siemens()
 * 说明:    西门子PLC所用串口的中断服务程序
**--------------------------------------------------------------------------------------------*/
void __sysonly plc_common_isr_siemens(PLC * plc)
{
    INT08U iir, rx_char;
    INT16S __expect_RX_BYTES;   /* 西门子PLC须回应的字节数(RTU) */
    SERIAL_PORT * comx;

    comx = &(plc->rs232);

    __expect_RX_BYTES = (INT16S)(plc->private); /*lint !e507 :Size incompatibility*/
    
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
        if(iir&1){                            /* No Interrupt Pending              */
            return;
        }
        switch(iir&UART_IIR_INT){
            case UART_IIR_THR:                /* Interrupt from TX(THR empty)      */
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
            case UART_IIR_RX:                 /* Interrupt from RX(RDR ready/FIFO) */
            case UART_IIR_RXTO:
                while(UART_RX_READY(comx->base)){
                    rx_char=inportbyte(comx->base+UART_RDR);
                    if(comx->rx_num>=comx->rx_buf_len){
                        comx->rx_num=0;
                    }
                    comx->rx_buf[comx->rx_num]=rx_char;
                    comx->rx_num++;
                    if(comx->rx_num>=__expect_RX_BYTES){
                        comx->rx_num++;
                        comx->rx_buf[comx->rx_num]=0;
                        comx->rx_packet++;
                        EventSet(&comx->rx_event);
                    }
                }
                break;
            default:                          /* Other interrupt                   */
                break;
        }
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    plc_control_siemens()
 *
 * 描述:    西门子PLC读写控制
 *
 * 输入:    plc     需读写控制的PLC
 *          action  PLC控制命令
 *
 * 输出:    ok/fail
**----------------------------------------------------------------------------------*/
BOOL __sysonly plc_control_siemens(PLC * plc, PLC_ACTION * action)
{
/*lint --e{679} :Suspicious Truncation in arithmetic expression */
    INT16S nch, nch_rx, nch_rx_lrc, i, resend_times;
    INT08U bcca,bccb;
    INT16U bcc;
    SERIAL_PORT * comx;
    int    plc_cmd, num;
    INT16U addr;
    INT16S * tdata;
    INT32S * ldata;
    INT08S * plc_tx_buf;
    INT08S * plc_rx_buf;
    INT16S * __expect_RX_BYTES; /* 西门子PLC须回应的字节数(RTU) */

    comx = &(plc->rs232);
    num  = action->number;
    plc_cmd = action->cmd;
    addr = (INT16U)action->addr;/*lint !e507 :Size incompatibility*/
    tdata = (INT16S *)action->associated;
    ldata = (INT32S *)action->associated;
    plc_tx_buf = (INT08S *)plc->tx_buf;
    plc_rx_buf = (INT08S *)plc->rx_buf;
    __expect_RX_BYTES = (INT16S *)&(plc->private);/*lint !e740*/
    
    #if 0
    DispatchLock();
    printf("cmd=%d, addr=%s, data=%p, num=%d\n", 
            action->cmd, action->addr, action->associated, action->number);
    DispatchUnlock();
    #endif
    
    switch(plc_cmd){
        case SIEMENS_PLC_RUN:
        case SIEMENS_PLC_STOP:
            num=1;
            break;
        case SIEMENS_PLC_READ_VW:
        case SIEMENS_PLC_WRITE_VW:
            break;
        case SIEMENS_PLC_READ_VD:
        case SIEMENS_PLC_WRITE_VD:
            num*=2;
            break;
        default:
            return fail;
    }
    
    FamesAssert(num>0 && num<=64);
    
    if((num<=0)||(num>64)){
        return fail;
    }
    if(tdata==NULL){
        return fail;
    }

    if(comx->status!=COM_STATUS_OPEN){
        SetErrorCode(ERROR_PLC_COM_NOT_OPEN);
        return fail;
    }

    TaskSleep(1L);

    plc_tx_buf[0]=(INT08S)plc->station_no;
    nch=0;
    nch_rx=0;
    switch(plc_cmd){
        case SIEMENS_PLC_READ_VW:
        case SIEMENS_PLC_READ_VD:
            addr>>=1; /* addr/=2; */
            plc_tx_buf[1]=3;
            plc_tx_buf[2]=(INT08S)(((INT16U)addr>>8)&0xff);
            plc_tx_buf[3]=(INT08S)((        addr   )&0xff);
            plc_tx_buf[4]=0;
            plc_tx_buf[5]=(INT08S)num;
            bcc=SIEMENS_PLC_RTU_CRC((INT08U *)plc_tx_buf, 6);
            plc_tx_buf[6]=(INT08S)((bcc   )&0xff);
            plc_tx_buf[7]=(INT08S)((bcc>>8)&0xff);        
            nch=8;
            nch_rx=5+num*2;
            break;
        case SIEMENS_PLC_WRITE_VW:
        case SIEMENS_PLC_WRITE_VD:
            addr>>=1; /* addr/=2; */
            plc_tx_buf[1]=16;
            plc_tx_buf[2]=(INT08S)(((INT16U)addr>>8)&0xff);
            plc_tx_buf[3]=(INT08S)((        addr   )&0xff);
            plc_tx_buf[4]=0;
            plc_tx_buf[5]=(INT08S)num;
            plc_tx_buf[6]=(INT08S)(num*2);
            switch(plc_cmd){
                case SIEMENS_PLC_WRITE_VW:
                    for(i=0;i<num;i++){
                        plc_tx_buf[7+i*2]=(INT08S)(((INT16U)tdata[i]>>8)&0xff);
                        plc_tx_buf[8+i*2]=(INT08S)((        tdata[i]   )&0xff);
                    }
                    break;
                case SIEMENS_PLC_WRITE_VD:
                    for(i=0;i<num/2;i++){
                        plc_tx_buf[ 7+i*4]=(INT08S)(((INT32U)ldata[i]>>24)&0xffL);
                        plc_tx_buf[ 8+i*4]=(INT08S)(((INT32U)ldata[i]>>16)&0xffL);
                        plc_tx_buf[ 9+i*4]=(INT08S)(((INT32U)ldata[i]>>8 )&0xffL);
                        plc_tx_buf[10+i*4]=(INT08S)((        ldata[i]    )&0xffL);
                    }
                    break;
                default:
                    break;
            }
            bcc=SIEMENS_PLC_RTU_CRC((INT08U *)plc_tx_buf, 7+num*2);
            plc_tx_buf[7+num*2]=(INT08S)((bcc   )&0xff);
            plc_tx_buf[8+num*2]=(INT08S)((bcc>>8)&0xff);        
            nch=9+num*2;
            nch_rx=8;
            break;
        case SIEMENS_PLC_RUN:
        case SIEMENS_PLC_STOP:
            return ok;
        default:
            return fail;
    }
    if(nch<5||nch_rx<5){
        return fail;
    }
    resend_times=0;
    plc_resend:
    if(resend_times>=PLC_RETRY_ERR){
        return fail;
    }
    resend_times++;
    i=0;
    do{
        if(i>=PLC_RETRY_RX_TO){               /* try 3 times if fail(timeout)       */
            SetErrorCode(ERROR_PLC_TIMEOUT_RX);
            return fail;
        }
        (*__expect_RX_BYTES) = nch_rx;
        MEMSET(plc_rx_buf, 0, PLC_BUF_SIZE-20);
        PreRxSerialPort(comx, (INT08U *)plc_rx_buf, PLC_BUF_SIZE-20);    
        SendSerialPort(comx, (INT08U *)plc_tx_buf, nch);
        i++;
    } while(!TaskWait(&(comx->rx_event), 25L+(INT32U)(INT32S)nch_rx*2L)); /* 按最小4800为准 */
                                              /* 到这里,已接收到PLC的响应           */
    TaskSleep(2L);

    if(MEMCMP(plc_rx_buf, plc_tx_buf, 2)){
        if(resend_times>=PLC_RETRY_ERR){
            SetErrorCode(ERROR_PLC_RX_RESPONSE);
        }
        goto plc_resend;
    }
    
    nch_rx_lrc=nch_rx-2;
    bcc=SIEMENS_PLC_RTU_CRC((INT08U *)plc_rx_buf, nch_rx_lrc);
    bcca=(bcc&0xff);
    bccb=(bcc>>8);
    if( bcca!=plc_rx_buf[nch_rx_lrc] || bccb!=plc_rx_buf[nch_rx_lrc+1] ){
        if(resend_times>=PLC_RETRY_ERR){
            SetErrorCode(ERROR_PLC_RX_LRC);
        }
        goto plc_resend;       
    }

    switch(plc_cmd){
        case SIEMENS_PLC_READ_VW:
        case SIEMENS_PLC_READ_VD:
            if(plc_rx_buf[2]!=num*2){
                if(resend_times>=PLC_RETRY_ERR){
                    SetErrorCode(ERROR_PLC_RX_RESPONSE);
                }
                goto plc_resend;
            }
            switch(plc_cmd){
                case SIEMENS_PLC_READ_VW:
                    for(i=0; i<num; i++){    
                        tdata[i] = (INT16S)(plc_rx_buf[3+i*2]);
                        tdata[i]<<=8; /*lint !e701*/
                        tdata[i]|= (INT16S)(plc_rx_buf[4+i*2]);
                    }
                    break;
                case SIEMENS_PLC_READ_VD:
                    for(i=0; i<num/2; i++){    
                        ldata[i]  = (INT32S)(plc_rx_buf[3+i*4]); 
                        ldata[i]<<=8; /*lint !e703*/
                        ldata[i] |= (INT32S)(plc_rx_buf[4+i*4]);
                        ldata[i]<<=8; /*lint !e703*/
                        ldata[i] |= (INT32S)(plc_rx_buf[5+i*4]);
                        ldata[i]<<=8; /*lint !e703*/
                        ldata[i] |= (INT32S)(plc_rx_buf[6+i*4]);
                    }
                    break;
                default:
                    break;
            }
            break;
        case SIEMENS_PLC_WRITE_VW:
        case SIEMENS_PLC_WRITE_VD:
            if(MEMCMP(&plc_rx_buf[2], &plc_tx_buf[2], 4)){
                if(resend_times>=PLC_RETRY_ERR){
                    SetErrorCode(ERROR_PLC_RX_RESPONSE);
                }
                goto plc_resend;
            }
            break;
        default:
            break;
    }
    
    return ok;
}

#endif /* FAMES_PLC_SVC_EN == 1 && FAMES_PLC_SIEMENS_EN == 1 */

/*==============================================================================================
 * 
 * 本文件结束: siemens.c
 * 
**============================================================================================*/

