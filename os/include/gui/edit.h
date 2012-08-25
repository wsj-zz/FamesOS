/******************************************************************************************
 * 文件: gui/widget/edit.h
 *
 * 描述: 文本编辑控件
 *
 * 作者: Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_EDIT_H
#define FAMES_GUI_WIDGET_EDIT_H


/*-----------------------------------------------------------------------------------------
 * 
 *      EDIT风格
 * 
**---------------------------------------------------------------------------------------*/
#define EDIT_STYLE_NO_BORDER        0x0001       /* 无边框     */
#define EDIT_STYLE_STATIC_BDR       0x0002       /* 静态边框   */
#define EDIT_STYLE_CLIENT_BDR       0x0004       /* CLIENT边框 */
#define EDIT_STYLE_MODAL_FRAME      0x0008       /* 模式框     */
#define EDIT_STYLE_XP_BORDER        0x0010       /* XP风格框   */
#define EDIT_STYLE_TRANSPARENT      0x0020       /* 透明       */
#define EDIT_STYLE_EDITING          0x0040       /* 正在编辑   */
#define EDIT_ALIGN_LEFT             0x0100       /* 左对齐     */
#define EDIT_ALIGN_CENTER           0x0200       /* 居中       */
#define EDIT_ALIGN_RIGHT            0x0400       /* 右对齐     */
#define EDIT_STYLE_READONLY         0x1000       /* 只读       */
#define EDIT_STYLE_PASSWORD         0x2000       /* 密码输入框 */
#define EDIT_STYLE_IN_VIEW          0x4000       /* VIEW编辑用 */

/*-----------------------------------------------------------------------------------------
 * 
 *      EDIT关联变量标志
 * 
**---------------------------------------------------------------------------------------*/
#define EDIT_ASSOC_FLAG_INT08     0x00010000    /* 关联变量是个 8位的数 */
#define EDIT_ASSOC_FLAG_INT16     0x00020000    /* 关联变量是个16位的数 */
#define EDIT_ASSOC_FLAG_INT32     0x00030000    /* 关联变量是个32位的数 */
#define EDIT_ASSOC_FLAG_INT64     0x00040000    /* 关联变量是个64位的数 */

/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_edit_init_private(gui_widget * edit, int text_len);
BOOL  guical  gui_edit_set_text(gui_widget * edit, INT08S * text);
BOOL  guical  gui_edit_get_text(gui_widget * edit, INT08S * text);
void          gui_draw_edit(gui_widget * edit);
BOOL  guical  gui_edit_blink_on(gui_widget * edit, int speed, int index, INT16U opt);
BOOL  guical  gui_edit_blink_off(gui_widget * edit, INT16U opt);
BOOL  guical  gui_edit_blink_set(gui_widget * edit, int index, INT16U opt);

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_edit_input()
 *
 * 参数:    @edit    the edit widget which will perform input
 *          @buf     the output buffer, should be of sufficient size
 *          @bytes   the max number of bytes can be entered (<=128)
 *          @option  not used currently
 *
 * 描述:    EDIT控件输入, 最多128个字符
**---------------------------------------------------------------------------------------*/
KEYCODE     gui_edit_input(gui_widget * edit, char * __BUF buf, int bytes, INT16U option);




#endif /* #ifndef FAMES_GUI_WIDGET_EDIT_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/widget/edit.h
 * 
**=======================================================================================*/


