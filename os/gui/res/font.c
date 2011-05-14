/***********************************************************************************************
 * 文件:    font.c
 *
 * 说明:    字形显示(或字库操作)
 *
 * 作者:    Jun
 *
 * 时间:    2010-12-29
 *
 * 注意:    此版本只能处理点阵字库
***********************************************************************************************/
#define  FAMES_FONT_C
#include <includes.h>

/*lint -e679 :Suspicious Truncation in arithmetic expression */
/*----------------------------------------------------------------------------------------------
 * 
 * 字体注册结构
 * 
**--------------------------------------------------------------------------------------------*/
struct font_register_s {
    FONTINFO hz, asc;
    struct font_register_s * next;
};

#define FONT_MAX_NR  16

static struct font_register_s font_register_buff[FONT_MAX_NR];
static int    font_registered = 0; /* 已注册个数 */


/*----------------------------------------------------------------------------------------------
 * 函数:    load_font()
 *  
 * 描述:    加载一种字节(加载到XMS中)
**--------------------------------------------------------------------------------------------*/
BOOL apical load_font(FONTINFO * fontinfo)
{
/*lint --e{534}*/
    XMSHANDLE handle;
    BOOL  retval;
    int fhdl;
    INT32U i;
    INT32U filesize;
    void * buf;
    INT08S filename[64];

    FamesAssert(fontinfo);

    if(!fontinfo)
        return fail;

    FamesAssert(fontinfo->filename);

    if(!fontinfo->filename)
        return fail;

    STRCPY(filename, fontinfo->filename);

    retval = fail;

    buf = mem_alloc(1024L);
    if(!buf)
        return fail;

    lock_kernel();
    fhdl = open(filename, O_RDONLY|O_BINARY);/*lint !e569*/
    if(fhdl == -1){
        goto out;
    }
    lseek(fhdl, 0L, SEEK_END);
    filesize = (INT32U)tell(fhdl);
    if(filesize < 16L){
        goto out;
    }
    #if 0
    printf("load_font(): filename=%s, filesize=%ld\n", fontinfo->filename, filesize);
    getch();
    #endif
    filesize = filesize+1023L;
    filesize >>= 10; /* /1024 */
    handle = XMSalloc((INT16U)filesize);
    if(!handle){
        goto out1;
    }
    lseek(fhdl, 0L, SEEK_SET);
    for(i=0; ; i++){
        if(eof(fhdl))
            break;
        if(!read(fhdl, (void *)buf, 1024)){/*lint !e643*/
        }
        if(!XMSput(handle, (void *)(i<<10L), (void *)buf, 1024L)){/*lint !e571*/
            ; /* load to xms */
        }
    }
    close(fhdl);
    fontinfo->handle = handle;
    fontinfo->loaded = YES;

out:
    unlock_kernel();
    mem_free(buf);
    
    return retval;

out1:
    close(fhdl);/*lint !e644, 这里应该是没有问题的, 禁了它*/
    goto out;    
}


/*----------------------------------------------------------------------------------------------
 * 函数:    unload_font()
 *  
 * 描述:    卸载一种字节(从XMS中删除)
**--------------------------------------------------------------------------------------------*/
BOOL apical unload_font(FONTINFO * fontinfo)
{
    FamesAssert(fontinfo);

    if(!fontinfo){
        return fail;
    }
    if(fontinfo->loaded!=YES){
        return fail;
    }
    if(fontinfo->handle==0){
        return fail;
    }
    lock_kernel();
    fontinfo->loaded = NO;
    unlock_kernel();
    XMSfree(fontinfo->handle);
    return ok;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    get_font_info()
 *  
 * 描述:    读取字体信息
**--------------------------------------------------------------------------------------------*/
BOOL apical get_font_info(FONTINFO * dest, int font)
{
    struct font_register_s * fnt;

    FamesAssert(dest);

    if(!dest || font < 0 || font >= font_registered)
        return fail;

    lock_kernel();
    fnt = &font_register_buff[font];
    *dest = fnt->asc;
    if(dest->loaded != YES){
        *dest = fnt->hz;
        dest->width /= 2;
    }
    unlock_kernel();
    
    if(dest->loaded != YES)
        return fail;

    return ok;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    register_font()
 *  
 * 描述:    注册一种字体
 *
 * 返回:    注册的字体句柄
**--------------------------------------------------------------------------------------------*/
int apical register_font(FONTINFO * hz, FONTINFO * asc)
{
    int retval;
    
    if(font_registered>=FONT_MAX_NR)
        return -1;

    lock_kernel();
    retval = font_registered;
    font_registered++;
    if(hz){
        font_register_buff[retval].hz = *hz;
    } else {
        font_register_buff[retval].hz.loaded = NO;
    }
    if(asc){
        font_register_buff[retval].asc = *asc;
    } else {
        font_register_buff[retval].asc.loaded = NO;
    }
    unlock_kernel();

    return retval;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    unregister_font()
 *  
 * 描述:    注销一种字体
**--------------------------------------------------------------------------------------------*/
BOOL apical unregister_font(int font)
{
    if(font<0 || font>=FONT_MAX_NR)
        return fail;

    if(font==0)
        return fail; /* 不允许注销系统字体 */

    lock_kernel();
    font_register_buff[font].hz.loaded = NO;
    font_register_buff[font].asc.loaded = NO;
    unlock_kernel();

    return ok;
}


/*----------------------------------------------------------------------------------------------
 * 
 *      字模缓冲
 * 
**--------------------------------------------------------------------------------------------*/
static COLOR FONTCACHE[((FONT_MAX_SIZE+7)/8)*FONT_MAX_SIZE];

BOOL __internal __get_font_cache(XMSHANDLE handle, int index, int size)
{
    INT32S posi;

    posi = (INT32S)index * size;

    #if 0
    if(handle){
        FILE * fp;
        fp=fopen("asc14", "rb");
        fseek(fp, 14L*index, SEEK_SET);
        fread(FONTCACHE, 14, 1, fp);
        fclose(fp);
    } else {
        FILE * fp;
        fp=fopen("hzk14", "rb");
        fseek(fp, 28L*index, SEEK_SET);
        fread(FONTCACHE, 28, 1, fp);
        fclose(fp);
    }
    return ok;
    #endif
    
    return (BOOL)XMSget(handle, (void *)FONTCACHE, (void *)posi, (INT32S)size);
}

/*----------------------------------------------------------------------------------------------
 * 函数:    draw_font()
 *  
 * 描述:    以指定字体显示一个字符串
**--------------------------------------------------------------------------------------------*/
BOOL apical draw_font(int x, int y, INT08S *str, COLOR color, int font, INT16U opt)
{
/*lint --e{534}*/
    int wid_asc, wid_hz;
    int height_asc, height_hz;
    XMSHANDLE aschdl, hzhdl;
    int ascbol, hzbol; /* bol = bytes per line */
    struct font_register_s * fnt;
    int i,j,k,dots;
    int zcode, bcode;

    FamesAssert(str);

    if(!str)
        return fail;

    if(font < 0 || font >= font_registered)
        return fail;

    fnt = &font_register_buff[font];

    wid_asc = fnt->asc.width;
    wid_hz  = fnt->hz.width;
    height_asc = fnt->asc.height;
    height_hz  = fnt->hz.height;

    aschdl = fnt->asc.handle;
    hzhdl  = fnt->hz.handle;

    ascbol = fnt->asc.bytesofline;
    hzbol  = fnt->hz.bytesofline;

    if(fnt->asc.loaded == NO){
        wid_asc = 0;
        height_asc = 0;
    }
    if(fnt->hz.loaded == NO){
        wid_hz = wid_asc*2;
        height_hz = height_asc;
    }
    if(wid_hz == 0)
        return fail;
    if(wid_asc == 0){
        wid_asc = wid_hz/2;
        height_asc = height_hz;
    }

    lock_kernel();
    for(i=0; str[i]; i++){
        if(str[i]>=0x20 && !(str[i]&0x80)){ /*ASC*/
            if(fnt->asc.loaded == NO)
                continue;
            if(!__get_font_cache(aschdl, ((int)str[i])-fnt->asc.start, (height_asc*ascbol)))
                continue;
            for(j=0; j<height_asc; j++){
                for(k=0; k<(ascbol); k++){
                    dots = (wid_asc-k*8);
                    if(dots>8)dots=8;
                    if(opt & DRAW_OPT_FIL_BG){
                        gdi_draw_h_bitmap_bg(x+k*8, y+j, dots, FONTCACHE[j*ascbol+k], color, WIDGET_BKCOLOR);
                    } else {
                        gdi_draw_h_bitmap(x+k*8, y+j, dots, FONTCACHE[j*ascbol+k], color);
                    }
                }
            }
            x+=wid_asc;
        } else if(str[i]&0x80 && str[i+1]&0x80){ /*HZ*/
		    zcode=(str[i]-0xa1)&0x7f;
		    bcode=(str[i+1]-0xa1)&0x7f;
            i++;
            if(fnt->hz.loaded == NO)
                continue;
            if(!__get_font_cache(hzhdl, (zcode*94+bcode)-fnt->hz.start, (height_hz*hzbol)))
                continue;
            for(j=0; j<height_hz; j++){
                for(k=0; k<(hzbol); k++){
                    dots = (wid_hz-k*8);
                    if(dots>8)dots=8;
                    if(opt & DRAW_OPT_FIL_BG){
                        gdi_draw_h_bitmap_bg(x+k*8, y+j, dots, FONTCACHE[j*hzbol+k], color, WIDGET_BKCOLOR);
                    } else {
                        gdi_draw_h_bitmap(x+k*8, y+j, dots, FONTCACHE[j*hzbol+k], color);
                    }
                }
            }
            x+=wid_hz;
        } else {
            /* 其它情况不理会 */
        }
    }
    unlock_kernel();

    return ok;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    draw_font_ex()
 *  
 * 描述:    以指定字体显示一个字符串
**--------------------------------------------------------------------------------------------*/
BOOL apical draw_font_ex(int x, int y, int width, int height, 
                        INT08S * str, 
                        COLOR color, COLOR bkcolor,
                        int font, INT16U opt)
{
/*lint --e{534}*/
    int wid_asc, wid_hz;
    int height_asc, height_hz;
    XMSHANDLE aschdl, hzhdl;
    int ascbol, hzbol; /* bol = bytes per line */
    struct font_register_s * fnt;
    int i,j,k,dots;
    int zcode, bcode;
    int total_len;

    FamesAssert(str);

    if(!str)
        return fail;

    if(font < 0 || font >= font_registered)
        return fail;

    fnt = &font_register_buff[font];

    wid_asc = fnt->asc.width;
    wid_hz  = fnt->hz.width;
    height_asc = fnt->asc.height;
    height_hz  = fnt->hz.height;

    aschdl = fnt->asc.handle;
    hzhdl  = fnt->hz.handle;

    ascbol = fnt->asc.bytesofline;
    hzbol  = fnt->hz.bytesofline;

    if(fnt->asc.loaded == NO){
        wid_asc = 0;
        height_asc = 0;
    }
    if(fnt->hz.loaded == NO){
        wid_hz = wid_asc*2;
        height_hz = height_asc;
    }
    if(wid_hz == 0)
        return fail;
    if(wid_asc == 0){
        wid_asc = wid_hz/2;
        height_asc = height_hz;
    }

    total_len = 0;
    
    for(i=0; str[i]; i++){/* 计算字符串的显示长度 */
        if(str[i]>=0x20 && !(str[i]&0x80)){
            total_len += wid_asc;
        } else if(str[i]&0x80 && str[i+1]&0x80){
            i++;
            total_len += wid_hz;
        }   
    }
    if(total_len < width){
        if(opt & DRAW_OPT_ALIGN_CENTER){/* 居中 */
            x += ((width-total_len)-1)/2;
        } else if(opt & DRAW_OPT_ALIGN_RIGHT){/* 右对齐 */
            x += (width-total_len);
        }
    }
    if(height_asc < height){
        y += ((height-height_asc)-1)/2;/* 垂直对齐 */
    }

    lock_kernel();
    for(i=0; str[i]; i++){
        if(str[i]>=0x20 && !(str[i]&0x80)){ /*ASC*/
            if(fnt->asc.loaded == NO)
                continue;
            if(!__get_font_cache(aschdl, ((int)str[i])-fnt->asc.start, (height_asc*ascbol)))
                continue;
            for(j=0; j<height_asc; j++){
                for(k=0; k<(ascbol); k++){
                    dots = (wid_asc-k*8);
                    if(dots>8)dots=8;
                    if(opt & DRAW_OPT_FIL_BG){
                        gdi_draw_h_bitmap_bg(x+k*8, y+j, dots, FONTCACHE[j*ascbol+k], color, bkcolor);
                    } else {
                        gdi_draw_h_bitmap(x+k*8, y+j, dots, FONTCACHE[j*ascbol+k], color);
                    }
                }
            }
            x+=wid_asc;
        } else if(str[i]&0x80 && str[i+1]&0x80){ /*HZ*/
		    zcode=(str[i]-0xa1)&0x7f;
		    bcode=(str[i+1]-0xa1)&0x7f;
            i++;
            if(fnt->hz.loaded == NO)
                continue;
            if(!__get_font_cache(hzhdl, (zcode*94+bcode)-fnt->hz.start, (height_hz*hzbol)))
                continue;
            for(j=0; j<height_hz; j++){
                for(k=0; k<(hzbol); k++){
                    dots = (wid_hz-k*8);
                    if(dots>8)dots=8;
                    if(opt & DRAW_OPT_FIL_BG){
                        gdi_draw_h_bitmap_bg(x+k*8, y+j, dots, FONTCACHE[j*hzbol+k], color, bkcolor);
                    } else {
                        gdi_draw_h_bitmap(x+k*8, y+j, dots, FONTCACHE[j*hzbol+k], color);
                    }
                }
            }
            x+=wid_hz;
        } else {
            /* 其它情况不理会 */
        }
    }
    unlock_kernel();

    return ok;
}


/*----------------------------------------------------------------------------------------------
 * 函数:    draw_font_for_widget()
 *  
 * 描述:    以指定字体显示一个字符串, 只用于控件
 *
 * 说明:    此函数会比较新旧字符串的内容, 以改善显示速度
**--------------------------------------------------------------------------------------------*/
BOOL apical draw_font_for_widget(int x, int y, int width, int height, 
                        INT08S *str, INT08S *oldstr,
                        COLOR color, COLOR bkcolor,
                        int font, INT16U opt)
{
/*lint --e{534}*/
    int wid_asc, wid_hz;
    int height_asc, height_hz;
    XMSHANDLE aschdl, hzhdl;
    int ascbol, hzbol; /* bol = bytes per line */
    struct font_register_s * fnt;
    int i,j,k,dots;
    int zcode, bcode;
    int x2, y2, x_start, x_orig;
    int str_changed;
    int total_len, oldstr_len, str_len;

    FamesAssert(str);
    FamesAssert(oldstr);

    if(!str || !oldstr) /* oldstr用于保存上一次显示的字符串 */
        return fail;

    if(font < 0 || font >= font_registered)
        return fail;

    x2 = (width + x) - 1;
    str_changed = 0;

    fnt = &font_register_buff[font];

    wid_asc = fnt->asc.width;
    wid_hz  = fnt->hz.width;
    height_asc = fnt->asc.height;
    height_hz  = fnt->hz.height;

    aschdl = fnt->asc.handle;
    hzhdl  = fnt->hz.handle;

    ascbol = fnt->asc.bytesofline;
    hzbol  = fnt->hz.bytesofline;

    if(fnt->asc.loaded == NO){
        wid_asc = 0;
        height_asc = 0;
    }
    if(fnt->hz.loaded == NO){
        wid_hz = wid_asc*2;
        height_hz = height_asc;
    }
    if(wid_hz == 0)
        return fail;
    if(wid_asc == 0){
        wid_asc = wid_hz/2;
        height_asc = height_hz;
    }

    total_len = 0;

    str_len    = STRLEN(str);
    oldstr_len = STRLEN(oldstr);
    if((str_len != oldstr_len) && (opt & (DRAW_OPT_ALIGN_CENTER|DRAW_OPT_ALIGN_RIGHT))){
        /* 新旧字符串长度不同, 而且又要居中(或右对齐)显示, 所以要全新显示一次 */
        oldstr[0] = 0;
    }

    /*  当oldstr的长度比str的长度小时, 要将oldstr的后面清空, 这样才不会残留下以前的东西,
     *  进而导致在与oldstr比较时, 因为两者相同而显示不出来! -- Jun, 2011-3-9
    */
    for(i=STRLEN(oldstr); i<=str_len; i++){
        oldstr[i] = 0;
    }

    for(i=0; str[i]; i++){/* 计算字符串的显示长度 */
        if(str[i]>=0x20 && !(str[i]&0x80)){
            total_len += wid_asc;
        } else if(str[i]&0x80 && str[i+1]&0x80){
            i++;
            total_len += wid_hz;
        }
    }
    x_orig = x; /* X的原值 */
    if(total_len < width){
        if(opt & DRAW_OPT_ALIGN_CENTER){ /* 居中 */
            x += ((width-total_len)-1)/2;
        } else if(opt & DRAW_OPT_ALIGN_RIGHT){/* 右对齐 */
            x += (width-total_len);
        }
    }
    x_start = x; /* X调整过后的值 */
    if(height_asc < height){
        y += ((height-height_asc)-1)/2;  /* 垂直对齐 */
    }

    lock_kernel();
    for(i=0; str[i]; i++){
        if(str[i]>=0x20 && !(str[i]&0x80)){ /* ASC */
            if(str[i] == oldstr[i])      /* 已显示过 */
                goto skip1;
            oldstr[i] = str[i];
            str_changed = 1;
            if(fnt->asc.loaded == NO)
                continue;
            if(!__get_font_cache(aschdl, ((int)str[i])-fnt->asc.start, (height_asc*ascbol)))
                continue;
            for(j=0; j<height_asc; j++){
                for(k=0; k<(ascbol); k++){
                    dots = (wid_asc-k*8);
                    if(dots>8)dots=8;
                    if(opt & DRAW_OPT_FIL_BG){
                        gdi_draw_h_bitmap_bg(x+k*8, y+j, dots, FONTCACHE[j*ascbol+k], color, bkcolor);
                    } else {
                        gdi_draw_h_bitmap(x+k*8, y+j, dots, FONTCACHE[j*ascbol+k], color);
                    }
                }
            }
        skip1:
            x+=wid_asc;
        } else if(str[i]&0x80 && str[i+1]&0x80){ /* HZ */
            if(str[i]   == oldstr[i] &&  /* 已显示过 */
               str[i+1] == oldstr[i+1]){
                i++;
                goto skip2;
            }
            oldstr[i]   = str[i];
            oldstr[i+1] = str[i+1];
            str_changed = 1;
		    zcode=(str[i]-0xa1)&0x7f;
		    bcode=(str[i+1]-0xa1)&0x7f;
            i++;
            if(fnt->hz.loaded == NO)
                continue;
            if(!__get_font_cache(hzhdl, (zcode*94+bcode)-fnt->hz.start, (height_hz*hzbol)))
                continue;
            for(j=0; j<height_hz; j++){
                for(k=0; k<(hzbol); k++){
                    dots = (wid_hz-k*8);
                    if(dots>8)dots=8;
                    if(opt & DRAW_OPT_FIL_BG){
                        gdi_draw_h_bitmap_bg(x+k*8, y+j, dots, FONTCACHE[j*hzbol+k], color, bkcolor);
                    } else {
                        gdi_draw_h_bitmap(x+k*8, y+j, dots, FONTCACHE[j*hzbol+k], color);
                    }
                }
            }
        skip2:
            x+=wid_hz;
        } else {
            oldstr[i] = str[i];
        }
    }
    oldstr[i] = 0; /* Append NUL */
    
    /*  当oldstr的长度比str的长度大时, 也要将oldstr的后面清空, 这样才不会残留下以前的东西,
     *  进而导致在与oldstr比较时, 因为两者相同而显示不出来! -- Jun, 2011-3-9
     *
     *  其实, 这一种情况上面也有处理, 不过, 这里的代码还要清除不显示的区域(屏幕),
     *  所以, 这里的代码还是有用的, 呵呵~ --- Jun, 2011-3-9
    */
    if(i<oldstr_len)
        str_changed = 1;
    if(str_changed && i<oldstr_len){
        for(; i<oldstr_len; i++)
            oldstr[i] = 0;
        y2=(y+height_hz)-1;
        if(x<x2){
            gdi_draw_box(x, y, x2, y2, bkcolor);
        }
        x_start--;
        if(x_orig<x_start){ 
            gdi_draw_box(x_orig, y, x_start, y2, bkcolor);
        }
    }
    unlock_kernel();

    return ok;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    load_sys_font()
 *  
 * 描述:    加载系统默认字体
**--------------------------------------------------------------------------------------------*/
FONTINFO _sys_font_asc = {NO, FONT_TYPE_ASCII,  0,  7, 14, 1, 0, "ASC14", NULL};
FONTINFO _sys_font_hz  = {NO, FONT_TYPE_GB2312, 0, 14, 14, 2, 0, "HZK14", NULL};

void apical load_sys_font(void)
{
    load_font(&_sys_font_asc);
    load_font(&_sys_font_hz);
    register_font(&_sys_font_hz, &_sys_font_asc);
}


/*==============================================================================================
 * 
 * 本文件结束: font.c
 * 
**============================================================================================*/


