/******************************************************************************************
 * 文件: gui/widget.c
 *
 * 说明: GUI控件定义
 *
 * 作者: Jun
******************************************************************************************/
#define FAMES_GUI_WIDGET_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 * 
 *      内部函数
 * 
**----------------------------------------------------------------------------------*/
BOOL guical __internal __gui_draw_widget_private(gui_widget * c);
void guical __internal __gui_set_widget_realrect(gui_widget * c);


/*------------------------------------------------------------------------------------
 * 函数:    gui_init_widget()
 *
 * 描述:    初始化一个控件
**----------------------------------------------------------------------------------*/
BOOL guical gui_init_widget(gui_widget * c)
{
    FamesAssert(c);

    if(!c)
        return fail;

    return MEMSET((INT08S *)c, 0, sizeof(gui_widget));
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_show_widget()
 *
 * 描述:    显示一个控件(及子控件)
 *
 * 说明:    与gui_hide_widget()是相反的操作
**----------------------------------------------------------------------------------*/
BOOL guical gui_show_widget(gui_widget * c)
{
    FamesAssert(c);

    if(!c)
        return fail;
    
    lock_kernel();
    if(c->flag & GUI_WIDGET_FLAG_HIDE){
        c->flag &= ~GUI_WIDGET_FLAG_HIDE;
        gui_refresh_widget(c);
    }
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_hide_widget()
 *
 * 描述:    隐藏一个控件(及子控件)
 *
 * 说明:    隐藏了一个控件树
**----------------------------------------------------------------------------------*/
BOOL guical gui_hide_widget(gui_widget * c)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    if(c->flag & GUI_WIDGET_FLAG_HIDE){
        ;
    } else {
        c->flag |= GUI_WIDGET_FLAG_HIDE;
        if(c->father){
            #if 0
            gui_refresh_widget(c->father);
            #endif
        }
    }
    c->flag &= ~GUI_WIDGET_FLAG_VISIBLE;
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_draw_widget()
 *
 * 描述:    在屏幕上画一个控件(及子控件)
 *
 * 说明:    此函数显示的其实并不是一个控件, 而是一个控件树
**----------------------------------------------------------------------------------*/
BOOL guical gui_draw_widget(gui_widget * c)
{
    gui_widget * t;
    BOOL retval;

    FamesAssert(c);

    if(!c)
        return fail;

    if(c->flag & GUI_WIDGET_FLAG_HIDE)
        return ok;
    
    lock_kernel();
    retval = ok;
    if(__gui_draw_widget_private(c)){
        t = c->child;
        while(t){
            if(!gui_draw_widget(t)){;}
            t = t->next;
        }
    } else {
        retval = fail;
    }
    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_refresh_widget()
 *
 * 描述:    刷新一个控件(及子控件)
 *
 * 说明:    其实只是加一个刷新标记而已
**----------------------------------------------------------------------------------*/
BOOL guical gui_refresh_widget(gui_widget * c)
{
    gui_widget * t;
    
    FamesAssert(c);

    if(!c)
        return fail;

    if(c->flag & GUI_WIDGET_FLAG_HIDE)
        return ok;

    #if 0
    if(c->flag & GUI_WIDGET_FLAG_REFRESH)
        return ok;
    #endif

    lock_kernel();
    c->flag |= GUI_WIDGET_FLAG_REFRESH;
    t = c->child;
    while(t){
        if(!gui_refresh_widget(t)){;}
        t = t->next;
    }
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_dirty()
 *
 * 描述:    标记一个控件已被弄脏(及子控件)
 *
 * 参数:    @dirty_rect: NULL=从myself_window计算dirty_rect
 *
 * 说明:    其实只是加一个脏标记而已
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_dirty(gui_widget * c, RECT * dirty_rect)
{
    gui_widget * t;
    gui_window_t * win;

    FamesAssert(c);

    if(!c)
        return fail;

    if(c->flag & GUI_WIDGET_FLAG_HIDE)
        return ok;

    lock_kernel();
    if (!dirty_rect) {
        win = gui_find_window_from_widget(c);
        if (!win)
            goto out;
        dirty_rect = &win->dirty_rect;
    }
    /* 看我们的位置与dirty_rect是否有重叠的地方 */
    if (!gdc_is_rect_intersect(&c->real_rect, dirty_rect))
        goto out;
    c->flag |= GUI_WIDGET_FLAG_DIRTY;
    t = c->child;
    while(t){
        if(!gui_set_widget_dirty(t, dirty_rect)){;}
        t = t->next;
    }
out:
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_rect()
 *
 * 描述:    设置一个控件的位置与大小
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_rect(gui_widget * c, RECT * rect)
{
    FamesAssert(c);
    FamesAssert(rect);

    if(!c || !rect)
        return fail;

    lock_kernel();
    *(&c->rect) = * rect;
    __gui_set_widget_realrect(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_location()
 *
 * 描述:    设置一个控件的位置
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_location(gui_widget * c, int x, int y)
{
    FamesAssert(c);
    if(!c)
        return fail;

    lock_kernel();
    c->rect.x = x;
    c->rect.y = y;
    __gui_set_widget_realrect(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_dimension()
 *
 * 描述:    设置一个控件的大小
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_dimension(gui_widget * c, int width, int height)
{
    FamesAssert(c);
    if(!c)
        return fail;

    lock_kernel();
    c->rect.width = width;
    c->rect.height = height;
    __gui_set_widget_realrect(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_changed()
 *
 * 描述:    标识一个控件的内容已被改变
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_changed(gui_widget * c)
{
    FamesAssert(c);
    if(!c)
        return fail;

    lock_kernel();
    c->flag |= GUI_WIDGET_FLAG_CHANGED;
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_clr_widget_changed()
 *
 * 描述:    清除控件的GUI_WIDGET_FLAG_CHANGED标志
**----------------------------------------------------------------------------------*/
BOOL guical gui_clr_widget_changed(gui_widget * c)
{
    FamesAssert(c);
    if(!c)
        return fail;

    lock_kernel();
    c->flag &= ~GUI_WIDGET_FLAG_CHANGED;
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_is_widget_changed()
 *
 * 描述:    检查一个控件的内容是否已被改变
**----------------------------------------------------------------------------------*/
BOOL guical gui_is_widget_changed(gui_widget * c)
{
    BOOL ret = NO;

    FamesAssert(c);
    if(!c)
        return fail;

    lock_kernel();
    ret = (c->flag & GUI_WIDGET_FLAG_CHANGED);
    unlock_kernel();

    return ret;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_move_widget_up()
 *
 * 描述:    将控件向上移动一段距离
**----------------------------------------------------------------------------------*/
BOOL guical gui_move_widget_up(gui_widget * c, int up)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->rect.y -= up;
    __gui_set_widget_realrect(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_move_widget_down()
 *
 * 描述:    将控件向下移动一段距离
**----------------------------------------------------------------------------------*/
BOOL guical gui_move_widget_down(gui_widget * c, int down)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->rect.y += down;
    __gui_set_widget_realrect(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_move_widget_left()
 *
 * 描述:    将控件向左移动一段距离
**----------------------------------------------------------------------------------*/
BOOL guical gui_move_widget_left(gui_widget * c, int left)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->rect.x -= left;
    __gui_set_widget_realrect(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_move_widget_right()
 *
 * 描述:    将控件向右移动一段距离
**----------------------------------------------------------------------------------*/
BOOL guical gui_move_widget_right(gui_widget * c, int right)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->rect.x += right;
    __gui_set_widget_realrect(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_wider()
 *
 * 描述:    将控件横向加宽
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_wider(gui_widget * c, int wider)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->rect.width += wider;
    __gui_set_widget_realrect(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_higher()
 *
 * 描述:    将控件纵向加宽
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_higher(gui_widget * c, int higher)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->rect.height += higher;
    __gui_set_widget_realrect(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_color()
 *
 * 描述:    设置控件前景色
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_color(gui_widget * c, COLOR color)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->color = color;
    gui_refresh_widget(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_bkcolor()
 *
 * 描述:    设置控件背景色
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_bkcolor(gui_widget * c, COLOR bkcolor)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->bkcolor = bkcolor;
    gui_refresh_widget(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_font()
 *
 * 描述:    设置控件字体
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_font(gui_widget * c, int font)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->font = font;
    gui_refresh_widget(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_style()
 *
 * 描述:    设置控件风格
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_style(gui_widget * c, INT16U style)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->style = style;
    gui_refresh_widget(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_associated()
 *
 * 描述:    设置控件关联变量
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_associated(gui_widget * c, 
                                       void * associated, INT32U flag)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->associated = associated;
    c->assoc_flag = flag;
    gui_refresh_widget(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_type()
 *
 * 描述:    设置控件类型
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_type(gui_widget * c, int type)
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->type = type;
    gui_refresh_widget(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_set_widget_draw_method()
 *
 * 描述:    设置控件的user_draw_method
 *
 * 参数:    draw_fn: 用户指定的画图方法, NULL为使用默认的画图方法
**----------------------------------------------------------------------------------*/
BOOL guical gui_set_widget_draw_method(gui_widget * c, void (*draw_fn)(gui_widget *))
{
    FamesAssert(c);

    if(!c)
        return fail;

    lock_kernel();
    c->user_draw_method = draw_fn;
    gui_refresh_widget(c);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_widget_link()
 *
 * 描述:    将一个控件树连接到另一个控件树中
 *
 * 历史:    此函数最初是将child链到了father的首结点位置上(队列头),
 *          而后将其修正为链到队列尾(其实本来就应该是这样子的) -- 2013/2/19
**----------------------------------------------------------------------------------*/
BOOL guical gui_widget_link(gui_widget * father, gui_widget * child)
{
    gui_widget ** t;

    FamesAssert(child);

    if(!child)
        return fail;

    if(!father)
        father = gui_global_root_widget;

    lock_kernel();
    child->father = father;
    child->next   = NULL;
    t = &(father->child);
    while(*t){
        if(*t == child){
            unlock_kernel();
            return fail; /* 队列中已经存在了, 不需要再入队 */
        }
        t = &(*t)->next;
    }
    *t = child;
    __gui_set_widget_realrect(child);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_widget_unlink()
 *
 * 描述:    从一个控件树删除一个子控件树
**----------------------------------------------------------------------------------*/
BOOL guical gui_widget_unlink(gui_widget * father, gui_widget * child)
{
    gui_widget ** t;
    
    FamesAssert(child);

    if(!child)
        return fail;

    if(!father)
        father = gui_global_root_widget;

    lock_kernel();
    t = &(father->child);
    while(*t){
        if(*t == child){
            *t = child->next;
            child->next = NULL;
            child->father = NULL;
            break;
        }
        t = &(*t)->next;
    }
    #if 0
    gui_refresh_widget(father);
    #endif
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_widget_unlink()
 *
 * 描述:    释放一个控件树到控件池中
**----------------------------------------------------------------------------------*/
BOOL guical gui_free_widget_tree(gui_widget * tree)
{
    gui_widget * t, * tt;

    FamesAssert(tree);

    if(!tree)
        return fail;

    lock_kernel();
    t = tree->child;
    while(t){
        tt = t->next;
        gui_free_widget_tree(t);
        t = tt;
    }
    gui_free_widget(tree);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_create_widget()
 *
 * 描述:    创建一个控件
 *
 * 返回:    新创建的控件指针/NULL
**----------------------------------------------------------------------------------*/
gui_widget * guical gui_create_widget( 
                                       int type,                   /* 类型  */
                                       int x, int y,               /* 位置  */
                                       int width, int height,      /* 大小  */
                                       COLOR color, COLOR bkcolor, /* 颜色  */
                                       int font,                   /* 字体  */
                                       INT16U style                /* 样式  */
                                     )
{
/*lint --e{534}*/
    gui_widget * c;

    c = gui_alloc_widget();

    if(!c)
        return c;

    if(x < 0)x=0;
    if(y < 0)y=0;
    if(width < 0)width=0;
    if(height < 0)height=0;

    gui_set_widget_type(c, type);
    gui_set_widget_location(c, x, y);
    gui_set_widget_dimension(c, width, height);
    gui_set_widget_color(c, color);
    gui_set_widget_bkcolor(c, bkcolor);
    gui_set_widget_font(c, font);
    gui_set_widget_style(c, style);
    gui_set_widget_draw_method(c, NULL);

    if (type == GUI_WIDGET_DESKTOP) { /* 桌面特殊了一点: 其位置及大小是确定的 */
        int scr_w, scr_h;
        gdi_get_screen_size(&scr_w, &scr_h);
        gui_set_widget_location(c, 0, 0);
        gui_set_widget_dimension(c, scr_w, scr_h);
    }

    return c;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_destroy_widget()
 *
 * 描述:    销毁一个控件
 *
 * 参数:    目标控件的指针
**----------------------------------------------------------------------------------*/
void guical gui_destroy_widget(gui_widget * c)
{
    gui_free_widget(c);
}

/*------------------------------------------------------------------------------------
 * 函数:    __gui_widget_get_property()
 *
 * 描述:    返回一个控件的特性
**----------------------------------------------------------------------------------*/
INT16U guical __internal __gui_widget_get_property(gui_widget * c)
{
    INT16U property = 0;

    FamesAssert(c);
    if (!c)
        return 0;

    switch (c->type) {
        case GUI_WIDGET_FORM:
            property = gui_form_get_property(c);
            break;
        case GUI_WIDGET_LABEL:
            property = gui_label_get_property(c);
            break;
        case GUI_WIDGET_EDIT:
            property = gui_edit_get_property(c);
            break;
        case GUI_WIDGET_BUTTON:
            property = gui_button_get_property(c);
            break;
        case GUI_WIDGET_PROGRESS:
            property = gui_progress_get_property(c);
            break;
        case GUI_WIDGET_PICTURE:
            property = gui_picture_get_property(c);
            break;
        case GUI_WIDGET_GROUPBOX:
            property = gui_groupbox_get_property(c);
            break;
        case GUI_WIDGET_VIEW:
            property = gui_view_get_property(c);
            break;
        case GUI_WIDGET_SYS_MNTR:
            property = gui_sys_mntr_get_property(c);
            break;
        case GUI_WIDGET_DASHEDLINE:
            property = gui_dashedline_get_property(c);
            break;
        case GUI_WIDGET_DESKTOP:
            property = gui_desktop_get_property(c);
            break;
        case GUI_WIDGET_NONE:
            break;
        default:
            property = __gui_usr_widget_get_property(c);
            break;
    }

    return property;
}

/*------------------------------------------------------------------------------------
 * 函数:    __gui_draw_widget_private()
 *
 * 描述:    显示一个特定的控件
**----------------------------------------------------------------------------------*/
BOOL guical __internal __gui_draw_widget_private(gui_widget * c)
{
    int has_dirty = NO;
    INT16U property;
    gui_window_t * win = NULL;

    FamesAssert(c);
    if (!c)
        return fail;

    #if 0
    if (c->flag & GUI_WIDGET_FLAG_REFRESH &&
        c->flag & GUI_WIDGET_FLAG_DIRTY) {
        printf("FLAG_REFRESH and FLAG_DIRTY both appear\n");
    }
    #endif

    if (c->flag & GUI_WIDGET_FLAG_DIRTY) {
        property = __gui_widget_get_property(c);
        if (property & GUI_WIDGET_PROP_REFRESH_DIRTY) {
            gui_refresh_widget(c); /* 需要刷新自己的控件树 */
        }
    }

    if (c->flag & GUI_WIDGET_FLAG_REFRESH ||
        !(c->flag & GUI_WIDGET_FLAG_DIRTY)) {
        win = gdc_get_myself_window();
        if (win) {
            has_dirty = gui_window_dirty_mask(win);
        }
    }

    if (c->user_draw_method) {
        (*c->user_draw_method)(c);
        goto draw_ok;
    }

    switch (c->type) {
        case GUI_WIDGET_FORM:
            gui_draw_form(c);
            break;
        case GUI_WIDGET_LABEL:
            gui_draw_label(c);
            break;
        case GUI_WIDGET_EDIT:
            gui_draw_edit(c);
            break;
        case GUI_WIDGET_BUTTON:
            gui_draw_button(c);
            break;            
        case GUI_WIDGET_PROGRESS:
            gui_draw_progress(c);
            break;
        case GUI_WIDGET_PICTURE:
            gui_draw_picture(c);
            break;
        case GUI_WIDGET_GROUPBOX:
            gui_draw_groupbox(c);
            break;
        case GUI_WIDGET_VIEW:
            gui_draw_view(c);
            break;
        case GUI_WIDGET_SYS_MNTR:
            gui_draw_sys_mntr(c);
            break;
        case GUI_WIDGET_DASHEDLINE:
            gui_draw_dashedline(c);
            break;
        case GUI_WIDGET_DESKTOP:
            gui_draw_desktop(c);
            break;
        case GUI_WIDGET_NONE:
            break;
        default:
            if (!__gui_draw_usr_widget(c)) {
                ;/* do something here */
            }
            break;
    }

draw_ok:
    if (win) {
        gui_window_dirty_unmask(win, has_dirty);
    }
    if (c->flag & GUI_WIDGET_FLAG_REFRESH) {
        c->flag |= GUI_WIDGET_FLAG_AFTER_REFRESH;
    } else {
        c->flag &= ~GUI_WIDGET_FLAG_AFTER_REFRESH;
    }
    c->flag &= ~GUI_WIDGET_FLAG_REFRESH;
    c->flag &= ~GUI_WIDGET_FLAG_DIRTY;
    c->flag |=  GUI_WIDGET_FLAG_VISIBLE;

    return ok;
}

/*------------------------------------------------------------------------------------
 * 函数:    __gui_set_widget_realrect()
 *
 * 描述:    计算一个控件树中各控件的绝对位置
**----------------------------------------------------------------------------------*/
void guical __internal __gui_set_widget_realrect(gui_widget * c)
{
    FamesAssert(c);

    if(!c)
        return;

    lock_kernel();
    if(c->rect.height < 0)
        c->rect.height = 0;
    if(c->rect.width  < 0)
        c->rect.width  = 0;
    c->real_rect = c->rect;
    c->inner_rect = c->rect;
    if(c->father){
        c->real_rect.x += c->father->real_rect.x;
        c->real_rect.y += c->father->real_rect.y;
        c->inner_rect = c->real_rect;
    }
    gui_refresh_widget(c);
    c = c->child;
    while(c){
        __gui_set_widget_realrect(c);
        c = c->next;
    }
    unlock_kernel();

    return;
}

/*=========================================================================================
 * 
 * 本文件结束: gui/widget.c
 * 
**=======================================================================================*/



