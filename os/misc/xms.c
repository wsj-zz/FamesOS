/*************************************************************************************
 * 文件: xms.c
 *
 * 描述: 扩展内存(XMS)相关函数
 *
 * 注意: 此文件必需以大模式编译(如: TCC -1 -B -ml ###.C)
 *
 * 作者: Jun
 *
 * 时间: 2010-03-03
 *
 * 版本: V0.1 (2010-03-03, 最初的版本)
*************************************************************************************/
#define  FAMES_XMS_C
#include "includes.h"

#if    XMS_EN == 1
/*------------------------------------------------------------------------------------
 * 函数:    XMSinit()
 * 描述:    XMS初始化
 * 输入:    无
 * 输出:    设置标志XMSOK为OK/FAIL并返回
**----------------------------------------------------------------------------------*/
INT16U XMSinit(void)
{
    static struct REGPACK rg;

    rg.r_ax=0x4300;
    DispatchLock();
    intr(0x2f, &rg);
    if((rg.r_ax&0xff)==0x80)
    {
        rg.r_ax=0x4310;
        intr(0x2f,&rg);
        XMSaddr=MK_FP(rg.r_es, rg.r_bx);
        XMSOK=ok;
    } else {
        XMSOK=fail;
    }
    DispatchUnlock();
    
    return XMSOK;
}

/*------------------------------------------------------------------------------------
 * 函数:    XMSfreesize()
 * 描述:    取当前自由扩展内存总的大小
 * 输入:    无
 * 输出:    以K字节计的自由扩展内存的大小
**----------------------------------------------------------------------------------*/
INT16U XMSfreesize(void)
{
    INT16U saved_dx;
    
    if(XMSOK==fail)
        return ((INT16U)0);
    DispatchLock();
    _AH=8; /*lint !e40 !e63 */
    (*XMSaddr)(); /*lint !e522 */
    saved_dx = (INT16U)_DX; /*lint !e40 !e63 */
    DispatchUnlock();
    
    return saved_dx;
}

/*------------------------------------------------------------------------------------
 * 函数:    XMSlargestblock()
 * 描述:    取当前最大的扩展内存块的大小
 * 输入:    无
 * 输出:    以K字节计的最大扩展内存块的大小
**----------------------------------------------------------------------------------*/
INT16U XMSlargestblock(void)
{
    INT16U saved_ax;
    
    if(XMSOK==fail)
        return ((INT16U)0);
    DispatchLock();
    _AH=8; /*lint !e40 !e63 */
    (*XMSaddr)(); /*lint !e522 */
    saved_ax = (INT16U)_AX; /*lint !e40 !e63 */
    DispatchUnlock();
    
    return saved_ax;
}

/*------------------------------------------------------------------------------------
 * 函数:    XMS()
 * 描述:    分配一块扩展内存(最多可分配32块)
 * 输入:    size 所要分配的扩展内存块的大小，以K字节为单位
 * 输出:    所分配的扩展内存块的句柄，若该值为0，表示分配失败
**----------------------------------------------------------------------------------*/
INT16U XMS(INT16U size)
{
    INT16U saved_ax;
    INT16U saved_dx;
    
    if(XMSOK==fail){
        return ((INT16U)0);
    }
    DispatchLock();
    _AH=9; /*lint !e40 !e63 */
    _DX=size; /*lint !e40 !e63 */
    (*XMSaddr)(); /*lint !e522 */
    saved_ax = _AX; /*lint !e40 !e63 */
    saved_dx = _DX; /*lint !e40 !e63 */
    DispatchUnlock();
    if(saved_ax!=1){
        return (INT16U)0;
    }
    return saved_dx;

}

/*------------------------------------------------------------------------------------
 * 函数:    _XMS()
 * 描述:    释放扩展内存块
 * 输入:    handle 所要释放的扩展内存块的句柄
 * 输出:    fail=失败，ok=成功
**----------------------------------------------------------------------------------*/
INT16U _XMS(INT16U handle)
{
    INT16U saved_ax;

    if(XMSOK==fail)
        return fail;
    if(handle==(INT16U)0)
        return fail;
    DispatchLock();
    _AH=0xa; /*lint !e40 !e63 */
    _DX=handle; /*lint !e40 !e63 */
    (*XMSaddr)(); /*lint !e522 */
    saved_ax = _AX; /*lint !e40 !e63 */
    DispatchUnlock();
    if(saved_ax){
        return ok;
    } else {
        return fail;
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    XMSrealloc()
 * 描述:    以新的尺寸重新分配扩展内存
 * 输入:    size   新的大小
 *          handle 要重新分配的扩展内存块的句柄
 * 输出:    fail=失败，OK=成功
**----------------------------------------------------------------------------------*/
INT16U XMSrealloc(INT16U handle, INT16U size)
{
    INT16U saved_ax;

    if(XMSOK==fail)
        return fail;
    if(handle==0)
        return fail;
    DispatchLock();
    _AH=0x0f; /*lint !e40 !e63 */
    _BX=size; /*lint !e40 !e63 */
    _DX=handle; /*lint !e40 !e63 */
    (*XMSaddr)(); /*lint !e522 */
    saved_ax = _AX; /*lint !e40 !e63 */
    DispatchUnlock();
    if(saved_ax){
        return ok;
    } else {
        return fail;
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    XMSmove()
 * 描述:    移动内存块
 * 输入:    内存移动信息块结构的指针
 * 输出:    ok
**----------------------------------------------------------------------------------*/
INT16U XMSmove(struct EMB *emb)
{
    if(XMSOK==fail)
        return fail;
    DispatchLock();
    asm push ax
    asm push ds
    asm push es
    asm push si
    _AX=_CS;       /*lint !e40 !e63 */
    asm push ax
    asm call aaaaa
    asm aaaaa: pop ax
    asm add ax,0x13
    asm push ax
    _AX=FP_SEG(XMSaddr);  /*lint !e40 !e63 */
    asm push ax
    _AX=FP_OFF(XMSaddr);  /*lint !e40 !e63 !e507 */
    asm push ax
    asm lds  si, emb
    asm mov  ah,0xb
    asm retf
    asm pop  si
    asm pop  es
    asm pop  ds
    asm pop  ax
    DispatchUnlock();
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    XMSput()
 * 描述:    写扩展内存
 * 输入:    handle 目的块的句柄
 *          dp     所写入数据块在目的块中的偏移值
 *          sp     数据块在常规内存中的地址
 *          length 所写入数据块的长度，以字节为单位
 * 输出:    ok
 * 注意:    源块只能在常规内存中，其句柄恒为0，因此不需传入源块的
 *          句柄，该函数负责对奇数数据长度的处理(其实没有处理)
**----------------------------------------------------------------------------------*/
INT16U XMSput(INT16U handle, void *dp, void *sp, INT32S length)
{
    struct EMB emb;
    
    FamesAssert(sp);
    if(!sp)
        return fail;
    
    if(XMSOK==fail)
        return fail;
    if(length&(INT32S)1L)length++;
    emb.Length=length;
    emb.SourceHandle=(INT16U)0;
    emb.SourceOfs=(INT32S)sp;
    emb.DestinationHandle=handle;
    emb.DestinationOfs=(INT32S)dp;
    return XMSmove(&emb);
}

/*------------------------------------------------------------------------------------
 * 函数:    XMSget()
 * 描述:    读扩展内存
 * 输入:    handle 源块的句柄
 *          dp     存放所读数据的常规内存缓冲区的地址
 *          sp     所读数据在源块中的偏移
 *          length 所读数据的长度，以字节计
 * 输出:    ok
 * 注意:    目的块只能在常规内存中，其句柄恒为0，因此
 *          不需传入目的块的句柄。该函数负责对奇数数据
 *          长度的处理。尽管该函数能读奇数长度的数据，
 *          但在长度为奇数时其速度明显较慢。
**----------------------------------------------------------------------------------*/
INT16U XMSget(INT16U handle, void *dp, void *sp, INT32S length)
{
    INT16U     v;
    INT08S     tb[2];
    struct EMB emb;

    FamesAssert(dp);

    if(!dp){
        return fail;
    }

    if(XMSOK==fail)
        return fail;
    
    if(length&(INT32S)1L) { /* 长度为奇数 */
        INT08S * p, * d;
        length--;
        if(length>(INT32S)0)
        {
            emb.Length=length;
            emb.SourceHandle=handle;
            emb.SourceOfs=(INT32S)sp;
            emb.DestinationHandle=0;
            emb.DestinationOfs=(INT32S)dp;
            v=XMSmove(&emb);
        }
        p=(INT08S *)tb;
        emb.Length=(INT32S)2L;
        emb.SourceHandle=handle;
        emb.SourceOfs=(INT32S)sp+length;
        emb.DestinationHandle=(INT16U)0;
        emb.DestinationOfs=(INT32S)p;
        v=XMSmove(&emb);
        d=(INT08S *)dp;
        d[length]=p[0];/* TC编译时会出现一个警告: 
                          Conversion may lose significant digits 
                          这是因为length是个32位的数,
                          而d[length]隐式的将之转成了16位的数, 实际上,
                          由于length的值不会超过一个16位的数(一个段的大小),
                          因此, 这个警告可以不必理会. --- Jun, 2010/12/28
                       */
    } else { /* 长度为偶数 */
        emb.Length=length;
        emb.SourceHandle=handle;
        emb.SourceOfs=(INT32S)sp;
        emb.DestinationHandle=(INT16U)0;
        emb.DestinationOfs=(INT32S)dp;
        v=XMSmove(&emb);
    }
    return (v);
}

/*------------------------------------------------------------------------------------
 * 
 * 
 *          下面的部分为XMS扩展内存的管理模块
 * 
 * 
**----------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------ 
 *          扩展内存分配列表:
 * 
 * 描述: 用于管理扩展内存的分配与回收
 *
 * 结构: handle   已分配XMS的句柄
 *       nbytes   分配的K字节数
 *       next     指向下一个结构
**----------------------------------------------------------------------------------*/
struct __xms__allocate_s {
    XMSHANDLE    handle;
    INT16U       nbytes;
    struct __xms__allocate_s * next;
};

#define XMS_ALLOC_LIST_MAX_SIZE  64

static struct __xms__allocate_s   __xms_alloc_buff[XMS_ALLOC_LIST_MAX_SIZE];
static struct __xms__allocate_s * __xms_alloc_list = NULL;
static struct __xms__allocate_s * __xms_alloc_free = NULL;


/*------------------------------------------------------------------------------------
 * 函数:    __xms_initialize_alloc_list()
 *
 * 描述:    初始化扩展内存分配列表
**----------------------------------------------------------------------------------*/
void __internal __xms_initialize_alloc_list(void)
{
    int  i;
    
    __xms_alloc_list = NULL;
    __xms_alloc_free = NULL;

    for(i=0; i<XMS_ALLOC_LIST_MAX_SIZE; i++){
        __xms_alloc_buff[i].handle = 0;
        __xms_alloc_buff[i].nbytes = 0;
        __xms_alloc_buff[i].next = __xms_alloc_free;
        __xms_alloc_free = &(__xms_alloc_buff[i]);
    }
}

/*------------------------------------------------------------------------------------
 * 函数:    __xms_chain_to_alloc_list()
 *
 * 描述:    链入扩展内存分配列表
**----------------------------------------------------------------------------------*/
BOOL __internal __xms_chain_to_alloc_list(XMSHANDLE handle, INT16U nbytes)
{
    struct __xms__allocate_s *s;
    BOOL   retval;
    
    FamesAssert(handle != 0);
    FamesAssert(nbytes != 0);

    if(0==handle || 0==nbytes){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(__xms_alloc_free){
        s = __xms_alloc_free;      /* allocate a free one */
        __xms_alloc_free = s->next;

        s->handle = handle;
        s->nbytes = nbytes;
        s->next = __xms_alloc_list;/* link to list */
        __xms_alloc_list = s;      

        retval = ok;
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    __xms_unchain_from_alloc_list()
 *
 * 描述:    与扩展内存分配列表断开联接
 *
 * 参数:    handle 扩展内存句柄
**----------------------------------------------------------------------------------*/
BOOL __internal __xms_unchain_from_alloc_list(XMSHANDLE handle)
{
    struct __xms__allocate_s **s, *t;
    BOOL retval;
    
    FamesAssert(handle);

    if(0==handle){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(__xms_alloc_list){
        for(s = &__xms_alloc_list; *s;){
            if((*s)->handle == handle){ /* found */
                t = (*s);
               (*s) = (*s)->next;       /* disconnect */;
                t->handle = 0;
                t->nbytes = 0;
                t->next = __xms_alloc_free; /* link to free list */
                __xms_alloc_free = t;
                retval = ok;
                break;
            }
            s = &(*s)->next;
        }
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    XMSalloc()
 *
 * 描述:    分配一块扩展内存(最多可分配32块)
 *
 * 输入:    size 所要分配的扩展内存块的大小，以K字节为单位
 *
 * 输出:    所分配的扩展内存块的句柄，若该值为0，表示分配失败
**----------------------------------------------------------------------------------*/
XMSHANDLE apical XMSalloc(INT16U size)
{
    XMSHANDLE handle;

    FamesAssert(size>0);

    if(0==size)
        return 0;

    handle = XMS(size);

    if(handle){
        __xms_chain_to_alloc_list(handle, size);
    }

    return handle;
}

/*------------------------------------------------------------------------------------
 * 函数:    XMSfree()
 *
 * 描述:    释放扩展内存块
 *
 * 输入:    handle 所要释放的扩展内存块的句柄
 *
 * 输出:    fail=失败，ok=成功
**----------------------------------------------------------------------------------*/
void apical XMSfree(XMSHANDLE handle)
{
    FamesAssert(handle);

    if(0==handle)
        return;

    if(__xms_unchain_from_alloc_list(handle)){
        _XMS(handle);
    }

    return;
}

/*------------------------------------------------------------------------------------
 * 函数:    init_xms_service()
 *
 * 描述:    扩展内存管理模块初始化
**----------------------------------------------------------------------------------*/
BOOL apical __init init_xms_service(void)
{
    void __exit onexit_xms_service(void);

    if(!RegisterOnExit(onexit_xms_service)){
        sys_print("init_xms_service: Failed to register onexit_xms_service!\n");
        return fail;
        /* 
        * do something else here 
        */
    }
    __xms_initialize_alloc_list();
    return (BOOL)XMSinit();
}

/*------------------------------------------------------------------------------------
 * 函数:    onexit_xms_service()
 *
 * 描述:    扩展内存管理模块退出例程
**----------------------------------------------------------------------------------*/
void __exit onexit_xms_service(void)
{
    lock_kernel();
    while(__xms_alloc_list){
        if(__xms_alloc_list->handle){
            _XMS(__xms_alloc_list->handle);
        }
        __xms_alloc_list = __xms_alloc_list->next;
    }
    unlock_kernel();
}


#endif                                        /* #if XMS_EN == 1 */

/*====================================================================================
 * 
 * 本文件结束: xms.h
 * 
**==================================================================================*/


