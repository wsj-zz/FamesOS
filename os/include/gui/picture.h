/******************************************************************************************
 * 文件: gui/widget/picture.h
 *
 * 描述: 图片控件
 *
 * 作者: Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_PICTURE_H
#define FAMES_GUI_WIDGET_PICTURE_H


/*-----------------------------------------------------------------------------------------
 * 
 *      PICTURE风格
 * 
**---------------------------------------------------------------------------------------*/
#define PICTURE_STYLE_NO_BORDER        0x0001       /* 无边框     */
#define PICTURE_STYLE_BORDER2          0x0002       /* 另一种边框 */
#define PICTURE_STYLE_STATIC_BDR       0x0004       /* 静态边框   */
#define PICTURE_STYLE_CLIENT_BDR       0x0008       /* CLIENT边框 */
#define PICTURE_STYLE_MODAL_FRAME      0x0010       /* 模式框     */
#define PICTURE_STYLE_TRANSPARENT      0x0020       /* 透明       */


/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_picture_init_private(gui_widget * pic);
BOOL  guical  gui_picture_set_picture(gui_widget * pic, BMPINFO * bmpinfo);
void          gui_draw_picture(gui_widget * pic);
INT16U        gui_picture_get_property(gui_widget * pic);




#endif /* #ifndef FAMES_GUI_WIDGET_PICTURE_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/widget/picture.h
 * 
**=======================================================================================*/


