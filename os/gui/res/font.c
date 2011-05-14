/***********************************************************************************************
 * �ļ�:    font.c
 *
 * ˵��:    ������ʾ(���ֿ����)
 *
 * ����:    Jun
 *
 * ʱ��:    2010-12-29
 *
 * ע��:    �˰汾ֻ�ܴ�������ֿ�
***********************************************************************************************/
#define  FAMES_FONT_C
#include <includes.h>

/*lint -e679 :Suspicious Truncation in arithmetic expression */
/*----------------------------------------------------------------------------------------------
 * 
 * ����ע��ṹ
 * 
**--------------------------------------------------------------------------------------------*/
struct font_register_s {
    FONTINFO hz, asc;
    struct font_register_s * next;
};

#define FONT_MAX_NR  16

static struct font_register_s font_register_buff[FONT_MAX_NR];
static int    font_registered = 0; /* ��ע����� */


/*----------------------------------------------------------------------------------------------
 * ����:    load_font()
 *  
 * ����:    ����һ���ֽ�(���ص�XMS��)
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
    close(fhdl);/*lint !e644, ����Ӧ����û�������, ������*/
    goto out;    
}


/*----------------------------------------------------------------------------------------------
 * ����:    unload_font()
 *  
 * ����:    ж��һ���ֽ�(��XMS��ɾ��)
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
 * ����:    get_font_info()
 *  
 * ����:    ��ȡ������Ϣ
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
 * ����:    register_font()
 *  
 * ����:    ע��һ������
 *
 * ����:    ע���������
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
 * ����:    unregister_font()
 *  
 * ����:    ע��һ������
**--------------------------------------------------------------------------------------------*/
BOOL apical unregister_font(int font)
{
    if(font<0 || font>=FONT_MAX_NR)
        return fail;

    if(font==0)
        return fail; /* ������ע��ϵͳ���� */

    lock_kernel();
    font_register_buff[font].hz.loaded = NO;
    font_register_buff[font].asc.loaded = NO;
    unlock_kernel();

    return ok;
}


/*----------------------------------------------------------------------------------------------
 * 
 *      ��ģ����
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
 * ����:    draw_font()
 *  
 * ����:    ��ָ��������ʾһ���ַ���
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
            /* ������������ */
        }
    }
    unlock_kernel();

    return ok;
}

/*----------------------------------------------------------------------------------------------
 * ����:    draw_font_ex()
 *  
 * ����:    ��ָ��������ʾһ���ַ���
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
    
    for(i=0; str[i]; i++){/* �����ַ�������ʾ���� */
        if(str[i]>=0x20 && !(str[i]&0x80)){
            total_len += wid_asc;
        } else if(str[i]&0x80 && str[i+1]&0x80){
            i++;
            total_len += wid_hz;
        }   
    }
    if(total_len < width){
        if(opt & DRAW_OPT_ALIGN_CENTER){/* ���� */
            x += ((width-total_len)-1)/2;
        } else if(opt & DRAW_OPT_ALIGN_RIGHT){/* �Ҷ��� */
            x += (width-total_len);
        }
    }
    if(height_asc < height){
        y += ((height-height_asc)-1)/2;/* ��ֱ���� */
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
            /* ������������ */
        }
    }
    unlock_kernel();

    return ok;
}


/*----------------------------------------------------------------------------------------------
 * ����:    draw_font_for_widget()
 *  
 * ����:    ��ָ��������ʾһ���ַ���, ֻ���ڿؼ�
 *
 * ˵��:    �˺�����Ƚ��¾��ַ���������, �Ը�����ʾ�ٶ�
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

    if(!str || !oldstr) /* oldstr���ڱ�����һ����ʾ���ַ��� */
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
        /* �¾��ַ������Ȳ�ͬ, ������Ҫ����(���Ҷ���)��ʾ, ����Ҫȫ����ʾһ�� */
        oldstr[0] = 0;
    }

    /*  ��oldstr�ĳ��ȱ�str�ĳ���Сʱ, Ҫ��oldstr�ĺ������, �����Ų����������ǰ�Ķ���,
     *  ������������oldstr�Ƚ�ʱ, ��Ϊ������ͬ����ʾ������! -- Jun, 2011-3-9
    */
    for(i=STRLEN(oldstr); i<=str_len; i++){
        oldstr[i] = 0;
    }

    for(i=0; str[i]; i++){/* �����ַ�������ʾ���� */
        if(str[i]>=0x20 && !(str[i]&0x80)){
            total_len += wid_asc;
        } else if(str[i]&0x80 && str[i+1]&0x80){
            i++;
            total_len += wid_hz;
        }
    }
    x_orig = x; /* X��ԭֵ */
    if(total_len < width){
        if(opt & DRAW_OPT_ALIGN_CENTER){ /* ���� */
            x += ((width-total_len)-1)/2;
        } else if(opt & DRAW_OPT_ALIGN_RIGHT){/* �Ҷ��� */
            x += (width-total_len);
        }
    }
    x_start = x; /* X���������ֵ */
    if(height_asc < height){
        y += ((height-height_asc)-1)/2;  /* ��ֱ���� */
    }

    lock_kernel();
    for(i=0; str[i]; i++){
        if(str[i]>=0x20 && !(str[i]&0x80)){ /* ASC */
            if(str[i] == oldstr[i])      /* ����ʾ�� */
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
            if(str[i]   == oldstr[i] &&  /* ����ʾ�� */
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
    
    /*  ��oldstr�ĳ��ȱ�str�ĳ��ȴ�ʱ, ҲҪ��oldstr�ĺ������, �����Ų����������ǰ�Ķ���,
     *  ������������oldstr�Ƚ�ʱ, ��Ϊ������ͬ����ʾ������! -- Jun, 2011-3-9
     *
     *  ��ʵ, ��һ���������Ҳ�д���, ����, ����Ĵ��뻹Ҫ�������ʾ������(��Ļ),
     *  ����, ����Ĵ��뻹�����õ�, �Ǻ�~ --- Jun, 2011-3-9
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
 * ����:    load_sys_font()
 *  
 * ����:    ����ϵͳĬ������
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
 * ���ļ�����: font.c
 * 
**============================================================================================*/


