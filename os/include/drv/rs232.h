/*************************************************************************************
 * 文件: rs232.h
 *
 * 说明: FamesOS的串口驱动(RS232)
 *
 * 作者: Jun
*************************************************************************************/
#ifndef FAMES_RS232_H
#define FAMES_RS232_H

/*
*下面定义UART(8250/16550)参考计时频率-------------------------------------------------
*/
#define UART_CLOCK    (1843200L)              /* UART8250 参考计时频率              */
#define UART_BR_MAX   (UART_CLOCK>>4)         /* UART8250 最高波特率                */

/*
*下面为UART(8250/16550)寄存器的偏移值-------------------------------------------------
*说明:
* 1) 当UART_LCR的第7位为1时, 0号与1号寄存器为16位的波特率除数(UART_BRD),
*    波特率=(115200/UART_BRD); 否则为THR(RDR)与IER
* 2) 中断使能寄存器(IER)的定义:
*    IER.0=资料接收到, IER.1=THR空了, IER.2=LSR有变化, IER.3=MSR有变化
* 3) 中断指示寄存器(IIR)的定义:
*    IIR.0          1=表示没有未处理中断(No Interrupt Pending)
*    IIR.3:2:1      011=LSR有状况,    010=有资料接收到(或RX FIFO满), 
*                   110=数据接收超时, 001=THR空了, 000=MSR有变化
*    IIR.5:4        全为0, 不使用
*    IIR.7:6        全为1(16550)或全为0(16450/8250)
*    IIR共可指出5种中断，且优先顺序为从左到右由高到低(其中010与110优先级相同)
* 4) FIFO控制寄存器(FCR)的定义:
*    FCR.0          1=FIFO致能
*    FCR.1          1=RX FIFO RESET
*    FCR.2          1=TX FIFO RESET
*    FCR.3          DMA MODE SELECT
*    FCR.5:4        0(NOT USED)
*    FCR.7:6        FIFO Trigger Level: 00=01, 01=04, 10=08, 11=14
* 5) 线控制寄存器(LCR)的定义:
*    LCR.7          1=(BRDL,BRDH), 0=(THR,IER)
*    LCR.6          1=BREAK
*    LCR.5:4:3    000=NONE, 001=ODD, 011=EVEN, 101=MARK, 111=SPACE (控制校验位)
*    LCR.2          0=1个结束符, 1=2个结束符
*    LCR.1:0       00=5, 01=6, 10=7, 11=8 (控制数据位)
* 6) Modem控制寄存器(MCR)的定义:
*    MCR.0          1=设定DTR,  设定UART的DTR输出
*    MCR.1          1=设定RTS,  设定UART的RTS输出
*    MCR.2          1=OUT1   ,  使用者可设定的输出
*    MCR.3          1=中断致能, 设为1时,才允许8250有中断功能!!!
*    MCR.4          1=回路测试
*    MCR.5:6:7      全为0, 不使用
* 7) 线状态寄存器(LSR)的定义:
*    LSR.0          1=资料接收到,  接到的数据已放入了RDR中
*    LSR.1          1=OVERRUN错误, 代表接收器内的数据被新到的数据覆盖
*    LSR.2          1=PARITY错误,  校验位错误
*    LSR.3          1=FRAMING错误, 收到的位元组在组合后发现不正确的结束位元
*    LSR.4          1=BREAK产生,   读取LSR后会设为0
*    LSR.5          1=THR空了,     THR资料已传至TSR移位寄存器,表示可以再传送资料
*    LSR.6          1=TSR空了,     TSR已经空了,表示所有位元组都送出去了
*    LSR.7          1=超时
* 8) Modem状态寄存器(MSR)的定义:
*    MSR.0          1=CTS有变化,   Modem已准备要从电脑接收数据
*    MSR.1          1=DSR有变化,   Modem电源已开,准备工作
*    MSR.2          1=RI有变化,    电话在响
*    MSR.3          1=DCD有变化,   Modem与另一个Modem连通了
*    MSR.4          CTS
*    MSR.5          DSR
*    MSR.6          响铃RI
*    MSR.7          载波DCD
* 9) Scratchpad Register(SPR):
*    8 bits of information can be stored in this register
*/
#define  UART_THR      0     /* 即将被发出的一个字节                                */
#define  UART_RDR      0     /* 刚接收到的一个字节                                  */
#define  UART_BRDL     0     /* 与UART_BRDH一起构成波特率除数(UART_LCR.7=1时)       */
#define  UART_IER      1     /* 中断使能寄存器                                      */
#define  UART_BRDH     1     /* 与UART_BRDL一起构成波特率除数(UART_LCR.7=1时)       */
#define  UART_IIR      2     /* 中断指示寄存器                                      */
#define  UART_FCR      2     /* FIFO控制寄存器                                      */
#define  UART_LCR      3     /* 线控制寄存器                                        */
#define  UART_MCR      4     /* Modem控制寄存器                                     */
#define  UART_LSR      5     /* 线状态寄存器                                        */
#define  UART_MSR      6     /* Modem状态寄存器                                     */
#define  UART_SPR      7     /* Scratchpad Register                                 */

/*
*下面定义串口数据结构-----------------------------------------------------------------
*/
typedef struct _SERIAL_PORT_{/* 串口控制结构(COM口或者说RS232口)                    */
    INT16S base;             /* 基地址                                              */
    INT16S irq;              /* 中断线                                              */
    INT32S baudrate;         /* 波特率                                              */
    INT16S parity;           /* 校验位                                              */
    INT16S data;             /* 数据位                                              */
    INT16S stop;             /* 停止位                                              */
    INT16S interrupt_flag;   /* 中断标志: 0=不中断,1=RX中断,2=TX中断,3=TX&RX中断    */
    INT16S fifo_control;     /* FIFO控制(主要是FIFO Trigger Level)                  */
    INT16S status;           /* 状态                                                */
    __isr__ (*isr)(void);    /* 中断服务例程                                        */
    __isr__ (*old)(void);    /* 旧的中断服务例程                                    */
    INT08U *rx_buf;          /* 接收缓冲                                            */
    INT16S rx_buf_len;       /* 接收缓冲大小                                        */
    INT16S rx_num;           /* 已接收字节数,这个值不能超过rx_buf_len               */
    ECB    rx_event;         /* 接收完成事件                                        */
    INT32U rx_packet;        /* The number of rx packets                            */
    INT08U *tx_buf;          /* 发送缓冲                                            */
    INT16S tx_buf_len;       /* 发送缓冲大小,或者是要发送的字节数                   */
    INT16S tx_num;           /* 已发送字节数,这个值到了tx_buf_len表示发送完毕       */
    ECB    tx_event;         /* 发送完成事件                                        */
    INT32U tx_packet;        /* The number of tx packets                            */
} SERIAL_PORT;

#define COM_PARITY_NONE  0   /* 无校验位                                            */
#define COM_PARITY_EVEN  1   /* 偶校验位                                            */
#define COM_PARITY_ODD   2   /* 奇校验位                                            */

#define COM_DATA_5       5   /* 5位数据位                                           */
#define COM_DATA_6       6   /* 6位数据位                                           */
#define COM_DATA_7       7   /* 7位数据位                                           */
#define COM_DATA_8       8   /* 8位数据位                                           */

#define COM_STOP_1       0   /* 1位停止位                                           */
#define COM_STOP_2       1   /* 2位停止位                                           */

#define COM_INT_NONE     0   /* 无中断,查询方式发送,不接收                          */
#define COM_INT_RX       1   /* RX中断,查询方式发送,中断接收                        */
#define COM_INT_TX       2   /* TX中断,中断方式发送,不接收                          */
#define COM_INT_BOTH     3   /* RX&TX中断,中断方式发送,中断接收                     */

#define COM_STATUS_CLOSE 0   /* 串口状态: 关闭                                      */
#define COM_STATUS_INIT  1   /* 串口状态: 已初始化                                  */
#define COM_STATUS_OPEN  2   /* 串口状态: 打开                                      */

/*
*FIFO控制命令-------------------------------------------------------------------------
*/
#define COM_FIFO_NONE    0   /* 串口不启用FIFO                                      */
#define COM_FIFO_TL_1    1   /* 启用FIFO，其Trigger Level= 1                        */
#define COM_FIFO_TL_4    2   /* 启用FIFO，其Trigger Level= 4                        */
#define COM_FIFO_TL_8    3   /* 启用FIFO，其Trigger Level= 8                        */
#define COM_FIFO_TL_14   4   /* 启用FIFO，其Trigger Level=14                        */

/*
*UART_IIR状态位模式-------------------------------------------------------------------
*/
#define UART_IIR_INT   0x0E  /* 中断指示位掩码                                      */
#define UART_IIR_LSR   0x06  /* LSR有变化                                           */
#define UART_IIR_RX    0x04  /* RDR有数据或FIFO触发(16550模式)                      */
#define UART_IIR_RXTO  0x0C  /* 接收数据超时(16550模式)                             */
#define UART_IIR_THR   0x02  /* THR已空                                             */
#define UART_IIR_MSR   0x00  /* MSR有变化                                           */

/*
*宏定义: 复位RX/TX缓冲区设定----------------------------------------------------------
*/
#define  SERIAL_PORT_RESET_BUF_RXTX(pcom)            \
            ENTER_CRITICAL();                        \
            pcom->rx_buf=NULL;                       \
            pcom->rx_buf_len=-1;                     \
            pcom->rx_num=0;                          \
            pcom->rx_packet=0L;                      \
            pcom->tx_buf=NULL;                       \
            pcom->tx_buf_len=-1;                     \
            pcom->tx_num=0;                          \
            pcom->tx_packet=0L;                      \
            EventInit(&pcom->rx_event);              \
            EventInit(&pcom->tx_event);              \
            EXIT_CRITICAL();

/*
*宏定义: --------------------------- -------------------------------------------------
*/
#define  UART_RX_READY(base)   (0x01&inportbyte((base)+UART_LSR)) /* 数据已准备好 */
#define  UART_TX_READY(base)   (0x20&inportbyte((base)+UART_LSR)) /* 可以发送数据 */

#define  IS_UART16550(base)    (0xC0==(0xC0&inportbyte((base)+UART_IIR)))


/*
*串口服务函数声明---------------------------------------------------------------------
*/
BOOL apical InitSerialPort(SERIAL_PORT * pcom);           
BOOL apical OpenSerialPort(SERIAL_PORT * pcom);           
BOOL apical CloseSerialPort(SERIAL_PORT * pcom);          
BOOL apical SetSerialPort(SERIAL_PORT * pcom, INT16S base, INT16S irq, 
                   INT32S baudrate, INT16S parity, INT16S data, INT16S stop, 
                   INT16S interrupt_flag, INT16S fifo_control, __isr__ (*isr)(void));
BOOL apical SendSerialPort(SERIAL_PORT * pcom, INT08U *buf, INT16S buf_len);
BOOL apical PreRxSerialPort(SERIAL_PORT * pcom, INT08U *buf, INT16S buf_len);


#endif /* #ifndef FAMES_RS232_H */

/*====================================================================================
 * 
 * 本文件结束: rs232.h
 * 
**==================================================================================*/


