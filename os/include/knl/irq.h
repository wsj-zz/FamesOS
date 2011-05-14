/*************************************************************************************
 * 文件: irq.h
 *
 * 描述: IRQ管理及ISR管理(用于支持IRQ共享)
 *
 * 注意: 此文件必需以大模式编译(如: TCC -1 -B -ml ###.C)
 *
 * 作者: Jun
*************************************************************************************/
#ifndef FAMES_IRQ_H
#define FAMES_IRQ_H


/*
*IRQ共享的开关------------------------------------------------------------------------
*/
#define SHARED_IRQ   1                        /* IRQ共享支持,1=支持                 */

#define IRQ_STK_EN   1                        /* IRQ独立栈空间的启用, 1=启用        */

/*
*下面定义IRQ管理相关的结构及变量------------------------------------------------------
*/
#define ISR_MAX_NUM 12                        /* ISR最大个数                        */
#define IRQ_MAX_NUM 16                        /* IRQ最大个数                        */
#define IRQ_STK_SZE 1024                      /* IRQ独立栈空间大小                  */

#define IRQ_MAX_NO  (IRQ_MAX_NUM-1)           /* IRQ最大可使用序号                  */
#define IRQ_MIN_NO   3                        /* IRQ最小可使用序号(最好设为3)       */

#if HW_TYPE == HW_PC
#if IRQ_MAX_NUM > 16
#error IRQ_MAX_NUM is too large (>16)
#endif
#if IRQ_MAX_NO < IRQ_MIN_NO
#error IRQ_MAX_NO must be greater than IRQ_MIN_NO!
#endif
#if IRQ_MIN_NO <= 2
#error IRQ_MIN_NO must be greater than IRQ2
#endif
#endif

/*
*下面为IRQ共享支持所需要的定义--------------------------------------------------------
*/
#if SHARED_IRQ == 1

#define LAST_ISR ((INT16S)-1)                 /* 指示这个ISR是本IRQ的最后一个       */ 

#define ISRNULL  (void interrupt (far *)(void))0L
                                              /* 指向中断服务函数的"空指针"         */

struct _ISR_ {                                /* 中断服务函数控制结构(ISR控制块)    */
    INT16S (*func)(void);                     /* 中断服务之功能函数                 */
    INT16S   next;
    INT16S   prev;
};

struct _IRQ_ {                                /* IRQ管理结构(IRQ控制块)             */
    void   interrupt (far *old)(void);        /* IRQ所对应之原中断向量              */
    INT16S   isr;                             /* IRQ的第一个ISR控制块               */
    INT16S   status;                          /* IRQ状态                            */
};

#define IRQ_STATUS_INIT  0                    /* IRQ状态之已初始化                  */
#define IRQ_STATUS_USED  1                    /* IRQ状态之已使用                    */

#ifdef FAMES_IRQ_C
static  INT16S irq_inited=NO;                 /* IRQ已初始化标记                    */
static  INT16S isr_used;                      /* 已用ISR控制块数目                  */
struct _ISR_   fames_isr[ISR_MAX_NUM];
struct _IRQ_   fames_irq[IRQ_MAX_NUM];
void interrupt (far *irq_isr[IRQ_MAX_NUM])(void); 
                                              /* 各IRQ的中断服务程序                */
static STACK_TYPE * __irq_stack[IRQ_MAX_NUM] = { NULL };
                                              /* IRQ中断服务程序的独立栈空间        */
static INT16U * __irq_stack_ss[IRQ_MAX_NUM] = { NULL };
static INT16U * __irq_stack_sp[IRQ_MAX_NUM] = { NULL }; 
#endif /* #ifdef FAMES_IRQ_C */

#ifdef FAMES_IRQ_C
/*
*各IRQ的中断服务程序之定义------------------------------------------------------------
*/
#define send_auto_eoi(this_irq)                       \
          if(this_irq<8){                             \
              outportbyte(0x20, 0x20);                \
          } else {                                    \
              outportbyte(0xA0, 0x20);                \
              outportbyte(0x20, 0x20);                \
          }    

#if     IRQ_STK_EN == 1
/*lint -save -esym(551, s_sp_*, s_ss_*, v_sp_*, v_ss_*)  */
/*lint       -esym(528, s_sp_*, s_ss_*, v_sp_*, v_ss_*)  */
/*lint       -esym(528, __isr_no_*, __retv_irq_*) */
/*lint       -esym(528, __this_irq_*)   */
/*lint       -esym(551, __irq_stack_*)  */
#define Define_ISR_vari_for_IRQ(irq)                  \
          static INT16S __isr_no_##irq;               \
          static INT16S __retv_irq_##irq;             \
          static INT16S __this_irq_##irq;             \
          static INT16U v_ss_##irq, v_sp_##irq;       \
          static INT16U s_ss_##irq, s_sp_##irq
/*lint -restore*/
#else
#define Define_ISR_vari_for_IRQ(irq)                  \
          static INT16S __isr_no_##irq;               \
          static INT16S __retv_irq_##irq;             \
          static INT16S __this_irq_##irq
#endif /* #if IRQ_STK_EN==1  */

#define Define_ISR_name_for_IRQ(irq)                  \
          void interrupt far irq_isr_##irq(void)
          
#if     IRQ_STK_EN == 1
#define Define_ISR_body_for_IRQ(irq)                  \
              asm mov v_ss_##irq, ss;                 \
              asm mov v_sp_##irq, sp;                 \
              asm mov ss, s_ss_##irq;                 \
              asm mov sp, s_sp_##irq;                 \
              __this_irq_##irq = irq;                 \
              IntNesting++;                           \
              __isr_no_##irq = fames_irq[irq].isr;    \
              for(;;){                                \
                if(__isr_no_##irq == LAST_ISR)break;  \
                if(fames_isr[__isr_no_##irq].func){   \
                    __retv_irq_##irq = (*fames_isr[__isr_no_##irq].func)(); \
                    if(__retv_irq_##irq==ok)break;    \
                }                                     \
                __isr_no_##irq=fames_isr[__isr_no_##irq].next; \
              }                                       \
              send_auto_eoi(__this_irq_##irq);        \
              DISABLE_INT();                          \
              IntNesting--;                           \
              asm mov ss, v_ss_##irq;                 \
              asm mov sp, v_sp_##irq;                 \
              return;
#else  /* #else IRQ_STK_EN==1 */
#define Define_ISR_body_for_IRQ(irq)                  \
              __this_irq_##irq = irq;                 \
              IntNesting++;                           \
              __isr_no_##irq = fames_irq[irq].isr;    \
              for(;;){                                \
                if(__isr_no_##irq == LAST_ISR)break;  \
                if(fames_isr[__isr_no_##irq].func){   \
                    __retv_irq_##irq = (*fames_isr[__isr_no_##irq].func)(); \
                    if(__retv_irq_##irq==ok)break;    \
                }                                     \
                __isr_no_##irq=fames_isr[__isr_no_##irq].next; \
              }                                       \
              send_auto_eoi(__this_irq_##irq);        \
              DISABLE_INT();                          \
              IntNesting--;                           \
              return;
#endif /* #if IRQ_STK_EN==1 */

          
/*
*用irq_isr_##irq设置irq_isr数组-------------------------------------------------------
*/
/*lint -save -emacro(717, set_irq_isr)*/
#if     IRQ_STK_EN == 1
#define set_irq_isr(irq)  do{   \
                             irq_isr[irq]=irq_isr_##irq; \
                             __irq_stack_ss[irq]=&s_ss_##irq; \
                             __irq_stack_sp[irq]=&s_sp_##irq; \
                             __irq_stack[irq] = NULL; }while(0)
#else
#define set_irq_isr(irq)  do{ irq_isr[irq]=irq_isr_##irq; }while(0)
#endif /* #if IRQ_STK_EN==1 */
/*lint -restore */

#endif                                        /* #ifdef FAMES_IRQ_C                 */

/*lint -save -emacro(717, isr_return_true, isr_return_false)*/
#define __isr__  INT16S                       /* definition of a key word for isr   */
#define isr_return_true()  do{ IntNesting--; return ok;   }while(0) 
                                              /* return true  in isr                */
#define isr_return_false() do{ IntNesting--; return fail; }while(0) 
                                              /* return false in isr                */
#else                                         /* #if SHARED_IRQ == 1                */
#define __isr__  void far interrupt
#define isr_return_true()  do{ send_auto_eoi(this_irq); IntNesting--; return; }while(0)
#define isr_return_false() do{ send_auto_eoi(this_irq); IntNesting--; return; }while(0)
#endif                                        /* #if SHARED_IRQ == 1                */
/*lint -restore */

#define enter_isr()  IntNesting++ 

/*
*IRQ与中断号--------------------------------------------------------------------------
*/
#if      IRQ_MAX_NO < 8                       /* IRQ最大序号小于8时,只需操作主8259A */
#define  GetIntNo(irq)     ((irq)+8)          /* 通过IRQ得到中断号                  */
#define  GetIrqMask(irq)   (0x1<<(irq))       /* 通过IRQ得到对应8259A的中断屏蔽字   */
#else
#define  GetIntNo(irq)     ((irq)+((irq)<8?0x8:0x68))
#define  GetIrqMask(irq)   (0x1<<((irq)&7))
#endif

/*
*下面为IRQ管理服务函数的声明----------------------------------------------------------
*/
void   apical __init InitIrqManager(void);                            
INT16S apical IrqConnect(INT16S irq, INT16S (*func)(void));
INT16S apical IrqDisConnect(INT16S irq, INT16S (*func)(void));


#endif /* #ifndef FAMES_IRQ_H */

/*====================================================================================
 * 
 * 本文件结束: irq.h
 * 
**==================================================================================*/

