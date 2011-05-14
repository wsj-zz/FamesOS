/*************************************************************************************
 * �ļ�:    error.h
 *
 * ˵��:    ������뼰������Ϣ
 *
 * ����:    Jun
 *
 * ʱ��:    2010-06-19
 *
 * �汾:    V0.1.1(δ���)
*************************************************************************************/
#ifndef FAMES_ERROR_H
#define FAMES_ERROR_H

/*
** ���涨�������� 
*/
#define  ERROR_NONE               0          /* �޴���                             */
#define  ERROR_FAMES_START        1          /* FAMES��������                      */
#define  ERROR_FAMES_SHUTDOWN     2          /* FAMES�ѹر�                        */
#define  ERROR_MESSAGE            3          /* һ����Ϣ                           */
#define  ERROR_NO_MEM             4          /* �ڴ治��                           */
#define  ERROR_TASK_NONEREADY     5          /* ����ʱ����û�о�������             */
#define  ERROR_TOO_MANY_DPC      10          /* ��ʱ��DPC����̫��                  */
#define  ERROR_PLC_TIMEOUT_RX    20          /* PLC���ճ�ʱ                        */
#define  ERROR_PLC_RX_RESPONSE   21          /* PLC��Ӧ��ǰ5���ֽڲ���             */
#define  ERROR_PLC_RX_LRC        22          /* PLC��Ӧ��LRCУ��ʧ��               */
#define  ERROR_PLC_RX_ERROR      23          /* PLC��Ӧ�а���������                */
#define  ERROR_PLC_COM_NOT_OPEN  24          /* PLC���ô���û�д�                */
#define  ERROR_RS232_OPEN_FAIL   30          /* ���ڴ�ʧ��                       */
#define  ERROR_ISR_REG_IRQ       100         /* IrqConnect()�е�irq������Χ        */
#define  ERROR_ISR_REG_FUNC      101         /* IrqConnect()�е�func��Ч           */
#define  ERROR_ISR_REG_ISR_FULL  102         /* IrqConnect()�з���ISR���ƿ�ʱʧ��  */
#define  ERROR_IRQ_NOT_USED      103         /* IrqDisConnect()�е�IRQ���ƿ�û�д򿪹�*/
#define  ERROR_ISR_NOT_FOUND     104         /* IrqDisConnect()�в���ISR���ƿ�ʧ��    */
#define  ERROR_UART_NOT_16550    200         /* UARTоƬ����16550���ϵ�,��֧��FIFO */
#define  ERROR_X_CHANGE_PAGE     500         /* X-DRIVER�л�ҳʧ��                 */
#define  ERROR_BMP_FILE         1000         /* BMP�ļ�������,����BMP�ļ�        */
#define  ERROR_BMP_COLORDEPTH   1001         /* BMP�ļ���ɫ���в�֧��              */
#define  ERROR_BMP_SIGNATURE    1002         /* BMP�ļ���ǩ������"BM"              */
#define  ERROR_BMP_FORMAT       1003         /* BMP�ļ�����(��ʽ)����              */
#define  ERROR_BMPINFO_USED     1010         /* ����BMPʱ, ����BMPINFO�ѱ�ռ��     */
#define  ERROR_BMPINFO_NOTREADY 1011         /* ж��BMPʱ, ����BMPINFO��Ч         */
#define  ERROR_BMPINFO_HANDLE   1012         /* ж�ػ���ʾBMPʱ, ����XMS�����Ч   */

#define  ERROR_XMS_ALLOCATION   1200         /* XMS�ڴ����ʧ��                    */




#define  SetErrorCode(errno)   /* ���ô������ */



#endif                                       /* #ifndef FAMES_ERROR_H              */

/*====================================================================================
 * 
 * ���ļ�����: error.h
 * 
**==================================================================================*/


