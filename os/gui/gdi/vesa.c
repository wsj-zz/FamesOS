/*************************************************************************************
** �ļ�: vesa.c
** ˵��: ͼ����ʾ�ӿ�(����, VESA)
** ע��: ���ļ������Դ�ģʽ����(��: TCC -1 -B -ml ###.C)
** ����: Jun
** ʱ��: 2010-03-27
** �汾: V0.1 (2010-03-27, ����İ汾)
*************************************************************************************/
#define  FAMES_X_VESA_C
#include "includes.h"

#if     FAMES_VESA_EN == 1

/*lint --e{534,563}*/
/*------------------------------------------------------------------------------------
 * ����:    X_OPEN_GRAPH()
 * ˵��:    ��ͼ��ģʽ
**----------------------------------------------------------------------------------*/
INT16S  X_OPEN_GRAPH(void)
{
    INT16S vmode;
    if(SCREEN.opened==TRUE){
        return ok;
    }
    DispatchLock();
    vmode=SCREEN.videomode;
    vmode=vmode;
    asm push ax
    asm push bx
    asm mov ax,0x4F02;
    asm mov bx,vmode;
    asm int 0x10;
    asm pop bx
    asm pop ax
    SCREEN.opened=TRUE;
    X_SET_PALETTE_DEFAULT();
    DispatchUnlock();
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    X_CLOSE_GRAPH()
 * ˵��:    �ر�ͼ��ģʽ(�ص��ı�ģʽ)
**----------------------------------------------------------------------------------*/
INT16S  X_CLOSE_GRAPH()
{
    if(SCREEN.opened!=TRUE){
        return ok;
    }
    DispatchLock();
    asm push ax
    asm push bx
    asm mov ax,0x4F02;
    asm mov bx,VMODE_TEXT;
    asm int 0x10;
    asm pop bx
    asm pop ax
    SCREEN.opened=FALSE;
    DispatchUnlock();
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    X_SET_PALETTE()
 * ˵��:    ����ĳһ��ɫ�ĵ�ɫ��
 * ����:    index   ��ɫֵ(0~255)
 *          red     ��ɫ����(0~63)
 *          green   ��ɫ����(0~63)
 *          blue    ��ɫ����(0~63)
 * ���:    ok/fail
**----------------------------------------------------------------------------------*/
INT16S  X_SET_PALETTE(INT16S index, INT08U  red, INT08U  grn, INT08U  blue)
{
    if(index>=256){
        return fail;
    }
    if((INT16S)red  >63)red  =63;
    if((INT16S)grn  >63)grn  =63;
    if((INT16S)blue >63)blue =63;
    DispatchLock();
    outportbyte(0x3c8,(INT08U)index);
    outportbyte(0x3c9,red  );
    outportbyte(0x3c9,grn  );
    outportbyte(0x3c9,blue );
    DispatchUnlock();
    FamesDelay(10L);
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    X_SET_PALETTE_DEFAULT()
 * ˵��:    ����FAMES��Ĭ�ϵ�ɫ��
**----------------------------------------------------------------------------------*/
INT16S  X_SET_PALETTE_DEFAULT(void)
{
    INT16S  index;
    
    for(index=0; index<256; index++){ /*lint --e{679}*/
        X_SET_PALETTE(index, DefaultPALETTE[3*index], 
                      DefaultPALETTE[3*index+1], 
                      DefaultPALETTE[3*index+2]
                     );
    }
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    X_PUT_PIXEL()
 * ˵��:    ��ָ��λ�û�һ����
 * ����:    x       ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          color   �����ɫ
 * ���:    ok/fail
 * ע��:    ������ר�����1024x768x256�������Ż�
**----------------------------------------------------------------------------------*/
INT16S  X_PUT_PIXEL(INT16S x, INT16S y, COLOR  color)
{
    if(x<0 || x>=SCREEN.width || y<0 || y>=SCREEN.height){
        return fail;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    DispatchLock();
    asm push ax
    asm push bx
    asm push dx
    asm push es
    asm push di
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3f
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm mov  al, color
    asm stosb
    asm pop di
    asm pop es
    asm pop dx
    asm pop bx
    asm pop ax
    DispatchUnlock();
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    X_GET_PIXEL()
 * ˵��:    ��ָ��λ�ö�һ�������ɫ
 * ����:    x       ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 * ���:    color   �����ɫ
 * ע��:    ������ר�����1024x768x256�������Ż�
**----------------------------------------------------------------------------------*/
COLOR   X_GET_PIXEL(INT16S x, INT16S y)
{
    register INT08U  color;
    
    if(x<0 || x>=SCREEN.width || y<0 || y>=SCREEN.height){
        return fail;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    DispatchLock();
    asm push ax
    asm push bx
    asm push dx
    asm push ds
    asm push si
    asm mov  si, y
    asm mov  dx, si
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_getpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_getpixel:
    asm and  si, 0x3f
    asm shl  si, 10
    asm add  si, x
    asm mov  ax, VIDEOSEG
    asm mov  ds, ax
    asm lodsb
    asm mov  color, al
    asm pop si
    asm pop ds
    asm pop dx
    asm pop bx
    asm pop ax
    DispatchUnlock();
    return color; /*lint !e530*/
}

/*------------------------------------------------------------------------------------
 * ����:    X_CLEAR_SCREEN()
 * ˵��:    ��ָ����ɫ����
 * ����:    color ��ɫ
**----------------------------------------------------------------------------------*/
INT16S  X_CLEAR_SCREEN(COLOR color)
{
    X_DRAW_BOX(0, 0, SCREEN.width-1, SCREEN.height-1, color);
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_H_LINE()
 * ˵��:    ��ˮƽ��
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          x2      ��β������(0~SCREEN.width-1)
 *          color   �ߵ���ɫ
 * ע��:    ������ר�����1024x768x256�������Ż�
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_H_LINE(INT16S x, INT16S y, INT16S x2, COLOR  color)
{
    if(x>x2){
        x += x2;
        x2 = x-x2;
        x -= x2;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    if(y < 0            )return fail;
    if(y >=SCREEN.height)return fail;
    if(x >=SCREEN.width )return fail;
    if(x2< 0            )return fail;
    if(x < 0            )x = 0;
    if(x2>=SCREEN.width )x2= SCREEN.width-1;
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push di
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3F
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm mov  al, color
    asm mov  cx, x2
    asm sub  cx, x
    asm inc  cx
    asm rep  stosb
    asm pop  di
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_V_LINE()
 * ˵��:    ����ֱ��
 * ����:    x       ������(0~SCREEN.width-1)
 *          y       ��ʼ������(0~SCREEN.height-1)
 *          y2      ��β������(0~SCREEN.height-1)
 *          color   �ߵ���ɫ
 * ע��:    ������ר�����1024x768x256�������Ż�
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_V_LINE(INT16S x, INT16S y, INT16S y2, COLOR  color)
{
    if(y>y2){
        y += y2;
        y2 = y-y2;
        y -= y2;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    if(x < 0            )return fail;
    if(x >=SCREEN.width )return fail;
    if(y >=SCREEN.height)return fail;
    if(y2< 0            )return fail;
    if(y < 0            )y = 0;
    if(y2>=SCREEN.height)y2= SCREEN.height-1;
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push di
    asm mov  cx, y2
    asm sub  cx, y
    asm inc  cx
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3f
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    do_color_stosb:
    asm mov  al, color
    do_stosb:
    asm stosb
    asm dec  cx
    asm jz   draw_v_line_end
    asm inc  word ptr y
    asm and  di, di                           /* �Է���x=1023ʱ����                 */
    asm jz   do_change_page1
    asm add  di, 1023
    asm jc   do_change_page2
    asm jmp  do_stosb
    do_change_page1:
    asm add  di, 1023
    do_change_page2:
    asm inc  dx
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    asm jmp  do_color_stosb
    draw_v_line_end:
    asm pop  di
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_H_IMAGE()
 * ˵��:    ����ɫӳ��(IMAGE)ˮƽ����
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          x2      ��β������(0~SCREEN.width-1)
 *          colors  ��ɫӳ��(��ɫ����)
 * ע��:    ������ר�����1024x768x256�������Ż�
 *          ��������Ҫ������ʾͼ��(λͼ)
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_H_IMAGE(INT16S x, INT16S y, INT16S x2, COLOR colors[])
{
    if(x>x2){
        x += x2;
        x2 = x-x2;
        x -= x2;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    if(colors==NULL     )return fail;  
    if(y < 0            )return fail;
    if(y >=SCREEN.height)return fail;
    if(x >=SCREEN.width )return fail;
    if(x2< 0            )return fail;
    if(x < 0            )x = 0;
    if(x2>=SCREEN.width )x2= SCREEN.width-1;
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push ds
    asm push di
    asm push si
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3F                         /* ����ES:DI */
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm lds  bx, dword ptr colors
    asm mov  si, bx                           /* ����DS:SI */
    asm mov  cx, x2
    asm sub  cx, x
    asm inc  cx
    asm rep  movsb
    asm pop  si
    asm pop  di
    asm pop  ds
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}

#if     FAMES_VESA_V_FUNC_EN == 1
/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_V_LINE()
 * ˵��:    ����ɫӳ��(IMAGE)��ֱ����
 * ����:    x       ������(0~SCREEN.width-1)
 *          y       ��ʼ������(0~SCREEN.height-1)
 *          y2      ��β������(0~SCREEN.height-1)
 *          colors  ��ɫӳ��(��ɫ����)
 * ע��:    ������ר�����1024x768x256�������Ż�
 *          ��������Ҫ������ʾͼ��(λͼ)
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_V_IMAGE(INT16S x, INT16S y, INT16S y2, COLOR colors[])
{/*lint --e{529}*/
    INT16S  c_index=0;
    if(y>y2){
        y += y2;
        y2 = y-y2;
        y -= y2;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    if(colors==NULL     )return fail;  
    if(x < 0            )return fail;
    if(x >=SCREEN.width )return fail;
    if(y >=SCREEN.height)return fail;
    if(y2< 0            )return fail;
    if(y < 0            )y = 0;
    if(y2>=SCREEN.height)y2= SCREEN.height-1;
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push di
    asm mov  cx, y2
    asm sub  cx, y
    asm inc  cx
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3f
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    do_color_stosb:
    asm  push es
    asm  push bx
    asm  les  bx, dword ptr colors
    asm  add  bx, word ptr c_index
    asm  mov  al, byte ptr es:[bx]
    asm  pop  bx
    asm  pop  es
    do_stosb:
    asm stosb
    asm inc  word ptr c_index
    asm dec  cx
    asm jz   draw_v_line_end
    asm inc  word ptr y
    asm and  di, di                           /* �Է���x=1023ʱ���� */
    asm jz   do_change_page1
    asm add  di, 1023
    asm jc   do_change_page2
    asm jmp  do_color_stosb
    do_change_page1:
    asm add  di, 1023
    do_change_page2:
    asm inc  dx
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    asm jmp  do_color_stosb
    draw_v_line_end:
    asm pop  di
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}
#endif

/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_H_BITMAP()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ�û���,���򲻻�
 *          һ����໭8����
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          dots    ����ĸ���
 *          bitmap  һ���ֽڵ�λģʽ
 *          color   ��ɫ
 * ע��:    ������ר�����1024x768x256�������Ż�
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_H_BITMAP(INT16S x, INT16S y, INT16S dots, INT08U bitmap, COLOR  color)
{
    if(x<0 || x>=SCREEN.width || y<0 || y>=SCREEN.height){
        return fail;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    if(dots > 8)
        dots=8;
    if(dots <= 0)
        return fail;
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push di
    asm push si
    _SI=SCREEN.width; /*lint !e40 !e63*/
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3F
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm mov  al, color
    asm mov  ah, bitmap
    asm mov  cx, dots
    asm mov  bx, x
    do_loop_start:
    asm cmp  bx, si
    asm jae  do_loop_end
    asm shl  ah, 1
    asm jc   do_put_dot
    asm inc  di
    asm jmp  do_jmp_loop
    do_put_dot:
    asm stosb
    do_jmp_loop:
    asm inc  bx
    asm loop do_loop_start
    do_loop_end:
    asm pop  si
    asm pop  di
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}

#if     FAMES_VESA_V_FUNC_EN == 1
/*------------------------------------------------------------------------------------
* ����: X_DRAW_V_BITMAP()
* ˵��: ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ�û���,���򲻻�
*       һ����໭8����
* ����: x       ��ʼ������(0~SCREEN.width-1)
*       y       ������(0~SCREEN.height-1)
*       bitmap  һ���ֽڵ�λģʽ
*       color   ��ɫ
* ע��: ������ר�����1024x768x256�������Ż�
* ��;: �������ɴ��ӿ��������ʾ, �����ڻ�����
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_V_BITMAP(INT16S x, INT16S y, INT08U bitmap, COLOR  color)
{
    if(x<0 || x>=SCREEN.width || y<0 || y>=SCREEN.height){
        return fail;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push di
    asm push si
    _SI=SCREEN.height; /*lint !e40 !e63*/
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3F
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm mov  cx, 8
    asm mov  bx, y
    do_color_stosb:
    asm mov  al, color
    asm mov  ah, bitmap
    do_loop_start:
    asm cmp  bx, si
    asm jae  do_loop_end
    asm shl  ah, 1
    asm jc   do_put_dot
    asm add  di, 1024
    asm jc   do_change_page2
    asm jmp  do_jmp_loop
    do_put_dot:
    asm stosb
    asm and  di, di                           /* �Է���x=1023ʱ����                 */
    asm jz   do_change_page1
    asm add  di, 1023
    asm jc   do_change_page2
    do_jmp_loop:
    asm inc  bx
    asm loop do_loop_start
    do_loop_end:
    asm jmp  draw_v_bitmap_end
    do_change_page1:
    asm add  di, 1023
    do_change_page2:
    asm push ax
    asm inc  dx
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    asm pop  ax
    asm jmp  do_jmp_loop
    draw_v_bitmap_end:
    asm pop  si
    asm pop  di
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}
#endif

/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_H_BITMAP_BG()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ����color����,
 *          ������bgcolor����, һ����໭8����.
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          dots    ����ĸ���
 *          bitmap  һ���ֽڵ�λģʽ
 *          color   ��ɫ
 *          bgcolor ������ɫ
 * ע��:    ������ר�����1024x768x256�������Ż�
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
 *          ����Ҫ������ɫ��ʱ��ǳ�����
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_H_BITMAP_BG(INT16S x, INT16S y, INT16S dots, INT08U bitmap, COLOR  color, COLOR bgcolor)
{
    if(x<0 || x>=SCREEN.width || y<0 || y>=SCREEN.height){
        return fail;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    if(dots > 8)
        dots=8;
    if(dots <= 0)
        return fail;
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push di
    asm push si
    _SI=SCREEN.width; /*lint !e40 !e63*/
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3F
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm mov  dl, color
    asm mov  dh, bgcolor
    asm mov  ah, bitmap
    asm mov  cx, dots
    asm mov  bx, x
    do_loop_start:
    asm cmp  bx, si
    asm jae  do_loop_end
    asm shl  ah, 1
    asm mov  al, dl
    asm jc   do_put_dot
    asm mov  al, dh
    do_put_dot:
    asm stosb
    asm inc  bx
    asm loop do_loop_start
    do_loop_end:
    asm pop  si
    asm pop  di
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}

#if     FAMES_VESA_V_FUNC_EN == 1
/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_V_BITMAP_BG()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ����color����,
 *          ������bgcolor����, һ����໭8����.
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ������(0~SCREEN.height-1)
 *          bitmap  һ���ֽڵ�λģʽ
 *          color   ��ɫ
 *          bgcolor ������ɫ
 * ע��:    ������ר�����1024x768x256�������Ż�
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
 *          ����Ҫ������ɫ��ʱ��ǳ�����
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_V_BITMAP_BG(INT16S x, INT16S y, INT08U bitmap, COLOR  color, COLOR bgcolor)
{
    if(x<0 || x>=SCREEN.width || y<0 || y>=SCREEN.height){
        return fail;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push di
    asm push si
    _SI=SCREEN.height; /*lint !e40 !e63*/
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3F
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm mov  cx, 8
    asm mov  bx, y
    do_color_stosb:
    asm mov  ah, bitmap
    do_loop_start:
    asm cmp  bx, si
    asm jae  do_loop_end
    asm shl  ah, 1
    asm mov  al, color
    asm jc   do_put_dot
    asm mov  al, bgcolor
    do_put_dot:
    asm stosb
    asm and  di, di                           /* �Է���x=1023ʱ����                 */
    asm jz   do_change_page1
    asm add  di, 1023
    asm jc   do_change_page2
    do_jmp_loop:
    asm inc  bx
    asm loop do_loop_start
    do_loop_end:
    asm jmp  draw_v_bitmap_end
    do_change_page1:
    asm add  di, 1023
    do_change_page2:
    asm push ax
    asm inc  dx
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    asm pop  ax
    asm jmp  do_jmp_loop
    draw_v_bitmap_end:
    asm pop  si
    asm pop  di
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}
#endif

/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_H_BITMAP_BGMAP()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ����color����,
 *          ������bgcolors�е���ɫ����, һ����໭8����.
 * ����:    x        ��ʼ������(0~SCREEN.width-1)
 *          y        ������(0~SCREEN.height-1)
 *          bitmap   һ���ֽڵ�λģʽ
 *          color    ��ɫ
 *          bgcolors ������ɫ
 * ע��:    ������ר�����1024x768x256�������Ż�
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
 *          ����Ҫ������ͼ�ε�ʱ��ǳ�����
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_H_BITMAP_BGMAP(INT16S x, INT16S y, INT16S dots, INT08U bitmap, COLOR  color, COLOR bgcolors[])
{/*lint --e{529}*/
    INT16S bgc_index=0;
    
    if(x<0 || x>=SCREEN.width || y<0 || y>=SCREEN.height){
        return fail;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    if(dots > 8)
        dots=8;
    if(dots <= 0)
        return fail;
    if(bgcolors==NULL){
        return X_DRAW_H_BITMAP(x, y, dots, bitmap, color); /* ���û�б�����ɫ,�򲻻�����ɫ */
    }
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push di
    asm push si
    _SI=SCREEN.width; /*lint !e40 !e63*/
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3F
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm mov  dl, color
    asm mov  ah, bitmap
    asm mov  cx, dots
    asm mov  bx, x
    do_loop_start:
    asm cmp  bx, si
    asm jae  do_loop_end
    asm shl  ah, 1
    asm mov  al, dl
    asm jc   do_put_dot
    asm push es
    asm push bx
    asm les  bx, dword ptr bgcolors
    asm add  bx, word ptr bgc_index
    asm mov  al, byte ptr es:[bx]
    asm pop  bx
    asm pop  es
    do_put_dot:
    asm stosb
    asm inc  word ptr bgc_index
    asm inc  bx
    asm loop do_loop_start
    do_loop_end:
    asm pop  si
    asm pop  di
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}

#if     FAMES_VESA_V_FUNC_EN == 1
/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_V_BITMAP_BG()
 * ˵��:    ����bitmap�е�λ����(����), ���ĳλΪ1,���ڶ�Ӧλ����color����,
 *          ������bgcolors�е���ɫ����, һ����໭8����.
 * ����:    x        ��ʼ������(0~SCREEN.width-1)
 *          y        ������(0~SCREEN.height-1)
 *          bitmap   һ���ֽڵ�λģʽ
 *          color    ��ɫ
 *          bgcolors ������ɫ
 * ע��:    ������ר�����1024x768x256�������Ż�
 * ��;:    �������ɴ��ӿ��������ʾ, �����ڻ�����
 *          ����Ҫ������ͼ�ε�ʱ��ǳ�����
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_V_BITMAP_BGMAP(INT16S x, INT16S y, INT08U bitmap, COLOR  color, COLOR bgcolors[])
{/*lint --e{529}*/
    INT16S bgc_index=0;

    if(x<0 || x>=SCREEN.width || y<0 || y>=SCREEN.height){
        return fail;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    DispatchLock();
    asm push ax
    asm push bx
    asm push cx
    asm push dx
    asm push es
    asm push di
    asm push si
    _SI=SCREEN.height; /*lint !e40 !e63*/
    asm mov  di, y
    asm mov  dx, di
    asm shr  dx, 6
    asm cmp  dx, SCREEN.curr_page
    asm jz   do_putpixel
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    do_putpixel:
    asm and  di, 0x3F
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm mov  cx, 8
    asm mov  bx, y
    do_color_stosb:
    asm mov  ah, bitmap
    do_loop_start:
    asm cmp  bx, si
    asm jae  do_loop_end
    asm shl  ah, 1
    asm mov  al, color
    asm jc   do_put_dot
    asm  push es
    asm  push bx
    asm  les  bx, dword ptr bgcolors
    asm  add  bx, word ptr bgc_index
    asm  mov  al, byte ptr es:[bx]
    asm  pop  bx
    asm  pop  es
    do_put_dot:
    asm stosb
    asm inc  word ptr bgc_index
    asm and  di, di                           /* �Է���x=1023ʱ����                 */
    asm jz   do_change_page1
    asm add  di, 1023
    asm jc   do_change_page2
    do_jmp_loop:
    asm inc  bx
    asm loop do_loop_start
    do_loop_end:
    asm jmp  draw_v_bitmap_end
    do_change_page1:
    asm add  di, 1023
    do_change_page2:
    asm push ax
    asm inc  dx
    asm mov  SCREEN.curr_page, dx
    asm mov  ax, 0x4F05
    asm mov  bx, 0
    asm int  0x10;
    asm pop  ax
    asm jmp  do_jmp_loop
    draw_v_bitmap_end:
    asm pop  si
    asm pop  di
    asm pop  es
    asm pop  dx
    asm pop  cx
    asm pop  bx
    asm pop  ax
    DispatchUnlock();
    return ok;
}
#endif

/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_RECT()
 * ˵��:    ������
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ��ʼ������(0~SCREEN.height-1)
 *          x2      ��β������(0~SCREEN.width-1)
 *          y2      ��β������(0~SCREEN.height-1)
 *          color   �ߵ���ɫ
 * ���:    ok/fail
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_RECT(INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color)
{
    if(x>x2){
        x += x2;
        x2 = x-x2;
        x -= x2;
    }
    if(y>y2){
        y += y2;
        y2 = y-y2;
        y -= y2;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    if(x >=SCREEN.width )return fail;
    if(x2< 0            )return fail;
    if(y >=SCREEN.height)return fail;
    if(y2< 0            )return fail;
    if(x < 0            ){
        x = 0;
    } else {
        X_DRAW_V_LINE(x , y , y2, color);        
    }
    if(x2>=SCREEN.width ){
        x2= SCREEN.width-1;
    } else {
        X_DRAW_V_LINE(x2, y , y2, color);
    }
    if(y < 0            ){
        y = 0;
    } else {
        X_DRAW_H_LINE(x , y , x2, color);
    }
    if(y2>=SCREEN.height){
        y2= SCREEN.height-1;
    } else {    
        X_DRAW_H_LINE(x , y2, x2, color);
    }
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    X_DRAW_BOX()
 * ˵��:    ��������
 * ����:    x       ��ʼ������(0~SCREEN.width-1)
 *          y       ��ʼ������(0~SCREEN.height-1)
 *          x2      ��β������(0~SCREEN.width-1)
 *          y2      ��β������(0~SCREEN.height-1)
 *          color   �ߵ���ɫ
**----------------------------------------------------------------------------------*/
INT16S  X_DRAW_BOX( INT16S x, INT16S y, INT16S x2, INT16S y2, COLOR  color)
{
    if(x>x2){
        x += x2;
        x2 = x-x2;
        x -= x2;
    }
    if(y>y2){
        y += y2;
        y2 = y-y2;
        y -= y2;
    }
    if(SCREEN.opened!=TRUE)
        return fail;
    if(x >=SCREEN.width )return fail;
    if(x2< 0            )return fail;
    if(y >=SCREEN.height)return fail;
    if(y2< 0            )return fail;
    if(x < 0            )x = 0;
    if(x2>=SCREEN.width )x2= SCREEN.width-1;
    if(y < 0            )y = 0;
    if(y2>=SCREEN.height)y2= SCREEN.height-1;

    for(;y<=y2; y++){
        X_DRAW_H_LINE(x, y, x2, color);
    }
    return ok;
}
#endif                                        /* #if FAMES_GDI_EN==1           */

/*
*���ļ�����: vesa.c ==================================================================
*/

