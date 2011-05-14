/***********************************************************************************************
 * �ļ�:    plc.h
 *
 * ˵��:    PLCͨ�ö���
 *
 * ����:    Jun
 *
 * ʱ��:    2010-9-2
***********************************************************************************************/
#ifndef FAMES_PLC_H
#define FAMES_PLC_H

/*----------------------------------------------------------------------------------------------
 * 
 * ֧�ֵ�PLC����
 * 
**--------------------------------------------------------------------------------------------*/
#define PLC_TYPE_FATEK       1     /* ����PLC   */       
#define PLC_TYPE_SIEMENS     2     /* ������PLC */


/*----------------------------------------------------------------------------------------------
 *          plc_action_s
 * 
 * ����: PLC�����ṹ, ��������λ����PLC��ĳ��ͨѶ
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

#define  PLC_ACTION_FLAG_DISABLE    0x10    /* ���� */

/*----------------------------------------------------------------------------------------------
 *          plc_descriptor_s
 * 
 * ����: PLC���ƿ�ṹ, ��λ����PLC��ͨѶ�Դ�Ϊ����
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
 *      PLCͨѶ��ض���
 * 
**--------------------------------------------------------------------------------------------*/
#define PLC_RETRY_RX_TO    3                 /* ����PLC���ݳ�ʱ�����Դ���          */  
#define PLC_RETRY_ERR      3                 /* PLC�����д���ʱ�����Դ���          */

#define PLC_BUF_SIZE      512                /* PLC���仺�����Ĵ�С                */

/*----------------------------------------------------------------------------------------------
 * 
 *      ��������
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
 * ���ļ�����: plc.h
 * 
**============================================================================================*/

