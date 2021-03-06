/******************************************************************************************
 * 文件:    gui/widget/sys_mntr.h
 *
 * 描述:    系统监控控件SYS_MNTR(System Monitor)
 *
 * 作者:    Jun
******************************************************************************************/
#ifndef FAMES_GUI_WIDGET_SYS_MNTR_H
#define FAMES_GUI_WIDGET_SYS_MNTR_H


/*-----------------------------------------------------------------------------------------
 * 
 *      SYS_MNTR风格
 * 
**---------------------------------------------------------------------------------------*/
#define SYS_MNTR_STYLE_NO_BORDER      0x0001       /* 无边框     */
#define SYS_MNTR_STYLE_STATIC_BDR     0x0002       /* 静态边框   */
#define SYS_MNTR_STYLE_CLIENT_BDR     0x0004       /* CLIENT边框 */
#define SYS_MNTR_STYLE_MODAL_FRAME    0x0008       /* 模式框     */
#define SYS_MNTR_STYLE_TRANSPARENT    0x0010       /* 透明       */

/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
BOOL  guical  gui_sys_mntr_init_private(gui_widget * sys_mntr);
void          gui_draw_sys_mntr(gui_widget * sys_mntr);
INT16U        gui_sys_mntr_get_property(gui_widget * sys_mntr);



#endif /* #ifndef FAMES_GUI_WIDGET_SYS_MNTR_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/widget/sys_mntr.h
 * 
**=======================================================================================*/


