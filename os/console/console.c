/*************************************************************************************
 * 文件:    console.c
 *
 * 说明:    控制台模块
 *
 * 作者:    Jun
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
 * 函数:    InitConsoleSvc()
 *
 * 描述:    控制台模块初始化
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
 * 函数:    OpenConsole()
 *
 * 描述:    打开控制台
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
 * 函数:    CloseConsole()
 *
 * 描述:    关闭控制台
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
 * 函数:    clrkey()
 *
 * 描述:    清空控制台按键缓冲
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
 * 函数: getkey()
 *
 * 描述: 在控制台按键缓冲中取一个键
 *
 * 输出: KEYCODE(NONEKEY代表无按键)
 *
 * 注意: 此函数在无按键时会将任务睡眠5毫秒, 因此不能在中断中调用
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

    if(NONEKEY == key){ /* 当前任务没有打开控制台 */
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
 * 函数: putkey()
 *
 * 描述: 增加一个按键到控制台按键缓冲
 *
 * 输入: key
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
 * 函数: waitkey()
 *
 * 描述: 等待控制台按键
 *
 * 输入: timeout 超时时间(毫秒)
 *
 * 输出: KEYCODE(NONEKEY代表无按键或超时)
 *
 * 注意: 不能在中断中调用
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
 * 函数:    SendConsoleKey()
 *
 * 描述:    将一个键值发送到控制台
 *
 * 说明:    这个函数本来是keysvc发送按值到系统的一个接口, 但对于目前的控制台
 *          来说, 这个函数尚未起到原来设想的作用, 不过将来再说吧...
**----------------------------------------------------------------------------------*/
void __sysonly apical SendConsoleKey(KEYCODE key)
{
    putkey(key);
}


/*====================================================================================
 * 
 * 本文件结束: console.c
 * 
**==================================================================================*/

