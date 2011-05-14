/*************************************************************************************
 * 文件: irq.c
 *
 * 描述: IRQ管理及ISR管理(用于支持IRQ共享)
 *
 * 注意: 此文件必需以大模式编译(如: TCC -1 -B -ml ###.C)
 *
 * 作者: Jun
*************************************************************************************/
#define  FAMES_IRQ_C
#include "includes.h"

#if SHARED_IRQ == 1

/*------------------------------------------------------------------------------------
 * 函数: ISR_for_IRQ###
 *
 * 描述: 各IRQ的中断服务程序
**----------------------------------------------------------------------------------*/
#if IRQ_MAX_NO>=0 && IRQ_MIN_NO<=0
Define_ISR_vari_for_IRQ(0);
Define_ISR_name_for_IRQ(0)
{
    Define_ISR_body_for_IRQ(0);
}
#endif
#if IRQ_MAX_NO>=1 && IRQ_MIN_NO<=1
Define_ISR_vari_for_IRQ(1);
Define_ISR_name_for_IRQ(1)
{
    Define_ISR_body_for_IRQ(1);
}
#endif
#if IRQ_MAX_NO>=2 && IRQ_MIN_NO<=2
Define_ISR_vari_for_IRQ(2);
Define_ISR_name_for_IRQ(2)
{
    Define_ISR_body_for_IRQ(2);
}
#endif
#if IRQ_MAX_NO>=3 && IRQ_MIN_NO<=3
Define_ISR_vari_for_IRQ(3);
Define_ISR_name_for_IRQ(3)
{
    Define_ISR_body_for_IRQ(3);
}
#endif
#if IRQ_MAX_NO>=4 && IRQ_MIN_NO<=4
Define_ISR_vari_for_IRQ(4);
Define_ISR_name_for_IRQ(4)
{
    Define_ISR_body_for_IRQ(4);
}
#endif
#if IRQ_MAX_NO>=5 && IRQ_MIN_NO<=5
Define_ISR_vari_for_IRQ(5);
Define_ISR_name_for_IRQ(5)
{
    Define_ISR_body_for_IRQ(5);
}
#endif
#if IRQ_MAX_NO>=6 && IRQ_MIN_NO<=6
Define_ISR_vari_for_IRQ(6);
Define_ISR_name_for_IRQ(6)
{
    Define_ISR_body_for_IRQ(6);
}
#endif
#if IRQ_MAX_NO>=7 && IRQ_MIN_NO<=7
Define_ISR_vari_for_IRQ(7);
Define_ISR_name_for_IRQ(7)
{
    Define_ISR_body_for_IRQ(7);
}
#endif
#if IRQ_MAX_NO>=8 && IRQ_MIN_NO<=8
Define_ISR_vari_for_IRQ(8);
Define_ISR_name_for_IRQ(8)
{
    Define_ISR_body_for_IRQ(8);
}
#endif
#if IRQ_MAX_NO>=9 && IRQ_MIN_NO<=9
Define_ISR_vari_for_IRQ(9);
Define_ISR_name_for_IRQ(9)
{
    Define_ISR_body_for_IRQ(9);
}
#endif
#if IRQ_MAX_NO>=10 && IRQ_MIN_NO<=10
Define_ISR_vari_for_IRQ(10);
Define_ISR_name_for_IRQ(10)
{
    Define_ISR_body_for_IRQ(10);
}
#endif
#if IRQ_MAX_NO>=11 && IRQ_MIN_NO<=11
Define_ISR_vari_for_IRQ(11);
Define_ISR_name_for_IRQ(11)
{
    Define_ISR_body_for_IRQ(11);
}
#endif
#if IRQ_MAX_NO>=12 && IRQ_MIN_NO<=12
Define_ISR_vari_for_IRQ(12);
Define_ISR_name_for_IRQ(12)
{
    Define_ISR_body_for_IRQ(12);
}
#endif
#if IRQ_MAX_NO>=13 && IRQ_MIN_NO<=13
Define_ISR_vari_for_IRQ(13);
Define_ISR_name_for_IRQ(13)
{
    Define_ISR_body_for_IRQ(13);
}
#endif
#if IRQ_MAX_NO>=14 && IRQ_MIN_NO<=14
Define_ISR_vari_for_IRQ(14);
Define_ISR_name_for_IRQ(14)
{
    Define_ISR_body_for_IRQ(14);
}
#endif
#if IRQ_MAX_NO>=15 && IRQ_MIN_NO<=15
Define_ISR_vari_for_IRQ(15);
Define_ISR_name_for_IRQ(15)
{
    Define_ISR_body_for_IRQ(15);
}
#endif

/*------------------------------------------------------------------------------------
 * 函数: InitIrqManager()
 *
 * 描述: 初始化IRQ控制块及ISR控制块
**----------------------------------------------------------------------------------*/
void apical __init InitIrqManager(void)
{
    void __exit OnExit_IrqManager(void);
    INT16S i;
    
    CALLED_ONLY_ONCE();
    for(i=0; i<IRQ_MAX_NUM; i++){
        fames_irq[i].isr=LAST_ISR;
        fames_irq[i].old=ISRNULL;
        fames_irq[i].status=IRQ_STATUS_INIT;
        #if IRQ_STK_EN == 1
        __irq_stack[i]=NULL;
        __irq_stack_ss[i]=NULL;
        __irq_stack_sp[i]=NULL;
        #endif
    }
    for(i=0; i<ISR_MAX_NUM; i++){
        fames_isr[i].func=NULL;
        fames_isr[i].next=LAST_ISR;
        fames_isr[i].prev=LAST_ISR;
    }    
    #if IRQ_MAX_NO>=0 && IRQ_MIN_NO<=0
    set_irq_isr(0);
    #endif
    #if IRQ_MAX_NO>=1 && IRQ_MIN_NO<=1
    set_irq_isr(1);
    #endif
    #if IRQ_MAX_NO>=2 && IRQ_MIN_NO<=2
    set_irq_isr(2);
    #endif
    #if IRQ_MAX_NO>=3 && IRQ_MIN_NO<=3
    set_irq_isr(3);
    #endif
    #if IRQ_MAX_NO>=4 && IRQ_MIN_NO<=4
    set_irq_isr(4);
    #endif
    #if IRQ_MAX_NO>=5 && IRQ_MIN_NO<=5
    set_irq_isr(5);
    #endif
    #if IRQ_MAX_NO>=6 && IRQ_MIN_NO<=6
    set_irq_isr(6);
    #endif
    #if IRQ_MAX_NO>=7 && IRQ_MIN_NO<=7
    set_irq_isr(7);
    #endif
    #if IRQ_MAX_NO>=8 && IRQ_MIN_NO<=8
    set_irq_isr(8);
    #endif
    #if IRQ_MAX_NO>=9 && IRQ_MIN_NO<=9
    set_irq_isr(9);
    #endif
    #if IRQ_MAX_NO>=10 && IRQ_MIN_NO<=10
    set_irq_isr(10);
    #endif
    #if IRQ_MAX_NO>=11 && IRQ_MIN_NO<=11
    set_irq_isr(11);
    #endif
    #if IRQ_MAX_NO>=12 && IRQ_MIN_NO<=12
    set_irq_isr(12);
    #endif
    #if IRQ_MAX_NO>=13 && IRQ_MIN_NO<=13
    set_irq_isr(13);
    #endif
    #if IRQ_MAX_NO>=14 && IRQ_MIN_NO<=14
    set_irq_isr(14);
    #endif
    #if IRQ_MAX_NO>=15 && IRQ_MIN_NO<=15
    set_irq_isr(15);    
    #endif
    if(!RegisterOnExit(OnExit_IrqManager)){
        sys_print("InitIrqManager: failed to register OnExit routine!\n");
    }
    isr_used=0;
    irq_inited=YES;
}

/*------------------------------------------------------------------------------------
 * 函数: OnExit_IrqManager()
 *
 * 描述: IRQ管理模块的退出例程
**----------------------------------------------------------------------------------*/
void __exit OnExit_IrqManager(void)
{
    #if IRQ_STK_EN == 1
    INT16S i;
    
    for(i=0; i<IRQ_MAX_NUM; i++){
        lock_kernel();
        if(__irq_stack[i]){
            mem_free(__irq_stack[i]);
            __irq_stack[i]=NULL;
        }
        if(fames_irq[i].status==IRQ_STATUS_USED){
            if(i<8){
                outportbyte(0x21, inportbyte(0x21)|GetIrqMask(i));
            } else {
                outportbyte(0xA1, inportbyte(0xA1)|GetIrqMask(i));
            }
            FamesSetVect(GetIntNo(i), fames_irq[i].old);
            fames_irq[i].status=IRQ_STATUS_INIT;
        }
        unlock_kernel();
    }
    #endif
}

/*------------------------------------------------------------------------------------
 * 函数: __GetIsrSlot()
 *
 * 描述: 查找一个空闲的ISR控制块
 *
 * 输入: 无
 *
 * 输出: 空闲ISR控制块的序号(0~ISR_MAX_NUM-1), 或
 *       查找失败(-1)
**----------------------------------------------------------------------------------*/
INT16S __internal __GetIsrSlot(void)
{
    INT16S i;
    for(i=0; i<ISR_MAX_NUM; i++){
        if(fames_isr[i].func==NULL){
            fames_isr[i].func=(INT16S(*)())1;
            return i;
        }
    }
    return (INT16S)-1;
}

/*------------------------------------------------------------------------------------
 * 函数: IrqConnect()
 *
 * 描述: 注册中断服务程序
 *
 * 输入: irq  中断线
 *       func 中断服务函数指针
 *
 * 输出: ok/fail
**----------------------------------------------------------------------------------*/
INT16S apical IrqConnect(INT16S irq, INT16S (*func)(void)) 
{
    INT16S isr_no;
    
    if(!irq_inited){
        InitIrqManager();
    }
    if(irq<IRQ_MIN_NO || irq>IRQ_MAX_NO){
        SetErrorCode(ERROR_ISR_REG_IRQ);
        return fail;
    }
    if(func==NULL){
        SetErrorCode(ERROR_ISR_REG_FUNC);
        return fail;
    }
    if(isr_used>=ISR_MAX_NUM){                /* ISR控制块已分配完毕                */
        SetErrorCode(ERROR_ISR_REG_ISR_FULL);         
        return fail;
    }

    #if IRQ_STK_EN ==1
    if(__irq_stack[irq] == NULL){             /* 为IRQ独立栈申请内存空间            */
        lock_kernel();
        __irq_stack[irq] = (STACK_TYPE *)mem_alloc((INT32U)sizeof(STACK_TYPE)*IRQ_STK_SZE);
        unlock_kernel();
        if(!__irq_stack[irq]){
            SetErrorCode(ERROR_NO_MEM);
            return fail;
        }
       (*__irq_stack_ss[irq]) = (INT16U)FP_SEG(&__irq_stack[irq][0]);  
       (*__irq_stack_sp[irq]) = (INT16U)FP_OFF(&__irq_stack[irq][IRQ_STK_SZE-1]);/*lint !e507*/
       (*__irq_stack_sp[irq])&= (STACK_TYPE)(~0x1);
    }
    #endif
    
    isr_no=__GetIsrSlot();
    if(isr_no<0){                             /* 没有找到空闲的ISR控制块            */
        SetErrorCode(ERROR_ISR_REG_ISR_FULL);         
        return fail;
    }
    isr_used++;
    if(fames_irq[irq].status==IRQ_STATUS_USED){
        fames_isr[isr_no].func=func;
        fames_isr[isr_no].next=fames_irq[irq].isr;
        fames_isr[isr_no].prev=LAST_ISR;
        disable();
        fames_isr[fames_irq[irq].isr].prev=isr_no;
        fames_irq[irq].isr=isr_no;
        enable();
    } else {                                  /* 这个IRQ控制块还没有打开过          */
        fames_irq[irq].isr=isr_no;
        fames_isr[isr_no].func=func;
        fames_isr[isr_no].next=LAST_ISR;
        fames_isr[isr_no].prev=LAST_ISR;
        disable();
        fames_irq[irq].old=FamesGetVect(GetIntNo(irq));
        FamesSetVect(GetIntNo(irq), irq_isr[irq]);
        if(irq<8){
            outportbyte(0x21, inportbyte(0x21)&(~(INT08U)GetIrqMask(irq)));
        } else {
            outportbyte(0x21, inportbyte(0x21)&(~(INT08U)GetIrqMask(2)));
            outportbyte(0xA1, inportbyte(0xA1)&(~(INT08U)GetIrqMask(irq)));
        }
        enable();
        fames_irq[irq].status=IRQ_STATUS_USED;
    }
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数: IrqDisConnect()
 *
 * 描述: 注销中断服务程序
 *
 * 输入: irq  中断线
 *       func 已注册过的中断服务函数指针
 *
 * 输出: ok/fail
**----------------------------------------------------------------------------------*/
INT16S apical IrqDisConnect(INT16S irq, INT16S (*func)(void)) 
{
    INT16S isr_no;
    
    if(!irq_inited){
        InitIrqManager();
    }
    if(irq<IRQ_MIN_NO || irq>IRQ_MAX_NO){
        SetErrorCode(ERROR_ISR_REG_IRQ);
        return fail;
    }
    if(func==NULL){
        SetErrorCode(ERROR_ISR_REG_FUNC);
        return fail;
    }
    if(isr_used<=0){                          /* ISR控制块已分配完毕                */
        SetErrorCode(ERROR_ISR_REG_ISR_FULL);         
        return fail;
    }
    if(fames_irq[irq].status==IRQ_STATUS_USED){
        isr_no=fames_irq[irq].isr;
        while(isr_no!=LAST_ISR){
            #ifdef FAMES_DEBUG
            gotoxy(1,13);
            printf("%08lX,%08lX,%08lX,%08lX,%08lX,%08lX,%08lX,%08lX",
                   fames_isr[0].func,
                   fames_isr[1].func,
                   fames_isr[2].func,
                   fames_isr[3].func,
                   fames_isr[4].func,
                   fames_isr[5].func,
                   fames_isr[6].func,
                   fames_isr[7].func
            );
            gotoxy(1,14);
            printf("fames_isr[%d].func=%08lX, func=%08lX",
               isr_no,
               fames_isr[isr_no].func, func
            );
            getch();
            #endif
            if(fames_isr[isr_no].func==func){
                disable();
                if(fames_isr[isr_no].prev==LAST_ISR){ /* 这个ISR是本IRQ的第一个ISR  */
                    INT16S t_isr_no;
                    t_isr_no=fames_irq[irq].isr=fames_isr[isr_no].next;
                    if(t_isr_no==LAST_ISR){   /* 这个IRQ上已经没有ISR了             */
                        if(irq<8){
                            outportb(0x21, inportb(0x21)|GetIrqMask(irq));
                        } else {
                            outportb(0xA1, inportb(0xA1)|GetIrqMask(irq));
                        }
                        FamesSetVect(GetIntNo(irq), fames_irq[irq].old);
                        fames_irq[irq].status=IRQ_STATUS_INIT;
                    } else {
                        fames_isr[t_isr_no].prev=LAST_ISR;
                    }
                } else {
                    INT16S t_isr_no, t_isr_no2;
                    t_isr_no=fames_isr[isr_no].prev;
                    t_isr_no2=fames_isr[t_isr_no].next=fames_isr[isr_no].next;
                    if(t_isr_no2!=LAST_ISR){
                        fames_isr[t_isr_no2].prev=t_isr_no;
                    }
                }
                fames_isr[isr_no].func=NULL;
                fames_isr[isr_no].next=LAST_ISR;
                fames_isr[isr_no].prev=LAST_ISR;
                isr_used--;
                enable();
                break;
            } else {
                isr_no=fames_isr[isr_no].next;
            }
        }
        if(isr_no==LAST_ISR){
            SetErrorCode(ERROR_ISR_NOT_FOUND);
            return fail;            
        }
    } else {
        SetErrorCode(ERROR_IRQ_NOT_USED);
        return fail;
    }
    return ok;
}


#endif /* #if SHARED_IRQ == 1 */

/*====================================================================================
 * 
 * 本文件结束: irq.c
 * 
**==================================================================================*/

