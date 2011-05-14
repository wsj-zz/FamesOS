/***********************************************************************************************
 * 文件:    fatek.h
 *
 * 描述:    永宏PLC相关定义
 *
 * 作者:    Jun
***********************************************************************************************/
#ifndef FAMES_PLC_FATEK_H
#define FAMES_PLC_FATEK_H

/*----------------------------------------------------------------------------------------------
 * 
 *      永宏PLC编译开关
 * 
**--------------------------------------------------------------------------------------------*/
#define FAMES_PLC_FATEK_EN     1 

/*----------------------------------------------------------------------------------------------
 * 
 *      永宏PLC操作命令(Fatek标准协议)
 * 
**--------------------------------------------------------------------------------------------*/
#define FATEK_PLC_READ_R       1              /* 永宏PLC读命令,R寄存器              */    
#define FATEK_PLC_READ_DR      2              /* 永宏PLC读命令,DR寄存器             */ 
#define FATEK_PLC_READ_M       3              /* 永宏PLC读命令,M接点                */
#define FATEK_PLC_WRITE_R      4              /* 永宏PLC写命令,R寄存器              */
#define FATEK_PLC_WRITE_DR     5              /* 永宏PLC写命令,DR寄存器             */   
#define FATEK_PLC_WRITE_M      6              /* 永宏PLC写命令,M接点                */
#define FATEK_PLC_CONTROL_M    7              /* 永宏PLC单点运作控制                */
  #define FATEK_PLC_M_DISABLE  1              /* 永宏PLC单点运作控制之抑能          */
  #define FATEK_PLC_M_ENABLE   2              /* 永宏PLC单点运作控制之致能          */
  #define FATEK_PLC_M_SET      3              /* 永宏PLC单点运作控制之设定          */
  #define FATEK_PLC_M_RESET    4              /* 永宏PLC单点运作控制之清除          */
#define FATEK_PLC_RUN          8              /* 永宏PLC之RUN命令                   */
#define FATEK_PLC_STOP         9              /* 永宏PLC之STOP命令                  */

/*----------------------------------------------------------------------------------------------
 * 
 *      通讯协议相关定义(STX/ETX)
 * 
**--------------------------------------------------------------------------------------------*/
#define FATEK_PROTOCOL_STX (INT08S)0x2
#define FATEK_PROTOCOL_ETX (INT08S)0x3

/*----------------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**--------------------------------------------------------------------------------------------*/
#if FAMES_PLC_SVC_EN == 1 && FAMES_PLC_FATEK_EN == 1
void __sysonly plc_common_isr_fatek(PLC * plc);
BOOL __sysonly plc_control_fatek(PLC * plc, PLC_ACTION * action);
#else
#define  plc_common_isr_fatek(plc)
#define  plc_control_fatek(plc, action) fail
#endif

#endif  /* #ifndef FAMES_PLC_FATEK_H */

/*==============================================================================================
 * 
 * 本文件结束: fatek.h
 * 
**============================================================================================*/

