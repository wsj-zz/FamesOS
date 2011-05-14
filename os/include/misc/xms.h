/*************************************************************************************
 * 文件: xms.h
 *
 * 描述: 扩展内存(XMS)相关定义
 *
 * 作者: Jun
 *
 * 时间: 2010-03-03
 *
 * 版本: V0.1 (2010-03-03, 最初的版本)
*************************************************************************************/
#ifndef FAMES_XMS_H
#define FAMES_XMS_H

/*
*XMS启用开关--------------------------------------------------------------------------
*/
#define XMS_EN   1                            /* 1=启用, 这里启用                   */

#if    XMS_EN == 1
/*
*下面定义XMS相关的结构及变量----------------------------------------------------------
*/
#define XMSHANDLE  INT16U

struct EMB {                                  /* EMB结构                            */
    INT32S  Length;
    INT16U  SourceHandle;
    INT32S  SourceOfs;
    INT16U  DestinationHandle;
    INT32S  DestinationOfs;
};
#ifdef FAMES_XMS_C
static INT16U XMSOK=0;                        /* XMS有效标志                        */
INT16U (* XMSaddr)(void);                     /* XMS功能例程入口地址                */
#endif                                        /* #ifdef  FAMES_XMS_C                */

/*
*下面为XMS相关函数的声明--------------------------------------------------------------
*/
INT16U  XMSinit(void);                        /* XMS初始化                          */
INT16U  XMSfreesize(void);                    /* 取当前自由扩展内存总的大小         */
INT16U  XMSlargestblock(void);                /* 取当前最大的扩展内存块的大小       */
INT16U  XMS(INT16U size);                     /* 分配一块扩展内存                   */
INT16U  _XMS(INT16U handle);                  /* 释放扩展内存块                     */
INT16U  XMSrealloc(INT16U handle,             /* 以新的尺寸重新分配扩展内存         */
                   INT16U size);
INT16U  XMSmove(struct EMB *emb);             /* 移动内存块                         */
INT16U  XMSput(INT16U handle, void * dp,      /* 写扩展内存                         */
               void * sp, INT32S length);
INT16U  XMSget(INT16U handle, void * dp,      /* 读扩展内存                         */
               void * sp, INT32S length);

XMSHANDLE apical XMSalloc(INT16U size);
void      apical XMSfree(XMSHANDLE handle);
BOOL apical __init init_xms_service(void);


#endif                                        /* #if     XMS_EN == 1                */

#endif                                        /* #ifndef FAMES_XMS_H                */

/*====================================================================================
 * 
 * 本文件结束: xms.h
 * 
**==================================================================================*/


