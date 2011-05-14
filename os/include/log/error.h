/*************************************************************************************
 * 文件:    error.h
 *
 * 说明:    错误代码及错误信息
 *
 * 作者:    Jun
 *
 * 时间:    2010-06-19
 *
 * 版本:    V0.1.1(未完成)
*************************************************************************************/
#ifndef FAMES_ERROR_H
#define FAMES_ERROR_H

/*
** 下面定义错误代码 
*/
#define  ERROR_NONE               0          /* 无错误                             */
#define  ERROR_FAMES_START        1          /* FAMES重新启动                      */
#define  ERROR_FAMES_SHUTDOWN     2          /* FAMES已关闭                        */
#define  ERROR_MESSAGE            3          /* 一般信息                           */
#define  ERROR_NO_MEM             4          /* 内存不足                           */
#define  ERROR_TASK_NONEREADY     5          /* 调度时发现没有就绪任务             */
#define  ERROR_TOO_MANY_DPC      10          /* 定时器DPC请求太多                  */
#define  ERROR_PLC_TIMEOUT_RX    20          /* PLC接收超时                        */
#define  ERROR_PLC_RX_RESPONSE   21          /* PLC回应的前5个字节不符             */
#define  ERROR_PLC_RX_LRC        22          /* PLC回应的LRC校验失败               */
#define  ERROR_PLC_RX_ERROR      23          /* PLC回应中包含错误码                */
#define  ERROR_PLC_COM_NOT_OPEN  24          /* PLC所用串口没有打开                */
#define  ERROR_RS232_OPEN_FAIL   30          /* 串口打开失败                       */
#define  ERROR_ISR_REG_IRQ       100         /* IrqConnect()中的irq超出范围        */
#define  ERROR_ISR_REG_FUNC      101         /* IrqConnect()中的func无效           */
#define  ERROR_ISR_REG_ISR_FULL  102         /* IrqConnect()中分配ISR控制块时失败  */
#define  ERROR_IRQ_NOT_USED      103         /* IrqDisConnect()中的IRQ控制块没有打开过*/
#define  ERROR_ISR_NOT_FOUND     104         /* IrqDisConnect()中查找ISR控制块失败    */
#define  ERROR_UART_NOT_16550    200         /* UART芯片不是16550以上的,不支持FIFO */
#define  ERROR_X_CHANGE_PAGE     500         /* X-DRIVER中换页失败                 */
#define  ERROR_BMP_FILE         1000         /* BMP文件不存在,或不是BMP文件        */
#define  ERROR_BMP_COLORDEPTH   1001         /* BMP文件的色深尚不支持              */
#define  ERROR_BMP_SIGNATURE    1002         /* BMP文件的签名不是"BM"              */
#define  ERROR_BMP_FORMAT       1003         /* BMP文件内容(格式)不对              */
#define  ERROR_BMPINFO_USED     1010         /* 加载BMP时, 发现BMPINFO已被占用     */
#define  ERROR_BMPINFO_NOTREADY 1011         /* 卸载BMP时, 发现BMPINFO无效         */
#define  ERROR_BMPINFO_HANDLE   1012         /* 卸载或显示BMP时, 发现XMS句柄无效   */

#define  ERROR_XMS_ALLOCATION   1200         /* XMS内存分配失败                    */




#define  SetErrorCode(errno)   /* 设置错误代码 */



#endif                                       /* #ifndef FAMES_ERROR_H              */

/*====================================================================================
 * 
 * 本文件结束: error.h
 * 
**==================================================================================*/


