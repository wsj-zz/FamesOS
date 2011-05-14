/*************************************************************************************
 * �ļ�: kbd.c
 *
 * ˵��: ��������
 *
 * ����: Jun
 *
 * ʱ��: 2010-04-13
 *
 * �汾: V0.1 (2010-04-13, ����İ汾)
*************************************************************************************/
#define  FAMES_KEYBOARD_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 *
 * �ɵļ����ж�ISR
 *
**----------------------------------------------------------------------------------*/
#if  OS_TYPE == OS_DOS
void interrupt (far *OldKeyboardISR)(void) = NULL;
void interrupt (far *OldCtrlC_ISR)(void)   = NULL;
#endif

#if  OS_TYPE == OS_DOS
/*------------------------------------------------------------------------------------
 * ����:    KeyboardISR()
 *
 * ˵��:    �µļ����ж�ISR
 *
 * �ʼ�:    ��ԭ���ļ����жϷ��������л���ж�(sti), ������ʱ���жϵ���ʱ(IRQ0),
 *          ����ϼ����ж�, ����IntNesting����0ʱ���������л�, ��ʼִ���µ�����,
 *          ����ʱ, �����жϻ�û��ִ�����, ����ֱ����һ�����ٴε��Ⱥ���ܼ���ִ��
 *          ��ȥ, ���м���ܻ��������(����)
**----------------------------------------------------------------------------------*/
void interrupt KeyboardISR(void)
{
    IntNesting++;
    (*OldKeyboardISR)();
    IntNesting--;
    return;
}

/*------------------------------------------------------------------------------------
 * ����:    CtrlC_ISR()
 *
 * ˵��:    Ctrl-C֮ISR
 *
 * �ʼ�:    ��Ĭ�������, ����Ctrl-C���˳�Ӧ�ó���, ��FamesOS�ǲ�����Ctrl-CӰ���
 *          ����, ������Ҫ�����������, �������ISR����ȡ���ж�0x23���ж�����.
**----------------------------------------------------------------------------------*/
void interrupt CtrlC_ISR(void)
{
    return;
}
#define CtrlC_IntNo  0x23

#endif

/*------------------------------------------------------------------------------------
 * ����:    InitKeyboard()
 *
 * ˵��:    ��װ�����жϷ�������
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
 * ����:    StopKeyboard()
 *
 * ˵��:    ȡ��(��ԭ)�����жϷ�������
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
 * ����:    GetKeyFromKeyBoard()
 *
 * ˵��:    ����һ�����뵽���̻���
 *
 * ����:    keycode ����
 *
 * ���:    ok/fail
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
 * ����:    GetKeyFromKeyBoard()
 *
 * ˵��:    ȡ���ؼ��̰����ļ���
 *
 * ����:    ��
 *
 * ���:    KEYCODE(NONEKEY�����ް���)
**----------------------------------------------------------------------------------*/
KEYCODE apical GetKeyFromKeyBoard(void)
{ /*lint --e{801,644,563,529} */
    KEYCODE key;
    static  INT08U KSTS=0;  /* ���̿��Ƽ�(shift, ctrl, alt)��״̬ */
    
    DispatchLock();
    asm mov  ah, 1
    asm int  0x16
    asm jnz  KeyAvailable
    asm mov  ah, 2
    asm int  0x16
    asm test al, 3
    asm jz   statuskey
    asm or   al, 3          /* ʹ���ߵ�SHIFT������һ��            */
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
 * ���ļ�����: kbd.c
 * 
**==================================================================================*/

