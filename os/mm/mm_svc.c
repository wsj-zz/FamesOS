/*************************************************************************************
 * 文件: mm_svc.c
 *
 * 说明: 存储管理服务
 *
 * 注意: 此文件必需以大模式编译(如: TCC -1 -B -ml ###.C)
 *
 * 作者: Jun
 *
 * 时间: 2010-12-8
 *
 * 版本: V0.1 (2010-12-8, 最初的版本)
*************************************************************************************/
#define  FAMES_MM_SVC_C
#include "includes.h"


BOOL __internal   __mem_chain_to_alloc_list(void *, void *, INT32U);
void __internal * __mem_unchain_from_alloc_list(void * p2);
void __internal   __mem_initialize_alloc_list(void);

/*------------------------------------------------------------------------------------
 * 函数:    __mem_alloc()
 *
 * 描述:    分配一段内存空间
 *
 * 参数:    nbytes: 需分配的字节数
**----------------------------------------------------------------------------------*/
void * apical __mem_alloc(INT32U nbytes)
{
    void * p;
    
    FamesAssert(nbytes>0);

    if(nbytes == 0)
        return NULL;

    lock_kernel();
    p = farmalloc(nbytes);
    unlock_kernel();
    #if 0
    printf("__mem_alloc(): p=%p\n", p); getch();
    #endif

    if(p){
        if(!__mem_chain_to_alloc_list(p, p, nbytes)){
            /*
            * do something here on failure to chain
            */
            ; 
        }
    }

    return p;
}

/*------------------------------------------------------------------------------------
 * 函数:    __mem_free()
 *
 * 描述:    释放一段内存空间
 *
 * 参数:    block: 内存空间地址
**----------------------------------------------------------------------------------*/
void apical __mem_free(void * block)
{
    FamesAssert(block);

    if(!block)
        return;

    block = __mem_unchain_from_alloc_list(block);

    if(block){
        lock_kernel();
        farfree(block);
        unlock_kernel();
    }
}

/*------------------------------------------------------------------------------------ 
 *          内存分配列表:
 * 
 * 描述: 用于管理内存的分配与回收
 *
 * 结构: ptr      已分配内存的指针
 *       ptr2     返回给应用程序的指针
 *       nbytes   分配的字节数
 *       next     指向下一个结构
**----------------------------------------------------------------------------------*/
struct __mem_allocate_s {
    void   * ptr;
    void   * ptr2;
    INT32U   nbytes;
    struct __mem_allocate_s * next;
};

#define MEM_ALLOC_LIST_MAX_SIZE  1024

struct __mem_allocate_s * __mem_alloc_buff;
struct __mem_allocate_s * __mem_alloc_list = NULL;
struct __mem_allocate_s * __mem_alloc_free = NULL;

/*lint -save -emacro(717, __mem_init_alloc_struct)*/
#define __mem_init_alloc_struct(s)  do { \
                    (s).ptr  = NULL;     \
                    (s).ptr2 = NULL;     \
                    (s).nbytes = 0uL;    \
                    (s).next = NULL;     \
                }while(0)
/*lint -restore*/

/*------------------------------------------------------------------------------------
 * 函数:    __mem_initialize_alloc_list()
 *
 * 说明:    初始化内存分配列表
**----------------------------------------------------------------------------------*/
void __internal __mem_initialize_alloc_list(void)
{
    int  i;
    
    __mem_alloc_list = NULL;
    __mem_alloc_free = NULL;

    lock_kernel();
    __mem_alloc_buff = farmalloc((INT32U)MEM_ALLOC_LIST_MAX_SIZE*sizeof(struct __mem_allocate_s));
    unlock_kernel();

    if(!__mem_alloc_buff)
        return;

    for(i=0; i<MEM_ALLOC_LIST_MAX_SIZE; i++){
        __mem_init_alloc_struct(__mem_alloc_buff[i]);
        __mem_alloc_buff[i].next = __mem_alloc_free;
        __mem_alloc_free = &(__mem_alloc_buff[i]);
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    __mem_chain_to_alloc_list()
 *
 * 说明:    链入内存分配列表
**----------------------------------------------------------------------------------*/
BOOL __internal __mem_chain_to_alloc_list(void  * ptr, 
                                          void  * ptr2, 
                                          INT32U nbytes)
{
    BOOL   retval;
    struct __mem_allocate_s *s;
    
    FamesAssert(ptr);
    FamesAssert(ptr2);
    FamesAssert(nbytes > 0);

    if(!ptr || !ptr2){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(__mem_alloc_free){
        s = __mem_alloc_free;      /* allocate a free one */
        __mem_alloc_free = s->next;

        __mem_init_alloc_struct(*s);
        s->ptr    = ptr;
        s->ptr2   = ptr2;
        s->nbytes = nbytes;
        s->next = __mem_alloc_list;/* link to list */
        __mem_alloc_list = s;      

        retval = ok;
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    __mem_unchain_from_alloc_list()
 *
 * 描述:    与内存分配列表断开联接
 *
 * 参数:    p2  用户要求释放的指针(.ptr2)
 *
 * 返回:    ptr 内存分配时的指针(.ptr)
 *
 * 说明:    在列表__mem_alloc_list中, 查找某个结点的ptr2元素, 如果ptr2
 *          与给定的参数p2相同, 那么就返回对应结点的ptr元素, 同时将这
 *          个结点与__mem_alloc_list断开, 这样做的目的是, 在有的时候,
 *          申请的内存可能需要一点处理, 这有可能会导致申请的内存与返回
 *          到应用程序的内存具有不同的地址, 结构__mem_allocate_s中的
 *          ptr与ptr2就代表了这样的对应关系.
**----------------------------------------------------------------------------------*/
void __internal * __mem_unchain_from_alloc_list(void * p2)
{
    void  * retval;
    struct __mem_allocate_s ** s, * t;
    
    FamesAssert(p2);

    if(!p2){
        return NULL;
    }

    retval = NULL;
    lock_kernel();
    if(__mem_alloc_list){
        for(s = &__mem_alloc_list; *s;){
            if((*s)->ptr2 == p2){           /* 查找ptr2, 返回ptr */
                t = (*s);
               (*s) = (*s)->next;           /* disconnect */
                retval = t->ptr;
                __mem_init_alloc_struct(*t);
                t->next = __mem_alloc_free; /* link to free list */
                __mem_alloc_free = t;
                break;
            }
            s = &(*s)->next;
        }
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    InitMemService()
 *
 * 描述:    内存管理服务器初始化
**----------------------------------------------------------------------------------*/
void apical __init InitMemService(void)
{
    void __exit OnexitMemService(void);

    if(!RegisterOnExit(OnexitMemService)){
        sys_print("InitMemService: Failed to register OnexitMemService!\n");
        /* 
        * do something else here 
        */
    }
    __mem_initialize_alloc_list();
}

/*------------------------------------------------------------------------------------
 * 函数:    OnexitMemService()
 *
 * 描述:    内存管理服务器退出例程
**----------------------------------------------------------------------------------*/
void __exit OnexitMemService(void)
{
    lock_kernel();
    while(__mem_alloc_list){
        if(__mem_alloc_list->ptr){
            farfree(__mem_alloc_list->ptr);
        }
        __mem_alloc_list = __mem_alloc_list->next;
    }
    if(__mem_alloc_buff){
        farfree(__mem_alloc_buff);
    }
    unlock_kernel();
}


/*====================================================================================
 * 
 * 本文件结束: mm_svc.c
 * 
**==================================================================================*/

