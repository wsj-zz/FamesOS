/******************************************************************************************
 * �ļ�:    gui/widget/view.h
 *
 * ����:    ������ͼ�ؼ�
 *
 * ����:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_VIEW_H
#define FAMES_GUI_WIDGET_VIEW_H


/*-----------------------------------------------------------------------------------------
 * 
 *      VIEW���
 * 
**---------------------------------------------------------------------------------------*/
#define VIEW_STYLE_NO_BORDER        0x0001       /* �ޱ߿�     */
#define VIEW_STYLE_SUBSIDE          0x0002       /* �³�       */
#define VIEW_STYLE_STATIC_BDR       0x0004       /* ��̬��     */
#define VIEW_STYLE_MODAL_FRAME      0x0008       /* ģʽ��     */
#define VIEW_STYLE_XP_BORDER        0x1000       /* XP���߿� */

#define VIEW_STYLE_FIELDS_TITLE     0x0010       /* ��λ����   */
#define VIEW_STYLE_MARK_BAR         0x0020       /* ����     */
#define VIEW_STYLE_STATISTIC_BAR    0x0040       /* ͳ����     */
#define VIEW_STYLE_NONE_FIRST       0x0080       /* ���ױ�״̬ */
#define VIEW_STYLE_NONE_SELECT      0x0100       /* ��ѡ��״̬ */

/*-----------------------------------------------------------------------------------------
 * 
 *      VIEW�ֶζ���ṹ
 * 
**---------------------------------------------------------------------------------------*/
typedef struct view_fields_struct { /* �����ֶεĽṹ */
    char  * caption;
    int     id;
    int     bytes;
    int     bytes_for_width;
    INT16U  style;
    INT16U  draw_style;
    char  * comment;
    /* �����ֶβ����ʼ�� */
    int     offset_x;
    int     width;
    int     height;
    int     old_buf_ptr;
} view_fields_t;

typedef int  (* view_get_max_index_f)(void);
typedef BOOL (* view_get_item_f)(int index, int field_id, char *buf, int buf_len, INT16U option);
typedef BOOL (* view_set_item_f)(int index, int field_id, char *buf, int buf_len, KEYCODE key, INT16U option);
typedef BOOL (* view_is_writable_f)(int index, int field_id, INT16U option);
typedef void (* view_show_record_f)(int index, int row, 
                                    int x, int y, int width_zoom, int height, 
                                    COLOR color, COLOR bkcolor, int font,
                                    COLOR marker_color, COLOR marker_bkcolor,
                                    void *old, int *fields_width, INT16U option);
typedef void (* view_show_statistics_f)(int index,
                                    int x, int y, int width, int height, 
                                    int color, int bkcolor, int font,
                                    INT08S *field_comment,
                                    INT08S *old, INT16U option);
typedef void (* view_notifier_f)(int index, int prev_index, INT16U option);

/*-----------------------------------------------------------------------------------------
 * 
 *      ��������
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_view_init_private(
                    gui_widget * view,                      /* VIEWָ��               */
                    view_fields_t * fields,                 /* �ֶζ���               */
                    view_get_max_index_f get_max_index,     /* ��������¼�����ĺ��� */
                    view_get_item_f  get_item,              /* �����ݵĺ���           */
                    view_set_item_f  set_item,              /* д���ݵĺ���           */
                    view_is_writable_f  is_writable,        /* ���������Ƿ��д�ĺ��� */
                    view_show_record_f  show_record,        /* ��ʾ��¼�ĺ���         */
                    char * statistics_caption,              /* ͳ��������             */
                    view_show_statistics_f show_statistics, /* ��ʾͳ�������ݵĺ���   */
                    view_notifier_f notifier_on_changed,    /* �ı�ѡ��ʱ��֪ͨ����   */
                    COLOR data_color,                       /* ������ǰ��ɫ           */
                    COLOR data_bkcolor,                     /* ����������ɫ           */
                    int  height_per_row,                    /* һ�м�¼����ʾ�߶�     */
                    BOOL alloc_memory);                     /* �Ƿ�����ڴ�           */
BOOL  guical  gui_view_goto_top(gui_widget *view);
BOOL  guical  gui_view_goto_bottom(gui_widget *view);
BOOL  guical  gui_view_move_up(gui_widget *view);
BOOL  guical  gui_view_move_down(gui_widget *view);
BOOL  guical  gui_view_page_up(gui_widget *view);
BOOL  guical  gui_view_page_down(gui_widget *view);
BOOL  guical  gui_view_select_index(gui_widget *view, int index);
int   guical  gui_view_get_selected(gui_widget *view);
BOOL  guical  gui_view_set_dashed(gui_widget *view, COLOR dashed_color, INT08U dashed_style);
void          gui_draw_view(gui_widget * view);
KEYCODE guical gui_view_editing(gui_widget *view, INT16U opt);



#endif /* #ifndef FAMES_GUI_WIDGET_VIEW_H */

/*=========================================================================================
 * 
 * ���ļ�����: gui/widget/view.h
 * 
**=======================================================================================*/


