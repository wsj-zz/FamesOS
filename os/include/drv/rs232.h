/*************************************************************************************
 * �ļ�: rs232.h
 *
 * ˵��: FamesOS�Ĵ�������(RS232)
 *
 * ����: Jun
*************************************************************************************/
#ifndef FAMES_RS232_H
#define FAMES_RS232_H

/*
*���涨��UART(8250/16550)�ο���ʱƵ��-------------------------------------------------
*/
#define UART_CLOCK    (1843200L)              /* UART8250 �ο���ʱƵ��              */
#define UART_BR_MAX   (UART_CLOCK>>4)         /* UART8250 ��߲�����                */

/*
*����ΪUART(8250/16550)�Ĵ�����ƫ��ֵ-------------------------------------------------
*˵��:
* 1) ��UART_LCR�ĵ�7λΪ1ʱ, 0����1�żĴ���Ϊ16λ�Ĳ����ʳ���(UART_BRD),
*    ������=(115200/UART_BRD); ����ΪTHR(RDR)��IER
* 2) �ж�ʹ�ܼĴ���(IER)�Ķ���:
*    IER.0=���Ͻ��յ�, IER.1=THR����, IER.2=LSR�б仯, IER.3=MSR�б仯
* 3) �ж�ָʾ�Ĵ���(IIR)�Ķ���:
*    IIR.0          1=��ʾû��δ�����ж�(No Interrupt Pending)
*    IIR.3:2:1      011=LSR��״��,    010=�����Ͻ��յ�(��RX FIFO��), 
*                   110=���ݽ��ճ�ʱ, 001=THR����, 000=MSR�б仯
*    IIR.5:4        ȫΪ0, ��ʹ��
*    IIR.7:6        ȫΪ1(16550)��ȫΪ0(16450/8250)
*    IIR����ָ��5���жϣ�������˳��Ϊ�������ɸߵ���(����010��110���ȼ���ͬ)
* 4) FIFO���ƼĴ���(FCR)�Ķ���:
*    FCR.0          1=FIFO����
*    FCR.1          1=RX FIFO RESET
*    FCR.2          1=TX FIFO RESET
*    FCR.3          DMA MODE SELECT
*    FCR.5:4        0(NOT USED)
*    FCR.7:6        FIFO Trigger Level: 00=01, 01=04, 10=08, 11=14
* 5) �߿��ƼĴ���(LCR)�Ķ���:
*    LCR.7          1=(BRDL,BRDH), 0=(THR,IER)
*    LCR.6          1=BREAK
*    LCR.5:4:3    000=NONE, 001=ODD, 011=EVEN, 101=MARK, 111=SPACE (����У��λ)
*    LCR.2          0=1��������, 1=2��������
*    LCR.1:0       00=5, 01=6, 10=7, 11=8 (��������λ)
* 6) Modem���ƼĴ���(MCR)�Ķ���:
*    MCR.0          1=�趨DTR,  �趨UART��DTR���
*    MCR.1          1=�趨RTS,  �趨UART��RTS���
*    MCR.2          1=OUT1   ,  ʹ���߿��趨�����
*    MCR.3          1=�ж�����, ��Ϊ1ʱ,������8250���жϹ���!!!
*    MCR.4          1=��·����
*    MCR.5:6:7      ȫΪ0, ��ʹ��
* 7) ��״̬�Ĵ���(LSR)�Ķ���:
*    LSR.0          1=���Ͻ��յ�,  �ӵ��������ѷ�����RDR��
*    LSR.1          1=OVERRUN����, ����������ڵ����ݱ��µ������ݸ���
*    LSR.2          1=PARITY����,  У��λ����
*    LSR.3          1=FRAMING����, �յ���λԪ������Ϻ��ֲ���ȷ�Ľ���λԪ
*    LSR.4          1=BREAK����,   ��ȡLSR�����Ϊ0
*    LSR.5          1=THR����,     THR�����Ѵ���TSR��λ�Ĵ���,��ʾ�����ٴ�������
*    LSR.6          1=TSR����,     TSR�Ѿ�����,��ʾ����λԪ�鶼�ͳ�ȥ��
*    LSR.7          1=��ʱ
* 8) Modem״̬�Ĵ���(MSR)�Ķ���:
*    MSR.0          1=CTS�б仯,   Modem��׼��Ҫ�ӵ��Խ�������
*    MSR.1          1=DSR�б仯,   Modem��Դ�ѿ�,׼������
*    MSR.2          1=RI�б仯,    �绰����
*    MSR.3          1=DCD�б仯,   Modem����һ��Modem��ͨ��
*    MSR.4          CTS
*    MSR.5          DSR
*    MSR.6          ����RI
*    MSR.7          �ز�DCD
* 9) Scratchpad Register(SPR):
*    8 bits of information can be stored in this register
*/
#define  UART_THR      0     /* ������������һ���ֽ�                                */
#define  UART_RDR      0     /* �ս��յ���һ���ֽ�                                  */
#define  UART_BRDL     0     /* ��UART_BRDHһ�𹹳ɲ����ʳ���(UART_LCR.7=1ʱ)       */
#define  UART_IER      1     /* �ж�ʹ�ܼĴ���                                      */
#define  UART_BRDH     1     /* ��UART_BRDLһ�𹹳ɲ����ʳ���(UART_LCR.7=1ʱ)       */
#define  UART_IIR      2     /* �ж�ָʾ�Ĵ���                                      */
#define  UART_FCR      2     /* FIFO���ƼĴ���                                      */
#define  UART_LCR      3     /* �߿��ƼĴ���                                        */
#define  UART_MCR      4     /* Modem���ƼĴ���                                     */
#define  UART_LSR      5     /* ��״̬�Ĵ���                                        */
#define  UART_MSR      6     /* Modem״̬�Ĵ���                                     */
#define  UART_SPR      7     /* Scratchpad Register                                 */

/*
*���涨�崮�����ݽṹ-----------------------------------------------------------------
*/
typedef struct _SERIAL_PORT_{/* ���ڿ��ƽṹ(COM�ڻ���˵RS232��)                    */
    INT16S base;             /* ����ַ                                              */
    INT16S irq;              /* �ж���                                              */
    INT32S baudrate;         /* ������                                              */
    INT16S parity;           /* У��λ                                              */
    INT16S data;             /* ����λ                                              */
    INT16S stop;             /* ֹͣλ                                              */
    INT16S interrupt_flag;   /* �жϱ�־: 0=���ж�,1=RX�ж�,2=TX�ж�,3=TX&RX�ж�    */
    INT16S fifo_control;     /* FIFO����(��Ҫ��FIFO Trigger Level)                  */
    INT16S status;           /* ״̬                                                */
    __isr__ (*isr)(void);    /* �жϷ�������                                        */
    __isr__ (*old)(void);    /* �ɵ��жϷ�������                                    */
    INT08U *rx_buf;          /* ���ջ���                                            */
    INT16S rx_buf_len;       /* ���ջ����С                                        */
    INT16S rx_num;           /* �ѽ����ֽ���,���ֵ���ܳ���rx_buf_len               */
    ECB    rx_event;         /* ��������¼�                                        */
    INT32U rx_packet;        /* The number of rx packets                            */
    INT08U *tx_buf;          /* ���ͻ���                                            */
    INT16S tx_buf_len;       /* ���ͻ����С,������Ҫ���͵��ֽ���                   */
    INT16S tx_num;           /* �ѷ����ֽ���,���ֵ����tx_buf_len��ʾ�������       */
    ECB    tx_event;         /* ��������¼�                                        */
    INT32U tx_packet;        /* The number of tx packets                            */
} SERIAL_PORT;

#define COM_PARITY_NONE  0   /* ��У��λ                                            */
#define COM_PARITY_EVEN  1   /* żУ��λ                                            */
#define COM_PARITY_ODD   2   /* ��У��λ                                            */

#define COM_DATA_5       5   /* 5λ����λ                                           */
#define COM_DATA_6       6   /* 6λ����λ                                           */
#define COM_DATA_7       7   /* 7λ����λ                                           */
#define COM_DATA_8       8   /* 8λ����λ                                           */

#define COM_STOP_1       0   /* 1λֹͣλ                                           */
#define COM_STOP_2       1   /* 2λֹͣλ                                           */

#define COM_INT_NONE     0   /* ���ж�,��ѯ��ʽ����,������                          */
#define COM_INT_RX       1   /* RX�ж�,��ѯ��ʽ����,�жϽ���                        */
#define COM_INT_TX       2   /* TX�ж�,�жϷ�ʽ����,������                          */
#define COM_INT_BOTH     3   /* RX&TX�ж�,�жϷ�ʽ����,�жϽ���                     */

#define COM_STATUS_CLOSE 0   /* ����״̬: �ر�                                      */
#define COM_STATUS_INIT  1   /* ����״̬: �ѳ�ʼ��                                  */
#define COM_STATUS_OPEN  2   /* ����״̬: ��                                      */

/*
*FIFO��������-------------------------------------------------------------------------
*/
#define COM_FIFO_NONE    0   /* ���ڲ�����FIFO                                      */
#define COM_FIFO_TL_1    1   /* ����FIFO����Trigger Level= 1                        */
#define COM_FIFO_TL_4    2   /* ����FIFO����Trigger Level= 4                        */
#define COM_FIFO_TL_8    3   /* ����FIFO����Trigger Level= 8                        */
#define COM_FIFO_TL_14   4   /* ����FIFO����Trigger Level=14                        */

/*
*UART_IIR״̬λģʽ-------------------------------------------------------------------
*/
#define UART_IIR_INT   0x0E  /* �ж�ָʾλ����                                      */
#define UART_IIR_LSR   0x06  /* LSR�б仯                                           */
#define UART_IIR_RX    0x04  /* RDR�����ݻ�FIFO����(16550ģʽ)                      */
#define UART_IIR_RXTO  0x0C  /* �������ݳ�ʱ(16550ģʽ)                             */
#define UART_IIR_THR   0x02  /* THR�ѿ�                                             */
#define UART_IIR_MSR   0x00  /* MSR�б仯                                           */

/*
*�궨��: ��λRX/TX�������趨----------------------------------------------------------
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
*�궨��: --------------------------- -------------------------------------------------
*/
#define  UART_RX_READY(base)   (0x01&inportbyte((base)+UART_LSR)) /* ������׼���� */
#define  UART_TX_READY(base)   (0x20&inportbyte((base)+UART_LSR)) /* ���Է������� */

#define  IS_UART16550(base)    (0xC0==(0xC0&inportbyte((base)+UART_IIR)))


/*
*���ڷ���������---------------------------------------------------------------------
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
 * ���ļ�����: rs232.h
 * 
**==================================================================================*/


