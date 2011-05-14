/***********************************************************************************************
 * 文件:    plc.h
 *
 * 说明:    PLC通用定义
 *
 * 作者:    Jun
 *
 * 时间:    2010-9-2
***********************************************************************************************/
#ifndef FAMES_PLC_H
#define FAMES_PLC_H

/*----------------------------------------------------------------------------------------------
 * 
 * 支持的PLC类型
 * 
**--------------------------------------------------------------------------------------------*/
#define PLC_TYPE_FATEK       1     /* 永宏PLC   */       
#define PLC_TYPE_SIEMENS     2     /* 西门子PLC */


/*----------------------------------------------------------------------------------------------
 *          plc_action_s
 * 
 * 描述: PLC命令块结构, 其表达了上位机与PLC的某次通讯
**--------------------------------------------------------------------------------------------*/
struct plc_action_s {
    INT16U   flag;
    int      id;
    int      cmd;
    INT08S * addr;
    void   * associated;
    int      number;
    int      timer;
    int      timerinit;
    INT32U   count;
    int      disabled;
    void  (* finish)(int id, BOOL success);
    struct plc_action_s * next;
};

typedef struct plc_action_s PLC_ACTION;

#define  PLC_ACTION_FLAG_NONE       0x00
#define  PLC_ACTION_FLAG_LINK       0x01
#define  PLC_ACTION_FLAG_UNLK       0x02
#define  PLC_ACTION_FLAG_ONCE       0x03
#define  PLC_ACTION_FLAG_ONCE_ASYC  0x04

#define  PLC_ACTION_FLAG_MASK       0x0F

#define  PLC_ACTION_FLAG_DISABLE    0x10    /* 禁用 */

/*----------------------------------------------------------------------------------------------
 *          plc_descriptor_s
 * 
 * 描述: PLC控制块结构, 上位机与PLC的通讯以此为基础
**--------------------------------------------------------------------------------------------*/
struct plc_descriptor_s {
    INT08S      * name;
    int           type;
    INT16U        flag;
    HANDLE        daemon;
    int           station_no;
    SERIAL_PORT   rs232;
    void        * tx_buf;
    void        * rx_buf;
    void        * private;
    BOOL          lock;
    struct   plc_action_s * action_list;
};

typedef struct plc_descriptor_s PLC;

#define  PLC_FLAG_USED  0x01U
#define  PLC_FLAG_OPEN  0x02U

/*----------------------------------------------------------------------------------------------
 * 
 *      PLC通讯相关定义
 * 
**--------------------------------------------------------------------------------------------*/
#define PLC_RETRY_RX_TO    3                 /* 接收PLC数据超时的重试次数          */  
#define PLC_RETRY_ERR      3                 /* PLC数据有错误时的重试次数          */

#define PLC_BUF_SIZE      512                /* PLC传输缓冲区的大小                */

/*----------------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**--------------------------------------------------------------------------------------------*/
PLC *apical  plc_alloc(void);
BOOL apical  plc_free(PLC * plc);
BOOL apical  open_plc(PLC * plc);                       
BOOL apical  shut_plc(PLC * plc);                 
BOOL apical  do_plc_action(PLC * plc, PLC_ACTION * action, INT16U flag);
BOOL apical  plc_set_param(PLC * plc, int type, INT08S * name,
                           void * private, int station_no,
                           INT16S base, INT16S irq, INT32S baudrate,
                           INT16S parity, INT16S databits, INT16S stopbits,
                           INT16S fifo_control
                          );
BOOL apical plc_set_action(PLC_ACTION * action, int id,
                           int cmd, INT08S * addr, 
                           void * associated, int number,
                           int timer, void (*finish)(int id, BOOL success),
                           int disabled
                          );

#endif /* #ifndef FAMES_PLC_H */

/*==============================================================================================
 * 
 * 本文件结束: plc.h
 * 
**============================================================================================*/

