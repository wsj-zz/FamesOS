/***********************************************************************************************
** �ļ�:    bmp.h
**
** ˵��:    ͼ��(BMP)��ض���
**
** ����:    Jun
**
** ʱ��:    2010-9-1
***********************************************************************************************/
#ifndef FAMES_BMP_H
#define FAMES_BMP_H

/*----------------------------------------------------------------------------------------------
 * 
 * BMPͼƬ���ÿ���
 * 
**--------------------------------------------------------------------------------------------*/
#define FAMES_BMP_EN  1

#define FAMES_BMP_NOP() 

/*----------------------------------------------------------------------------------------------
 * 
 * BMP��Ϣ�鼰����
 * 
**--------------------------------------------------------------------------------------------*/
typedef struct {                              /* BMPͼ����Ϣ��--------------------- */
    INT16S    ready;                          /* �Ƿ��Ѽ���, 1=��                   */
    XMSHANDLE handle;                         /* ��ͼ����XMS�еľ��                */
    INT16S    width;                          /* ͼ����, ����                     */
    INT16S    height;                         /* ͼ��߶�, ����                     */
    INT16S    maxwidth;                       /* ��ͼ��������ʾ�������,0=������  */
    INT16S    maxheight;                      /* ��ͼ��������ʾ�����߶�,0=������  */ 
    INT16S    colordepth;                     /* ͼ��ɫ��, λ/����                  */
    INT16S    bytesofline;                    /* һ��ͼ����ֽ���                   */
}BMPINFO;                                     /* ---------------------------------- */

#define FAMES_BMP_READY_NO    0               /* BMPͼ����δ����(�����ʧ��)        */
#define FAMES_BMP_READY_YES   1               /* BMPͼ���Ѽ���,������ʾ             */

#define FAMES_BMP_CD_1        1               /* ��ɫͼ��                           */
#define FAMES_BMP_CD_4        4               /* 16ɫ                               */
#define FAMES_BMP_CD_8        8               /* 256ɫ                              */              
#define FAMES_BMP_CD_16      16               /* ��ǿɫ                             */
#define FAMES_BMP_CD_24      24               /* ���ɫ                             */
#define FAMES_BMP_CD_32      32               /* ���ɫ                             */

/*----------------------------------------------------------------------------------------------
 * 
 * BMP�ļ�������ݽṹ
 * 
**--------------------------------------------------------------------------------------------*/
typedef struct{                               /* BMP�ؼ��ṹ, 18���ֽ�------------- */
    INT08U  identifier[2];                    /* ��ʶ: "BM"                         */
    INT32U  file_size;                        /* �ļ���С,���ֽڼ�                  */
    INT32U  reserved;                         /* ����                               */
    INT32U  bitmap_data_offset;               /* BMP��������ļ�ͷ��ƫ��            */
    INT32U  bitmap_header_size;               /* BMP��Ϣͷ��С                      */
}BmpCoreInfo;                                 /* ---------------------------------- */

typedef struct{                               /* BMP��Ϣͷ, 36���ֽ�--------------- */
    INT32U  width;                            /* BMP ��ȣ������ؼ�                 */
    INT32U  height;                           /* BMP �߶ȣ������ؼ�                 */
    INT16U  planes;                           /* ֡��(λ����)                       */
    INT16U  bits_per_pixel;                   /* ����λ����: 8, 16, 24, 32, 1, 4  */
    INT32U  compression;                      /* ѹ�����                           */
    INT32U  bitmap_data_size;                 /* BMP ���ݴ�С��rounded to 4 byte    */
    INT32U  hresolution;                      /* ˮƽ�ֱ���?                        */
    INT32U  vresolution;                      /* ��ֱ�ֱ���?                        */
    INT32U  colors;                           /* ��ɫ��                             */
    INT32U  important_colors;                 /* ��Ҫ��ɫ��                         */
}BmpInfoHeader;                               /* ---------------------------------- */

/*----------------------------------------------------------------------------------------------
 * 
 * ���汾��BMPͼ��֧�ֵ�����������߶�
 * 
**--------------------------------------------------------------------------------------------*/
#define BMP_MAX_WIDTH   (1024)                /* �����                           */
#define BMP_MAX_HEIGHT  (768)                 /* ���߶�                           */

/*----------------------------------------------------------------------------------------------
 * 
 * ��ʾBMPͼ��ʱ���õĻ�����
 * 
**--------------------------------------------------------------------------------------------*/
#if    FAMES_BMP_EN == 1
#ifdef FAMES_BMP_C
COLOR  BMPCACHE[BMP_MAX_WIDTH+16];             /* �����ݴ��XMSȡ�ص�����           */
#endif /* FAMES_BMP_C */
#endif /* FAMES_BMP_EN == 1 */

/*----------------------------------------------------------------------------------------------
 * 
 * ��������
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
 * ���ļ�����: bmp.h
 * 
**============================================================================================*/

