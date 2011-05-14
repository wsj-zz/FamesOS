/*************************************************************************************
** 文件: vesa.c
** 说明: 图形显示接口(驱动, VESA)
** 注意: 此文件必需以大模式编译(如: TCC -1 -B -ml ###.C)
** 作者: Jun
** 时间: 2010-03-27
** 版本: V0.1 (2010-03-27, 最初的版本)
*************************************************************************************/
#define  FAMES_X_VESA_C
#include "includes.h"

#if     FAMES_VESA_EN == 1

/*lint --e{534,563}*/
/*------------------------------------------------------------------------------------
 * 函数:    X_OPEN_GRAPH()
 * 说明:    打开图形模式
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
 * 函数:    X_CLOSE_GRAPH()
 * 说明:    关闭图形模式(回到文本模式)
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
 * 函数:    X_SET_PALETTE()
 * 说明:    设置某一颜色的调色板
 * 输入:    index   颜色值(0~255)
 *          red     红色分量(0~63)
 *          green   绿色分量(0~63)
 *          blue    蓝色分量(0~63)
 * 输出:    ok/fail
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
 * 函数:    X_SET_PALETTE_DEFAULT()
 * 说明:    启用FAMES的默认调色板
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
 * 函数:    X_PUT_PIXEL()
 * 说明:    在指定位置画一个点
 * 输入:    x       横坐标(0~SCREEN.width-1)
 *          y       纵坐标(0~SCREEN.height-1)
 *          color   点的颜色
 * 输出:    ok/fail
 * 注意:    本函数专门针对1024x768x256进行了优化
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
 * 函数:    X_GET_PIXEL()
 * 说明:    在指定位置读一个点的颜色
 * 输入:    x       横坐标(0~SCREEN.width-1)
 *          y       纵坐标(0~SCREEN.height-1)
 * 输出:    color   点的颜色
 * 注意:    本函数专门针对1024x768x256进行了优化
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
 * 函数:    X_CLEAR_SCREEN()
 * 说明:    以指定颜色清屏
 * 输入:    color 颜色
**----------------------------------------------------------------------------------*/
INT16S  X_CLEAR_SCREEN(COLOR color)
{
    X_DRAW_BOX(0, 0, SCREEN.width-1, SCREEN.height-1, color);
    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    X_DRAW_H_LINE()
 * 说明:    画水平线
 * 输入:    x       起始横坐标(0~SCREEN.width-1)
 *          y       纵坐标(0~SCREEN.height-1)
 *          x2      结尾横坐标(0~SCREEN.width-1)
 *          color   线的颜色
 * 注意:    本函数专门针对1024x768x256进行了优化
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
 * 函数:    X_DRAW_V_LINE()
 * 说明:    画垂直线
 * 输入:    x       横坐标(0~SCREEN.width-1)
 *          y       起始纵坐标(0~SCREEN.height-1)
 *          y2      结尾纵坐标(0~SCREEN.height-1)
 *          color   线的颜色
 * 注意:    本函数专门针对1024x768x256进行了优化
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
    asm and  di, di                           /* 以防在x=1023时出错                 */
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
 * 函数:    X_DRAW_H_IMAGE()
 * 说明:    按颜色映象(IMAGE)水平画线
 * 输入:    x       起始横坐标(0~SCREEN.width-1)
 *          y       纵坐标(0~SCREEN.height-1)
 *          x2      结尾横坐标(0~SCREEN.width-1)
 *          colors  颜色映象(颜色数组)
 * 注意:    本函数专门针对1024x768x256进行了优化
 *          本函数主要用于显示图形(位图)
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
    asm and  di, 0x3F                         /* 建立ES:DI */
    asm shl  di, 10
    asm add  di, x
    asm mov  ax, VIDEOSEG
    asm mov  es, ax
    asm lds  bx, dword ptr colors
    asm mov  si, bx                           /* 建立DS:SI */
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
 * 函数:    X_DRAW_V_LINE()
 * 说明:    按颜色映象(IMAGE)垂直画线
 * 输入:    x       横坐标(0~SCREEN.width-1)
 *          y       起始纵坐标(0~SCREEN.height-1)
 *          y2      结尾纵坐标(0~SCREEN.height-1)
 *          colors  颜色映象(颜色数组)
 * 注意:    本函数专门针对1024x768x256进行了优化
 *          本函数主要用于显示图形(位图)
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
    asm and  di, di                           /* 以防在x=1023时出错 */
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
 * 函数:    X_DRAW_H_BITMAP()
 * 说明:    根据bitmap中的位画点(横向), 如果某位为1,则在对应位置画点,否则不画
 *          一次最多画8个点
 * 输入:    x       起始横坐标(0~SCREEN.width-1)
 *          y       纵坐标(0~SCREEN.height-1)
 *          dots    画点的个数
 *          bitmap  一个字节的位模式
 *          color   颜色
 * 注意:    本函数专门针对1024x768x256进行了优化
 * 用途:    本函数可大大加快字体的显示, 可用于画虚线
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
* 函数: X_DRAW_V_BITMAP()
* 说明: 根据bitmap中的位画点(纵向), 如果某位为1,则在对应位置画点,否则不画
*       一次最多画8个点
* 输入: x       起始横坐标(0~SCREEN.width-1)
*       y       纵坐标(0~SCREEN.height-1)
*       bitmap  一个字节的位模式
*       color   颜色
* 注意: 本函数专门针对1024x768x256进行了优化
* 用途: 本函数可大大加快字体的显示, 可用于画虚线
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
    asm and  di, di                           /* 以防在x=1023时出错                 */
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
 * 函数:    X_DRAW_H_BITMAP_BG()
 * 说明:    根据bitmap中的位画点(横向), 如果某位为1,则在对应位置以color画点,
 *          否则以bgcolor画点, 一次最多画8个点.
 * 输入:    x       起始横坐标(0~SCREEN.width-1)
 *          y       纵坐标(0~SCREEN.height-1)
 *          dots    画点的个数
 *          bitmap  一个字节的位模式
 *          color   颜色
 *          bgcolor 背景颜色
 * 注意:    本函数专门针对1024x768x256进行了优化
 * 用途:    本函数可大大加快字体的显示, 可用于画虚线
 *          在需要画背景色的时候非常有用
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
 * 函数:    X_DRAW_V_BITMAP_BG()
 * 说明:    根据bitmap中的位画点(纵向), 如果某位为1,则在对应位置以color画点,
 *          否则以bgcolor画点, 一次最多画8个点.
 * 输入:    x       起始横坐标(0~SCREEN.width-1)
 *          y       纵坐标(0~SCREEN.height-1)
 *          bitmap  一个字节的位模式
 *          color   颜色
 *          bgcolor 背景颜色
 * 注意:    本函数专门针对1024x768x256进行了优化
 * 用途:    本函数可大大加快字体的显示, 可用于画虚线
 *          在需要画背景色的时候非常有用
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
    asm and  di, di                           /* 以防在x=1023时出错                 */
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
 * 函数:    X_DRAW_H_BITMAP_BGMAP()
 * 说明:    根据bitmap中的位画点(横向), 如果某位为1,则在对应位置以color画点,
 *          否则以bgcolors中的颜色画点, 一次最多画8个点.
 * 输入:    x        起始横坐标(0~SCREEN.width-1)
 *          y        纵坐标(0~SCREEN.height-1)
 *          bitmap   一个字节的位模式
 *          color    颜色
 *          bgcolors 背景颜色
 * 注意:    本函数专门针对1024x768x256进行了优化
 * 用途:    本函数可大大加快字体的显示, 可用于画虚线
 *          在需要画背景图形的时候非常有用
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
        return X_DRAW_H_BITMAP(x, y, dots, bitmap, color); /* 如果没有背景颜色,则不画背景色 */
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
 * 函数:    X_DRAW_V_BITMAP_BG()
 * 说明:    根据bitmap中的位画点(纵向), 如果某位为1,则在对应位置以color画点,
 *          否则以bgcolors中的颜色画点, 一次最多画8个点.
 * 输入:    x        起始横坐标(0~SCREEN.width-1)
 *          y        纵坐标(0~SCREEN.height-1)
 *          bitmap   一个字节的位模式
 *          color    颜色
 *          bgcolors 背景颜色
 * 注意:    本函数专门针对1024x768x256进行了优化
 * 用途:    本函数可大大加快字体的显示, 可用于画虚线
 *          在需要画背景图形的时候非常有用
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
    asm and  di, di                           /* 以防在x=1023时出错                 */
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
 * 函数:    X_DRAW_RECT()
 * 说明:    画矩形
 * 输入:    x       起始横坐标(0~SCREEN.width-1)
 *          y       起始纵坐标(0~SCREEN.height-1)
 *          x2      结尾横坐标(0~SCREEN.width-1)
 *          y2      结尾纵坐标(0~SCREEN.height-1)
 *          color   线的颜色
 * 输出:    ok/fail
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
 * 函数:    X_DRAW_BOX()
 * 说明:    画矩形条
 * 输入:    x       起始横坐标(0~SCREEN.width-1)
 *          y       起始纵坐标(0~SCREEN.height-1)
 *          x2      结尾横坐标(0~SCREEN.width-1)
 *          y2      结尾纵坐标(0~SCREEN.height-1)
 *          color   线的颜色
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
*本文件结束: vesa.c ==================================================================
*/

