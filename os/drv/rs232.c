/*************************************************************************************
** 文件: rs232.c
** 说明: FamesOS的串口驱动(RS232)
** 作者: Jun
*************************************************************************************/
#define  FAMES_RS232_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 * 函数:    InitSerialPort()
 * 说明:    串口对象初始化
 * 输入:    COMx 串口对象指针
 * 输出:    ok/fail
 * 注意:    
**----------------------------------------------------------------------------------*/
BOOL apical InitSerialPort(SERIAL_PORT * COMx)
{
    CRITICAL_DEFINE()
    
    if(COMx==NULL){
        return fail;
    }
    #if 0
    if(COMx->status==COM_STATUS_INIT){        /* 已初始化,不需要再初始化了          */
        return fail;
    }
    #endif
    if(COMx->status==COM_STATUS_OPEN){        /* 打开状态,不允许初始化              */
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
 * 函数:    OpenSerialPort()
 * 说明:    打开串口
 * 输入:    COMx 串口对象指针
 * 输出:    ok/fail
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
    if(COMx->status==COM_STATUS_OPEN){        /* 已打开,不需要再打开                */
        return fail;
    }
    if(COMx->status==COM_STATUS_INIT){        /* 已初始化,但没有设置参数,不允许打开 */
        return fail;
    }
    if(COMx->base<=0){                        /* 基地址无效,不能打开                */
        return fail;
    }
    if(COMx->baudrate < 10L || COMx->baudrate > UART_BR_MAX){ 
        return fail;                          /* 波特率无效,不能打开                */
    }
    for(;;){                                  /* 如果波特率不支持,就找一个最接近的  */
        if(UART_BR_MAX%COMx->baudrate)COMx->baudrate--;
        else break;
    }
    
    com_lcr=(INT08U)0;
    com_brd=UART_BR_MAX/COMx->baudrate;    
    switch(COMx->parity){                     /* 设定校验位                         */
        case COM_PARITY_NONE:
            break;
        case COM_PARITY_EVEN:
            com_lcr|=0x18;
            break;
        case COM_PARITY_ODD:
            com_lcr|=0x08;
            break;
        default:
            return fail;                      /* 校验位设定不对                     */ 
    }
    switch(COMx->stop){                       /* 设定停止位                         */
        case COM_STOP_1:
            break;
        case COM_STOP_2:
            com_lcr|=0x04;
            break;
        default:
            return fail;                      /* 停止位设定不对                     */ 
    }
    switch(COMx->data){                       /* 设定数据位                         */
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
            return fail;                      /* 数据位设定不对                     */ 
    }
    com_fcr=(INT08U)0x07;
    switch(COMx->fifo_control){               /* 设置FIFO                           */
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
        default:                              /* FIFO设置不对                       */
            return fail;
    }
    if(COMx->interrupt_flag != COM_INT_NONE){
        if(COMx->irq < IRQ_MIN_NO || COMx->irq > IRQ_MAX_NO){
            return fail;                      /* irq不支持                          */
        }
        if(COMx->isr == NULL){
            return fail;                      /* isr为空                            */
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
                return fail;                  /* 不支持的中断允许设定               */
        }
        outportbyte(COMx->base+UART_IER, 0);
        outportbyte(COMx->base+UART_MCR, 0); 
        inportbyte(COMx->base+UART_IIR);
        inportbyte(COMx->base+UART_RDR);
        ENTER_CRITICAL();
        outportbyte(COMx->base+UART_IER, com_ier);
        outportbyte(COMx->base+UART_MCR, 0x08);  /* 打开UART的中断功能              */
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
        outportbyte(COMx->base+UART_IER, 0x00);  /* 关闭8250的中断功能...          */
        outportbyte(COMx->base+UART_MCR, 0x00);  /* ...                            */
    }
    outportbyte(COMx->base+UART_LCR, 0x80);
    outportword(COMx->base,(INT16U)(INT16S)com_brd);
    outportbyte(COMx->base+UART_LCR, com_lcr);
    if(COMx->fifo_control!=COM_FIFO_NONE){
        outportbyte(COMx->base+UART_FCR, 0x01);  /* 致能UART的FIFO                 */
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
 * 函数:    CloseSerialPort()
 * 说明:    关闭串口
 * 输入:    COMx 串口对象指针
 * 输出:    ok/fail
 * 笔记:    这个函数刚写完时,总有错误,经过调试,发现这个函数竟然会出现堆栈不平衡的
 *          现象,原因是在编译后,setvect()调用后竟然会有mov sp,bp这句话,
 *          而接下来的popf(由EXIT_CRITICAL())就破坏了堆栈!
 *          修改方法为将ENTER_CRITICAL()与EXIT_CRITICAL()替换为asm cli与asm sti.
 *          经过测试,函数运行正常
 *
 *        真奇怪, 这里始终有点问题 - jun, 9/3/2010
**----------------------------------------------------------------------------------*/
BOOL apical CloseSerialPort(SERIAL_PORT * COMx)
{
    CRITICAL_DEFINE()
    
    if(COMx==NULL){
        return fail;
    }
    if(COMx->status!=COM_STATUS_OPEN){        /* 没有打开,不需要关闭                */
        return fail;
    }
    if(inportbyte(COMx->base+UART_LSR)&0x1){
        inportbyte(COMx->base+UART_RDR);
    }
    if(COMx->interrupt_flag!=COM_INT_NONE){
        DISABLE_INT();
        outportbyte(COMx->base+UART_IER, 0x00);  /* 关闭8250的中断功能...           */
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
        outportbyte(COMx->base+UART_FCR, 0x00);  /* 关闭FIFO                        */
    }
    
    /*SERIAL_PORT_RESET_BUF_RXTX(COMx);*/
    
    ENTER_CRITICAL();
    COMx->status=COM_STATUS_CLOSE;
    EXIT_CRITICAL();
    return ok;    
}

/*------------------------------------------------------------------------------------
 * 函数:    SetSerialPort()
 * 说明:    串口参数设定
 * 输入:    COMx            串口对象指针
 *          base            串口基地址
 *          irq             串口中断线
 *          baudrate        波特率
 *          parity          校验位
 *          data            数据位
 *          stop            停止位
 *          interrupt_flag  中断支持设定
 *          fifo_control    FIFO启用及Trigger Level设定
 *          isr             中断服务函数
 * 输出:    ok/fail
 * 注意:    此函数会关闭串口(已打开时)或设定串口为关闭状态
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
    if(COMx->status==COM_STATUS_OPEN){        /* 如果串口已经打开,则要先关闭        */
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
 * 函数:    SendSerialPort()
 * 说明:    发送到串口
 * 输入:    COMx    串口对象指针
 *          buf     要发送的字符串
 *          buf_len 要发送的字节数
 * 输出:    ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical SendSerialPort(SERIAL_PORT * COMx, INT08U * buf, INT16S buf_len)
{
    INT16S sent;
    CRITICAL_DEFINE()
    
    if(COMx==NULL){
        return fail;
    }
    if(COMx->status!=COM_STATUS_OPEN){        /* 没有打开,不能发送                  */
        return fail;
    }
    if(buf==NULL){
        return fail;
    }
    if(buf_len==0){
        return fail;
    }
    ENTER_CRITICAL();
    COMx->tx_buf=buf;                         /* 设定发送缓冲                       */
    COMx->tx_buf_len=buf_len;                 /* 设定发送字节数                     */
    EXIT_CRITICAL();
    COMx->tx_packet++;
    if(COMx->interrupt_flag == COM_INT_TX || COMx->interrupt_flag == COM_INT_BOTH){
        inportbyte(COMx->base+UART_IIR);
        ENTER_CRITICAL();
        COMx->tx_num=1;                       /* 已发送了1个字节                    */
        EXIT_CRITICAL();
        EventReset(&COMx->tx_event);
        while(!UART_TX_READY(COMx->base)){;}
        outportbyte(COMx->base,buf[0]);       /* 这个指令将触发中断发送             */
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
                                              /* 这里的超时时间按最小4800为准       */
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
 * 函数:    PreRxSerialPort()
 * 说明:    设定串口接收缓冲
 * 输入:    COMx    串口对象指针
 *          buf     接收缓冲指针
 *          buf_len 接收缓冲大小
 * 输出:    ok/fail
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
    COMx->rx_buf=buf;                         /* 设定接收缓冲                       */
    COMx->rx_buf_len=buf_len;                 /* 设定接收字节数                     */
    COMx->rx_num=0;
    COMx->rx_packet++;
    EXIT_CRITICAL();
    EventReset(&COMx->rx_event);
    return ok;
}

/*====================================================================================
 * 
 * 本文件结束: rs232.c
 * 
**==================================================================================*/


