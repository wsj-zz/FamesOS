/************************************************************************************************
** 文件: tool.h
** 说明: 辅助函数,变量等
** 作者: Jun
** 时间: 2010-7-12
************************************************************************************************/
#ifndef FAMES_TOOL_H
#define FAMES_TOOL_H


/*----------------------------------------------------------------------------------------------
 *                   基本数据转换选项
 *
 *  转换选项的低8位指定了目标(源)字符串的长度, 为0时代表不控制长度
 *  高8位为控制位, 其含义如下:
 *
 *  下面的选项用于 INT#toSTR()
 *  CHG_OPT_RAW      不转换, 直接将字节加到目标字符串中
 *  CHG_OPT_DEC      将数字转换为十进制串(否则为十六进制串)
 *  CHG_OPT_SIG      符号标志
 *  CHG_OPT_FIL      是否指定宽度
 *  CHG_OPT_ZER      如果指定宽度, 高位是否填存为0(反之填存为空格)
 *  CHG_OPT_LFT      如果指定宽度, 是否左对齐
 *  CHG_OPT_FRC      是否转为实数串(有小数点)
 *  CHG_OPT_END      是否要为字符串加上结束符
 *
 *  下面的选项用于STRtoSTR()
 *  CHG_OPT_UPR      将字符串转为大写(仅用于字符串转换)
 *  CHG_OPT_LOW      将字符串转为小写(仅用于字符串转换)
 *  CHG_OPT_RVT      字符串反转
 *  CHG_OPT_END      为字符串加上结束符
 *
 *  下面的选项用于STRtoINT#()
 *  CHG_OPT_RAW      不转换，直接将字符(串)解释为数字
 *  CHG_OPT_DEC      指定字符串为十进制串(否则为十六进制串)
 *  CHG_OPT_RVT      将字符串反转后再解释(用于不同的字节序)
 *   
 *  下面的选项用于INT#toBINSTR()
 *  CHG_OPT_END      为字符串加上结束符
 *
 *  当指定为实数时, 低8位分为了两部分, 其高4位代表整个长度, 低4位代表小数级数(i.e. 3代表0.001)
 *                  小数级数最大为9
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
 * 数据转换函数
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

#define DECtoCHAR(numeric)  HEXCHAR[(INT16U)(numeric)]   /* 将一个数字转为对应的字符 */
#define HEXtoCHAR(numeric)  HEXCHAR[(INT16U)(numeric)]   /* 将一个数字转为对应的字符 */

INT16U  INT16XCHG(INT16U value);
INT32U  INT32XCHG(INT32U value);

#define GET_UDP_PORT(PORT) INT16XCHG((INT16U)PORT)
#define GET_TCP_PORT(PORT) GET_UDP_PORT((PORT))
#define GET_IP_ADDR(IPv4)  INT32XCHG((INT32U)IPv4)

INT08S  UPPER(INT08S ch);
INT08S  LOWER(INT08S ch);

/*----------------------------------------------------------------------------------------------
 * 字符串及内存拷贝等函数
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
 * I/O函数
**--------------------------------------------------------------------------------------------*/
void   outportbyte(INT16S port, INT08U data);
INT08U inportbyte (INT16S port);
void   outportword(INT16S port, INT16U data);
INT16U inportword (INT16S port);
void   outportdword(INT16S port,INT32U data);
INT32U inportdword (INT16S port);

/*----------------------------------------------------------------------------------------------
 * 文本模式下的输出函数
**--------------------------------------------------------------------------------------------*/
void textprintstr(INT16S x, INT16S y, INT08S * str, INT08U color);
void textprintchar(INT16S x, INT16S y, INT08S ch, INT08U color);
void textprintdec16(INT16S x, INT16S y, INT16S val, INT08U color, INT16U opt);
void textprintdec32(INT16S x, INT16S y, INT32S val, INT08U color, INT16U opt);
void textprinthex16(INT16S x, INT16S y, INT16S val, INT08U color);
void textprinthex32(INT16S x, INT16S y, INT32S val, INT08U color);



/*----------------------------------------------------------------------------------------------
 * 调试命令
**--------------------------------------------------------------------------------------------*/
#define DEBUG_MODE 1

#if     DEBUG_MODE == 1
#else
#endif

#endif                                          /* #ifndef FAMES_TOOL_H                       */

/*
*本文件结束: tool.h =============================================================================
*/

