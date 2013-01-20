/******************************************************************************************
 * �ļ�: gui/widget/usr_wid.c
 *
 * ˵��: �Զ���ؼ����͵�ע��
 *
 * ����: Jun
******************************************************************************************/
#define FAMES_GUI_USR_WIDGET_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 * 
 * �ؼ�����ע�Ỻ����
 * 
**----------------------------------------------------------------------------------*/
#define GUI_USR_WIDGET_MAX_NUM   64  /* �Զ���ؼ���������� */

static gui_widget_type   __usr_widget_buf[GUI_USR_WIDGET_MAX_NUM];

static gui_widget_type * __usr_widget_list = NULL;
static gui_widget_type * __usr_widget_free = NULL;

static int __usr_widget_id = GUI_WIDGET_USR_START_ID; /* ���ڼ�¼��һ���ؼ����͵�ID */

/*------------------------------------------------------------------------------------
 * ����:    gui_initialize_usr_widget()
 *
 * ����:    ��ʼ���Զ���ؼ�ע�Ỻ��
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
 * ����:    gui_register_usr_widget()
 *
 * ����:    ע��һ���Զ���ؼ�����
 *
 * ����:    usr_widget �ؼ����ͽṹ(����Ϊgui_widget_type)
 * 
 * ����:    �Զ���ؼ���ID��, 0����ע��ʧ��
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
 * ����:    gui_deregister_usr_widget()
 *
 * ����:    ע��һ���Զ���ؼ�����
 *
 * ����:    widget_id ��ע���Ŀؼ�����ID��
 * 
 * ����:    ok/fail
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
 * ����:    __gui_usr_widget_get_property()
 *
 * ����:    �����Զ���ؼ��Ŀؼ�����
 *
 * ����:    c  �ؼ�ָ��
 *
 * ˵��:    �ڲ�����, Ӧ�ó��򲻿�ֱ�ӵ���!
 *
 * FIXME:   Ŀǰʼ�շ���GUI_WIDGET_PROP_REFRESH_DIRTY
**----------------------------------------------------------------------------------*/
INT16U __gui_usr_widget_get_property(gui_widget * c)
{
    c = c;
    return GUI_WIDGET_PROP_REFRESH_DIRTY;
}

/*------------------------------------------------------------------------------------
 * ����:    __gui_draw_usr_widget()
 *
 * ����:    ��һ���Զ���ؼ�
 *
 * ����:    c  �ؼ�ָ��
 * 
 * ˵��:    �ڲ�����, Ӧ�ó��򲻿�ֱ�ӵ���!
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
 * ���ļ�����: gui/widget/usr_wid.c
 * 
**=======================================================================================*/



