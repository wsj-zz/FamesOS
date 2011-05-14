/***********************************************************************************************
** 文件:    bmp.h
**
** 说明:    图象(BMP)相关定义
**
** 作者:    Jun
**
** 时间:    2010-9-1
***********************************************************************************************/
#ifndef FAMES_BMP_H
#define FAMES_BMP_H

/*----------------------------------------------------------------------------------------------
 * 
 * BMP图片启用开关
 * 
**--------------------------------------------------------------------------------------------*/
#define FAMES_BMP_EN  1

#define FAMES_BMP_NOP() 

/*----------------------------------------------------------------------------------------------
 * 
 * BMP信息块及常量
 * 
**--------------------------------------------------------------------------------------------*/
typedef struct {                              /* BMP图象信息块--------------------- */
    INT16S    ready;                          /* 是否已加载, 1=是                   */
    XMSHANDLE handle;                         /* 本图象在XMS中的句柄                */
    INT16S    width;                          /* 图象宽度, 象素                     */
    INT16S    height;                         /* 图象高度, 象素                     */
    INT16S    maxwidth;                       /* 本图象允许显示的最大宽度,0=不限制  */
    INT16S    maxheight;                      /* 本图象允许显示的最大高度,0=不限制  */ 
    INT16S    colordepth;                     /* 图象色深, 位/象素                  */
    INT16S    bytesofline;                    /* 一行图象的字节数                   */
}BMPINFO;                                     /* ---------------------------------- */

#define FAMES_BMP_READY_NO    0               /* BMP图象尚未加载(或加载失败)        */
#define FAMES_BMP_READY_YES   1               /* BMP图象已加载,可以显示             */

#define FAMES_BMP_CD_1        1               /* 单色图象                           */
#define FAMES_BMP_CD_4        4               /* 16色                               */
#define FAMES_BMP_CD_8        8               /* 256色                              */              
#define FAMES_BMP_CD_16      16               /* 增强色                             */
#define FAMES_BMP_CD_24      24               /* 真彩色                             */
#define FAMES_BMP_CD_32      32               /* 真彩色                             */

/*----------------------------------------------------------------------------------------------
 * 
 * BMP文件相关数据结构
 * 
**--------------------------------------------------------------------------------------------*/
typedef struct{                               /* BMP关键结构, 18个字节------------- */
    INT08U  identifier[2];                    /* 标识: "BM"                         */
    INT32U  file_size;                        /* 文件大小,以字节计                  */
    INT32U  reserved;                         /* 保留                               */
    INT32U  bitmap_data_offset;               /* BMP数据相对文件头的偏移            */
    INT32U  bitmap_header_size;               /* BMP信息头大小                      */
}BmpCoreInfo;                                 /* ---------------------------------- */

typedef struct{                               /* BMP信息头, 36个字节--------------- */
    INT32U  width;                            /* BMP 宽度，以象素计                 */
    INT32U  height;                           /* BMP 高度，以象素计                 */
    INT16U  planes;                           /* 帧数(位面数)                       */
    INT16U  bits_per_pixel;                   /* 象素位宽，如: 8, 16, 24, 32, 1, 4  */
    INT32U  compression;                      /* 压缩规格                           */
    INT32U  bitmap_data_size;                 /* BMP 数据大小，rounded to 4 byte    */
    INT32U  hresolution;                      /* 水平分辨率?                        */
    INT32U  vresolution;                      /* 垂直分辨率?                        */
    INT32U  colors;                           /* 颜色数                             */
    INT32U  important_colors;                 /* 重要颜色数                         */
}BmpInfoHeader;                               /* ---------------------------------- */

/*----------------------------------------------------------------------------------------------
 * 
 * 本版本对BMP图象支持的最大宽度与最大高度
 * 
**--------------------------------------------------------------------------------------------*/
#define BMP_MAX_WIDTH   (1024)                /* 最大宽度                           */
#define BMP_MAX_HEIGHT  (768)                 /* 最大高度                           */

/*----------------------------------------------------------------------------------------------
 * 
 * 显示BMP图象时所用的缓冲区
 * 
**--------------------------------------------------------------------------------------------*/
#if    FAMES_BMP_EN == 1
#ifdef FAMES_BMP_C
COLOR  BMPCACHE[BMP_MAX_WIDTH+16];             /* 用于暂存从XMS取回的数据           */
#endif /* FAMES_BMP_C */
#endif /* FAMES_BMP_EN == 1 */

/*----------------------------------------------------------------------------------------------
 * 
 * 函数声明
 * 
**--------------------------------------------------------------------------------------------*/
#if      FAMES_BMP_EN == 1
BOOLEAN  apical InitBMPINFO(BMPINFO * bmpinfo);  
BOOLEAN  apical LoadBmp(BMPINFO * bmpinfo, FILENAME bmpfile);
BOOLEAN  apical UnloadBmp(BMPINFO * bmpinfo);
BOOLEAN  apical ShowBmp(INT16S x, INT16S y, BMPINFO * bmpinfo); 
INT16S   apical GetBmpLine(BMPINFO * bmpinfo, INT16S lineno); 
BOOLEAN  apical SetBmpRect(BMPINFO * bmpinfo, INT16S maxwidth, INT16S maxheight);
BOOLEAN  apical GetBmpPalette(INT08U palette[], FILENAME bmpfile); 
#else
/*
#define  InitBMPINFO(a)      FAMES_BMP_NOP()
#define  LoadBmp(a,b)        FAMES_BMP_NOP()
#define  UnloadBmp(a)        FAMES_BMP_NOP()
#define  ShowBmp(a,b,c)      FAMES_BMP_NOP()
#define  GetBmpLine(a,b)     FAMES_BMP_NOP()
#define  SetBmpRect(a,b,c)   FAMES_BMP_NOP()
#define  GetBmpPalette(a,b)  FAMES_BMP_NOP()
*/
#endif                                        /* #if FAMES_BMP_EN==1                */

#endif                                        /* #ifndef FAMES_BMP_H                */

/*==============================================================================================
 * 
 * 本文件结束: bmp.h
 * 
**============================================================================================*/

