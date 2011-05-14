/************************************************************************************************
** �ļ�: tool.h
** ˵��: ��������,������
** ����: Jun
** ʱ��: 2010-7-12
************************************************************************************************/
#ifndef FAMES_TOOL_H
#define FAMES_TOOL_H


/*----------------------------------------------------------------------------------------------
 *                   ��������ת��ѡ��
 *
 *  ת��ѡ��ĵ�8λָ����Ŀ��(Դ)�ַ����ĳ���, Ϊ0ʱ�������Ƴ���
 *  ��8λΪ����λ, �京������:
 *
 *  �����ѡ������ INT#toSTR()
 *  CHG_OPT_RAW      ��ת��, ֱ�ӽ��ֽڼӵ�Ŀ���ַ�����
 *  CHG_OPT_DEC      ������ת��Ϊʮ���ƴ�(����Ϊʮ�����ƴ�)
 *  CHG_OPT_SIG      ���ű�־
 *  CHG_OPT_FIL      �Ƿ�ָ�����
 *  CHG_OPT_ZER      ���ָ�����, ��λ�Ƿ����Ϊ0(��֮���Ϊ�ո�)
 *  CHG_OPT_LFT      ���ָ�����, �Ƿ������
 *  CHG_OPT_FRC      �Ƿ�תΪʵ����(��С����)
 *  CHG_OPT_END      �Ƿ�ҪΪ�ַ������Ͻ�����
 *
 *  �����ѡ������STRtoSTR()
 *  CHG_OPT_UPR      ���ַ���תΪ��д(�������ַ���ת��)
 *  CHG_OPT_LOW      ���ַ���תΪСд(�������ַ���ת��)
 *  CHG_OPT_RVT      �ַ�����ת
 *  CHG_OPT_END      Ϊ�ַ������Ͻ�����
 *
 *  �����ѡ������STRtoINT#()
 *  CHG_OPT_RAW      ��ת����ֱ�ӽ��ַ�(��)����Ϊ����
 *  CHG_OPT_DEC      ָ���ַ���Ϊʮ���ƴ�(����Ϊʮ�����ƴ�)
 *  CHG_OPT_RVT      ���ַ�����ת���ٽ���(���ڲ�ͬ���ֽ���)
 *   
 *  �����ѡ������INT#toBINSTR()
 *  CHG_OPT_END      Ϊ�ַ������Ͻ�����
 *
 *  ��ָ��Ϊʵ��ʱ, ��8λ��Ϊ��������, ���4λ������������, ��4λ����С������(i.e. 3����0.001)
 *                  С���������Ϊ9
**--------------------------------------------------------------------------------------------*/
#define  CHG_OPT_NUL      0x0000
#define  CHG_OPT_RAW      0x0100
#define  CHG_OPT_DEC      0x0200
#define  CHG_OPT_SIG      0x0400
#define  CHG_OPT_FIL      0x0800
#define  CHG_OPT_ZER      0x1000
#define  CHG_OPT_LFT      0x2000
#define  CHG_OPT_FRC      0x4000
#define  CHG_OPT_END      0x8000
#define  CHG_OPT_UPR      0x4000
#define  CHG_OPT_LOW      0x2000
#define  CHG_OPT_RVT      0x1000


/*----------------------------------------------------------------------------------------------
 * ����ת������
**--------------------------------------------------------------------------------------------*/
INT08S *INT08toSTR(INT08S * dst, INT08S   src, INT16U opt);
INT08S *INT16toSTR(INT08S * dst, INT16S   src, INT16U opt);
INT08S *INT32toSTR(INT08S * dst, INT32S   src, INT16U opt);
INT08S *STRtoSTR  (INT08S * dst, INT08S * src, INT16U opt);
INT08S  STRtoINT08(INT08S * src, INT16U   opt);
INT16S  STRtoINT16(INT08S * src, INT16U   opt);
INT32S  STRtoINT32(INT08S * src, INT16U   opt);

INT08S *INT08toBINSTR(INT08S * dst, INT08S src, INT16U opt);
INT08S *INT16toBINSTR(INT08S * dst, INT16S src, INT16U opt);
INT08S *INT32toBINSTR(INT08S * dst, INT32S src, INT16U opt);

INT16S  CHARtoDEC(INT08S ch);
INT16S  CHARtoHEX(INT08S ch);

#define DECtoCHAR(numeric)  HEXCHAR[(INT16U)(numeric)]   /* ��һ������תΪ��Ӧ���ַ� */
#define HEXtoCHAR(numeric)  HEXCHAR[(INT16U)(numeric)]   /* ��һ������תΪ��Ӧ���ַ� */

INT16U  INT16XCHG(INT16U value);
INT32U  INT32XCHG(INT32U value);

#define GET_UDP_PORT(PORT) INT16XCHG((INT16U)PORT)
#define GET_TCP_PORT(PORT) GET_UDP_PORT((PORT))
#define GET_IP_ADDR(IPv4)  INT32XCHG((INT32U)IPv4)

INT08S  UPPER(INT08S ch);
INT08S  LOWER(INT08S ch);

/*----------------------------------------------------------------------------------------------
 * �ַ������ڴ濽���Ⱥ���
**--------------------------------------------------------------------------------------------*/
INT16S  STRLEN (INT08S * str);
INT16S  STRCPY (INT08S * dst, INT08S * src);
INT16S  STRCMP (INT08S * s1,  INT08S * s2);
INT16S  STRICMP(INT08S * s1,  INT08S * s2);
INT16S  MEMCPY (INT08S * dst, INT08S * src, INT16S len);
INT16S  MEMCMP (INT08S * s1,  INT08S * s2,  INT16S len);
INT16S  MEMICMP(INT08S * s1,  INT08S * s2,  INT16S len);
INT16S  MEMSET (INT08S * dst, INT08S data, INT16S len);

/*----------------------------------------------------------------------------------------------
 * I/O����
**--------------------------------------------------------------------------------------------*/
void   outportbyte(INT16S port, INT08U data);
INT08U inportbyte (INT16S port);
void   outportword(INT16S port, INT16U data);
INT16U inportword (INT16S port);
void   outportdword(INT16S port,INT32U data);
INT32U inportdword (INT16S port);

/*----------------------------------------------------------------------------------------------
 * �ı�ģʽ�µ��������
**--------------------------------------------------------------------------------------------*/
void textprintstr(INT16S x, INT16S y, INT08S * str, INT08U color);
void textprintchar(INT16S x, INT16S y, INT08S ch, INT08U color);
void textprintdec16(INT16S x, INT16S y, INT16S val, INT08U color, INT16U opt);
void textprintdec32(INT16S x, INT16S y, INT32S val, INT08U color, INT16U opt);
void textprinthex16(INT16S x, INT16S y, INT16S val, INT08U color);
void textprinthex32(INT16S x, INT16S y, INT32S val, INT08U color);



/*----------------------------------------------------------------------------------------------
 * ��������
**--------------------------------------------------------------------------------------------*/
#define DEBUG_MODE 1

#if     DEBUG_MODE == 1
#else
#endif

#endif                                          /* #ifndef FAMES_TOOL_H                       */

/*
*���ļ�����: tool.h =============================================================================
*/

