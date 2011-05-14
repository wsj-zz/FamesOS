/******************************************************************************************
 * 文件: gui/widget/pool.c
 *
 * 描述: 控件池(控件的分配与回收)
 *
 * 作者: Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_POOL_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * 
 *      控件池相关变量常量
 * 
**---------------------------------------------------------------------------------------*/
#define  GUI_WIDGET_POOL_SIZE  128  /* 一个控件池的大小 */
#define  GUI_WIDGET_POOL_NMBR  16   /* 控件池的个数     */

static gui_widget * __WIDGET_POOL[GUI_WIDGET_POOL_NMBR] = {NULL,};
static int          __WIDGET_POOL_ALLOCATED_NUM = 0;

static gui_widget * __widget_pool_free = NULL;

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_pool_init()
 *
 * 描述:    控件池初始化(分配第一个缓冲)
**---------------------------------------------------------------------------------------*/
BOOL guical __init gui_widget_pool_init(void)
{
    gui_widget * t;
    int  i;

    lock_kernel();
    for(i=0; i<GUI_WIDGET_POOL_NMBR; i++){
        __WIDGET_POOL[i] = NULL;
    }
    __widget_pool_free = NULL;
    unlock_kernel();

    t = mem_alloc((INT32U)GUI_WIDGET_POOL_SIZE*sizeof(gui_widget));

    if(!t)
        return fail;

    lock_kernel();
    __WIDGET_POOL[0] = t;
    __WIDGET_POOL_ALLOCATED_NUM = 1;
    for(i=0; i<GUI_WIDGET_POOL_SIZE; i++, t++){
        gui_init_widget(t);
        t->next = __widget_pool_free;
        __widget_pool_free = t;        
    }
    gui_global_root_widget = __widget_pool_free;
    __widget_pool_free = __widget_pool_free->next;
    gui_init_widget(gui_global_root_widget);
    gui_global_root_widget->type = GUI_WIDGET_NONE;
    unlock_kernel();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_widget_pool_free()
 *
 * 描述:    释放所有控件池(只在GUI模块的退出例程中调用)
**---------------------------------------------------------------------------------------*/
void guical __exit gui_widget_pool_free(void)
{
    int  i;
    
    if(__WIDGET_POOL_ALLOCATED_NUM <= 0)
        return;
    if(__widget_pool_free)
        gui_free_widget_tree(gui_global_root_widget);

    for(i=0; i<GUI_WIDGET_POOL_NMBR; i++){
        if(__WIDGET_POOL[i]){
            mem_free(__WIDGET_POOL[i]);
        }
    }    
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_alloc_widget()
 *
 * 描述:    从控件池中分配一个控件结构
**---------------------------------------------------------------------------------------*/
gui_widget * guical gui_alloc_widget(void)
{
    gui_widget * t = NULL;
    int  i;

    t = __widget_pool_free;

    if(t){
        lock_kernel();
        __widget_pool_free = __widget_pool_free->next;
        unlock_kernel();
    } else {
        lock_kernel();
        if(__WIDGET_POOL_ALLOCATED_NUM >= GUI_WIDGET_POOL_NMBR){
            t = NULL;
        } else {
            t = mem_alloc((INT32U)GUI_WIDGET_POOL_SIZE*sizeof(gui_widget));
            if(t){ /* allocate memory ok */
                __WIDGET_POOL[__WIDGET_POOL_ALLOCATED_NUM] = t;
                __WIDGET_POOL_ALLOCATED_NUM++;
                for(i=0; i<GUI_WIDGET_POOL_SIZE; i++, t++){
                    gui_init_widget(t);
                    t->next = __widget_pool_free;
                    __widget_pool_free = t;        
                }
                t = __widget_pool_free;
                __widget_pool_free = __widget_pool_free->next;
            } /* else t=NULL; */
        }
        unlock_kernel();
    }

    if(t)
        gui_init_widget(t);
    
    return t;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_free_widget()
 *
 * 描述:    释放一个控件结构到控件池
**---------------------------------------------------------------------------------------*/
void guical gui_free_widget(gui_widget * gc)
{
    FamesAssert(gc);

    if(!gc)
        return;

    if(gc->private_data){
        mem_free(gc->private_data);
    }

    gui_init_widget(gc);
    
    lock_kernel();
    gc->next = __widget_pool_free;
    __widget_pool_free = gc;
    unlock_kernel();

    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/pool.c
 * 
**=======================================================================================*/



