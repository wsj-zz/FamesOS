/*************************************************************************************
 * �ļ�: irq.h
 *
 * ����: IRQ����ISR����(����֧��IRQ����)
 *
 * ע��: ���ļ������Դ�ģʽ����(��: TCC -1 -B -ml ###.C)
 *
 * ����: Jun
*************************************************************************************/
#ifndef FAMES_IRQ_H
#define FAMES_IRQ_H


/*
*IRQ����Ŀ���------------------------------------------------------------------------
*/
#define SHARED_IRQ   1                        /* IRQ����֧��,1=֧��                 */

#define IRQ_STK_EN   1                        /* IRQ����ջ�ռ������, 1=����        */

/*
*���涨��IRQ������صĽṹ������------------------------------------------------------
*/
#define ISR_MAX_NUM 12                        /* ISR������                        */
#define IRQ_MAX_NUM 16                        /* IRQ������                        */
#define IRQ_STK_SZE 1024                      /* IRQ����ջ�ռ��С                  */

#define IRQ_MAX_NO  (IRQ_MAX_NUM-1)           /* IRQ����ʹ�����                  */
#define IRQ_MIN_NO   3                        /* IRQ��С��ʹ�����(�����Ϊ3)       */

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
*����ΪIRQ����֧������Ҫ�Ķ���--------------------------------------------------------
*/
#if SHARED_IRQ == 1

#define LAST_ISR ((INT16S)-1)                 /* ָʾ���ISR�Ǳ�IRQ�����һ��       */ 

#define ISRNULL  (void interrupt (far *)(void))0L
                                              /* ָ���жϷ�������"��ָ��"         */

struct _ISR_ {                                /* �жϷ��������ƽṹ(ISR���ƿ�)    */
    INT16S (*func)(void);                     /* �жϷ���֮���ܺ���                 */
    INT16S   next;
    INT16S   prev;
};

struct _IRQ_ {                                /* IRQ����ṹ(IRQ���ƿ�)             */
    void   interrupt (far *old)(void);        /* IRQ����Ӧ֮ԭ�ж�����              */
    INT16S   isr;                             /* IRQ�ĵ�һ��ISR���ƿ�               */
    INT16S   status;                          /* IRQ״̬                            */
};

#define IRQ_STATUS_INIT  0                    /* IRQ״̬֮�ѳ�ʼ��                  */
#define IRQ_STATUS_USED  1                    /* IRQ״̬֮��ʹ��                    */

#ifdef FAMES_IRQ_C
static  INT16S irq_inited=NO;                 /* IRQ�ѳ�ʼ�����                    */
static  INT16S isr_used;                      /* ����ISR���ƿ���Ŀ                  */
struct _ISR_   fames_isr[ISR_MAX_NUM];
struct _IRQ_   fames_irq[IRQ_MAX_NUM];
void interrupt (far *irq_isr[IRQ_MAX_NUM])(void); 
                                              /* ��IRQ���жϷ������                */
static STACK_TYPE * __irq_stack[IRQ_MAX_NUM] = { NULL };
                                              /* IRQ�жϷ������Ķ���ջ�ռ�        */
static INT16U * __irq_stack_ss[IRQ_MAX_NUM] = { NULL };
static INT16U * __irq_stack_sp[IRQ_MAX_NUM] = { NULL }; 
#endif /* #ifdef FAMES_IRQ_C */

#ifdef FAMES_IRQ_C
/*
*��IRQ���жϷ������֮����------------------------------------------------------------
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
*��irq_isr_##irq����irq_isr����-------------------------------------------------------
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
*IRQ���жϺ�--------------------------------------------------------------------------
*/
#if      IRQ_MAX_NO < 8                       /* IRQ������С��8ʱ,ֻ�������8259A */
#define  GetIntNo(irq)     ((irq)+8)          /* ͨ��IRQ�õ��жϺ�                  */
#define  GetIrqMask(irq)   (0x1<<(irq))       /* ͨ��IRQ�õ���Ӧ8259A���ж�������   */
#else
#define  GetIntNo(irq)     ((irq)+((irq)<8?0x8:0x68))
#define  GetIrqMask(irq)   (0x1<<((irq)&7))
#endif

/*
*����ΪIRQ���������������----------------------------------------------------------
*/
void   apical __init InitIrqManager(void);                            
INT16S apical IrqConnect(INT16S irq, INT16S (*func)(void));
INT16S apical IrqDisConnect(INT16S irq, INT16S (*func)(void));


#endif /* #ifndef FAMES_IRQ_H */

/*====================================================================================
 * 
 * ���ļ�����: irq.h
 * 
**==================================================================================*/

