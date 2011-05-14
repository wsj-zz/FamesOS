/*************************************************************************************
 * �ļ�:    knl/core.h
 *
 * ����:    �ں�ѡ��
 *
 * ����:    Jun
 *
 * ʱ��:    2011-2-16
 *
 * ˵��:    ������ѡ��ں˱�����
*************************************************************************************/
#ifndef FAMES_CORE_H
#define FAMES_CORE_H

/*------------------------------------------------------------------------------------
 * 
 *          ������ѡ��
 * 
**----------------------------------------------------------------------------------*/
#define DISPATCH_PREEMPTIVE_EN  1     /* ����ʽ����,     1=֧��              */
#define DISPATCH_ROUNDROBIN_EN  1     /* ʱ��Ƭ��ת����, 1=֧��              */

/*------------------------------------------------------------------------------------
 * 
 *          �ں���ض���
 * 
**----------------------------------------------------------------------------------*/
#define FamesOS         0x80          /* FamesOSϵͳ�жϺ�                   */
#define TickIntNo       0x08          /* ʱ���жϺ�                          */
#define TicksPerSec     1000          /* ÿ��δ���(��̶�Ϊ1000)            */

#define MAX_TASKS       16            /* ���������                          */
#define MAX_PRIORITY    15            /* ������ȼ���[1,32]                  */
#if    (MAX_PRIORITY<1)||(MAX_PRIORITY>32)
#error  MAX_PRIORITY does not support this value, Please check it!
#endif

/*------------------------------------------------------------------------------------
 * 
 *          �ں˱���
 * 
**----------------------------------------------------------------------------------*/
#ifdef  FAMES_INIT_C
INT16S  IntNesting       = 0;         /* �ж�Ƕ�ײ���                        */
INT16S  LockNesting      = 0;         /* ���������                          */
INT32U  SecondsFromStart = 0UL;       /* �ӿ�����������еĺ�����            */
BOOL    FamesOSStarted   = NO;        /* FamesOS�Ƿ�������                   */
#if     DISPATCH_ROUNDROBIN_EN == 1
INT16S  TimeSlice        = 0;         /* ʱ��Ƭ��С, 0=�ر�ʱ��Ƭ��ת����    */
#endif
#else
extern  INT16S  IntNesting;
extern  INT16S  LockNesting;
extern  INT32U  SecondsFromStart;
extern  BOOL    FamesOSStarted;
#if     DISPATCH_ROUNDROBIN_EN == 1
extern  INT16S  TimeSlice;
#endif
#endif /* FAMES_INIT_C */


#endif /* #ifndef FAMES_CORE_H */
/*====================================================================================
 * 
 * ���ļ�����: knl/core.h
 * 
**==================================================================================*/


