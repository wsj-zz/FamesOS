/*************************************************************************************
 * 文件:    init.h
 *
 * 描述:    初始化
 *
 * 作者:    Jun
 *
 * 时间:    2010-7-5
 *
 * 说明:    FamesOS在此处初始化, 并启动第一个用户任务(start)
*************************************************************************************/
#ifndef FAMES_INIT_H
#define FAMES_INIT_H

/*------------------------------------------------------------------------------------
 * 
 *          变量
 * 
**----------------------------------------------------------------------------------*/
#ifdef  FAMES_INIT_C
void interrupt (far *DOS_TimeTick)(void);   /* DOS原本的时钟中断向量               */
static jmp_buf   JumpBuf;                   /* 保存FamesOS运行前的系统状态         */
static BOOL      ExitAppFlag=NO;            /* 应用退出标志                        */
static INT16U    ElapsedOverhead=0;         /* 消耗时间校正值                      */
#endif 

/*------------------------------------------------------------------------------------
 * 
 *          函数声明
 * 
**----------------------------------------------------------------------------------*/
void   apical InitFamesOS(void);            /* 初始化FamesOS                       */
void   apical StartOS(void);                /* 开始运行                            */
void   apical ExitFamesOS(void);            /* 完全退出FamesOS的运行,返回DOS       */
void   apical InitApplication(void);        /* 初始化应用程序(须用户定义)          */
void   apical ExitApplication(void);        /* 退出应用程序的运行                  */
void   apical InitTimeTick(void);           /* 安装时钟滴答                        */
void   apical StopTimeTick(void);           /* 关闭时钟滴答                        */
void   interrupt TimeTick(void);            /* 时钟滴答ISR                         */
void   apical ElapsedInit(void);            /* 测算消耗时间初始化                  */
void   apical ElapsedStart(void);           /* 测算消耗时间开始                    */
INT16U apical ElapsedStop(void);            /* 测算消耗时间结束                    */
void   apical FamesDelay(INT32U);           /* FamesOS硬延时                       */

void   apical FamesSetVect(INT16S interruptno, void interrupt (far *isr)());
INT32U apical __FamesGetVect(INT16S interruptno);

#define       FamesGetVect(interruptno)  \
                    (void interrupt (far *)(void))(__FamesGetVect((interruptno)))

#define DELAY(us) FamesDelay(us)

/*------------------------------------------------------------------------------------
 * 
 *  用户启始任务, 由init创建, 由应用程序实现
 * 
**----------------------------------------------------------------------------------*/
void __task start(void *data);  


#endif /* #ifndef FAMES_INIT_H */
/*====================================================================================
 * 
 * 本文件结束: init.h
 * 
**==================================================================================*/


