/*************************************************************************************
 * �ļ�:    lock.h
 *
 * ˵��:    ������
 *
 * ����:    Jun
 *
 * ʱ��:    2010-9-16
*************************************************************************************/
#ifndef FAMES_FAMESOS_LOCK_H
#define FAMES_FAMESOS_LOCK_H

/*------------------------------------------------------------------------------------
 * 
 * ͨ����,  ����lock������һ��INT16S��BOOLEAN, �����ʼ��ΪNO
 * 
 * ����: static INT16S lock=NO;
**----------------------------------------------------------------------------------*/
/*lint -emacro(506, function_lock)      */
/*lint -emacro(717, function_unlock)    */
/*lint -emacro(717, function_init_lock) */
#define function_lock(lock)     do {                        \
                                    DispatchLock();         \
                                    if(!lock){              \
                                        lock=YES;           \
                                        DispatchUnlock();   \
                                        break;              \
                                    }                       \
                                    DispatchUnlock();       \
                                    TaskSleep(2L);          \
                                } while(1)
#define function_unlock(lock)   do {                        \
                                    DispatchLock();         \
                                    lock=NO;                \
                                    DispatchUnlock();       \
                                } while(0)
#define function_init_lock(lock, val)                       \
                                do {                        \
                                    DispatchLock();         \
                                    lock = val;             \
                                    DispatchUnlock();       \
                                } while(0)

/*------------------------------------------------------------------------------------
 * 
 * ������,  ����lock������һ��INT16S��BOOLEAN
 * 
**----------------------------------------------------------------------------------*/
#define os_mutex_lock(lock)      function_lock(lock)
#define os_mutex_unlock(lock)    function_unlock(lock)
#define os_mutex_init(lock, val) function_init_lock(lock, val)

/*------------------------------------------------------------------------------------
 * 
 * �ں�����ԭ����
 * 
**----------------------------------------------------------------------------------*/
#define lock_kernel()           DispatchLock()
#define unlock_kernel()         DispatchUnlock()

#define in_atomic()             ENTER_CRITICAL()
#define out_atomic()            EXIT_CRITICAL()
#define prepare_atomic()        CRITICAL_DEFINE()

/*------------------------------------------------------------------------------------
 * 
 * ԭ�Ӳ���
 * 
**----------------------------------------------------------------------------------*/
#define atomic_do(statement)    in_atomic();  \
                                statement;    \
                                out_atomic()



#endif /* FAMES_FAMESOS_LOCK_H */

/*====================================================================================
 * 
 * ���ļ�����: lock.h
 * 
**==================================================================================*/



