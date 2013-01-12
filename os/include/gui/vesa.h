/*************************************************************************************
** �ļ�: vesa.h
** ˵��: ͼ����ʾ�ӿ�(����, VESA)
** ����: Jun
** ʱ��: 2010-03-03
** �汾: V0.1 (2010-03-03, ����İ汾)
*************************************************************************************/
#ifndef FAMES_X_DRIVER_H
#define FAMES_X_DRIVER_H

/*
*���뿪��-----------------------------------------------------------------------------
*/
#define FAMES_VESA_EN         1           /* �Ƿ�Ҫ����X-DRIVER����,    1=��      */

#define FAMES_VESA_NOP()                  /* NON-OPERATION in X-DRIVER            */ 

/*
*��Ƶ�����ַ-------------------------------------------------------------------------
*/
#define VIDEOSEG   (            0xA000     )  /* ��Ƶ��ֵ                       */
#define VIDEOADDR  ((char far *)0xA0000000L)  /* ��Ƶ�׵�ַ                     */

/*
*��ɫ������---------------------------------------------------------------------------
*/
#define COLOR_TYPE_16  1                      /* 16  ɫ                           */
#define COLOR_TYPE_256 2                      /* 256 ɫ                           */
#define COLOR_TYPE_16W 3                      /* 16λɫ(��ǿɫ)                   */
#define COLOR_TYPE_32W 4                      /* 32λɫ(���ɫ)                   */

#define COLOR_TYPE     COLOR_TYPE_256         /* ��ǰʹ��256ɫ(����������ɫ����)  */

/*
*��Ƶģʽ-----------------------------------------------------------------------------
*/
#define VMODE_TEXT              0x3           /* �ı�ģʽ(80x25)                    */

#define VMODE_640x480x16        0x12          /* ͼ��ģʽ(640x480    16ɫ, TC2.0)   */
#define VMODE_640x480x256       0x101         /* ͼ��ģʽ(640x480   256ɫ)          */
#define VMODE_640x480x16W       0x111         /* ͼ��ģʽ(640x480  16λɫ)          */
#define VMODE_640x480x32W       0x112         /* ͼ��ģʽ(640x480  32λɫ)          */

#define VMODE_800x600x16        0x102         /* ͼ��ģʽ(800x600    16ɫ)          */
#define VMODE_800x600x256       0x103         /* ͼ��ģʽ(800x600   256ɫ)          */
#define VMODE_800x600x16W       0x114         /* ͼ��ģʽ(800x600  16λɫ)          */
#define VMODE_800x600x32W       0x115         /* ͼ��ģʽ(800x600  32λɫ)          */

#define VMODE_1024x768x16       0x104         /* ͼ��ģʽ(1024x768   16ɫ)          */
#define VMODE_1024x768x256      0x105         /* ͼ��ģʽ(1024x768  256ɫ)          */
#define VMODE_1024x768x16W      0x117         /* ͼ��ģʽ(1024x768 16λɫ)          */
#define VMODE_1024x768x32W      0x118         /* ͼ��ģʽ(1024x768 32λɫ)          */

/*
*���涨����Ļ(��VESA�����Կ�)��صĽṹ������-----------------------------------------
*/
typedef struct {                              /* ��Ļ���ƿ�(Screen Control Block)   */
    INT16S  opened;                           /* ͼ��ģʽ�Ƿ��Ѵ�                 */
    INT16S  videomode;                        /* ���õ���Ƶģʽ                     */
    INT16S  width;                            /* ��Ļ���(������)                   */
    INT16S  height;                           /* ��Ļ�߶�                           */
    INT16S  colorbits;                        /* ɫ��(ÿ����ռ��λ��)               */
    INT16S  curr_x;                           /* ��ǰ��X����                        */
    INT16S  curr_y;                           /* ��ǰ��Y����                        */
    INT16S  curr_page;                        /* ��ǰ��ҳ��,���ڻ�ҳ                */
    INT16U  curr_addr;                        /* ��ǰ���ص��ƫ�Ƶ�ַ               */
    INT08S *vmode_string;                     /* ��Ƶģʽ�����ַ���                 */
}SCB;                          

#if COLOR_TYPE == COLOR_TYPE_256              /* ��ɫ���Ͷ���                       */
#define COLOR     INT08U                      /* ���������õ���256ɫ                */
#else
typedef struct {
    INT08U red;                               /* ��                                 */
    INT08U green;                             /* ��                                 */
    INT08U blue;                              /* ��                                 */
}COLOR;
#endif


/*
*��ǰ����Ļ���ƿ�---------------------------------------------------------------------
*/
#ifdef FAMES_X_VESA_C
SCB        SCREEN={FALSE,VMODE_1024x768x256,1024,768,8,0,0,-1,0, "1024x768x256"};
#else
extern SCB SCREEN; 
#endif

/*
*Ĭ�ϵ�ɫ��---------------------------------------------------------------------------
*/
#ifdef   FAMES_X_VESA_C
#include "gui\palette.h"
#define  DefaultPALETTE   FamesOS_PALETTE_WinXP
#endif

/*
*��������-----------------------------------------------------------------------------
*/
#if     FAMES_VESA_EN == 1
INT16S  X_OPEN_GRAPH(void);                   /* ��ͼ��ģʽ                       */
INT16S  X_CLOSE_GRAPH(void);                  /* �ر�ͼ��ģʽ                       */
INT16S  X_GET_SCREEN_SIZE(INT16S * width,     /* ��ȡ��Ļ��С                       */
                          INT16S * height
                         );
INT16S  X_SET_PALETTE(INT16S index,           /* ����ĳһ��ɫ�ĵ�ɫ��               */  
                      INT08U red,
                      INT08U green,
                      INT08U blue
                     );
INT16S  X_SET_PALETTE_DEFAULT(void);          /* ����ɫ����ΪĬ��״̬(FAMES�ĵ�ɫ��)*/
INT16S  X_PUT_PIXEL(INT16S x,                 /* ����                               */
                    INT16S y,
                    COLOR  color
                   );
COLOR   X_GET_PIXEL(INT16S x,                 /* ����                               */
                    INT16S y
                   );
INT16S  X_CLEAR_SCREEN(COLOR color);          /* ����                               */
INT16S  X_DRAW_H_LINE(INT16S x,               /* ��ˮƽ��                           */
                      INT16S y,
                      INT16S x2,
                      COLOR  color
                     );
INT16S  X_DRAW_V_LINE(INT16S x,               /* ����ֱ��                           */
                      INT16S y,
                      INT16S y2,
                      COLOR  color
                     );
INT16S  X_DRAW_H_IMAGE(INT16S x,              /* ����ɫӳ��(IMAGE)ˮƽ����          */
                       INT16S y, 
                       INT16S x2, 
                       COLOR  colors[]
                      );
INT16S  X_DRAW_H_BITMAP(INT16S x,             /* ����λģʽ����(����)               */
                        INT16S y, 
                        INT16S dots,
                        INT08U bitmap, 
                        COLOR  color
                       );
INT16S  X_DRAW_H_BITMAP_BG(INT16S x,          /* ����λģʽ����(����), ��һ����ɫ   */
                           INT16S y, 
                           INT16S dots,
                           INT08U bitmap, 
                           COLOR  color, 
                           COLOR  bgcolor
                          );
INT16S  X_DRAW_H_BITMAP_BGMAP(INT16S x,       /* ����λģʽ����(����), ����Ϊͼ��   */
                              INT16S y, 
                              INT16S dots,
                              INT08U bitmap, 
                              COLOR  color, 
                              COLOR  bgcolors[]
                             );
INT16S  X_DRAW_V_IMAGE(INT16S x,              /* ����ɫӳ��(IMAGE)��ֱ����          */
                       INT16S y, 
                       INT16S y2, 
                       COLOR colors[]
                      );
INT16S  X_DRAW_V_BITMAP(INT16S x,             /* ����λģʽ����(����)               */
                        INT16S y, 
                        INT08U bitmap, 
                        COLOR  color
                       );
INT16S  X_DRAW_V_BITMAP_BG(INT16S x,          /* ����λģʽ����(����), ��һ����ɫ   */ 
                           INT16S y, 
                           INT08U bitmap, 
                           COLOR  color, 
                           COLOR  bgcolor
                          );
INT16S  X_DRAW_V_BITMAP_BGMAP(INT16S x,       /* ����λģʽ����(����), ����Ϊͼ��   */ 
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
*���ļ�����: xdriver.h ===============================================================
*/


