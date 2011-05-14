/*************************************************************************************
 * �ļ�: mm_svc.c
 *
 * ˵��: �洢�������
 *
 * ע��: ���ļ������Դ�ģʽ����(��: TCC -1 -B -ml ###.C)
 *
 * ����: Jun
 *
 * ʱ��: 2010-12-8
 *
 * �汾: V0.1 (2010-12-8, ����İ汾)
*************************************************************************************/
#define  FAMES_MM_SVC_C
#include "includes.h"


BOOL __internal   __mem_chain_to_alloc_list(void *, void *, INT32U);
void __internal * __mem_unchain_from_alloc_list(void * p2);
void __internal   __mem_initialize_alloc_list(void);

/*------------------------------------------------------------------------------------
 * ����:    __mem_alloc()
 *
 * ����:    ����һ���ڴ�ռ�
 *
 * ����:    nbytes: �������ֽ���
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
 * ����:    __mem_free()
 *
 * ����:    �ͷ�һ���ڴ�ռ�
 *
 * ����:    block: �ڴ�ռ��ַ
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
 *          �ڴ�����б�:
 * 
 * ����: ���ڹ����ڴ�ķ��������
 *
 * �ṹ: ptr      �ѷ����ڴ��ָ��
 *       ptr2     ���ظ�Ӧ�ó����ָ��
 *       nbytes   ������ֽ���
 *       next     ָ����һ���ṹ
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
 * ����:    __mem_initialize_alloc_list()
 *
 * ˵��:    ��ʼ���ڴ�����б�
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
 * ����:    __mem_chain_to_alloc_list()
 *
 * ˵��:    �����ڴ�����б�
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
 * ����:    __mem_unchain_from_alloc_list()
 *
 * ����:    ���ڴ�����б�Ͽ�����
 *
 * ����:    p2  �û�Ҫ���ͷŵ�ָ��(.ptr2)
 *
 * ����:    ptr �ڴ����ʱ��ָ��(.ptr)
 *
 * ˵��:    ���б�__mem_alloc_list��, ����ĳ������ptr2Ԫ��, ���ptr2
 *          ������Ĳ���p2��ͬ, ��ô�ͷ��ض�Ӧ����ptrԪ��, ͬʱ����
 *          �������__mem_alloc_list�Ͽ�, ��������Ŀ����, ���е�ʱ��,
 *          ������ڴ������Ҫһ�㴦��, ���п��ܻᵼ��������ڴ��뷵��
 *          ��Ӧ�ó�����ڴ���в�ͬ�ĵ�ַ, �ṹ__mem_allocate_s�е�
 *          ptr��ptr2�ʹ����������Ķ�Ӧ��ϵ.
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
            if((*s)->ptr2 == p2){           /* ����ptr2, ����ptr */
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
 * ����:    InitMemService()
 *
 * ����:    �ڴ�����������ʼ��
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
 * ����:    OnexitMemService()
 *
 * ����:    �ڴ����������˳�����
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
 * ���ļ�����: mm_svc.c
 * 
**==================================================================================*/

