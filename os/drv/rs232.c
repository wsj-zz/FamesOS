/*************************************************************************************
** �ļ�: rs232.c
** ˵��: FamesOS�Ĵ�������(RS232)
** ����: Jun
*************************************************************************************/
#define  FAMES_RS232_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 * ����:    InitSerialPort()
 * ˵��:    ���ڶ����ʼ��
 * ����:    COMx ���ڶ���ָ��
 * ���:    ok/fail
 * ע��:    
**----------------------------------------------------------------------------------*/
BOOL apical InitSerialPort(SERIAL_PORT * COMx)
{
    CRITICAL_DEFINE()
    
    if(COMx==NULL){
        return fail;
    }
    #if 0
    if(COMx->status==COM_STATUS_INIT){        /* �ѳ�ʼ��,����Ҫ�ٳ�ʼ����          */
        return fail;
    }
    #endif
    if(COMx->status==COM_STATUS_OPEN){        /* ��״̬,�������ʼ��              */
        return fail;
    }
    COMx->base=0;
    COMx->baudrate=0L;
    COMx->data=0;
    COMx->stop=0;
    COMx->parity=COM_PARITY_NONE;
    COMx->irq=-1;
    COMx->isr=NULL;
    COMx->old=NULL;
    COMx->interrupt_flag=COM_INT_NONE;
    COMx->fifo_control=COM_FIFO_NONE;
    COMx->status=COM_STATUS_INIT;
    SERIAL_PORT_RESET_BUF_RXTX(COMx);
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    OpenSerialPort()
 * ˵��:    �򿪴���
 * ����:    COMx ���ڶ���ָ��
 * ���:    ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical OpenSerialPort(SERIAL_PORT * COMx)
{
    INT08U com_lcr;                           /* UART.LCR                           */
    INT08U com_ier;                           /* UART.IER                           */
    INT08U com_fcr;                           /* UART.FCR                           */
    INT32S com_brd;                           /* UART.BRD                           */
    CRITICAL_DEFINE()
    
    if(COMx==NULL){
        return fail;
    }
    if(COMx->status==COM_STATUS_OPEN){        /* �Ѵ�,����Ҫ�ٴ�                */
        return fail;
    }
    if(COMx->status==COM_STATUS_INIT){        /* �ѳ�ʼ��,��û�����ò���,������� */
        return fail;
    }
    if(COMx->base<=0){                        /* ����ַ��Ч,���ܴ�                */
        return fail;
    }
    if(COMx->baudrate < 10L || COMx->baudrate > UART_BR_MAX){ 
        return fail;                          /* ��������Ч,���ܴ�                */
    }
    for(;;){                                  /* ��������ʲ�֧��,����һ����ӽ���  */
        if(UART_BR_MAX%COMx->baudrate)COMx->baudrate--;
        else break;
    }
    
    com_lcr=(INT08U)0;
    com_brd=UART_BR_MAX/COMx->baudrate;    
    switch(COMx->parity){                     /* �趨У��λ                         */
        case COM_PARITY_NONE:
            break;
        case COM_PARITY_EVEN:
            com_lcr|=0x18;
            break;
        case COM_PARITY_ODD:
            com_lcr|=0x08;
            break;
        default:
            return fail;                      /* У��λ�趨����                     */ 
    }
    switch(COMx->stop){                       /* �趨ֹͣλ                         */
        case COM_STOP_1:
            break;
        case COM_STOP_2:
            com_lcr|=0x04;
            break;
        default:
            return fail;                      /* ֹͣλ�趨����                     */ 
    }
    switch(COMx->data){                       /* �趨����λ                         */
        case COM_DATA_5:
            break;
        case COM_DATA_6:
            com_lcr|=0x1;
            break;
        case COM_DATA_7:
            com_lcr|=0x2;
            break;
        case COM_DATA_8:
            com_lcr|=0x3;
            break;
        default:
            return fail;                      /* ����λ�趨����                     */ 
    }
    com_fcr=(INT08U)0x07;
    switch(COMx->fifo_control){               /* ����FIFO                           */
        case COM_FIFO_NONE:
            break;
        case COM_FIFO_TL_1:
            break;
        case COM_FIFO_TL_4:
            com_fcr|=0x40;
            break;
        case COM_FIFO_TL_8:
            com_fcr|=0x80;
            break;
        case COM_FIFO_TL_14:
            com_fcr|=0xC0;
            break;
        default:                              /* FIFO���ò���                       */
            return fail;
    }
    if(COMx->interrupt_flag != COM_INT_NONE){
        if(COMx->irq < IRQ_MIN_NO || COMx->irq > IRQ_MAX_NO){
            return fail;                      /* irq��֧��                          */
        }
        if(COMx->isr == NULL){
            return fail;                      /* isrΪ��                            */
        }
        com_ier=(INT08U)0;
        switch(COMx->interrupt_flag){
            case COM_INT_RX:
                com_ier |= 0x1;
                break;
            case COM_INT_TX:
                com_ier |= 0x2;
                break;
            case COM_INT_BOTH:
                com_ier |= 0x3;
                break;
            default:
                return fail;                  /* ��֧�ֵ��ж������趨               */
        }
        outportbyte(COMx->base+UART_IER, 0);
        outportbyte(COMx->base+UART_MCR, 0); 
        inportbyte(COMx->base+UART_IIR);
        inportbyte(COMx->base+UART_RDR);
        ENTER_CRITICAL();
        outportbyte(COMx->base+UART_IER, com_ier);
        outportbyte(COMx->base+UART_MCR, 0x08);  /* ��UART���жϹ���              */
        EXIT_CRITICAL();
        NOP();
        #if SHARED_IRQ == 1
        IrqConnect(COMx->irq, COMx->isr);
        #else
        COMx->old = FamesGetVect(GetIntNo(COMx->irq));
        FamesSetVect(GetIntNo(COMx->irq), COMx->isr);
        if(COMx->irq<8){
            outportbyte(0x21, inportbyte(0x21)&(~(INT08U)GetIrqMask(COMx->irq)));
        } else {
            outportbyte(0x21, inportbyte(0x21)&(~(INT08U)GetIrqMask(2)));
            outportbyte(0xA1, inportbyte(0xA1)&(~(INT08U)GetIrqMask(COMx->irq)));
        }
        #endif
    } else {
        inportbyte(COMx->base+UART_RDR);
        outportbyte(COMx->base+UART_IER, 0x00);  /* �ر�8250���жϹ���...          */
        outportbyte(COMx->base+UART_MCR, 0x00);  /* ...                            */
    }
    outportbyte(COMx->base+UART_LCR, 0x80);
    outportword(COMx->base,(INT16U)(INT16S)com_brd);
    outportbyte(COMx->base+UART_LCR, com_lcr);
    if(COMx->fifo_control!=COM_FIFO_NONE){
        outportbyte(COMx->base+UART_FCR, 0x01);  /* ����UART��FIFO                 */
        if(IS_UART16550(COMx->base)){
            outportbyte(COMx->base+UART_FCR, com_fcr);
        } else {
            COMx->fifo_control=COM_FIFO_NONE;
        }
    }
    
    /*SERIAL_PORT_RESET_BUF_RXTX(COMx);*/

    ENTER_CRITICAL();
    COMx->status=COM_STATUS_OPEN;
    EXIT_CRITICAL();
    DELAY(500L);
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    CloseSerialPort()
 * ˵��:    �رմ���
 * ����:    COMx ���ڶ���ָ��
 * ���:    ok/fail
 * �ʼ�:    ���������д��ʱ,���д���,��������,�������������Ȼ����ֶ�ջ��ƽ���
 *          ����,ԭ�����ڱ����,setvect()���ú�Ȼ����mov sp,bp��仰,
 *          ����������popf(��EXIT_CRITICAL())���ƻ��˶�ջ!
 *          �޸ķ���Ϊ��ENTER_CRITICAL()��EXIT_CRITICAL()�滻Ϊasm cli��asm sti.
 *          ��������,������������
 *
 *        �����, ����ʼ���е����� - jun, 9/3/2010
**----------------------------------------------------------------------------------*/
BOOL apical CloseSerialPort(SERIAL_PORT * COMx)
{
    CRITICAL_DEFINE()
    
    if(COMx==NULL){
        return fail;
    }
    if(COMx->status!=COM_STATUS_OPEN){        /* û�д�,����Ҫ�ر�                */
        return fail;
    }
    if(inportbyte(COMx->base+UART_LSR)&0x1){
        inportbyte(COMx->base+UART_RDR);
    }
    if(COMx->interrupt_flag!=COM_INT_NONE){
        DISABLE_INT();
        outportbyte(COMx->base+UART_IER, 0x00);  /* �ر�8250���жϹ���...           */
        outportbyte(COMx->base+UART_MCR, 0x00);  /* ...                             */
        #if SHARED_IRQ == 1
        IrqDisConnect(COMx->irq, COMx->isr);
        #else
        if(COMx->irq<8){
            outportbyte(0x21, inportbyte(0x21)|GetIrqMask(COMx->irq));
        } else {
            outportbyte(0xA1, inportbyte(0xA1)|GetIrqMask(COMx->irq));
        }
        FamesSetVect(GetIntNo(COMx->irq), COMx->old);
        #endif
        ENABLE_INT();
    }
    if(COMx->fifo_control!=COM_FIFO_NONE){
        outportbyte(COMx->base+UART_FCR, 0x00);  /* �ر�FIFO                        */
    }
    
    /*SERIAL_PORT_RESET_BUF_RXTX(COMx);*/
    
    ENTER_CRITICAL();
    COMx->status=COM_STATUS_CLOSE;
    EXIT_CRITICAL();
    return ok;    
}

/*------------------------------------------------------------------------------------
 * ����:    SetSerialPort()
 * ˵��:    ���ڲ����趨
 * ����:    COMx            ���ڶ���ָ��
 *          base            ���ڻ���ַ
 *          irq             �����ж���
 *          baudrate        ������
 *          parity          У��λ
 *          data            ����λ
 *          stop            ֹͣλ
 *          interrupt_flag  �ж�֧���趨
 *          fifo_control    FIFO���ü�Trigger Level�趨
 *          isr             �жϷ�����
 * ���:    ok/fail
 * ע��:    �˺�����رմ���(�Ѵ�ʱ)���趨����Ϊ�ر�״̬
**----------------------------------------------------------------------------------*/
BOOL apical SetSerialPort(SERIAL_PORT * COMx, INT16S base, INT16S irq, 
                                 INT32S baudrate, INT16S parity,
                                 INT16S data, INT16S stop, 
                                 INT16S interrupt_flag,
                                 INT16S fifo_control,
                                 __isr__ (*isr)(void))
{
    if(COMx==NULL){
        return fail;
    }
    if(base <= 0){
        return fail;
    }
    if(baudrate < 10L || baudrate > UART_BR_MAX){
        return fail;
    }
    switch(parity){
        case COM_PARITY_NONE:
        case COM_PARITY_EVEN:
        case COM_PARITY_ODD:
            break;
        default:
            return fail;
    }
    switch(data){
        case COM_DATA_5:
        case COM_DATA_6:
        case COM_DATA_7:
        case COM_DATA_8:
            break;
        default:
            return fail;
    }
    switch(stop){
        case COM_STOP_1:
        case COM_STOP_2:
            break;
        default:
            return fail;
    }            
    if(interrupt_flag != COM_INT_NONE){
        switch(interrupt_flag){
            case COM_INT_RX:
            case COM_INT_TX:
            case COM_INT_BOTH:
                break;
            default:
                return fail;
        }
        if(irq < IRQ_MIN_NO || irq > IRQ_MAX_NO){
            return fail;
        }
        if(isr == NULL){
            return fail;
        }
    }
    switch(fifo_control){
        case COM_FIFO_NONE:
        case COM_FIFO_TL_1:
        case COM_FIFO_TL_4:
        case COM_FIFO_TL_8:
        case COM_FIFO_TL_14:
            break;
        default:
            return fail;
    }
    if(COMx->status==COM_STATUS_OPEN){        /* ��������Ѿ���,��Ҫ�ȹر�        */
        CloseSerialPort(COMx);
    }    
    DispatchLock();
    COMx->base=base;
    COMx->baudrate=baudrate;
    COMx->parity=parity;
    COMx->data=data;
    COMx->stop=stop;
    COMx->interrupt_flag=interrupt_flag;
    COMx->fifo_control=fifo_control;
    COMx->irq=irq;
    COMx->isr=isr;
    COMx->old=NULL;
    COMx->status=COM_STATUS_CLOSE;
    DispatchUnlock();
    /*
    SERIAL_PORT_RESET_BUF_RXTX(COMx);
    */
    return ok;    
}

/*------------------------------------------------------------------------------------
 * ����:    SendSerialPort()
 * ˵��:    ���͵�����
 * ����:    COMx    ���ڶ���ָ��
 *          buf     Ҫ���͵��ַ���
 *          buf_len Ҫ���͵��ֽ���
 * ���:    ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical SendSerialPort(SERIAL_PORT * COMx, INT08U * buf, INT16S buf_len)
{
    INT16S sent;
    CRITICAL_DEFINE()
    
    if(COMx==NULL){
        return fail;
    }
    if(COMx->status!=COM_STATUS_OPEN){        /* û�д�,���ܷ���                  */
        return fail;
    }
    if(buf==NULL){
        return fail;
    }
    if(buf_len==0){
        return fail;
    }
    ENTER_CRITICAL();
    COMx->tx_buf=buf;                         /* �趨���ͻ���                       */
    COMx->tx_buf_len=buf_len;                 /* �趨�����ֽ���                     */
    EXIT_CRITICAL();
    COMx->tx_packet++;
    if(COMx->interrupt_flag == COM_INT_TX || COMx->interrupt_flag == COM_INT_BOTH){
        inportbyte(COMx->base+UART_IIR);
        ENTER_CRITICAL();
        COMx->tx_num=1;                       /* �ѷ�����1���ֽ�                    */
        EXIT_CRITICAL();
        EventReset(&COMx->tx_event);
        while(!UART_TX_READY(COMx->base)){;}
        outportbyte(COMx->base,buf[0]);       /* ���ָ������жϷ���             */
        if(COMx->fifo_control!=COM_FIFO_NONE){
            while(UART_TX_READY(COMx->base)){
                if(COMx->tx_num<COMx->tx_buf_len){
                    outportbyte(COMx->base+UART_THR, COMx->tx_buf[COMx->tx_num]);
                    COMx->tx_num++;
                    if(COMx->tx_num>=COMx->tx_buf_len){
                        return ok;
                    }
                } else {
                    return ok;
                }
            }
        }
        return TaskWait(&COMx->tx_event, (INT32U)((INT16U)(buf_len*2)));
                                              /* ����ĳ�ʱʱ�䰴��С4800Ϊ׼       */
    } else {
        for(sent=0; sent<buf_len; sent++){
            while(!UART_TX_READY(COMx->base)){;}
            outportbyte(COMx->base,buf[sent]); 
        }
        COMx->tx_num=sent;
        return ok;
    }
}

/*------------------------------------------------------------------------------------
 * ����:    PreRxSerialPort()
 * ˵��:    �趨���ڽ��ջ���
 * ����:    COMx    ���ڶ���ָ��
 *          buf     ���ջ���ָ��
 *          buf_len ���ջ����С
 * ���:    ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical PreRxSerialPort(SERIAL_PORT * COMx, INT08U * buf, INT16S buf_len)
{
    CRITICAL_DEFINE()
    
    if(COMx==NULL){
        return fail;
    }
    if(buf==NULL){
        return fail;
    }
    if(buf_len==0){
        return fail;
    }
    /*
    buf[0]='\0';
    */
    ENTER_CRITICAL();
    COMx->rx_buf=buf;                         /* �趨���ջ���                       */
    COMx->rx_buf_len=buf_len;                 /* �趨�����ֽ���                     */
    COMx->rx_num=0;
    COMx->rx_packet++;
    EXIT_CRITICAL();
    EventReset(&COMx->rx_event);
    return ok;
}

/*====================================================================================
 * 
 * ���ļ�����: rs232.c
 * 
**==================================================================================*/


