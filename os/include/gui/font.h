/***********************************************************************************************
 * �ļ�:    font.h
 *
 * ˵��:    ������ض���
 *
 * ����:    Jun
 *
 * ʱ��:    2010-12-29
***********************************************************************************************/
#ifndef FAMES_FONT_H
#define FAMES_FONT_H

/*----------------------------------------------------------------------------------------------
 * 
 * FONT��Ϣ�鼰����
 * 
**--------------------------------------------------------------------------------------------*/
typedef struct {                    
    BOOL      loaded;               /* ���ر�־, YES=�Ѽ���                         */
    INT16U    type;                 /* �ֿ�����                                     */
    XMSHANDLE handle;               /* ���ֿ���XMS�еľ��                          */
    int       width;                /* ������, ����                               */
    int       height;               /* ����߶�, ����                               */
    int       bytesofline;          /* һ����ģ���ֽ���                             */
    int       start;                /* ��һ����ģ�ı���                             */
    INT08S   *filename;             /* �ֿ��ļ���                                   */
    INT08S   *ref_table;            /* ����������С�ֿ�, ָ�����б�, �����õ�     */
}FONTINFO;                           

#define FONT_TYPE_ASCII    0x0      /* �ֿ��������, ASCII, GB                      */
#define FONT_TYPE_GB2312   0x1

#define FONT_MAX_SIZE      96       /* �������ߴ�                                 */


/*----------------------------------------------------------------------------------------------
 * 
 * draw_font()��ѡ��(opt)
 * 
**--------------------------------------------------------------------------------------------*/
#define  DRAW_OPT_FIL_BG       0x01  /* �Ƿ����屳��                 */

#define  DRAW_OPT_ALIGN_RIGHT  0x10  /* �Ҷ���   */
#define  DRAW_OPT_ALIGN_CENTER 0x20  /* ����     */


/*----------------------------------------------------------------------------------------------
 * 
 * �ֿ�������ı���ʾ(��������)
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
void  apical load_sys_font(void); /* ����ϵͳĬ������ */


#endif /* #ifndef FAMES_FONT_H                */
/*==============================================================================================
 * 
 * ���ļ�����: font.h
 * 
**============================================================================================*/

