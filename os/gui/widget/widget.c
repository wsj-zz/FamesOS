/******************************************************************************************
 * �ļ�: gui/widget.c
 *
 * ˵��: GUI�ؼ�����
 *
 * ����: Jun
******************************************************************************************/
#define FAMES_GUI_WIDGET_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 * 
 *      �ڲ�����
 * 
**----------------------------------------------------------------------------------*/
BOOL guical __internal __gui_draw_widget_private(gui_widget * c);
void guical __internal __gui_set_widget_realrect(gui_widget * c);


/*------------------------------------------------------------------------------------
 * ����:    gui_init_widget()
 *
 * ����:    ��ʼ��һ���ؼ�
**----------------------------------------------------------------------------------*/
BOOL guical gui_init_widget(gui_widget * c)
{
    FamesAssert(c);

    if(!c)
        return fail;

    return MEMSET((INT08S *)c, 0, sizeof(gui_widget));
}

/*------------------------------------------------------------------------------------
 * ����:    gui_show_widget()
 *
 * ����:    ��ʾһ���ؼ�(���ӿؼ�)
 *
 * ˵��:    ��gui_hide_widget()���෴�Ĳ���
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
 * ����:    gui_hide_widget()
 *
 * ����:    ����һ���ؼ�(���ӿؼ�)
 *
 * ˵��:    ������һ���ؼ���
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
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gui_draw_widget()
 *
 * ����:    ����Ļ�ϻ�һ���ؼ�(���ӿؼ�)
 *
 * ˵��:    �˺�����ʾ����ʵ������һ���ؼ�, ����һ���ؼ���
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
 * ����:    gui_refresh_widget()
 *
 * ����:    ˢ��һ���ؼ�(���ӿؼ�)
 *
 * ˵��:    ��ʵֻ�Ǽ�һ��ˢ�±�Ƕ���
**----------------------------------------------------------------------------------*/
BOOL guical gui_refresh_widget(gui_widget * c)
{
    gui_widget * t;
    
    FamesAssert(c);

    if(!c)
        return fail;

    if(c->flag & GUI_WIDGET_FLAG_HIDE)
        return ok;
    
    if(c->flag & GUI_WIDGET_FLAG_REFRESH)
        return ok;

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
 * ����:    gui_set_widget_rect()
 *
 * ����:    ����һ���ؼ���λ�����С
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
 * ����:    gui_set_widget_location()
 *
 * ����:    ����һ���ؼ���λ��
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
 * ����:    gui_set_widget_dimension()
 *
 * ����:    ����һ���ؼ��Ĵ�С
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
 * ����:    gui_move_widget_up()
 *
 * ����:    ���ؼ������ƶ�һ�ξ���
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
 * ����:    gui_move_widget_down()
 *
 * ����:    ���ؼ������ƶ�һ�ξ���
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
 * ����:    gui_move_widget_left()
 *
 * ����:    ���ؼ������ƶ�һ�ξ���
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
 * ����:    gui_move_widget_right()
 *
 * ����:    ���ؼ������ƶ�һ�ξ���
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
 * ����:    gui_set_widget_wider()
 *
 * ����:    ���ؼ�����ӿ�
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
 * ����:    gui_set_widget_higher()
 *
 * ����:    ���ؼ�����ӿ�
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
 * ����:    gui_set_widget_color()
 *
 * ����:    ���ÿؼ�ǰ��ɫ
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
 * ����:    gui_set_widget_bkcolor()
 *
 * ����:    ���ÿؼ�����ɫ
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
 * ����:    gui_set_widget_font()
 *
 * ����:    ���ÿؼ�����
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
 * ����:    gui_set_widget_style()
 *
 * ����:    ���ÿؼ����
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
 * ����:    gui_set_widget_associated()
 *
 * ����:    ���ÿؼ���������
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
 * ����:    gui_set_widget_type()
 *
 * ����:    ���ÿؼ�����
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
 * ����:    gui_widget_link()
 *
 * ����:    ��һ���ؼ������ӵ���һ���ؼ�����
**----------------------------------------------------------------------------------*/
BOOL guical gui_widget_link(gui_widget * father, gui_widget * child)
{
    FamesAssert(child);

    if(!child)
        return fail;

    if(!father)
        father = gui_global_root_widget;

    lock_kernel();
    child->father = father;
    child->next   = NULL;
    child->next = father->child;
    father->child = child;
    __gui_set_widget_realrect(child);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gui_widget_unlink()
 *
 * ����:    ��һ���ؼ���ɾ��һ���ӿؼ���
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
 * ����:    gui_widget_unlink()
 *
 * ����:    �ͷ�һ���ؼ������ؼ�����
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
        gui_free_widget(t);
        t = tt;
    }
    gui_free_widget(tree);
    unlock_kernel();

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    gui_create_widget()
 *
 * ����:    ����һ���ؼ�
 *
 * ����:    �´����Ŀؼ�ָ��/NULL
**----------------------------------------------------------------------------------*/
gui_widget * guical gui_create_widget( 
                                       int type,                   /* ����  */
                                       int x, int y,               /* λ��  */
                                       int width, int height,      /* ��С  */
                                       COLOR color, COLOR bkcolor, /* ��ɫ  */
                                       int font,                   /* ����  */
                                       INT16U style                /* ��ʽ  */
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

    return c;
}

/*------------------------------------------------------------------------------------
 * ����:    gui_destroy_widget()
 *
 * ����:    ����һ���ؼ�
 *
 * ����:    Ŀ��ؼ���ָ��
**----------------------------------------------------------------------------------*/
void guical gui_destroy_widget(gui_widget * c)
{
    gui_free_widget(c);
}

/*------------------------------------------------------------------------------------
 * ����:    __gui_draw_widget_private()
 *
 * ����:    ��ʾһ���ض��Ŀؼ�
**----------------------------------------------------------------------------------*/
BOOL guical __internal __gui_draw_widget_private(gui_widget * c)
{
    FamesAssert(c);

    if(!c)
        return fail;

    switch(c->type){
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
        case GUI_WIDGET_NONE:
            break;
        default:
            if(!__gui_draw_usr_widget(c)){
                ;/* do something here */
            }
            break;
    }
    c->flag &= ~GUI_WIDGET_FLAG_REFRESH;

    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    __gui_set_widget_realrect()
 *
 * ����:    ����һ���ؼ����и��ؼ��ľ���λ��
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
 * ���ļ�����: gui/widget.c
 * 
**=======================================================================================*/



