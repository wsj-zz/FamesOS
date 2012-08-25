/*************************************************************************************
 * 文件:    type.h
 *
 * 说明:    基本数据类型定义
 *
 * 作者:    Jun
 *
 * 时间:    2010-6-24
*************************************************************************************/
#ifndef FAMES_DATATYPE_H
#define FAMES_DATATYPE_H

/*
*基本类型-----------------------------------------------------------------------------
*/
typedef          char    INT08S;              /* 8 位有符号整数                    */
typedef          int     INT16S;              /* 16位有符号整数                    */
typedef          long    INT32S;              /* 32位有符号整数                    */
typedef unsigned char    INT08U;              /* 8 位无符号整数                    */
typedef unsigned int     INT16U;              /* 16位无符号整数                    */
typedef unsigned long    INT32U;              /* 32位无符号整数                    */

typedef          float   FLOAT;               /* 单精度实数                        */
typedef          double  DOUBLE;              /* 双精度实数                        */
typedef          int     BOOLEAN;             /* 布尔值                            */
typedef          int     BOOL;                /* 布尔值                            */


/*
*常用类型-----------------------------------------------------------------------------
*/
typedef          char *  FILENAME;            /* 文件名                            */
typedef          char *  STRING;              /* 字符串                            */

/*
*调用约定-----------------------------------------------------------------------------
*/
#if   0
#define  apical  pascal                       /* FamesOS API的调用约定             */
#endif
#define  apical

/*
*函数标识-----------------------------------------------------------------------------
*/
#define  __internal                           /* 内部函数, 只在定义的文件中可用    */
#define  __export                             /* 可输出的函数, 其它文件也可用      */
#define  __arch                               /* 平台相关函数, 平台移植时需要重写  */
#define  __sysonly                            /* 只能用在系统中, 应用程序不可调用  */
#define  __userdef                            /* 需用户定义的函数                  */
#define  __init                               /* 初始化例程                        */
#define  __exit                               /* 退出例程                          */
#define  __task                               /* 是一个任务函数                    */
#define  __daemon                             /* 是一个后台任务函数                */

/*
*函数参数-----------------------------------------------------------------------------
*/
#define  __IN                                 /* 输入参数                           */
#define  __OUT                                /* 输出参数                           */
#define  __CONST const                        /* 只读参数                           */
#define  __NONULL                             /* 参数不能为空(尤其是指针)           */
#define  __BUF                                /* 参数是个buffer, 须有足够的空间     */

/*
*FamesOS系统类型----------------------------------------------------------------------
*/
typedef          int     STACK_TYPE;          /* 堆栈类型                          */

typedef void  (* TASK_TYPE)(void *);          /* 任务类型                          */
                                              
typedef void  (* DPC_FUNC)(void *, INT16S);   /* DPC回调函数                       */

typedef signed   int     HANDLE;              /* 句柄类型                          */

#define                  InvalidHandle   (-1) /* 无效句柄                          */


#endif                                        /* #ifndef FAMES_DATATYPE_H          */

/*====================================================================================
 * 
 * 本文件结束: type.h
 * 
**==================================================================================*/


