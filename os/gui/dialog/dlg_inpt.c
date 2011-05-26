/******************************************************************************************
 * �ļ�:    gui/dialog/dlg_inpt.c
 *
 * ˵��:    ����Ի���
 *
 * ����:    Jun
 *
 * ʱ��:    2011-2-28
******************************************************************************************/
#define  FAMES_GUI_DIALOG_INPUT_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * ����:    input_dialog_initialize()
 *
 * ����:    ����Ի����ʼ��(����)
 *
 * ����:    input_dialog �Ի��������ṹ(����ǰ����)
 *
 * ����:    form  �Ի����������ָ��
**---------------------------------------------------------------------------------------*/
gui_widget * input_dialog_initialize(dialog_input_header * input_dialog)
{
    dialog_input_item * item;
    gui_widget * widget;
    gui_widget * form;
    int x, y, width, height, font, __t;
    COLOR color, bkcolor;
    
    FamesAssert(input_dialog);
    if(!input_dialog)
        return NULL;
    
    FamesAssert(input_dialog->items);
    
    item = input_dialog->items;
    if(!item)
        return NULL;

    input_dialog->current_index = 0;
    input_dialog->ok_comment = NULL;
    input_dialog->cancel_comment = NULL;

    x = input_dialog->x;
    y = input_dialog->y;
    width = input_dialog->width;
    height = input_dialog->height;
    font = input_dialog->font;
    color = input_dialog->color;
    bkcolor = input_dialog->bkcolor;

    /* ������ */
    widget = gui_create_widget(GUI_WIDGET_FORM, x, y, width, height, color, bkcolor, font, input_dialog->style);
    form = widget;
    if(!widget)
        goto some_error;
    input_dialog->main_form = widget;
    gui_form_init_private(widget, 128);
    if(input_dialog->icon){
        gui_form_set_icon(widget, input_dialog->icon);
    }
    if(input_dialog->dialog_name){
        gui_form_set_caption(widget, input_dialog->dialog_name);
    }
    
    /* ������ */
    widget = gui_create_widget(GUI_WIDGET_LABEL, 5, (height-35), (width-11), 30, color, bkcolor, font, LABEL_STYLE_CLIENT_BDR);
    if(!widget)
        goto some_error;
    input_dialog->status_bar = widget;
    gui_widget_link(form, widget);
    gui_label_init_private(widget, 128);
    gui_label_set_text(widget, "");
    height -= 38;
    
    /* OK��ť */
    __t  = (width-160);
    __t /= 3;
    widget = gui_create_widget(GUI_WIDGET_BUTTON, (__t-20), (height-36), 128, 30, 0, 0, font, 0);
    if(!widget)
        goto some_error;
    input_dialog->ok_button = widget;
    gui_widget_link(form, widget);
    gui_button_init_private(widget, 32);
    gui_button_set_caption(widget, "ȷ��[F10]");

    /* CANCEL��ť */
    __t += 128;
    widget = gui_create_widget(GUI_WIDGET_BUTTON, (width-__t), (height-36), 128, 30, 0, 0, font, 0);
    if(!widget)
        goto some_error;
    input_dialog->cancel_button = widget;
    gui_widget_link(form, widget);
    gui_button_init_private(widget, 32);
    gui_button_set_caption(widget, "ȡ��[ESC]");
    height -= 40;

    /* �༭��Ŀ */
    while(item->caption){
        int __len;
        x = item->x;
        y = item->y;
        width = item->width;
        height = item->height;
        font = item->font;
        switch(item->type){
            case INPUT_DIALOG_ITEM_TYPE_EDIT:
                __len = STRLEN(item->caption)*8; /* ����Ϊһ���ַ�ռ8������, ��Ҫʱ���޸� */
                __len += 6;
                widget = gui_create_widget(GUI_WIDGET_LABEL, x, y, __len, height, color, bkcolor, font, LABEL_STYLE_TRANSPARENT);
                if(!widget)
                    goto some_error;
                item->label = widget;
                gui_label_init_private(widget, (STRLEN(item->caption)+8));
                gui_label_set_text(widget, item->caption);
                gui_widget_link(form, widget);
                widget = gui_create_widget(GUI_WIDGET_EDIT,  (x+__len), y, (width-__len), height, 0, 0, font, item->style);
                if(!widget)
                    goto some_error;
                item->edit = widget;
                gui_edit_init_private(widget, (item->bytes+16));
                gui_edit_set_text(widget, "");
                gui_widget_link(form, widget);
                break;
            case INPUT_DIALOG_ITEM_TYPE_GROUP:
                widget = gui_create_widget(GUI_WIDGET_GROUPBOX, x, y, width, height, color, bkcolor, font, item->style);
                if(!widget)
                    goto some_error;
                gui_groupbox_init_private(widget, 64);
                gui_groupbox_set_caption(widget, item->caption);
                gui_widget_link(form, widget);
                break;
            default:
                break;
        }
        item++;
    }

    return form;

some_error:
    sys_print("input_dialog_initialize(): failed to create widgets!\n");
    ExitApplication();
    return NULL;
}

/*-----------------------------------------------------------------------------------------
 * ����:    __waitkey_for_ok_cancel()
 *
 * ����:    �ȴ�����, ����OK/CANCEL��ť
**---------------------------------------------------------------------------------------*/
static KEYCODE __waitkey_for_ok_cancel(INT32U timeout)
{
    KEYCODE key;
    int  wait = 1;

    do {
        key = waitkey(timeout);
        switch(key){
            case ENTER:
            case ESC:
            case F10:
            case TAB:
            case LEFT:
            case RIGHT:
            case UP:
            case DOWN:
            case SHIFT_TAB:                
                wait = 0;
                break;
            default:
                break;
        }
    } while(wait);

    return key;
}

/*-----------------------------------------------------------------------------------------
 * ����:    input_dialog_method()
 *
 * ����:    ����Ի��򷽷�(�������)
**---------------------------------------------------------------------------------------*/
int input_dialog_method(dialog_input_header * input_dialog, 
                        void (*prepare)(int id, char * buf, void * data, INT16U opt),
                        int  (*finish)(int id, char *buf, void * data, KEYCODE key),
                        void * data, int keep_index)
{
    dialog_input_item * item, * t;
    char buf[256];
    int  loop, number, index;
    int  retval, __v;
    KEYCODE key;

    FamesAssert(input_dialog);
    FamesAssert(finish);

    if(!input_dialog || !finish)
        return fail;

    FamesAssert(input_dialog->items);
    item = input_dialog->items;
    if(!item)
        return fail;

    number = 0;
    buf[0] = 0;
    while(item->caption){
        number++;
        if(prepare && (item->type == INPUT_DIALOG_ITEM_TYPE_EDIT)){
            prepare(item->id, buf, data, 0);
            gui_edit_set_text(item->edit, buf);
        } else {
          /* �������ִ���������, Ŀǰ����Ҫ! - Jun, 3-18-2011 */
        }
        item++;
    }

    key = ENTER; /* �����һ��ITEM��GROUP, ��ô�����(ENTER)���Խ�֮���� */

    if(keep_index)
        index = input_dialog->current_index;
    else
        index = 0;

    item = input_dialog->items;
    for(loop = 1; loop;){
        retval = fail;
        if(index < 0)
            index = 0;
        if((index < number) && (item[index].type != INPUT_DIALOG_ITEM_TYPE_EDIT)){
            switch(key){
                case SHIFT_TAB:
                case UP:
                case LEFT:
                case CTRL_END:
                    key = UP;
                    break;
                default:
                    key = ENTER;
                    break;
            }
        } else if(index < number){
            t = &item[index];
            gui_edit_get_text(t->edit, buf);
            if(prepare){
                prepare(t->id, buf, data, INPUT_DIALOG_PREPARE_OPT_EDITING);
            }
            if(t->comment && input_dialog->status_bar){
                gui_label_set_text(input_dialog->status_bar, t->comment);
            }
            input_again:
            key = gui_edit_input(t->edit, buf, t->bytes, 0);
            if(key == ESC){ /* ȡ������ */
                retval = fail;
                input_dialog->current_index = index;
                break;
            }
            __v = finish(t->id, buf, data, key);
            if(__v == 0) /* ������Ч�Ҳ�ȡ���������, �Ǿ�ֻ���������� */
                goto input_again;
            if(__v > 0){ /* ������ȷ, ������ʾһ�� */
                gui_edit_get_text(t->edit, buf);
                if(prepare && (t->type == INPUT_DIALOG_ITEM_TYPE_EDIT)){
                    prepare(t->id, buf, data, 0);
                    gui_edit_set_text(t->edit, buf);
                }
            }
            if(__v < 0){ /* ��ˢ������Ԫ�� */
                t = item;
                while(t->caption){
                    if(prepare && (t->type == INPUT_DIALOG_ITEM_TYPE_EDIT)){
                        prepare(t->id, buf, data, 0);
                        gui_edit_set_text(t->edit, buf);
                    }
                    t++;
                }
            }
        } else if(index == number){
            gui_set_widget_style(input_dialog->ok_button, BUTTON_STYLE_PRESSED);
            if(input_dialog->status_bar){
                char * s = input_dialog->ok_comment;
                gui_label_set_text(input_dialog->status_bar, (s?s:" -- F10 --"));
            }
            key = __waitkey_for_ok_cancel(0L);
            if(key == ENTER){
                loop = 0;
                retval = ok;
                input_dialog->current_index = 0;
            }
            gui_set_widget_style(input_dialog->ok_button, 0);
        } else if(index == (number+1)){
            gui_set_widget_style(input_dialog->cancel_button, BUTTON_STYLE_PRESSED);
            if(input_dialog->status_bar){
                char * s = input_dialog->cancel_comment;
                gui_label_set_text(input_dialog->status_bar, (s?s:" -- ESC --"));
            }
            key = __waitkey_for_ok_cancel(0L);
            if(key == ENTER){
                loop = 0;
                retval = fail;
                input_dialog->current_index = 0;
            }
            gui_set_widget_style(input_dialog->cancel_button, 0);
        }
        switch(key){
            case TAB:
            case ENTER:
            case DOWN:
            case RIGHT:
                index++;
                if(index > (number+1))
                    index = 0;
                break;
            case SHIFT_TAB:
            case UP:
            case LEFT:
                index--;
                if(index < 0)
                    index = (number-1); /* ��ǰ��ʱ, ��������ť */
                break;
            case CTRL_HOME:
                index=0;
                break;
            case CTRL_END:
                index = (number-1);
                break;
            case F10:
                loop = 0;
                retval = 1;
                input_dialog->current_index = index;
                break;
            case ESC:
                loop = 0;
                retval = fail;
                input_dialog->current_index = index;
                break;
            default:
                break;
        }
    }

    return retval;
}

/*-----------------------------------------------------------------------------------------
 * ����:    input_dialog_alert()
 *
 * ����:    ���뾯��
**---------------------------------------------------------------------------------------*/
void input_dialog_alert(dialog_input_header * input_dialog, char * s, COLOR alert_color)
{
    FamesAssert(input_dialog);
    FamesAssert(s);
    if(!input_dialog || !s)
        return;
    if(alert_color == 0)
        alert_color = COLOR_WARNING;
    gui_set_widget_bkcolor(input_dialog->status_bar, alert_color);
    gui_set_widget_color(input_dialog->status_bar, 0);
    gui_label_set_text(input_dialog->status_bar, s);
    waitkey(1300L);
    gui_set_widget_color(input_dialog->status_bar, 0);    
    gui_set_widget_bkcolor(input_dialog->status_bar, input_dialog->bkcolor);    
}

/*-----------------------------------------------------------------------------------------
 * ����:    input_dialog_preset_title()
 *
 * ����:    Ԥ���öԻ������
**---------------------------------------------------------------------------------------*/
void input_dialog_preset_title(dialog_input_header * input_dialog, char * title)
{
    FamesAssert(input_dialog);

    if(!input_dialog || !title)
        return;

    input_dialog->dialog_name = title;
}

/*-----------------------------------------------------------------------------------------
 * ����:    input_dialog_preset_item_name()
 *
 * ����:    Ԥ���öԻ���ĳһԪ����
**---------------------------------------------------------------------------------------*/
void input_dialog_preset_item_name(dialog_input_header * input_dialog, int id, char * name)
{
    dialog_input_item * item;

    FamesAssert(input_dialog);

    if(!input_dialog || !name)
        return;

    item = input_dialog->items;

    if(!item)
        return;

    while(item->caption){
        if(item->id == id){
            item->caption = name;
        }
        item++;
    }
}

/*-----------------------------------------------------------------------------------------
 * ����:    input_dialog_preset_item_comment()
 *
 * ����:    Ԥ���öԻ���ĳһԪ�ص�ע����Ϣ
**---------------------------------------------------------------------------------------*/
void input_dialog_preset_item_comment(dialog_input_header * input_dialog, int id, char * comment)
{
    dialog_input_item * item;

    FamesAssert(input_dialog);

    if(!input_dialog || !comment)
        return;

    item = input_dialog->items;

    if(!item)
        return;

    while(item->caption){
        if(item->id == id){
            item->comment = comment;
        }
        item++;
    }
}

/*-----------------------------------------------------------------------------------------
 * ����:    input_dialog_set_ok_comment()
 *
 * ����:    ���öԻ�����OK��ť��ע����Ϣ
**---------------------------------------------------------------------------------------*/
void input_dialog_set_ok_comment(dialog_input_header * input_dialog, char * ok_comment)
{
    FamesAssert(input_dialog);

    if(!input_dialog || !ok_comment)
        return;

    lock_kernel();
    input_dialog->ok_comment = ok_comment;
    unlock_kernel();
}

/*-----------------------------------------------------------------------------------------
 * ����:    input_dialog_set_cancel_comment()
 *
 * ����:    ���öԻ�����CANCEL��ť��ע����Ϣ
**---------------------------------------------------------------------------------------*/
void input_dialog_set_cancel_comment(dialog_input_header * input_dialog, char * cancel_comment)
{
    FamesAssert(input_dialog);

    if(!input_dialog || !cancel_comment)
        return;

    lock_kernel();
    input_dialog->cancel_comment = cancel_comment;
    unlock_kernel();
}

/*-----------------------------------------------------------------------------------------
 * ����:    input_dialog_set_buttons_caption()
 *
 * ����:    ���öԻ���ť����
**---------------------------------------------------------------------------------------*/
void input_dialog_set_buttons_caption(dialog_input_header * input_dialog, char * ok_caption, char * cancel_caption)
{
    FamesAssert(input_dialog);

    if(!input_dialog || !ok_caption || !cancel_caption)
        return;

    if(input_dialog->ok_button)
        gui_button_set_caption(input_dialog->ok_button, ok_caption);
    if(input_dialog->cancel_button)
        gui_button_set_caption(input_dialog->cancel_button, cancel_caption);
}


/*=========================================================================================
 * 
 * ���ļ�����: gui/dialog/dlg_inpt.c
 * 
**=======================================================================================*/


