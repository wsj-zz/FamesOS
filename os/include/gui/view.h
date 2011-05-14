/******************************************************************************************
 * 文件:    gui/widget/view.h
 *
 * 描述:    数据视图控件
 *
 * 作者:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_VIEW_H
#define FAMES_GUI_WIDGET_VIEW_H


/*-----------------------------------------------------------------------------------------
 * 
 *      VIEW风格
 * 
**---------------------------------------------------------------------------------------*/
#define VIEW_STYLE_NO_BORDER        0x0001       /* 无边框     */
#define VIEW_STYLE_SUBSIDE          0x0002       /* 下沉       */
#define VIEW_STYLE_STATIC_BDR       0x0004       /* 静态框     */
#define VIEW_STYLE_MODAL_FRAME      0x0008       /* 模式框     */
#define VIEW_STYLE_XP_BORDER        0x1000       /* XP风格边框 */

#define VIEW_STYLE_FIELDS_TITLE     0x0010       /* 栏位标题   */
#define VIEW_STYLE_MARK_BAR         0x0020       /* 左标记     */
#define VIEW_STYLE_STATISTIC_BAR    0x0040       /* 统计条     */
#define VIEW_STYLE_NONE_FIRST       0x0080       /* 无首笔状态 */
#define VIEW_STYLE_NONE_SELECT      0x0100       /* 无选中状态 */

/*-----------------------------------------------------------------------------------------
 * 
 *      VIEW字段定义结构
 * 
**---------------------------------------------------------------------------------------*/
typedef struct view_fields_struct { /* 定义字段的结构 */
    char  * caption;
    int     id;
    int     bytes;
    int     bytes_for_width;
    INT16U  style;
    INT16U  draw_style;
    char  * comment;
    /* 下面字段不需初始化 */
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
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_view_init_private(
                    gui_widget * view,                      /* VIEW指针               */
                    view_fields_t * fields,                 /* 字段定义               */
                    view_get_max_index_f get_max_index,     /* 返回最大记录索引的函数 */
                    view_get_item_f  get_item,              /* 读数据的函数           */
                    view_set_item_f  set_item,              /* 写数据的函数           */
                    view_is_writable_f  is_writable,        /* 返回数据是否可写的函数 */
                    view_show_record_f  show_record,        /* 显示记录的函数         */
                    char * statistics_caption,              /* 统计栏标题             */
                    view_show_statistics_f show_statistics, /* 显示统计栏内容的函数   */
                    view_notifier_f notifier_on_changed,    /* 改变选择时的通知函数   */
                    COLOR data_color,                       /* 数据区前景色           */
                    COLOR data_bkcolor,                     /* 数据区背景色           */
                    int  height_per_row,                    /* 一行记录的显示高度     */
                    BOOL alloc_memory);                     /* 是否分配内存           */
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
 * 本文件结束: gui/widget/view.h
 * 
**=======================================================================================*/


