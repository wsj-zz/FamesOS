/******************************************************************************************
 * 文件: gui/widget/progress.h
 *
 * 描述: 进度条控件
 *
 * 作者: Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_PROGRESS_H
#define FAMES_GUI_WIDGET_PROGRESS_H

/*-----------------------------------------------------------------------------------------
 * 
 *      PROGRESS控件关联变量类型
 * 
**---------------------------------------------------------------------------------------*/
struct progress_assoc_s {
    int  full;
    int  curr;
};

typedef struct progress_assoc_s progress_assoc_t;

/*-----------------------------------------------------------------------------------------
 * 
 *      PROGRESS风格
 * 
**---------------------------------------------------------------------------------------*/
#define PROGRESS_STYLE_BORDER           0x0001       /* 边框       */
#define PROGRESS_STYLE_CLIENT_BDR       0x0002       /* CLIENT边框 */
#define PROGRESS_STYLE_MODAL_FRAME      0x0004       /* 模式框     */
#define PROGRESS_STYLE_XP_BORDER        0x0010       /* XP风格框   */
#define PROGRESS_STYLE_SMOOTH           0x0020       /* 居中       */
#define PROGRESS_ALIGN_VERTICAL         0x0100       /* 左对齐     */


/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_progress_init_private(gui_widget * progress, int full_value);
BOOL guical gui_progress_set_value(gui_widget * progress, int value);
void        gui_draw_progress(gui_widget * progress);




#endif /* #ifndef FAMES_GUI_WIDGET_PROGRESS_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/widget/progress.h
 * 
**=======================================================================================*/


