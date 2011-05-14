/*************************************************************************************
 * �ļ�: keysvc.c
 *
 * ˵��: ����(���̰���,���ⰴ��)��ط�������
 *
 * ע��: ���ļ������Դ�ģʽ����(��: TCC -1 -B -ml ###.C)
 *
 * ����: Jun
 *
 * ʱ��: 2010-04-13
 *
 * �汾: V0.1 (2010-04-13, ����İ汾)
*************************************************************************************/
#define  FAMES_KEY_SVC_C
#include "includes.h"


/*------------------------------------------------------------------------------------
 * 
 * ���������
 * 
**----------------------------------------------------------------------------------*/
static INT16U KeyboardDaemonFrequency = 100;  /* KeyboardDaemon������Ƶ�� */
static INT16U KeyboardDaemonStackSize = 1024; /* ���̷�������Ķ�ջ��С   */
static HANDLE KeyboardDaemonHandle = InvalidHandle;

void    __internal apical __ClrKeyGlobal(void);         
BOOL    __internal apical __PutKeyGlobal(KEYCODE key); 
KEYCODE __internal apical __GetKeyGlobal(void);         
KEYCODE __internal apical __WaitKeyGlobal(INT32U timeout);
/*------------------------------------------------------------------------------------
 * ����:    __ClrKeyGlobal()
 *
 * ˵��:    ��λ(���)���뻺����
 *
 * ����:    ��
 *
 * ���:    ok
**----------------------------------------------------------------------------------*/
void __internal apical __ClrKeyGlobal(void)
{
    while(GetKeyFromKeyBoard()){;}
    KEYBUF.front=0;
    KEYBUF.rear=0;
    KEYBUF.num=0;
    MEMSET((INT08S *)KEYBUF.key, NONEKEY, KEY_BUFFER_SIZE*2);
}

/*------------------------------------------------------------------------------------
 * ����: __PutKeyGlobal()
 *
 * ˵��: ��key����ȫ�ּ��뻺����
 *
 * ����: key
 *
 * ���: fail ����ʧ��: 1)key����Ч��; 2)���뻺������
 *       ok   ����ɹ�
**----------------------------------------------------------------------------------*/
BOOL __internal apical __PutKeyGlobal(KEYCODE key)
{
    if(key==NONEKEY){
        return fail;
    }
    DispatchLock();
    if(KEYBUF.num>=KEY_BUFFER_SIZE){
        DispatchUnlock();
        return fail;
    } else {
        KEYBUF.key[KEYBUF.front]=key;
        KEYBUF.front++;
        KEYBUF.num++;
        if(KEYBUF.front>=KEY_BUFFER_SIZE){
            KEYBUF.front=0;
        }
        DispatchUnlock();
    }
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����: __GetKeyGlobal()
 *
 * ˵��: �ڼ��뻺����ȡ��
 *
 * ����: ��
 *
 * ���: KEYCODE(NONEKEY�����ް���)
 *
 * ע��: �˺������ް���ʱ�Ὣ����˯��5����, ��˲������ж��е���
**----------------------------------------------------------------------------------*/
KEYCODE __internal apical __GetKeyGlobal(void)
{
    KEYCODE key;
    key=GetKeyFromKeyBoard();
    while(key!=NONEKEY){
        __PutKeyGlobal(key);
        key=GetKeyFromKeyBoard();
    }

    if(KEYBUF.num<=0){
        TaskSleep(5L);
        key=NONEKEY;
    } else {
        lock_kernel();
        key=KEYBUF.key[KEYBUF.rear];
        KEYBUF.rear++;
        KEYBUF.num--;
        if(KEYBUF.rear>=KEY_BUFFER_SIZE){
            KEYBUF.rear=0;
        }
        unlock_kernel();
    }
    return key;
}

/*------------------------------------------------------------------------------------
 * ����: __WaitKeyGlobal()
 *
 * ˵��: �ȴ�����, �ȴ���ʱ������Ч��NONEKEY
 *
 * ����: timeout ��ʱʱ��(����)
 *
 * ���: KEYCODE(NONEKEY�����ް�����ʱ)
 *
 * ע��: �������ж��е���
**----------------------------------------------------------------------------------*/
KEYCODE __internal apical __WaitKeyGlobal(INT32U timeout)
{
    KEYCODE  key;
    CRITICAL_DEFINE()
    
    ENTER_CRITICAL();
    CurrentTCB->TaskTimer = timeout;
    EXIT_CRITICAL();
    for(;;){
        key = __GetKeyGlobal();
        if(key!=NONEKEY)break;
        if(timeout > 0L){
            if(CurrentTCB->TaskTimer==(INT32U)0L){
                return NONEKEY;
            }
        }
    }
    return key;
}

/*------------------------------------------------------------------------------------
 * 
 * ȫ��������ṹ�����̹��ӽṹ
 *
**----------------------------------------------------------------------------------*/
#define GLOBAL_SPECIAL_KEY_NUM   32
#define GLOBAL_KEY_HOOK_NUM      32

struct global_special_key_s {
    KEYCODE keycode;
    KEYFUNC func;
    struct global_special_key_s * next;
};

struct global_key_hook_s {
    KEYHOOK hook;
    struct global_key_hook_s * next;
};

struct global_special_key_s  global_special_keys[GLOBAL_SPECIAL_KEY_NUM];
struct global_special_key_s *global_special_keys_free = NULL;
struct global_special_key_s *global_special_keys_list = NULL;

struct global_key_hook_s     global_key_hooks[GLOBAL_KEY_HOOK_NUM];
struct global_key_hook_s    *global_key_hooks_free = NULL;
struct global_key_hook_s    *global_key_hooks_list = NULL;

BOOL   global_special_key_lock = NO;
BOOL   global_key_hook_lock    = NO;

/*------------------------------------------------------------------------------------
 * 
 * ����ΪһЩ��������:
 *
 * __Init_SpecialKey()     �����ģ���ʼ��
 * __Free_SpecialKey()     �ͷ�һ�������
 * __Alloc_SpecialKey()    ����һ�������
 *
 * __Init_KeyHook()        ���̹���ģ���ʼ�� 
 * __Free_KeyHook()        �ͷ�һ������
 * __Alloc_KeyHook()       ����һ������
 *
**----------------------------------------------------------------------------------*/
void  apical __init __Init_SpecialKey(void)
{
    INT16S i;

    global_special_keys_free = NULL;
    global_special_keys_list = NULL;

    os_mutex_init(global_special_key_lock, NO);

    for(i=0; i<GLOBAL_SPECIAL_KEY_NUM; i++){
        global_special_keys[i].keycode = NONEKEY;
        global_special_keys[i].func    = NULL;
        global_special_keys[i].next    = global_special_keys_free;
        global_special_keys_free       = &global_special_keys[i];
    }
    return;
}

struct global_special_key_s * apical __Alloc_SpecialKey(void)
{
    struct global_special_key_s * retval;

    retval = NULL;

    os_mutex_lock(global_special_key_lock);
    if(global_special_keys_free){
        retval = global_special_keys_free;
        global_special_keys_free = global_special_keys_free->next;
    }
    os_mutex_unlock(global_special_key_lock);

    return retval;
}

BOOL apical __Free_SpecialKey(struct global_special_key_s * sk)
{
    FamesAssert(sk);
    
    if(!sk){
        return fail;
    }

    os_mutex_lock(global_special_key_lock);
    sk->keycode = NONEKEY;
    sk->func    = NULL;
    sk->next    = global_special_keys_free;
    global_special_keys_free = sk;
    os_mutex_unlock(global_special_key_lock);

    return ok;
}

void  apical __init __Init_KeyHook(void)
{
    INT16S i;

    global_key_hooks_free = NULL;
    global_key_hooks_list = NULL;

    os_mutex_init(global_key_hook_lock, NO);

    for(i=0; i<GLOBAL_KEY_HOOK_NUM; i++){
        global_key_hooks[i].hook    = NULL;
        global_key_hooks[i].next    = global_key_hooks_free;
        global_key_hooks_free       = &global_key_hooks[i];
    }
    return;
}

struct global_key_hook_s * apical __Alloc_KeyHook(void)
{
    struct global_key_hook_s * retval;

    retval = NULL;

    os_mutex_lock(global_key_hook_lock);
    if(global_key_hooks_free){
        retval = global_key_hooks_free;
        global_key_hooks_free = global_key_hooks_free->next;
    }
    os_mutex_unlock(global_key_hook_lock);

    return retval;
}

BOOL apical __Free_KeyHook(struct global_key_hook_s * kh)
{
    FamesAssert(kh);
    
    if(!kh){
        return fail;
    }

    os_mutex_lock(global_key_hook_lock);
    kh->hook    = NULL;
    kh->next    = global_key_hooks_free;
    global_key_hooks_free = kh;
    os_mutex_unlock(global_key_hook_lock);

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����: RegisterSpecialKey()
 *
 * ˵��: ע��ȫ�������(��ݼ�)
**----------------------------------------------------------------------------------*/
BOOLEAN apical RegisterSpecialKey(KEYCODE keycode, KEYFUNC func)
{
    struct global_special_key_s * sk;

    FamesAssert(keycode != NONEKEY);
    FamesAssert(func);

    if(keycode == NONEKEY || !func){
        return fail;
    }

    sk = __Alloc_SpecialKey();

    if(!sk){
        return fail;
    }

    sk->keycode = keycode;
    sk->func    = func;

    lock_kernel();
    sk->next = global_special_keys_list;
    global_special_keys_list = sk;
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����: DeregisterSpecialKey()
 *
 * ˵��: ע��ȫ�������, һ��ֻ��ע��һ����(��һ����ͬ������ע��)
**----------------------------------------------------------------------------------*/
BOOLEAN apical DeregisterSpecialKey(KEYCODE keycode)
{
    BOOLEAN retval;
    struct  global_special_key_s ** sk, *t;

    FamesAssert(keycode != NONEKEY);

    if(keycode == NONEKEY){
        return fail;
    }

    retval = fail;

    for(sk = &global_special_keys_list; *sk;){
        if((*sk)->keycode == keycode){
            retval = ok;
            lock_kernel();
            t=*sk;
            (*sk)=(*sk)->next;
            unlock_kernel();
            __Free_SpecialKey(t);
            break;
        } else {
            lock_kernel();
            sk=&(*sk)->next;
            unlock_kernel();
        }
    }

    return retval;
}

/*------------------------------------------------------------------------------------
 * ����: RegisterKeyHook()
 *
 * ˵��: ע����̹���
**----------------------------------------------------------------------------------*/
BOOLEAN apical RegisterKeyHook(KEYHOOK hook)
{
    struct global_key_hook_s * kh;

    FamesAssert(hook);

    if(!hook){
        return fail;
    }

    kh = __Alloc_KeyHook();

    if(!kh){
        return fail;
    }

    kh->hook = hook;

    lock_kernel();
    kh->next = global_key_hooks_list;
    global_key_hooks_list = kh;
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����: DeregisterKeyHook()
 *
 * ˵��: ע�����̹���
**----------------------------------------------------------------------------------*/
BOOLEAN apical DeregisterKeyHook(KEYHOOK hook)
{
    BOOLEAN retval;
    struct  global_key_hook_s ** kh, *t;

    FamesAssert(hook);

    if(!hook){
        return fail;
    }

    retval = fail;

    for(kh = &global_key_hooks_list; *kh;){
        if((*kh)->hook == hook){
            retval = ok;
            lock_kernel();
            t=*kh;
            (*kh)=(*kh)->next;
            unlock_kernel();
            __Free_KeyHook(t);
        } else {
            lock_kernel();
            kh=&(*kh)->next;
            unlock_kernel();
        }
    }

    return retval;
}

/*------------------------------------------------------------------------------------
 * ����: KeyboardDaemon()
 *
 * ˵��: ���̷����̨, �����ջ��С��Ϊ1024
**----------------------------------------------------------------------------------*/
BOOL __KeyboardDaemon_MsgHdlr(HANDLE task, MSGCB * msg);

void __daemon KeyboardDaemon(void * data)
{
    static BOOL entered = NO;
    INT32U  sleep_ms;
    MSGCB   msg;
    KEYCODE key;
    struct global_key_hook_s    * kh;
    struct global_special_key_s * sk;
    BOOLEAN is_special_key;

    data = data;

    lock_kernel();
    if(entered){
        unlock_kernel();
        task_return;
    }
    entered = YES;
    unlock_kernel();

    sleep_ms = 1000L/(INT32U)KeyboardDaemonFrequency;

    if(sleep_ms==0L)sleep_ms++;

    lock_kernel();
    CurrentTCB->msg_handler = __KeyboardDaemon_MsgHdlr;
    unlock_kernel();

    InitMessage(&msg);

    for(;;){
        if(GetMessage(&msg)){
            ; /* now, nothing here */
        }
        is_special_key = NO;
        key = __WaitKeyGlobal(0L);
        lock_kernel();
        kh = global_key_hooks_list;
        sk = global_special_keys_list;
        while(kh){ /* do keyboard hooks */
            if(kh->hook){
                (*kh->hook)(key);
            }
            kh = kh->next;
        }
        while(sk){ /* do special keys */
            if(key == sk->keycode){
                if(sk->func){
                    (*sk->func)();
                }
                is_special_key = YES;
            }
            sk = sk->next;
        }
        unlock_kernel();

        if(is_special_key == NO){
            SendConsoleKey(key); /* normal key, send it */
        }
        
        TaskSleep(sleep_ms);
    }/* for(;;) */
}

/* ���̷���������Ϣ������ 
*/
BOOL __KeyboardDaemon_MsgHdlr(HANDLE task, MSGCB * msg)
{
    FamesAssert(msg);

    if(!msg){
        return fail;
    }

    if(task != KeyboardDaemonHandle){
        return fail;
    }
    switch(msg->msg){
        case SYS_MSG_KEY:
            __PutKeyGlobal((KEYCODE)msg->wParam);
            return ok;
        default:
            return fail;
    }
}

/*------------------------------------------------------------------------------------
 * ����: OnExit_keyboard()
 *
 * ˵��: ����ģ���˳�����
**----------------------------------------------------------------------------------*/
void __exit OnExit_keyboard(void)
{
    if(KeyboardDaemonHandle != InvalidHandle){
        TaskDelete(KeyboardDaemonHandle);
    }
    KeyboardDaemonHandle = InvalidHandle;
    StopKeyboard();
}

/*------------------------------------------------------------------------------------
 * ����: InitKeyService()
 *
 * ˵��: ���������ʼ��
**----------------------------------------------------------------------------------*/
void apical __init InitKeyService(void)
{
    BOOL retval = fail;

    KeyboardDaemonHandle = InvalidHandle;
    
    InitKeyboard();
    __ClrKeyGlobal();
    __Init_SpecialKey();
    __Init_KeyHook();
    if(!RegisterOnExit(OnExit_keyboard)){
        ; /* failed to register, do something here */
    }
    KeyboardDaemonHandle = TaskCreate(KeyboardDaemon, NULL, 
                                      "keystroke-service", 
                                      NULL, 
                                      KeyboardDaemonStackSize, 
                                      PRIO_KEY, TASK_CREATE_OPT_NONE);
    if(KeyboardDaemonHandle == InvalidHandle){
        sys_print("InitKeyService: failed to create KeyboardDaemon!\n");
        retval = fail;
    } else {
        retval = ok;
    }

    retval = retval; /* prevent warning */
    
    return;
}


/*====================================================================================
 * 
 * ���ļ�����: keysvc.c
 * 
**==================================================================================*/

