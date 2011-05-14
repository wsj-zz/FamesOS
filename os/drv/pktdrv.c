/***********************************************************************************************
 * 文件:    pktdrv.c
 *
 * 描述:    PacketDriver类驱动接口
 *
 * 作者:    Jun
 *
 * 时间:    2010-11-1
***********************************************************************************************/
#define FAMES_NIC_PKTDRV_C
#include "includes.h"


#if NIC_PKTDRV_EN == 1

#define PKTDRV_DEBUG_EN 0

/*----------------------------------------------------------------------------------------------
 * 
 * 重要说明: 由于PKTDRV驱动中有中断处理过程, 而这些中断处理程序并不符合FamesOS对中断服务
 *           程序的要求, 因此, 若要使用PKTDRV, 就必须关闭占先式调度的特性
 * 
 * FamesOS对ISR的要求: 在进入中断时要执行: IntNesting++;  
 *                     在退出中断时要执行: IntNesting--;  
**--------------------------------------------------------------------------------------------*/
#if DISPATCH_PREEMPTIVE_EN!=0
#error DISPATCH_PREEMPTIVE_EN must be set to 0!
#endif


void interrupt (far *pktdrv_entry)(void) = NULL;
INT16U  pktdrv_entry_cs=0, pktdrv_entry_ip=0;

INT16S  pktdrv_svc_intno=0;
INT16S  pktdrv_open_state=NO;

INT16S  pktdrv_handle=0;
INT16S  pktdrv_lasterr=0;
INT08U  pktdrv_mac_addr[6];
INT08S  pktdrv_nic_name[32];

INT08S  pktdrv_rx_buf[PKTDRV_MAX_LEN];   /* PKTDRV.receiver的接收缓冲 */
INT16S  pktdrv_rx_bytes=0;               /* PKTDRV.receiver接收字节数 */
INT16S  pktdrv_rx_rcvd=NO;               /* PKTDRV.receiver已接收标志 */

#define PKTDRV_xmit_timeOut 3000L
INT16S  pktdrv_xmit_timeout=NO;

#define call_pktdrv_entry() __call_pktdrv_entry()

void __call_pktdrv_entry(void)
{
    asm nop;
    asm nop;
    asm nop;
    asm nop;
    asm nop;
}


void __pktdrv_xmit_timeout(void * data, INT16S nr)
{
    data = data;
    nr   = nr;
    pktdrv_xmit_timeout = YES;
}

void __pktdrv_receiver(void)
{
/*lint --e{563}*/
    asm pop  di; /* 这两句是为了抵消TC自动加入的push si, push di */
    asm pop  si;
    
    asm push ds;
    asm push ax;
    asm mov  ax, seg pktdrv_rx_buf;
    asm mov  ds, ax;
    
    asm mov  ax, pktdrv_rx_rcvd;  /* 数据是否已处理, 1=未处理 */
    asm cmp  ax, fail;
    asm pop  ax;
    asm jnz  make_esdi_0;

    asm cmp  ax, 0;
    asm jz   ax_equ_0;
    asm cmp  ax, 1;
    asm jz   ax_equ_1;
    
    make_esdi_0:
    asm mov  di, 0;
    asm mov  es, di;
    asm pop  ds
    asm retf;

    ax_equ_0:
    asm cmp  cx, 1516;
    asm jnb  make_esdi_0;
    asm mov  pktdrv_rx_bytes, cx;
    asm mov  ax, seg pktdrv_rx_buf;
    asm mov  es, ax
    asm mov  di, offset pktdrv_rx_buf;
    asm pop  ds
    asm retf;

    ax_equ_1:
    asm mov  pktdrv_rx_rcvd, YES;
    asm pop  ds
    asm retf;
}

BOOL init_pktdrv(void)
{
    return ok;
}

BOOL open_pktdrv(void)
{ 
/*lint --e{550, 563}*/
    static INT08S type[]="\0x8\0x6";
    INT16U v_si, v_di, v_es, v_ds;
    INT16S v_ax;
    INT16S retval=fail;

    if(pktdrv_open_state){
        return fail;
    }
    
    lock_kernel();

    v_ds = FP_SEG(&type[0]);
    v_si = FP_OFF(&type[0]);
    v_es = FP_SEG(__pktdrv_receiver);
    v_di = FP_OFF(__pktdrv_receiver);

    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push ds
    asm push si
    asm push es
    asm push di
    
    /* access_type() */
    asm mov ax, 0x0201;
    asm mov bx, 0xFFFF;
    asm mov dl, 0;
    asm mov ds, v_ds;
    asm mov si, v_si;
    asm mov cx, 0
    asm mov es, v_es;
    asm mov di, v_di;
    call_pktdrv_entry();              
    asm jb some_error1;          
    asm mov word ptr retval, ok           
    asm mov v_ax, ax;            
    asm jmp end_this1          
    some_error1:           
    asm mov word ptr retval, fail;        
    asm mov dl, dh
    asm xor dh, dh
    asm mov pktdrv_lasterr, dx;  
    pktdrv_lasterr&=0xff;        
    end_this1:
    if(!retval)  /*lint !e774*/
        goto end_open;
    pktdrv_handle = v_ax; /*lint !e530*/

    /* get_address() */
    v_es = FP_SEG(&pktdrv_mac_addr[0]);
    v_di = FP_OFF(&pktdrv_mac_addr[0]);
    asm mov ax, 0x0600;
    asm mov bx, pktdrv_handle;
    asm mov cx, 6;
    asm mov es, v_es;
    asm mov di, v_di;
    call_pktdrv_entry();              
    asm jb some_error2;          
    asm mov word ptr retval, ok           
    asm mov v_ax, ax;            
    asm jmp end_this2          
    some_error2:           
    asm mov word ptr retval, fail;        
    asm mov dl, dh
    asm xor dh, dh
    asm mov pktdrv_lasterr, dx;  
    pktdrv_lasterr&=0xff;        
    end_this2:
    if(!retval)  /*lint !e774 */
        goto release_handle;

    /* set_rcv_mode() */
    asm mov ax, 0x1400;
    asm mov bx, pktdrv_handle;
    asm mov cx, 3; /* receive own and broadcast packets */
    call_pktdrv_entry();              
    asm jb some_error3;          
    asm mov word ptr retval, ok           
    asm mov v_ax, ax;            
    asm jmp end_this3          
    some_error3:           
    asm mov word ptr retval, fail;        
    asm mov dl, dh
    asm xor dh, dh
    asm mov pktdrv_lasterr, dx;  
    pktdrv_lasterr&=0xff;        
    end_this3:
    if(!retval)  /*lint !e774 */
        goto release_handle;

    /* driver_info() */
    asm mov ax, 0x1FF;
    asm mov bx, pktdrv_handle;
    call_pktdrv_entry();              
    asm jb some_error4;          
    asm mov word ptr retval, ok           
    asm mov v_ax, ax;            
    asm jmp end_this4          
    some_error4:           
    asm mov word ptr retval, fail;        
    asm mov dl, dh
    asm xor dh, dh
    asm mov pktdrv_lasterr, dx;  
    pktdrv_lasterr&=0xff;        
    end_this4:
    if(!retval)  /*lint !e774 */
        goto release_handle;
    asm mov v_ds, ds;
    asm mov v_si, si; /* ds:si -> name */

    goto end_open;
    
    release_handle:
    asm mov ax, 0x0300;
    asm mov bx, pktdrv_handle;
    call_pktdrv_entry();              
    asm jb some_error5;          
    asm mov word ptr retval, ok           
    asm mov v_ax, ax;            
    asm jmp end_this5          
    some_error5:           
    asm mov word ptr retval, fail;        
    asm mov dl, dh
    asm xor dh, dh
    asm mov pktdrv_lasterr, dx;  
    pktdrv_lasterr&=0xff;        
    end_this5:
    retval = fail;
    pktdrv_handle = 0;

    end_open:
    asm pop di
    asm pop es
    asm pop si
    asm pop ds
    asm pop dx
    asm pop cx
    asm pop bx
    asm pop ax

    STRCPY((INT08S *)pktdrv_nic_name, (INT08S *)MK_FP(v_ds, v_si));
    set_local_mac(pktdrv_mac_addr);
    PKTDRV_DRV.NicName = pktdrv_nic_name;

    pktdrv_open_state = retval;

    unlock_kernel();

    #if PKTDRV_DEBUG_EN==1 
    lock_kernel();
    if(1){
        static char msg[100];
        sprintf(msg, "open_pktdrv(): pktdrv_handle=%04X, mac=%02X:%02X:%02X:%02X:%02X:%02X, intno=%04X\n", 
            pktdrv_handle, 
            pktdrv_mac_addr[0],
            pktdrv_mac_addr[1],
            pktdrv_mac_addr[2],
            pktdrv_mac_addr[3],
            pktdrv_mac_addr[4],
            pktdrv_mac_addr[5],
            pktdrv_svc_intno);
        textprintstr(1, 3, msg, 0);
    }
    unlock_kernel();
    #endif
    
    return retval;
}


BOOL stop_pktdrv(void)
{
/*lint --e{563}*/
    INT16S retval;
    
    if(pktdrv_open_state==NO)
        return fail;

    retval = fail;
    
    lock_kernel();

    /* release_type */
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm mov ax, 0x0300;
    asm mov bx, pktdrv_handle;
    call_pktdrv_entry();              
    asm jb some_error6;          
    asm mov word ptr retval, ok           
    asm jmp end_this6          
    some_error6:           
    asm mov word ptr retval, fail;        
    asm mov dl, dh
    asm xor dh, dh
    asm mov pktdrv_lasterr, dx;  
    pktdrv_lasterr&=0xff;        
    end_this6:
    asm pop dx
    asm pop cx
    asm pop bx
    asm pop ax

    pktdrv_handle = 0;
    
    unlock_kernel();
    
    return retval;
}

BOOL xmit_pktdrv(void * srcbuf, INT16S buflen)
{
/*lint --e{550,563}*/
    static INT16S __lock=NO;
    BOOL   retval;
    INT16U v_si, v_ds;
    static INT08S * xmitbuf = NULL;

    if(!islink_pktdrv()){
        return fail;
    }

    if(!xmitbuf) /* 申请内存空间, 如果失败则直接返回 */
        allocate_buffer(xmitbuf, INT08S *, (INT32U)PKTDRV_MAX_LEN, return fail);
    
    if(buflen > MAX_ETH_FRAME_SIZE){
        buflen = MAX_ETH_FRAME_SIZE;
    }
    if(buflen < 64){
        buflen = 64;
    }
    os_mutex_lock(__lock);
    v_si=FP_OFF(&xmitbuf[0]);/*lint !e507*/
    v_ds=FP_SEG(&xmitbuf[0]);
    MEMCPY((INT08S *)xmitbuf, (INT08S *)srcbuf, buflen);
    pktdrv_xmit_timeout=NO;
    TimerSet(TimerNIC, PKTDRV_xmit_timeOut, TIMER_TYPE_ONCE, __pktdrv_xmit_timeout, NULL);
    while(!pktdrv_xmit_timeout){
        lock_kernel();
        asm push ax
        asm push bx
        asm push cx
        asm push dx
        asm push ds
        asm push si
        asm mov ax, 0x0400;
        asm mov ds, v_ds;
        asm mov si, v_si;
        asm mov cx, buflen;
        call_pktdrv_entry();              
        asm jb some_error7;          
        asm mov word ptr retval, ok           
        asm jmp end_this7;          
        some_error7:           
        asm mov word ptr retval, fail;
        asm mov dl, dh
        asm xor dh, dh
        asm mov pktdrv_lasterr, dx;  
        pktdrv_lasterr&=0xff;        
        end_this7:
        asm pop si
        asm pop ds
        asm pop dx
        asm pop cx
        asm pop bx
        asm pop ax
        unlock_kernel();
        if(retval) /*lint !e530*/
            break;
        TaskSleep(100L);
    }
    TimerStop(TimerNIC);
    os_mutex_unlock(__lock); /*lint !e717*/
    
    #if PKTDRV_DEBUG_EN==1 
    lock_kernel();
    if(1){
        printf("xmit_pktdrv(): pktdrv_entry=%p, buflen=%d\n", pktdrv_entry, buflen);
    }
    unlock_kernel();
    #endif

    return retval;
}

BOOL poll_pktdrv(void * dstbuf, INT16S * buflen)
{
    INT16S        rx_rcvd;
    static INT16S __lock=NO;
    prepare_atomic()

    FamesAssert(dstbuf && buflen);

    if(!dstbuf || !buflen)
        return fail;

    #if PKTDRV_DEBUG_EN==1 
    lock_kernel();
    if(1){
        printf("poll_pktdrv()+s: pktdrv_rx_rcvd=%d, pktdrv_rx_buf=%p%c", 
            pktdrv_rx_rcvd, pktdrv_rx_buf, pktdrv_rx_rcvd?'\n':'\r');
    }
    unlock_kernel();
    #endif
        
    if(!islink_pktdrv()){
        return fail;
    }
    os_mutex_lock(__lock); /*lint !e506*/
    in_atomic();
    rx_rcvd=pktdrv_rx_rcvd;
    out_atomic();
    if(rx_rcvd){  /* a packet received */
        in_atomic();
        *buflen = pktdrv_rx_bytes;
        out_atomic();
        
        #if PKTDRV_DEBUG_EN==1 
        lock_kernel();
        if(1){
            printf("poll_pktdrv()+1: buflen=%d, pktdrv_rx_bytes=%d\n",  *buflen, pktdrv_rx_bytes);
        }
        unlock_kernel();
        #endif
        
        if(*buflen > MAX_ETH_FRAME_SIZE){
            *buflen = MAX_ETH_FRAME_SIZE;
        }
        MEMCPY((INT08S *)dstbuf, (INT08S *)pktdrv_rx_buf, *buflen);
        in_atomic();
        pktdrv_rx_rcvd=NO;
        out_atomic();
        os_mutex_unlock(__lock); /*lint !e717*/

        #if PKTDRV_DEBUG_EN==1 
        lock_kernel();
        if(1){
            printf("poll_pktdrv()+e: pktdrv_rx_rcvd=%d, buflen=%d\n", pktdrv_rx_rcvd, *buflen);
        }
        unlock_kernel();
        #endif

        return ok;
    } else {
        *buflen=0;
        os_mutex_unlock(__lock); /*lint !e717*/
        return fail;
    }
}

BOOL ctrl_pktdrv(INT16S cmd, void * data)
{
    data = data;
    switch(cmd){
        case 0:
            break;
        default:
            break;
    }
    return ok;
}

BOOL get_status_pktdrv(void * status)
{
    status = status;
    return ok;
}

BOOL islink_pktdrv(void)
{
    if(pktdrv_handle)
        return ok;
    return fail;
}

void watchdog_pktdrv(void * data, INT16S nr)
{
    data = data;
    nr   = nr;
 /* There is nothing here */
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pktdrv_setup()
 * 
 * 功能:    检查Packet Driver是否已安装, 计算其入口地址
**--------------------------------------------------------------------------------------------*/
BOOL pktdrv_setup(void)
{
    INT08U * pktdrv_intno_locator=NULL;
    prepare_atomic()
    
    pktdrv_svc_intno = pktdrv_probe();

    if(pktdrv_svc_intno < 0x60 || pktdrv_svc_intno > 0x80)
        return fail;
    
    lock_kernel();
    pktdrv_entry    = FamesGetVect(pktdrv_svc_intno);
    pktdrv_entry_cs = FP_SEG((INT32U)pktdrv_entry);
    pktdrv_entry_ip = FP_OFF((INT32U)pktdrv_entry);

    pktdrv_entry    = pktdrv_entry;
    pktdrv_entry_cs = pktdrv_entry_cs;
    pktdrv_entry_ip = pktdrv_entry_ip;
    pktdrv_lasterr  = pktdrv_lasterr;
    unlock_kernel();

    pktdrv_intno_locator = (INT08U far *)__call_pktdrv_entry; /*lint !e611*/
    in_atomic();
    pktdrv_intno_locator[0]=(INT08U)'\xCD'; /*lint !e743, 构造一条int指令 */
    pktdrv_intno_locator[1]=(INT08U)(pktdrv_svc_intno&0xFF);
    pktdrv_intno_locator[2]=(INT08U)'\xCB'; /*lint !e743 */   
    out_atomic();
    
    #if PKTDRV_DEBUG_EN==1 
    lock_kernel();
    if(1){
        static char msg[100];
        sprintf(msg, "pktdrv_setup(): pktdrv_svc_intno=0x%x, pktdrv_entry=%p\n", pktdrv_svc_intno, pktdrv_entry);
        textprintstr(1, 2, msg, 0);
    }
    unlock_kernel();
    #endif
    
    return ok;
}


/*----------------------------------------------------------------------------------------------
 * 
 * 函数:    pktdrv_probe()
 * 
 * 功能:    在系统中, 查找是否有已安装的Packet Driver
 *
 * 说明:    在中断服务函数里判断是否有一个字符串"PKT DRVR", 如果有, 说明已安装了
 *          PKTDRV驱动, 扫描中断的正确范围为: 0x60-0x66, 0x68-0x6f, 0x78-0x80
 *          我们这里扫描0x60-0x80这个范围
**--------------------------------------------------------------------------------------------*/
INT16S pktdrv_probe(void)
{
    INT08S * sign_str="PKT DRVR";
    INT08S * ptr;
    INT16S int_no;
    INT16S svc_int_no=0;

    for(int_no = 0x60; int_no <= 0x80; int_no ++) {
        ptr = (INT08S *)FamesGetVect(int_no);  /*lint !e611 */
        ptr += 3;
        if(!MEMCMP(ptr, sign_str, 9)){
            svc_int_no=int_no;
            break;
        }
    }

    #if PKTDRV_DEBUG_EN==1 
    lock_kernel();
    if(1){
        static char msg[100];
        sprintf(msg, "pktdrv_probe(): svc_int_no=0x%x", svc_int_no);
        textprintstr(1, 1, msg, 0);
    }
    unlock_kernel();
    #endif

    return svc_int_no;
}

#endif   /* #if NIC_PKTDRV_EN == 1     */


/*==============================================================================================
 * 
 * 本文件结束: pktdrv.c
 * 
**============================================================================================*/

