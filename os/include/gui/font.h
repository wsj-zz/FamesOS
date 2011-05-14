/***********************************************************************************************
 * 文件:    font.h
 *
 * 说明:    字体相关定义
 *
 * 作者:    Jun
 *
 * 时间:    2010-12-29
***********************************************************************************************/
#ifndef FAMES_FONT_H
#define FAMES_FONT_H

/*----------------------------------------------------------------------------------------------
 * 
 * FONT信息块及常量
 * 
**--------------------------------------------------------------------------------------------*/
typedef struct {                    
    BOOL      loaded;               /* 加载标志, YES=已加载                         */
    INT16U    type;                 /* 字库类型                                     */
    XMSHANDLE handle;               /* 本字库在XMS中的句柄                          */
    int       width;                /* 字体宽度, 象素                               */
    int       height;               /* 字体高度, 象素                               */
    int       bytesofline;          /* 一行字模的字节数                             */
    int       start;                /* 第一个字模的编码                             */
    INT08S   *filename;             /* 字库文件名                                   */
    INT08S   *ref_table;            /* 如果是特殊的小字库, 指向其列表, 暂无用到     */
}FONTINFO;                           

#define FONT_TYPE_ASCII    0x0      /* 字库基本类型, ASCII, GB                      */
#define FONT_TYPE_GB2312   0x1

#define FONT_MAX_SIZE      96       /* 字体最大尺寸                                 */


/*----------------------------------------------------------------------------------------------
 * 
 * draw_font()的选项(opt)
 * 
**--------------------------------------------------------------------------------------------*/
#define  DRAW_OPT_FIL_BG       0x01  /* 是否画字体背景                 */

#define  DRAW_OPT_ALIGN_RIGHT  0x10  /* 右对齐   */
#define  DRAW_OPT_ALIGN_CENTER 0x20  /* 居中     */


/*----------------------------------------------------------------------------------------------
 * 
 * 字库操作及文本显示(函数声明)
 * 
**--------------------------------------------------------------------------------------------*/
BOOL  apical load_font(FONTINFO * fontinfo);
BOOL  apical unload_font(FONTINFO * fontinfo);
BOOL  apical get_font_info(FONTINFO * dest, int font);
int   apical register_font(FONTINFO * hz, FONTINFO * asc);
BOOL  apical unregister_font(int font);
BOOL  apical draw_font(int x, int y, INT08S  * str, COLOR color, int font, INT16U opt);
BOOL  apical draw_font_ex(int x, int y, int width, int height, 
                        INT08S * str, 
                        COLOR color, COLOR bkcolor,
                        int font, INT16U opt);
BOOL  apical draw_font_for_widget(int x, int y, int width, int height, 
                        INT08S * str, INT08S * oldstr,
                        COLOR color, COLOR bkcolor,
                        int font, INT16U opt);
void  apical load_sys_font(void); /* 加载系统默认字体 */


#endif /* #ifndef FAMES_FONT_H                */
/*==============================================================================================
 * 
 * 本文件结束: font.h
 * 
**============================================================================================*/

