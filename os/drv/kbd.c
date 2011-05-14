/*************************************************************************************
 * 文件: kbd.c
 *
 * 说明: 键盘驱动
 *
 * 作者: Jun
 *
 * 时间: 2010-04-13
 *
 * 版本: V0.1 (2010-04-13, 最初的版本)
*************************************************************************************/
#define  FAMES_KEYBOARD_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 *
 * 旧的键盘中断ISR
 *
**----------------------------------------------------------------------------------*/
#if  OS_TYPE == OS_DOS
void interrupt (far *OldKeyboardISR)(void) = NULL;
void interrupt (far *OldCtrlC_ISR)(void)   = NULL;
#endif

#if  OS_TYPE == OS_DOS
/*------------------------------------------------------------------------------------
 * 函数:    KeyboardISR()
 *
 * 说明:    新的键盘中断ISR
 *
 * 笔记:    在原来的键盘中断服务例程中会打开中断(sti), 这样当时钟中断到来时(IRQ0),
 *          便会打断键盘中断, 且在IntNesting等于0时进行任务切换, 开始执行新的任务,
 *          但这时, 键盘中断还没有执行完毕, 并且直到上一任务被再次调度后才能继续执行
 *          下去, 这中间可能会产生错误(待续)
**----------------------------------------------------------------------------------*/
void interrupt KeyboardISR(void)
{
    IntNesting++;
    (*OldKeyboardISR)();
    IntNesting--;
    return;
}

/*------------------------------------------------------------------------------------
 * 函数:    CtrlC_ISR()
 *
 * 说明:    Ctrl-C之ISR
 *
 * 笔记:    在默认情况下, 按下Ctrl-C会退出应用程序, 而FamesOS是不能受Ctrl-C影响的
 *          所以, 我们需要屏蔽这个功能, 即用这个ISR函数取代中断0x23的中断向量.
**----------------------------------------------------------------------------------*/
void interrupt CtrlC_ISR(void)
{
    return;
}
#define CtrlC_IntNo  0x23

#endif

/*------------------------------------------------------------------------------------
 * 函数:    InitKeyboard()
 *
 * 说明:    安装键盘中断服务例程
**----------------------------------------------------------------------------------*/
void apical __init InitKeyboard(void)
{
    CALLED_ONLY_ONCE();
    #if OS_TYPE == OS_DOS
    DISABLE_INT();
    OldKeyboardISR = FamesGetVect(0x9);
    FamesSetVect(0x9, KeyboardISR);
    OldCtrlC_ISR   = FamesGetVect(CtrlC_IntNo);
    FamesSetVect(CtrlC_IntNo, CtrlC_ISR);
    ENABLE_INT();
    #endif
}

/*------------------------------------------------------------------------------------
 * 函数:    StopKeyboard()
 *
 * 说明:    取消(或复原)键盘中断服务例程
**----------------------------------------------------------------------------------*/
void apical __exit StopKeyboard(void)
{
    #if OS_TYPE == OS_DOS
    DISABLE_INT();
    FamesSetVect(0x9, OldKeyboardISR);
    FamesSetVect(CtrlC_IntNo, OldCtrlC_ISR);
    ENABLE_INT();
    #endif
}

/*------------------------------------------------------------------------------------
 * 函数:    GetKeyFromKeyBoard()
 *
 * 说明:    发送一个键码到键盘缓冲
 *
 * 输入:    keycode 键码
 *
 * 输出:    ok/fail
**----------------------------------------------------------------------------------*/
BOOL apical PutKeyToKeyBoard(KEYCODE keycode)
{
    BOOL retval;

    DispatchLock();
    asm push cx;
    asm mov  cx, keycode;
    asm mov  ah, 5;
    asm int  0x16;
    asm xor  ah, ah
    asm mov  retval, ax;
    asm pop  cx
    DispatchUnlock();

    if(retval==0) /*lint !e530*/
        retval=ok;
    else 
        retval=fail;

    return retval;
}


/*------------------------------------------------------------------------------------
 * 函数:    GetKeyFromKeyBoard()
 *
 * 说明:    取本地键盘按键的键码
 *
 * 输入:    无
 *
 * 输出:    KEYCODE(NONEKEY代表无按键)
**----------------------------------------------------------------------------------*/
KEYCODE apical GetKeyFromKeyBoard(void)
{ /*lint --e{801,644,563,529} */
    KEYCODE key;
    static  INT08U KSTS=0;  /* 键盘控制键(shift, ctrl, alt)的状态 */
    
    DispatchLock();
    asm mov  ah, 1
    asm int  0x16
    asm jnz  KeyAvailable
    asm mov  ah, 2
    asm int  0x16
    asm test al, 3
    asm jz   statuskey
    asm or   al, 3          /* 使两边的SHIFT键功能一样            */
    statuskey:
    asm xchg al, KSTS
    asm xor  al, KSTS
    asm and  al, 0xF
    asm jz   none_key
    asm mov  ah, KSTS
    asm and  ax, 0xF00
    asm add  ah, 0xF0
    asm mov  key,ax
    goto exit_ret;

    KeyAvailable:
    asm mov  ah, 0
    asm int  0x16
    asm and  al, al
    asm jz   set_retval
    asm cmp  al, 0xe0
    asm jz   set_retval
    asm xor  ah, ah
    set_retval:
    asm mov  key,ax
    goto exit_ret;

    none_key:
    key=NONEKEY;
    exit_ret:
    DispatchUnlock();

    return key;
}


/*====================================================================================
 * 
 * 本文件结束: kbd.c
 * 
**==================================================================================*/

