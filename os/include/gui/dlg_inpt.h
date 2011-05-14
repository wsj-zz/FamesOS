/******************************************************************************************
 * 文件:    gui/dialog/dlg_inpt.h
 *
 * 说明:    输入对话框
 *
 * 作者:    Jun
 *
 * 时间:    2011-2-28
******************************************************************************************/
#ifndef  FAMES_GUI_DIALOG_INPUT_H
#define  FAMES_GUI_DIALOG_INPUT_H


/*-----------------------------------------------------------------------------------------
 * 
 *      输入对话框的定义方法
 * 
**---------------------------------------------------------------------------------------*/
struct __dialog_input_item_struct {
    int     type;
    char  * caption;
    int     id;
    int     bytes;
    int     x,y,width, height;
    int     font;
    INT16U  style;
    char  * comment;
    gui_widget * edit;
    gui_widget * label;
};

typedef struct __dialog_input_item_struct  dialog_input_item;

struct __dialog_input_header_struct {
    char    * dialog_name;
    BMPINFO * icon;
    int       x,y,width, height;
    COLOR     color, bkcolor;
    int       font;
    INT16U    style;
    dialog_input_item * items;
    gui_widget * main_form;
    gui_widget * status_bar;
    gui_widget * ok_button;
    gui_widget * cancel_button;
    char       * ok_comment;
    char       * cancel_comment;
    int       current_index;
};

typedef struct __dialog_input_header_struct  dialog_input_header;

#define INPUT_DIALOG_ITEM_TYPE_EDIT   0
#define INPUT_DIALOG_ITEM_TYPE_GROUP  1


#define DECLARE_INPUT_DIALOG_START(var) \
            dialog_input_item var##_items[] = {
#define INPUT_DIALOG_SET(var, name, icon, x, y, width, height, color, bkcolor, font, style) \
            { 0, NULL, } }; /* items的最后一项 */ \
            dialog_input_header var = { name, icon, x, y, width, height, color, bkcolor, font, style, &var##_items[0], }
#define DECLARE_INPUT_DIALOG_ENDED(var)

#define INPUT_DIALOG_ITEM(caption, id, bytes, x, y, width, height, font, style, comment) \
            { INPUT_DIALOG_ITEM_TYPE_EDIT, caption, id, bytes, x, y, width, height, font, style, comment, NULL, NULL },

#define INPUT_DIALOG_GROUP(caption, id, x, y, width, height, font, style) \
            { INPUT_DIALOG_ITEM_TYPE_GROUP, caption, id, -1, x, y, width, height, font, style, NULL, NULL, NULL },

/*-----------------------------------------------------------------------------------------
 *                      输入对话框的定义举例:
 *
 *      DECLARE_INPUT_DIALOG_START(edit_order)
 *      INPUT_DIALOG_ITEM("工号:", 0, 16, 40,  38,  196, 28, 1, 0)
 *      INPUT_DIALOG_ITEM("单号:", 1, 16, 256, 38,  196, 28, 1, 0)
 *      INPUT_DIALOG_ITEM("剖数:", 2, 16, 40,  72,  196, 28, 1, 0)
 *      INPUT_DIALOG_ITEM("压线:", 3, 16, 40,  106, 628, 28, 1, 0)
 *      INPUT_DIALOG_ITEM("压型:", 4, 16, 472, 38,  196, 28, 1, 0)
 *      INPUT_DIALOG_ITEM("楞别:", 5, 16, 256, 72,  196, 28, 1, 0)
 *      INPUT_DIALOG_ITEM("修边:", 6, 16, 472, 72,  196, 28, 1, 0)
 *      INPUT_DIALOG_SET(edit_order, "订单编辑", &icon, 140, 418, 738, 221, COLOR_BLACK, COLOR_BLUE, 1, FORM_STYLE_XP_BORDER|FORM_STYLE_TITLE)
 *      DECLARE_INPUT_DIALOG_ENDED(edit_order);
 * 
 *      input_dialog_initialize(&edit_order);   //OK
 *
**---------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------
 * 
 *      函数声明
 * 
**---------------------------------------------------------------------------------------*/
gui_widget * input_dialog_initialize(dialog_input_header * input_dialog);

int  input_dialog_method(dialog_input_header * input_dialog, 
                         void (*prepare)(int id, char * buf, void * data, INT16U opt),
                         int  (*finish)(int id, char * buf, void * data, KEYCODE key),
                         void * data, int keep_index);

void input_dialog_alert(dialog_input_header * input_dialog, char * s, COLOR alert_color);

void input_dialog_preset_title(dialog_input_header * input_dialog, char * title);
void input_dialog_preset_item_name(dialog_input_header * input_dialog, int id, char * name);
void input_dialog_preset_item_comment(dialog_input_header * input_dialog, int id, char * comment);

void input_dialog_set_ok_comment(dialog_input_header * input_dialog, char * ok_comment);
void input_dialog_set_cancel_comment(dialog_input_header * input_dialog, char * cancel_comment);
void input_dialog_set_buttons_caption(dialog_input_header * input_dialog, char * ok_caption, char * cancel_caption);


/*-----------------------------------------------------------------------------------------
 * 
 *      PREPARE选项
 * 
**---------------------------------------------------------------------------------------*/
#define INPUT_DIALOG_PREPARE_OPT_EDITING  0x01



#endif /* FAMES_GUI_DIALOG_INPUT_H */

/*=========================================================================================
 * 
 * 本文件结束: gui/dialog/dlg_inpt.h
 * 
**=======================================================================================*/


