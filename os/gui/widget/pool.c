/******************************************************************************************
 * �ļ�: gui/widget/pool.c
 *
 * ����: �ؼ���(�ؼ��ķ��������)
 *
 * ����: Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_POOL_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * 
 *      �ؼ�����ر�������
 * 
**---------------------------------------------------------------------------------------*/
#define  GUI_WIDGET_POOL_SIZE  128  /* һ���ؼ��صĴ�С */
#define  GUI_WIDGET_POOL_NMBR  16   /* �ؼ��صĸ���     */

static gui_widget * __WIDGET_POOL[GUI_WIDGET_POOL_NMBR] = {NULL,};
static int          __WIDGET_POOL_ALLOCATED_NUM = 0;

static gui_widget * __widget_pool_free = NULL;

/*-----------------------------------------------------------------------------------------
 * ����:    gui_widget_pool_init()
 *
 * ����:    �ؼ��س�ʼ��(�����һ������)
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
 * ����:    gui_widget_pool_free()
 *
 * ����:    �ͷ����пؼ���(ֻ��GUIģ����˳������е���)
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
 * ����:    gui_alloc_widget()
 *
 * ����:    �ӿؼ����з���һ���ؼ��ṹ
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
 * ����:    gui_free_widget()
 *
 * ����:    �ͷ�һ���ؼ��ṹ���ؼ���
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
 * ���ļ�����: gui/widget/pool.c
 * 
**=======================================================================================*/



