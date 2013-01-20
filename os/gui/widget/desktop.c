/******************************************************************************************
 * 文件:    gui/widget/desktop.c
 *
 * 描述:    桌面控件及窗口
 *
 * 作者:    Jun
 *
 * 时间:    2012-12-21
******************************************************************************************/
#define  FAMES_GUI_DESKTOP_C
#include "includes.h"


/*-----------------------------------------------------------------------------------------
 *
 * The 'desktop' for system, init routines, operations
 *
 * Note: the desktop defaultly is disabled, it should be enabled before being used.
 *
**---------------------------------------------------------------------------------------*/
static gui_widget * global_desktop = NULL;
static gui_window_t * global_desktop_window = NULL;

void __internal gui_desktop_init(void)
{
    global_desktop = gui_create_widget(GUI_WIDGET_DESKTOP, 0, 0, 0, 0, 
                                       COLOR_BLACK, COLOR_BLACK, 0, 0);
    if (!global_desktop)
        return;
    gui_hide_widget(global_desktop);
    gui_widget_link(NULL, global_desktop);

    global_desktop_window = gui_create_window(global_desktop);
    if (!global_desktop_window)
        return;

    gui_show_window(global_desktop_window);
}

int guical gui_desktop_enable(void)
{
    if (!global_desktop)
        return fail;

    gui_show_widget(global_desktop);

    return ok;
}

int guical gui_desktop_disable(void)
{
    if (!global_desktop)
        return fail;

    gui_hide_widget(global_desktop);

    return ok;
}

int guical gui_desktop_set_color(COLOR color)
{
    if (!global_desktop)
        return fail;

    gui_set_widget_color(global_desktop, color);
    gui_set_widget_bkcolor(global_desktop, color);

    return ok;
}

INT16U gui_desktop_get_property(gui_widget * desktop)
{
    desktop = desktop;
    return GUI_WIDGET_PROP_NONE;
}

/*-----------------------------------------------------------------------------------------
 *
 * Desktop: the default draw method
 *
**---------------------------------------------------------------------------------------*/
void  __internal gui_draw_desktop(gui_widget * c)
{
/*lint --e{534}*/
    int screen_width, screen_height;
    COLOR bkcolor;

    FamesAssert(c);

    if(!c)
        return;

    bkcolor = c->bkcolor;
    if (bkcolor==0)
        bkcolor = WIDGET_BKCOLOR;
    
    if (c->flag & GUI_WIDGET_FLAG_NEED_REFRESH) {
        if(c->style & DESKTOP_STYLE_TRANSPARENT)
            goto out;

        gdi_get_screen_size(&screen_width, &screen_height);

        gdi_draw_box(0, 0, (screen_width - 1), (screen_height - 1), bkcolor);
    }

out:
    gui_clr_widget_changed(c);
    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/desktop.c
 * 
**=======================================================================================*/


