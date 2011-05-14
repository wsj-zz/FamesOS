/*************************************************************************************
 * 文件:    lock.h
 *
 * 说明:    锁机制
 *
 * 作者:    Jun
 *
 * 时间:    2010-9-16
*************************************************************************************/
#ifndef FAMES_FAMESOS_LOCK_H
#define FAMES_FAMESOS_LOCK_H

/*------------------------------------------------------------------------------------
 * 
 * 通用锁,  参数lock必须是一个INT16S或BOOLEAN, 且须初始化为NO
 * 
 * 例如: static INT16S lock=NO;
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
 * 互斥锁,  参数lock必须是一个INT16S或BOOLEAN
 * 
**----------------------------------------------------------------------------------*/
#define os_mutex_lock(lock)      function_lock(lock)
#define os_mutex_unlock(lock)    function_unlock(lock)
#define os_mutex_init(lock, val) function_init_lock(lock, val)

/*------------------------------------------------------------------------------------
 * 
 * 内核锁及原子锁
 * 
**----------------------------------------------------------------------------------*/
#define lock_kernel()           DispatchLock()
#define unlock_kernel()         DispatchUnlock()

#define in_atomic()             ENTER_CRITICAL()
#define out_atomic()            EXIT_CRITICAL()
#define prepare_atomic()        CRITICAL_DEFINE()

/*------------------------------------------------------------------------------------
 * 
 * 原子操作
 * 
**----------------------------------------------------------------------------------*/
#define atomic_do(statement)    in_atomic();  \
                                statement;    \
                                out_atomic()



#endif /* FAMES_FAMESOS_LOCK_H */

/*====================================================================================
 * 
 * 本文件结束: lock.h
 * 
**==================================================================================*/



