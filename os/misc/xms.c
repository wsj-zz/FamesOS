/*************************************************************************************
 * �ļ�: xms.c
 *
 * ����: ��չ�ڴ�(XMS)��غ���
 *
 * ע��: ���ļ������Դ�ģʽ����(��: TCC -1 -B -ml ###.C)
 *
 * ����: Jun
 *
 * ʱ��: 2010-03-03
 *
 * �汾: V0.1 (2010-03-03, ����İ汾)
*************************************************************************************/
#define  FAMES_XMS_C
#include "includes.h"

#if    XMS_EN == 1
/*------------------------------------------------------------------------------------
 * ����:    XMSinit()
 * ����:    XMS��ʼ��
 * ����:    ��
 * ���:    ���ñ�־XMSOKΪOK/FAIL������
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
 * ����:    XMSfreesize()
 * ����:    ȡ��ǰ������չ�ڴ��ܵĴ�С
 * ����:    ��
 * ���:    ��K�ֽڼƵ�������չ�ڴ�Ĵ�С
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
 * ����:    XMSlargestblock()
 * ����:    ȡ��ǰ������չ�ڴ��Ĵ�С
 * ����:    ��
 * ���:    ��K�ֽڼƵ������չ�ڴ��Ĵ�С
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
 * ����:    XMS()
 * ����:    ����һ����չ�ڴ�(���ɷ���32��)
 * ����:    size ��Ҫ�������չ�ڴ��Ĵ�С����K�ֽ�Ϊ��λ
 * ���:    ���������չ�ڴ��ľ��������ֵΪ0����ʾ����ʧ��
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
 * ����:    _XMS()
 * ����:    �ͷ���չ�ڴ��
 * ����:    handle ��Ҫ�ͷŵ���չ�ڴ��ľ��
 * ���:    fail=ʧ�ܣ�ok=�ɹ�
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
 * ����:    XMSrealloc()
 * ����:    ���µĳߴ����·�����չ�ڴ�
 * ����:    size   �µĴ�С
 *          handle Ҫ���·������չ�ڴ��ľ��
 * ���:    fail=ʧ�ܣ�OK=�ɹ�
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
 * ����:    XMSmove()
 * ����:    �ƶ��ڴ��
 * ����:    �ڴ��ƶ���Ϣ��ṹ��ָ��
 * ���:    ok
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
 * ����:    XMSput()
 * ����:    д��չ�ڴ�
 * ����:    handle Ŀ�Ŀ�ľ��
 *          dp     ��д�����ݿ���Ŀ�Ŀ��е�ƫ��ֵ
 *          sp     ���ݿ��ڳ����ڴ��еĵ�ַ
 *          length ��д�����ݿ�ĳ��ȣ����ֽ�Ϊ��λ
 * ���:    ok
 * ע��:    Դ��ֻ���ڳ����ڴ��У�������Ϊ0����˲��贫��Դ���
 *          ������ú���������������ݳ��ȵĴ���(��ʵû�д���)
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
 * ����:    XMSget()
 * ����:    ����չ�ڴ�
 * ����:    handle Դ��ľ��
 *          dp     ����������ݵĳ����ڴ滺�����ĵ�ַ
 *          sp     ����������Դ���е�ƫ��
 *          length �������ݵĳ��ȣ����ֽڼ�
 * ���:    ok
 * ע��:    Ŀ�Ŀ�ֻ���ڳ����ڴ��У�������Ϊ0�����
 *          ���贫��Ŀ�Ŀ�ľ�����ú����������������
 *          ���ȵĴ������ܸú����ܶ��������ȵ����ݣ�
 *          ���ڳ���Ϊ����ʱ���ٶ����Խ�����
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
    
    if(length&(INT32S)1L) { /* ����Ϊ���� */
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
        d[length]=p[0];/* TC����ʱ�����һ������: 
                          Conversion may lose significant digits 
                          ������Ϊlength�Ǹ�32λ����,
                          ��d[length]��ʽ�Ľ�֮ת����16λ����, ʵ����,
                          ����length��ֵ���ᳬ��һ��16λ����(һ���εĴ�С),
                          ���, ���������Բ������. --- Jun, 2010/12/28
                       */
    } else { /* ����Ϊż�� */
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
 *          ����Ĳ���ΪXMS��չ�ڴ�Ĺ���ģ��
 * 
 * 
**----------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------ 
 *          ��չ�ڴ�����б�:
 * 
 * ����: ���ڹ�����չ�ڴ�ķ��������
 *
 * �ṹ: handle   �ѷ���XMS�ľ��
 *       nbytes   �����K�ֽ���
 *       next     ָ����һ���ṹ
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
 * ����:    __xms_initialize_alloc_list()
 *
 * ����:    ��ʼ����չ�ڴ�����б�
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
 * ����:    __xms_chain_to_alloc_list()
 *
 * ����:    ������չ�ڴ�����б�
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
 * ����:    __xms_unchain_from_alloc_list()
 *
 * ����:    ����չ�ڴ�����б�Ͽ�����
 *
 * ����:    handle ��չ�ڴ���
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
 * ����:    XMSalloc()
 *
 * ����:    ����һ����չ�ڴ�(���ɷ���32��)
 *
 * ����:    size ��Ҫ�������չ�ڴ��Ĵ�С����K�ֽ�Ϊ��λ
 *
 * ���:    ���������չ�ڴ��ľ��������ֵΪ0����ʾ����ʧ��
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
 * ����:    XMSfree()
 *
 * ����:    �ͷ���չ�ڴ��
 *
 * ����:    handle ��Ҫ�ͷŵ���չ�ڴ��ľ��
 *
 * ���:    fail=ʧ�ܣ�ok=�ɹ�
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
 * ����:    init_xms_service()
 *
 * ����:    ��չ�ڴ����ģ���ʼ��
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
 * ����:    onexit_xms_service()
 *
 * ����:    ��չ�ڴ����ģ���˳�����
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
 * ���ļ�����: xms.h
 * 
**==================================================================================*/


