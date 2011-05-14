/***********************************************************************************************
** 文件:    onexit.c
**
** 说明:    FamesOS退出例程处理
**
** 作者:    Jun
**
** 时间:    2010-6-28(创建)
**
** 备注:    退出例程是指一个模块在系统退出时应该执行的动作, 而OnExit提供了
**          组织及处理(调用)这些动作的功能
***********************************************************************************************/
#define  FAMES_ON_EXIT_C
#include "includes.h"

#if FAMES_ON_EXIT_EN == 1
/*----------------------------------------------------------------------------------------------
 *   OnExit数据结构及辅助函数声明:
 *
 *   onexit_s:       OnExit数据结构
 *   onexit_buf:     OnExit例程缓冲区
 *   onexit_free:    OnExit空闲列表
 *   onexit_list:    OnExit注册列表
**--------------------------------------------------------------------------------------------*/
struct onexit_s {
    onexit_func  func;
    struct onexit_s *next;
};

static struct onexit_s  onexit_buf[ON_EXIT_NUM];

static struct onexit_s *onexit_free = NULL;
static struct onexit_s *onexit_list = NULL;

/*----------------------------------------------------------------------------------------------
 * 函数:    InitializeOnExit()
 *
 * 说明:    初始化OnExit模块
**--------------------------------------------------------------------------------------------*/
void apical InitializeOnExit(void)
{
    int  i;

    onexit_free = NULL;
    onexit_list = NULL;
    
    for(i=0; i<ON_EXIT_NUM; i++){
        onexit_buf[i].func =  NULL;
        onexit_buf[i].next =  onexit_free;
        onexit_free        = &onexit_buf[i];
    }
}

/*----------------------------------------------------------------------------------------------
 * 函数:    RegisterOnExit()
 *
 * 说明:    注册OnExit例程
**--------------------------------------------------------------------------------------------*/
BOOL apical RegisterOnExit(onexit_func onexit)
{
    BOOL   retval;
    struct onexit_s *oe;
    
    FamesAssert(onexit);

    if(!onexit){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(onexit_free){
        oe = onexit_free;       /* allocate a free one */
        onexit_free = oe->next;

        oe->next = onexit_list; /* link to onexit_list */
        oe->func = onexit;
        onexit_list = oe;      

        retval = ok;
    }
    unlock_kernel();

    return retval;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    DeregisterOnExit()
 *
 * 说明:    注销一个例程
**--------------------------------------------------------------------------------------------*/
BOOL apical DeregisterOnExit(onexit_func onexit)
{
    BOOL   retval;
    struct onexit_s **oe, *t;
    
    FamesAssert(onexit);

    if(!onexit){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(onexit_list){
        for(oe = &onexit_list; *oe;){
            if((*oe)->func == onexit){
                t = (*oe);
               (*oe) = (*oe)->next; /* disconnect */
                t->next = onexit_free; /* link to free list */
                t->func = NULL;
                onexit_free = t;
                retval = ok;
                break;
            }
            oe = &(*oe)->next;
        }
    }
    unlock_kernel();

    return retval;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    ProcessOnExit()
 *
 * 说明:    执行OnExit例程, 此函数只能由ExitFamesOS()调用
**--------------------------------------------------------------------------------------------*/
void apical ProcessOnExit(void)
{
    struct onexit_s *oe;

    lock_kernel();
    oe = onexit_list;
    while(oe){
        if(oe->func){
            (*oe->func)();
        }
        oe = oe->next;
    }
    unlock_kernel();
}

#endif /*#if FAMES_ON_EXIT_EN == 1*/

/*==============================================================================================
 * 
 * 本文件结束: onexit.c
 * 
**============================================================================================*/

