/*************************************************************************************
** 文件: all.h
** 说明: 全局定义文件，这里定义了整个系统各部分都将用到的数据
** 作者: Jun
** 时间: 2010-6-5
*************************************************************************************/
#ifndef  FAMES_ALL_H
#define  FAMES_ALL_H

/*
*确保某个函数只被调用一次-------------------------------------------------------------
*/
#define  CALLED_ONLY_ONCE()           \
              static INT16S called=0; \
              if(called!=0)return;    \
              called=1;

/*
*下面定义常用常量 --------------------------------------------------------------------
*/
#undef   TRUE
#define  TRUE          1                     /* 逻辑真, 代表成功                   */
#undef   FALSE
#define  FALSE         0                     /* 逻辑假, 代表失败                   */

#define  YES           1
#define  NO            0

#undef   ok
#define  ok           TRUE
#undef   fail
#define  fail         FALSE

#ifdef FAMES_INIT_C
const  INT08S HEXCHAR[]  ="0123456789ABCDEF";  /* Hex letters                      */
const  INT08U BITOFFSET[]={0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80}; /* Bit Position   */
const  INT16U BITOFF16[] ={0x1,  0x2,  0x4,  0x8,  0x10,  0x20,  0x40,  0x80,
                           0x100,0x200,0x400,0x800,0x1000,0x2000,0x4000,0x8000};
const  INT16U UnMapTbl[] ={0, 0, 1, 0,         /* 0000, 0001, 0010, 0011           */
                           2, 0, 1, 0,         /* 0100, 0101, 0110, 0111           */
                           3, 0, 1, 0,         /* 1000, 1001, 1010, 1011           */
                           2, 0, 1, 0};        /* 1100, 1101, 1110, 1111           */
#else
extern const INT08S HEXCHAR[];
extern const INT08U BITOFFSET[];
extern const INT16U BITOFF16[];
extern const INT16U UnMapTbl[];
#endif

#ifdef FAMES_INIT_C
INT16S    _argc_;   /* Parameters from system cmd-line */
INT08S ** _argv_;
INT08S ** _envs_;
#else
extern INT16S    _argc_;
extern INT08S ** _argv_;
extern INT08S ** _envs_;
#endif


/*
*优先级分配表, 最大不能超过12 --------------------------------------------------------
*/
#define PRIO_HIGHEST     0
#define PRIO_LOWEST     (MAX_PRIORITY-3)
#define PRIO_STARTUP     1
#define PRIO_CONTROL     1
#define PRIO_SYSTEM      1
#define PRIO_TIMER       1
#define PRIO_DEBUG       PRIO_SYSTEM
#define PRIO_MONITOR     PRIO_SYSTEM
#define PRIO_MAINLOOP    2
#define PRIO_PLC         2
#define PRIO_KEY         3
#define PRIO_GUI         8
#define PRIO_MOUSE       3
#define PRIO_NORMAL      6
#define PRIO_NET         10 
#define PRIO_SHARE       10
#define PRIO_NET_FAMES   10
#define PRIO_OFFICE      11


#endif                                        /* #ifndef FAMES_ALL_H               */

/*
*本文件结束: all.h ===================================================================
*/

