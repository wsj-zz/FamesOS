/******************************************************************************************
 * 文件: gui/widget/usr_wid.c
 *
 * 说明: 自定义控件类型的注册
 *
 * 作者: Jun
******************************************************************************************/
#define FAMES_GUI_USR_WIDGET_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 * 
 * 控件类型注册缓冲区
 * 
**----------------------------------------------------------------------------------*/
#define GUI_USR_WIDGET_MAX_NUM   64  /* 自定义控件的最大数量 */

static gui_widget_type   __usr_widget_buf[GUI_USR_WIDGET_MAX_NUM];

static gui_widget_type * __usr_widget_list = NULL;
static gui_widget_type * __usr_widget_free = NULL;

static int __usr_widget_id = GUI_WIDGET_USR_START_ID; /* 用于记录下一个控件类型的ID */

/*------------------------------------------------------------------------------------
 * 函数:    gui_initialize_usr_widget()
 *
 * 描述:    初始化自定义控件注册缓冲
**----------------------------------------------------------------------------------*/
void  guical gui_initialize_usr_widget(void)
{
    int i;

    CALLED_ONLY_ONCE();
    
    __usr_widget_id = GUI_WIDGET_USR_START_ID;

    __usr_widget_list = NULL;
    __usr_widget_free = NULL;

    for(i=0; i<GUI_USR_WIDGET_MAX_NUM; i++){
        __usr_widget_buf[i].widget_id = 0;
        __usr_widget_buf[i].draw = NULL;
        __usr_widget_buf[i].next = __usr_widget_free;
        __usr_widget_free = &__usr_widget_buf[i];
    }

    return;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_register_usr_widget()
 *
 * 描述:    注册一个自定义控件类型
 *
 * 参数:    usr_widget 控件类型结构(类型为gui_widget_type)
 * 
 * 返回:    自定义控件的ID号, 0代表注册失败
**----------------------------------------------------------------------------------*/
int guical gui_register_usr_widget(gui_widget_type * usr_widget)
{
    int widget_id;
    gui_widget_type *t;

    FamesAssert(usr_widget);

    if(!usr_widget)
        return 0;

    FamesAssert(usr_widget->draw);

    if(!usr_widget->draw)
        return 0;

    lock_kernel();
    
    if(__usr_widget_id >= GUI_WIDGET_USR_ENDED_ID){
        widget_id = 0;
        goto out;
    }
    if(!__usr_widget_free){
        widget_id = 0;
        goto out;
    }
    
    t = __usr_widget_free;             /* allocate */
    __usr_widget_free = t->next;

    t->widget_id = __usr_widget_id++;
    t->draw      = usr_widget->draw;

    t->next      = __usr_widget_list; /* link */
    __usr_widget_list = t;

    widget_id = t->widget_id;
    usr_widget->widget_id = widget_id;
    
    out:
    unlock_kernel();

    return widget_id;
}

/*------------------------------------------------------------------------------------
 * 函数:    gui_deregister_usr_widget()
 *
 * 描述:    注销一个自定义控件类型
 *
 * 参数:    widget_id 需注销的控件类型ID号
 * 
 * 返回:    ok/fail
**----------------------------------------------------------------------------------*/
BOOL  guical gui_deregister_usr_widget(int widget_id)
{
    BOOL retval;
    gui_widget_type **list, *t;

    FamesAssert(widget_id >= GUI_WIDGET_USR_START_ID);
    FamesAssert(widget_id <= GUI_WIDGET_USR_ENDED_ID);
    
    if(widget_id < GUI_WIDGET_USR_START_ID)
        return fail;
    if(widget_id > GUI_WIDGET_USR_ENDED_ID)
        return fail;

    lock_kernel();

    retval = fail;

    list = &__usr_widget_list;
    while(*list){
        if((*list)->widget_id == widget_id){/* found */
            t = *list;       /* save to t */
            *list = t->next;  /* unlink */
            t->widget_id = 0; /* reset */
            t->draw      = NULL;
            t->next = __usr_widget_free; /* link to free */
            __usr_widget_free = t;
            retval = ok;
            break;
        }
        list = &(*list)->next;
    }

    unlock_kernel();

    return retval;
}

/*------------------------------------------------------------------------------------
 * 函数:    __gui_usr_widget_get_property()
 *
 * 描述:    返回自定义控件的控件特性
 *
 * 参数:    c  控件指针
 *
 * 说明:    内部函数, 应用程序不可直接调用!
 *
 * FIXME:   目前始终返回GUI_WIDGET_PROP_REFRESH_DIRTY
**----------------------------------------------------------------------------------*/
INT16U __gui_usr_widget_get_property(gui_widget * c)
{
    c = c;
    return GUI_WIDGET_PROP_REFRESH_DIRTY;
}

/*------------------------------------------------------------------------------------
 * 函数:    __gui_draw_usr_widget()
 *
 * 描述:    画一个自定义控件
 *
 * 参数:    c  控件指针
 * 
 * 说明:    内部函数, 应用程序不可直接调用!
**----------------------------------------------------------------------------------*/
BOOL __internal guical __gui_draw_usr_widget(gui_widget * c)
{
    int widget_id;
    BOOL  retval;
    gui_widget_type * t;
    void (*draw)(gui_widget * c);

    FamesAssert(c);

    if(!c)
        return 0;

    retval = fail;
    draw = NULL;
    widget_id = c->type;

    if(widget_id < GUI_WIDGET_USR_START_ID)
        return retval;
    if(widget_id > GUI_WIDGET_USR_ENDED_ID)
        return retval;
    
    lock_kernel();
    t = __usr_widget_list; 
    while(t){
        if(t->widget_id == widget_id){
            draw = t->draw;
            break;
        }
        t = t->next;
    }
    unlock_kernel();

    if(draw){
        retval = ok;
        (*draw)(c);
    }
    
    return retval;
}

/*=========================================================================================
 * 
 * 本文件结束: gui/widget/usr_wid.c
 * 
**=======================================================================================*/



