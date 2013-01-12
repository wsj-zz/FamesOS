/*************************************************************************************
** 文件: vesa.h
** 说明: 图形显示接口(驱动, VESA)
** 作者: Jun
** 时间: 2010-03-03
** 版本: V0.1 (2010-03-03, 最初的版本)
*************************************************************************************/
#ifndef FAMES_X_DRIVER_H
#define FAMES_X_DRIVER_H

/*
*编译开关-----------------------------------------------------------------------------
*/
#define FAMES_VESA_EN         1           /* 是否要生成X-DRIVER代码,    1=是      */

#define FAMES_VESA_NOP()                  /* NON-OPERATION in X-DRIVER            */ 

/*
*视频缓冲地址-------------------------------------------------------------------------
*/
#define VIDEOSEG   (            0xA000     )  /* 视频段值                       */
#define VIDEOADDR  ((char far *)0xA0000000L)  /* 视频首地址                     */

/*
*颜色数定义---------------------------------------------------------------------------
*/
#define COLOR_TYPE_16  1                      /* 16  色                           */
#define COLOR_TYPE_256 2                      /* 256 色                           */
#define COLOR_TYPE_16W 3                      /* 16位色(增强色)                   */
#define COLOR_TYPE_32W 4                      /* 32位色(真彩色)                   */

#define COLOR_TYPE     COLOR_TYPE_256         /* 当前使用256色(用来定义颜色类型)  */

/*
*视频模式-----------------------------------------------------------------------------
*/
#define VMODE_TEXT              0x3           /* 文本模式(80x25)                    */

#define VMODE_640x480x16        0x12          /* 图形模式(640x480    16色, TC2.0)   */
#define VMODE_640x480x256       0x101         /* 图形模式(640x480   256色)          */
#define VMODE_640x480x16W       0x111         /* 图形模式(640x480  16位色)          */
#define VMODE_640x480x32W       0x112         /* 图形模式(640x480  32位色)          */

#define VMODE_800x600x16        0x102         /* 图形模式(800x600    16色)          */
#define VMODE_800x600x256       0x103         /* 图形模式(800x600   256色)          */
#define VMODE_800x600x16W       0x114         /* 图形模式(800x600  16位色)          */
#define VMODE_800x600x32W       0x115         /* 图形模式(800x600  32位色)          */

#define VMODE_1024x768x16       0x104         /* 图形模式(1024x768   16色)          */
#define VMODE_1024x768x256      0x105         /* 图形模式(1024x768  256色)          */
#define VMODE_1024x768x16W      0x117         /* 图形模式(1024x768 16位色)          */
#define VMODE_1024x768x32W      0x118         /* 图形模式(1024x768 32位色)          */

/*
*下面定义屏幕(或VESA兼容显卡)相关的结构及变量-----------------------------------------
*/
typedef struct {                              /* 屏幕控制块(Screen Control Block)   */
    INT16S  opened;                           /* 图形模式是否已打开                 */
    INT16S  videomode;                        /* 所用的视频模式                     */
    INT16S  width;                            /* 屏幕宽度(象素数)                   */
    INT16S  height;                           /* 屏幕高度                           */
    INT16S  colorbits;                        /* 色深(每象素占用位数)               */
    INT16S  curr_x;                           /* 当前的X坐标                        */
    INT16S  curr_y;                           /* 当前的Y坐标                        */
    INT16S  curr_page;                        /* 当前的页号,用于换页                */
    INT16U  curr_addr;                        /* 当前象素点的偏移地址               */
    INT08S *vmode_string;                     /* 视频模式描述字符串                 */
}SCB;                          

#if COLOR_TYPE == COLOR_TYPE_256              /* 颜色类型定义                       */
#define COLOR     INT08U                      /* 我们现在用的是256色                */
#else
typedef struct {
    INT08U red;                               /* 红                                 */
    INT08U green;                             /* 绿                                 */
    INT08U blue;                              /* 蓝                                 */
}COLOR;
#endif


/*
*当前的屏幕控制块---------------------------------------------------------------------
*/
#ifdef FAMES_X_VESA_C
SCB        SCREEN={FALSE,VMODE_1024x768x256,1024,768,8,0,0,-1,0, "1024x768x256"};
#else
extern SCB SCREEN; 
#endif

/*
*默认调色板---------------------------------------------------------------------------
*/
#ifdef   FAMES_X_VESA_C
#include "gui\palette.h"
#define  DefaultPALETTE   FamesOS_PALETTE_WinXP
#endif

/*
*函数声明-----------------------------------------------------------------------------
*/
#if     FAMES_VESA_EN == 1
INT16S  X_OPEN_GRAPH(void);                   /* 打开图形模式                       */
INT16S  X_CLOSE_GRAPH(void);                  /* 关闭图形模式                       */
INT16S  X_GET_SCREEN_SIZE(INT16S * width,     /* 读取屏幕大小                       */
                          INT16S * height
                         );
INT16S  X_SET_PALETTE(INT16S index,           /* 设置某一颜色的调色板               */  
                      INT08U red,
                      INT08U green,
                      INT08U blue
                     );
INT16S  X_SET_PALETTE_DEFAULT(void);          /* 将调色板设为默认状态(FAMES的调色板)*/
INT16S  X_PUT_PIXEL(INT16S x,                 /* 画点                               */
                    INT16S y,
                    COLOR  color
                   );
COLOR   X_GET_PIXEL(INT16S x,                 /* 读点                               */
                    INT16S y
                   );
INT16S  X_CLEAR_SCREEN(COLOR color);          /* 清屏                               */
INT16S  X_DRAW_H_LINE(INT16S x,               /* 画水平线                           */
                      INT16S y,
                      INT16S x2,
                      COLOR  color
                     );
INT16S  X_DRAW_V_LINE(INT16S x,               /* 画垂直线                           */
                      INT16S y,
                      INT16S y2,
                      COLOR  color
                     );
INT16S  X_DRAW_H_IMAGE(INT16S x,              /* 按颜色映象(IMAGE)水平画线          */
                       INT16S y, 
                       INT16S x2, 
                       COLOR  colors[]
                      );
INT16S  X_DRAW_H_BITMAP(INT16S x,             /* 根据位模式画点(横向)               */
                        INT16S y, 
                        INT16S dots,
                        INT08U bitmap, 
                        COLOR  color
                       );
INT16S  X_DRAW_H_BITMAP_BG(INT16S x,          /* 根据位模式画点(横向), 单一背景色   */
                           INT16S y, 
                           INT16S dots,
                           INT08U bitmap, 
                           COLOR  color, 
                           COLOR  bgcolor
                          );
INT16S  X_DRAW_H_BITMAP_BGMAP(INT16S x,       /* 根据位模式画点(横向), 背景为图形   */
                              INT16S y, 
                              INT16S dots,
                              INT08U bitmap, 
                              COLOR  color, 
                              COLOR  bgcolors[]
                             );
INT16S  X_DRAW_V_IMAGE(INT16S x,              /* 按颜色映象(IMAGE)垂直画线          */
                       INT16S y, 
                       INT16S y2, 
                       COLOR colors[]
                      );
INT16S  X_DRAW_V_BITMAP(INT16S x,             /* 根据位模式画点(纵向)               */
                        INT16S y, 
                        INT08U bitmap, 
                        COLOR  color
                       );
INT16S  X_DRAW_V_BITMAP_BG(INT16S x,          /* 根据位模式画点(纵向), 单一背景色   */ 
                           INT16S y, 
                           INT08U bitmap, 
                           COLOR  color, 
                           COLOR  bgcolor
                          );
INT16S  X_DRAW_V_BITMAP_BGMAP(INT16S x,       /* 根据位模式画点(纵向), 背景为图形   */ 
                              INT16S y, 
                              INT08U bitmap, 
                              COLOR  color, 
                              COLOR  bgcolors[]
                             );
#else                                         /* #if FAMES_GDI_EN==1           */
/*
#define X_OPEN_GRAPH()                   FAMES_GDI_NOP()
#define X_CLOSE_GRAPH()                  FAMES_GDI_NOP()
#define X_SET_PALETTE(a,b,c,d)           FAMES_GDI_NOP()
#define X_SET_PALETTE_DEFAULT()          FAMES_GDI_NOP()
#define X_PUT_PIXEL(a,b,c)               FAMES_GDI_NOP()
#define X_GET_PIXEL(a,b)                 FAMES_GDI_NOP()
#define X_CLEAR_SCREEN(a)                FAMES_GDI_NOP()
#define X_DRAW_H_LINE(a,b,c,d)           FAMES_GDI_NOP()
#define X_DRAW_V_LINE(a,b,c,d)           FAMES_GDI_NOP()
#define X_DRAW_H_IMAGE(a,b,c,d)          FAMES_GDI_NOP()
#define X_DRAW_V_IMAGE(a,b,c,d)          FAMES_GDI_NOP()
#define X_DRAW_H_BITMAP(a,b,c,d)         FAMES_GDI_NOP()
#define X_DRAW_V_BITMAP(a,b,c,d)         FAMES_GDI_NOP()
#define X_DRAW_H_BITMAP_BG(a,b,c,d,e)    FAMES_GDI_NOP()
#define X_DRAW_V_BITMAP_BG(a,b,c,d,e)    FAMES_GDI_NOP()
#define X_DRAW_H_BITMAP_BGMAP(a,b,c,d,e) FAMES_GDI_NOP()
#define X_DRAW_V_BITMAP_BGMAP(a,b,c,d,e) FAMES_GDI_NOP()
*/
#endif                                        /* #if FAMES_VESA_EN==1           */

#endif                                        /* #ifndef FAMES_VESA_H           */

/*
*本文件结束: xdriver.h ===============================================================
*/


