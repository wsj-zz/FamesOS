/*************************************************************************************
** 文件: hdware.h
** 说明: 与硬件相关的定义
** 作者: Jun
** 时间: 2010-02-07
** 版本: V0.1 (2010-02-07, 最初的版本)
*************************************************************************************/
#ifndef FAMES_HARDWARE_H
#define FAMES_HARDWARE_H

/*
*CPU类型定义 -------------------------------------------------------------------------
*/
#define CPU_NONE       0                      /* 没有CPU                            */
#define CPU_i386       3                      /* Intel 386                          */
#define CPU_80C51      5                      /* Intel 8051                         */
#define CPU_ARM7       7                      /* MCU ARM7                           */
#define CPU_ARM9       9                      /* MCU ARM9                           */
#define CPU_OTHER      99                     /* 别的CPU类型                        */

/*
*计算机类型定义 ----------------------------------------------------------------------
*/
#define HW_NONE        0                      /* ..NONE..                           */  
#define HW_PC          1                      /* PC机                               */
#define HW_OTHER       99                     /* 其它种类                           */

/*
*宿主操作系统类型 --------------------------------------------------------------------
*/
#define OS_NONE        0                      /* 没有宿主操作系统                   */
#define OS_DOS622      6                      /* 宿主操作系统为DOS6.22              */
#define OS_DOS7        7                      /* 宿主操作系统为DOS7                 */
#define OS_DOS         OS_DOS622              /* DOS                                */
#define OS_WIN9X       9                      /* Windows 9x                         */
#define OS_WINNT       10                     /* Windows NT                         */
#define OS_WINXP       15                     /* Windows XP                         */
#define OS_OTHER       99                     /* 其它                               */

/*
*当前所用的硬件环境 ------------------------------------------------------------------
*/
#define CPU_TYPE       CPU_i386               /* CPU 类型: Intel 386                */
#define HW_TYPE        HW_PC                  /* 机器类型: PC                       */
#define OS_TYPE        OS_DOS                 /* 宿主操作系统为DOS                  */

#endif

/*
*本文件结束: hdware.h ================================================================
*/


