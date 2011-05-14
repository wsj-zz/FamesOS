/*************************************************************************************
 * �ļ�:    console.c
 *
 * ˵��:    ����̨ģ��
 *
 * ����:    Jun
*************************************************************************************/
#define  FAMES_CONSOLE_C
#include "includes.h"


#define console_keybuf_size 32

typedef struct console_keybuf_s{
    KEYCODE key[console_keybuf_size];
    INT16S  front;               
    INT16S  rear;                
    INT16S  number;                 
}console_keybuf_t;        

console_keybuf_t console_keybuf={{0,}, 0, 0, 0};

/*------------------------------------------------------------------------------------
 * ����:    InitConsoleSvc()
 *
 * ����:    ����̨ģ���ʼ��
**----------------------------------------------------------------------------------*/
void apical __init InitConsoleSvc(void)
{
    console_keybuf.front=0;
    console_keybuf.rear=0;
    console_keybuf.number=0;
    MEMSET((INT08S *)console_keybuf.key, NONEKEY, console_keybuf_size*2);
    InitKeyService();
}

/*------------------------------------------------------------------------------------
 * ����:    OpenConsole()
 *
 * ����:    �򿪿���̨
**----------------------------------------------------------------------------------*/
BOOL apical OpenConsole(void)
{
    prepare_atomic()
        
    in_atomic();
    CurrentTCB->console = YES;
    out_atomic();

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    CloseConsole()
 *
 * ����:    �رտ���̨
**----------------------------------------------------------------------------------*/
BOOL apical CloseConsole(void)
{
    prepare_atomic()
        
    in_atomic();
    CurrentTCB->console = NO;
    out_atomic();

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    clrkey()
 *
 * ����:    ��տ���̨��������
**----------------------------------------------------------------------------------*/
void  clrkey(void)
{
    lock_kernel();
    console_keybuf.front=0;
    console_keybuf.rear=0;
    console_keybuf.number=0;
    MEMSET((INT08S *)console_keybuf.key, NONEKEY, console_keybuf_size*2);
    unlock_kernel();
}

/*------------------------------------------------------------------------------------
 * ����: getkey()
 *
 * ����: �ڿ���̨����������ȡһ����
 *
 * ���: KEYCODE(NONEKEY�����ް���)
 *
 * ע��: �˺������ް���ʱ�Ὣ����˯��5����, ��˲������ж��е���
**----------------------------------------------------------------------------------*/
KEYCODE getkey(void)
{
    KEYCODE key;
    prepare_atomic()

    in_atomic();
    if(CurrentTCB->console){
        key = 0x1234;
    }
    else {
        key = NONEKEY;
    }
    out_atomic();

    if(NONEKEY == key){ /* ��ǰ����û�д򿪿���̨ */
        goto out;
    }
    
    lock_kernel();
    if(console_keybuf.number<=0){
        key=NONEKEY;
    } else {
        key=console_keybuf.key[console_keybuf.rear];
        console_keybuf.rear++;
        console_keybuf.number--;
        if(console_keybuf.rear>=console_keybuf_size){
            console_keybuf.rear=0;
        }
    }
    unlock_kernel();

out:
    if(NONEKEY == key)
        TaskSleep(5L);
    
    return key;
}

/*------------------------------------------------------------------------------------
 * ����: putkey()
 *
 * ����: ����һ������������̨��������
 *
 * ����: key
**----------------------------------------------------------------------------------*/
BOOL putkey(KEYCODE key)
{
    BOOL retval;
    
    if(key==NONEKEY){
        return fail;
    }
    lock_kernel();
    if(console_keybuf.number>=console_keybuf_size){
        retval = fail;
    } else {
        console_keybuf.key[console_keybuf.front]=key;
        console_keybuf.front++;
        console_keybuf.number++;
        if(console_keybuf.front>=console_keybuf_size){
            console_keybuf.front=0;
        }
        retval = ok;
    }
    unlock_kernel();
    
    return retval;
}

/*------------------------------------------------------------------------------------
 * ����: waitkey()
 *
 * ����: �ȴ�����̨����
 *
 * ����: timeout ��ʱʱ��(����)
 *
 * ���: KEYCODE(NONEKEY�����ް�����ʱ)
 *
 * ע��: �������ж��е���
**----------------------------------------------------------------------------------*/
KEYCODE waitkey(INT32U timeout)
{
    KEYCODE key;
    INT32U  times;
    prepare_atomic()

    times = (timeout+4L)/5L;

    key = NONEKEY;
    
    in_atomic();
    CurrentTCB->TaskTimer=timeout;
    out_atomic();
    for(;times>0L||timeout==0L;times--){
        key=getkey();
        if(key!=NONEKEY)break;
    }
    return key;
}

/*------------------------------------------------------------------------------------
 * ����:    SendConsoleKey()
 *
 * ����:    ��һ����ֵ���͵�����̨
 *
 * ˵��:    �������������keysvc���Ͱ�ֵ��ϵͳ��һ���ӿ�, ������Ŀǰ�Ŀ���̨
 *          ��˵, ���������δ��ԭ�����������, ����������˵��...
**----------------------------------------------------------------------------------*/
void __sysonly apical SendConsoleKey(KEYCODE key)
{
    putkey(key);
}


/*====================================================================================
 * 
 * ���ļ�����: console.c
 * 
**==================================================================================*/

