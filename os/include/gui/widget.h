/******************************************************************************************
 * 文件: gui/widget.h
 *
 * 说明: GUI控件定义
 *
 * 作者: Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_H
#define FAMES_GUI_WIDGET_H


/*-----------------------------------------------------------------------------------------
 * 
 *      控件结构
 * 
**---------------------------------------------------------------------------------------*/
struct gui_widget_s {
    int      type;
    INT16U   flag;
    INT16U   style;
    void   * associated;     /* associated variable     */
    INT32U   assoc_flag;
    struct gui_widget_s * next;
    struct gui_widget_s * child;
    struct gui_widget_s * father;
    RECT     rect;
    RECT     real_rect;
    RECT     inner_rect;
    COLOR    color, bkcolor;
    int      font;
    /*
    gdc_t  * gdc; //not used right now
    */
    void  (* user_draw_method)(struct gui_widget_s * c);
    void   * private_data;
};

typedef struct gui_widget_s gui_widget;

/*-----------------------------------------------------------------------------------------
 * 
 *      控件类型
 * 
**---------------------------------------------------------------------------------------*/
enum __GUI_WIDGET_TYPE {
    GUI_WIDGET_NONE = 0,
    GUI_WIDGET_DESKTOP,
    GUI_WIDGET_FORM,
    GUI_WIDGET_LABEL,
    GUI_WIDGET_EDIT,
    GUI_WIDGET_BUTTON,
    GUI_WIDGET_PROGRESS,
    GUI_WIDGET_PICTURE,
    GUI_WIDGET_GROUPBOX,
    GUI_WIDGET_VIEW,
    GUI_WIDGET_SYS_MNTR,
    GUI_WIDGET_DASHEDLINE,
    NR_GUI_WIDGET_TYPES
};

/*-----------------------------------------------------------------------------------------
 * 
 *      控件标志
 * 
**---------------------------------------------------------------------------------------*/
#define  GUI_WIDGET_FLAG_HIDE         0x0001   /* 隐藏 */
#define  GUI_WIDGET_FLAG_REFRESH      0x0002   /* 刷新 */
#define  GUI_WIDGET_FLAG_DIRTY        0x0004   /* 脏, 控件需要刷新DirtyRect */
#define  GUI_WIDGET_FLAG_AFTER_REF    0x0008   /* 刚刚刷新之后, 暂不用 */

#define  GUI_WIDGET_FLAG_NEED_REFRESH \
            (GUI_WIDGET_FLAG_REFRESH | GUI_WIDGET_FLAG_DIRTY)

/* FIXME: 此标志实现的太过粗糙, 需要仔细考虑...
 * 在很多情况下, 此标志的意义都是错的, 因为这个标志只在hide中清除,
 * 所以当一个控件被断开时, 此标志还是存在的,
 * 还有, 当一个祖先控件被hide时, 其子孙控件的该标志也仍存在,
 * 这些都是BUG, 需要重新实现这一部分.
*/
#define  GUI_WIDGET_FLAG_VISIBLE      0x0100   /* 已显示的 */

#define  GUI_WIDGET_FLAG_CHANGED      0x0200   /* 控件内容已改变 */

#define  GUI_WIDGET_FLAG_WINDOW       0x1000   /* 窗口的根控件 */

/*-----------------------------------------------------------------------------------------
 * 
 *      控件特性
 * 
**---------------------------------------------------------------------------------------*/
#define  GUI_WIDGET_PROP_NONE           0x0000
#define  GUI_WIDGET_PROP_REFRESH_DIRTY  0x0001  /* DIRTY信号来时, 控件全部重绘 */


/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_init_widget(gui_widget * c);
BOOL guical gui_show_widget(gui_widget * c);
BOOL guical gui_hide_widget(gui_widget * c);
BOOL guical gui_draw_widget(gui_widget * c);
BOOL guical gui_refresh_widget(gui_widget * c);
BOOL guical gui_set_widget_dirty(gui_widget * c, RECT * dirty_rect);
BOOL guical gui_set_widget_rect(gui_widget * c, RECT * rect);
BOOL guical gui_set_widget_location(gui_widget * c, int x, int y);
BOOL guical gui_set_widget_dimension(gui_widget * c, int width, int height);
BOOL guical gui_set_widget_changed(gui_widget * c);
BOOL guical gui_is_widget_changed(gui_widget * c);
BOOL guical gui_move_widget_up(gui_widget * c, int up);
BOOL guical gui_move_widget_down(gui_widget * c, int down);
BOOL guical gui_move_widget_left(gui_widget * c, int left);
BOOL guical gui_move_widget_right(gui_widget * c, int right);
BOOL guical gui_set_widget_wider(gui_widget * c, int wider);
BOOL guical gui_set_widget_higher(gui_widget * c, int higher);
BOOL guical gui_set_widget_color(gui_widget * c, COLOR color);
BOOL guical gui_set_widget_bkcolor(gui_widget * c, COLOR bkcolor);
BOOL guical gui_set_widget_font(gui_widget * c, int font);
BOOL guical gui_set_widget_style(gui_widget * c, INT16U style);
BOOL guical gui_set_widget_associated(gui_widget * c, void * associated, INT32U flag);
BOOL guical gui_set_widget_type(gui_widget * c, int type);
BOOL guical gui_set_widget_draw_method(gui_widget * c, void (*draw_fn)(gui_widget *));
BOOL guical gui_widget_link(gui_widget * father, gui_widget * child);
BOOL guical gui_widget_unlink(gui_widget * father, gui_widget * child);
BOOL guical gui_free_widget_tree(gui_widget * tree);

gui_widget * guical gui_create_widget( /* 创建一个控件 */
                                       int type,                   /* 类型  */
                                       int x, int y,               /* 位置  */
                                       int width, int height,      /* 大小  */
                                       COLOR color, COLOR bkcolor, /* 颜色  */
                                       int font,                   /* 字体  */
                                       INT16U style                /* 样式  */
                                     );
void guical gui_destroy_widget(gui_widget * c); /* 销毁一个控件 */

/*-----------------------------------------------------------------------------------------
 * 
 *      公共例程函数声明(gui/widget/common.c)
 * 
**---------------------------------------------------------------------------------------*/
int  guical gui_widget_draw_normal_bdr(int x, int y, int x1, int y1);
int  guical gui_widget_draw_static_bdr(int x, int y, int x1, int y1);
int  guical gui_widget_draw_subside_bdr(int x, int y, int x1, int y1);
int  guical gui_widget_draw_xp_bdr(int x, int y, int x1, int y1);
int  guical gui_widget_draw_client_bdr(int x, int y, int x1, int y1);
int  guical gui_widget_draw_modal_frame(int x, int y, int x1, int y1);
BOOL guical gui_init_rect(RECT * rect, int x, int y, int width, int height);
void guical draw_vertical_dashed_line(int x, int y, int y1, COLOR color, INT08U mask);
void guical draw_horizontal_dashed_line(int x, int y, int x1, COLOR color, INT08U mask);
void guical draw_dashed_rect(int x, int y, int x1, int y1, COLOR color, INT08U mask);
int  guical gui_widget_draw_groupbox_bdr(int x, int y, int x1, int y1);



#define  ___gui_widget_xy_move(move)  \
                    (x+=move, y+=move, x1-=move, y1-=move)



#endif /* #ifndef FAMES_GUI_WIDGET_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/widget.h
 * 
**=======================================================================================*/


