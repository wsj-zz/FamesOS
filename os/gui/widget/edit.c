/******************************************************************************************
 * 文件: gui/widget/edit.c
 *
 * 描述: 文本编辑控件
 *
 * 作者: Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_EDIT_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * 
 *      EDIT私有结构
 * 
**---------------------------------------------------------------------------------------*/
struct gui_edit_private_s {
    INT08S * text;
    INT08S * __text;
    INT08S * text_old;
    int      len;
    BOOL     blink_on;
    int      blink_speed;
    int      blink_index;
    int      blink_state;
    int      blink_counter;
    INT16U   blink_option;
};

typedef struct gui_edit_private_s gui_edit_private;

/*-----------------------------------------------------------------------------------------
 * 函数:    __gui_edit_show_blink()
 *
 * 描述:    显示光标
**---------------------------------------------------------------------------------------*/
void __internal __gui_edit_show_blink(gui_widget * edit, gui_edit_private * t, INT16U style)
{
    int index, font, w, h, x, y, height;
    RECT * inner_rect;
    FONTINFO fontinfo;
    COLOR color;

    style = style;

    index = t->blink_index;
    font  = edit->font;

    if(!get_font_info(&fontinfo, font))
        return;

    w = fontinfo.width;
    h = fontinfo.height;

    inner_rect = &edit->inner_rect;
    x      = inner_rect->x;
    y      = inner_rect->y;
    height = inner_rect->height;
    
    if(h < height){
        y += ((height-h)-1)/2;  /* 垂直对齐 */
    }
    x += (w * index);

    color = edit->color;

    gdi_draw_box(x, y, x+1, y+(h-1), color);
    
    t->blink_state = 1;
    t->blink_counter = 0;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    __gui_edit_hide_blink()
 *
 * 描述:    隐藏光标
**---------------------------------------------------------------------------------------*/
void __internal __gui_edit_hide_blink(gui_widget * edit, gui_edit_private * t, INT16U style)
{
    int index, font, w, h, x, y, height;
    RECT  * inner_rect;
    FONTINFO fontinfo;
    COLOR bkcolor;

    style = style;

    index = t->blink_index;
    font  = edit->font;

    if(!get_font_info(&fontinfo, font))
        return;

    w = fontinfo.width;
    h = fontinfo.height;

    inner_rect = &edit->inner_rect;
    x      = inner_rect->x;
    y      = inner_rect->y;
    height = inner_rect->height;

    if(h < height){
        y += ((height-h)-1)/2;  /* 垂直对齐 */
    }
    x += (w * index);

    bkcolor = edit->bkcolor;
    if(bkcolor==0)
        bkcolor = WIDGET_EDIT_BKCOLOR;

    if(index < (t->len-1)){
        t->text_old[index] = -1; /* 强制刷新指定位置的字符, 消除光标的显示 */
    } else {
        gdi_draw_box(x, y, x+1, y+(h-1), bkcolor);
    }
    t->blink_state = 0;
    t->blink_counter = 0;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_edit_blink_on()
 *
 * 描述:    打开光标(开始闪烁)
**---------------------------------------------------------------------------------------*/
BOOL guical gui_edit_blink_on(gui_widget * edit, int speed, int index, INT16U opt)
{
    gui_edit_private * t;

    FamesAssert(edit);
    FamesAssert(speed > 0);
    FamesAssert(index >= 0);
    if(!edit || speed <= 0 || index < 0)
        return fail;
    
    FamesAssert(edit->type == GUI_WIDGET_EDIT);
    if(edit->type != GUI_WIDGET_EDIT)
        return fail;

    lock_kernel();
    t = (gui_edit_private *)edit->private_data;
    unlock_kernel();
    if(!t)
        return fail;

    lock_kernel();
    t->blink_on = 1;
    t->blink_speed = speed;
    t->blink_index = index;
    t->blink_state = 0;
    t->blink_counter = 0;
    t->blink_option = opt;
    if(edit->style & (EDIT_ALIGN_CENTER|EDIT_ALIGN_RIGHT)){
        edit->style &= ~(EDIT_ALIGN_CENTER|EDIT_ALIGN_RIGHT);
        gui_refresh_widget(edit);
    }
    __gui_edit_show_blink(edit, t, edit->style);
    unlock_kernel();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_edit_blink_off()
 *
 * 描述:    关闭光标(停止闪烁)
**---------------------------------------------------------------------------------------*/
BOOL guical gui_edit_blink_off(gui_widget * edit, INT16U opt)
{
    gui_edit_private * t;

    FamesAssert(edit);
    if(!edit)
        return fail;
    
    FamesAssert(edit->type == GUI_WIDGET_EDIT);
    if(edit->type != GUI_WIDGET_EDIT)
        return fail;

    lock_kernel();
    t = (gui_edit_private *)edit->private_data;
    unlock_kernel();
    if(!t)
        return fail;

    lock_kernel();
    t->blink_on = 0;
    __gui_edit_hide_blink(edit, t, edit->style);
    gui_draw_edit(edit);
    unlock_kernel();

    opt = opt;

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_edit_blink_set()
 *
 * 描述:    设置光标位置
**---------------------------------------------------------------------------------------*/
BOOL guical gui_edit_blink_set(gui_widget * edit, int index, INT16U opt)
{
    gui_edit_private * t;

    FamesAssert(edit);
    FamesAssert(index >= 0);
    if(!edit || index < 0)
        return fail;

    FamesAssert(edit->type == GUI_WIDGET_EDIT);
    if(edit->type != GUI_WIDGET_EDIT)
        return fail;

    lock_kernel();
    t = (gui_edit_private *)edit->private_data;
    unlock_kernel();
    if(!t)
        return fail;

    lock_kernel();
    t->blink_on = 1;
    __gui_edit_hide_blink(edit, t, edit->style);
    t->blink_index = index;
    t->blink_option = opt;
    __gui_edit_show_blink(edit, t, edit->style);
    unlock_kernel();

    opt = opt;

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_edit_input()
 *
 * 参数:    @edit    the edit widget which will perform input
 *          @buf     the output buffer, should be of sufficient size
 *          @bytes   the max number of bytes can be entered (<=128)
 *          @option  not used currently
 *
 * 描述:    EDIT控件输入, 最多128个字符
**---------------------------------------------------------------------------------------*/
#define GUI_INPUT_MAX_BYTES   128   /* 一次可输入的最大字节数        */

KEYCODE gui_edit_input(gui_widget * edit, char * __BUF buf, int bytes, INT16U option)
{
    int  i, index, first, blink;
    char __buf[GUI_INPUT_MAX_BYTES+2];
    KEYCODE key;
    INT16U old_style;
    COLOR old_color, old_bkcolor;
    BOOL __not_ok;

    FamesAssert(edit);
    FamesAssert(buf);
    FamesAssert(bytes > 0 && bytes <= GUI_INPUT_MAX_BYTES);

    if(!edit || !buf || bytes <= 0){
        return NONEKEY;
    }

    lock_kernel();
    old_style = edit->style;
    old_color = edit->color;
    old_bkcolor = edit->bkcolor;
    unlock_kernel();
    gui_set_widget_color(edit, COLOR_WHITE);
    gui_set_widget_bkcolor(edit, COLOR_BROWN);

    option = option;

    index = 0;
    blink = 0;
    first = 1;

    __not_ok = 1;

    #if 0
    message(buf);
    #endif

    if(bytes > GUI_INPUT_MAX_BYTES)
        bytes = GUI_INPUT_MAX_BYTES;

    MEMSET(__buf, 0, sizeof(__buf));
    buf[bytes] = 0;
    STRCPY(__buf, buf);

    gui_edit_set_text(edit, __buf);
    TaskSleep(20L); /* 睡眠的意思是给edit以刷新的机会 */
    gui_edit_blink_on(edit, 6, blink, 0); /* 打开光标     */

    while(__not_ok){
        #if 0
        if("debug"){
            char __s[128];
            sprintf(__s, "index = %d, __buf=[%s]", index, __buf);
            message(__s);
        }
        #endif
        key = waitkey(0L);
        switch(key){
            case ESC:    /* 返回 */
            case ENTER:
            case TAB:
                __not_ok = 0;
                break;
            case LEFT:
                first = 0;
                if(index > 0){
                    index--;
                    gui_edit_blink_set(edit, index, 0);
                } else {
                    __not_ok = 0;
                }
                break;
            case RIGHT:     
                first = 0;
                if((index < bytes) && (__buf[index]!=0)){
                    index++;
                    gui_edit_blink_set(edit, index, 0);
                }
                break;
            case HOME:
                first = 0;
                if(index > 0){
                    index = 0;
                    gui_edit_blink_set(edit, index, 0);
                } else {
                    __not_ok = 0;                    
                }
                break;
            case END:
                first = 0;
                if(index < STRLEN(__buf)){
                    index = STRLEN(__buf);
                    gui_edit_blink_set(edit, index, 0);
                } else {
                    __not_ok = 0;                    
                }
                break;
            case BACKSPACE:
                first = 0;
                if(index > 0){
                    if(index >= bytes){
                        index = bytes-1;
                        __buf[index] = 0;
                    } else {
                        index--;
                        for(i = index; __buf[i]; i++){
                            __buf[i] = __buf[i+1]; /*lint !e679*/
                        }
                    }
                    gui_edit_blink_set(edit, index, 0);
                    gui_edit_set_text(edit, __buf);
                }
                break;
            case DELETE:
                first = 0;
                if(index < bytes){
                    if(index == (bytes-1)){
                        __buf[index] = 0;
                    } else {
                        for(i = index; __buf[i]; i++){
                            __buf[i] = __buf[i+1]; /*lint !e679*/
                        }
                    }
                    gui_edit_blink_set(edit, index, 0);
                    gui_edit_set_text(edit, __buf);
                }
                break;
            case CTRL:
            case ALT:
            case SHIFT:
            case NONEKEY:
            case NONE_KEY:
            case CTRL|ALT:
            case ALT|SHIFT:
            case CTRL|SHIFT:
            case CTRL|ALT|SHIFT:
                break;
            default:
                if((key&0xFF) >= SPACE){ /* 可显示字符 */
                    if(first){
                        index = 0;
                        first = 0;
                        for(i=0; i<bytes; i++)
                            __buf[i]=0;
                    }
                    i = index;
                    if(i >= bytes)
                        i--;
                    __buf[i]=(char)key;
                    i++;
                    gui_edit_set_text(edit, __buf);
                    if(i != index){
                        gui_edit_blink_set(edit, i, 0);
                    }
                    index = i;
                } else { /* 可能是功能键, 如F1, F2, CTRL-C等 */
                    __not_ok = 0;
                }
                break;
        }
    }
    
    gui_edit_blink_off(edit, 0); /* 关闭光标     */
    gui_set_widget_style(edit, old_style);
    gui_set_widget_color(edit, old_color);
    gui_set_widget_bkcolor(edit, old_bkcolor);
    
    __buf[bytes] = 0;
    STRCPY(buf, __buf);          /* 拷贝编辑内容 */

    return key;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_edit_init_private()
 *
 * 描述:    实例初始化, 主要是给私有结构申请内存
**---------------------------------------------------------------------------------------*/
BOOL guical gui_edit_init_private(gui_widget * edit, int text_len)
{
    int  bytes;
    INT08S * buf;
    gui_edit_private * t, * t2;
    
    FamesAssert(edit);
    FamesAssert(text_len > 0);
    if(!edit || text_len <= 0)
        return fail;

    FamesAssert(edit->type == GUI_WIDGET_EDIT);
    if(edit->type != GUI_WIDGET_EDIT)
        return fail;

    bytes = (text_len+2)*3 + (int)sizeof(gui_edit_private);

    buf = (INT08S *)mem_alloc((INT32U)(INT32S)bytes);

    if(buf){
        MEMSET(buf, 0, bytes);
        t = (gui_edit_private *)buf;/*lint !e826*/
        t->text = buf+sizeof(gui_edit_private);
        t->__text = t->text + text_len;
        t->text_old = t->__text + text_len + 2;
        t->len  = text_len;
        t->text[0] = 0;
        t->__text[0] = 0;
        t->text_old[0] = 0;
        if(edit->private_data){
            lock_kernel();
            t2 = edit->private_data;
            edit->private_data = NULL;
            unlock_kernel();
            mem_free(t2);
        }
        lock_kernel();
        edit->private_data = (void *)t;
        unlock_kernel();
        return ok;
    } else {
        return fail;
    }
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_edit_set_text()
 *
 * 描述:    设置EDIT控件的内容(字符串)
**---------------------------------------------------------------------------------------*/
BOOL guical gui_edit_set_text(gui_widget * edit, INT08S * text)
{
    gui_edit_private * t;
    int len;

    FamesAssert(edit);
    FamesAssert(text);

    if(!edit || !text)
        return fail;

    lock_kernel();
    t = (gui_edit_private *)edit->private_data;
    unlock_kernel();
    if(!t)
        return fail;

    len = STRLEN(text);
    if(len > t->len)
        len = t->len-1;

    lock_kernel();
    MEMCPY(t->text, text, len);
    t->text[len] = 0;
    unlock_kernel();
    
    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_edit_get_text()
 *
 * 描述:    读取EDIT控件的内容(字符串)
**---------------------------------------------------------------------------------------*/
BOOL guical gui_edit_get_text(gui_widget * edit, INT08S * text)
{
    gui_edit_private * t;
    int len;

    FamesAssert(edit);
    FamesAssert(text);

    if(!edit || !text)
        return fail;

    lock_kernel();
    t = (gui_edit_private *)edit->private_data;
    unlock_kernel();
    if(!t)
        return fail;

    lock_kernel();
    len = t->len-1;
    MEMCPY(text, t->text, len);
    text[len] = 0;
    unlock_kernel();
    
    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_draw_edit()
 *
 * 描述:    EDIT控件的绘图(或刷新)
**---------------------------------------------------------------------------------------*/
void gui_draw_edit(gui_widget * edit)
{
/*lint --e{534}*/
    int x, y, x1, y1, move;
    COLOR bkcolor;
    gui_edit_private * t;
    RECT * inner_rect;
    
    FamesAssert(edit);

    if(!edit)
        return;

    t = (gui_edit_private *)edit->private_data;
    if(!t)
        return;
    
    inner_rect = &edit->inner_rect;
    bkcolor = edit->bkcolor;
    if(bkcolor==0)
        bkcolor = WIDGET_EDIT_BKCOLOR;

    if(edit->flag & GUI_WIDGET_FLAG_REFRESH){
        x  = edit->real_rect.x;
        y  = edit->real_rect.y;
        x1 = edit->real_rect.width + x;
        y1 = edit->real_rect.height + y;
        if(edit->style & EDIT_STYLE_READONLY)
            bkcolor = WIDGET_BKCOLOR;
        if(edit->style & EDIT_STYLE_EDITING){
            move = gui_widget_draw_xp_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
            move = gui_widget_draw_subside_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(edit->style & EDIT_STYLE_IN_VIEW){
            gdi_draw_rect(x, y, x1, y1, COLOR_WHITE);
            ___gui_widget_xy_move(1);
        } 
        if(edit->style & EDIT_STYLE_XP_BORDER){
            move = gui_widget_draw_xp_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(edit->style & (EDIT_STYLE_TRANSPARENT|EDIT_STYLE_NO_BORDER|EDIT_STYLE_EDITING|EDIT_STYLE_IN_VIEW)){
            ;
        } else {
            move = gui_widget_draw_client_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        }
        if(edit->style & EDIT_STYLE_MODAL_FRAME){
            move = gui_widget_draw_modal_frame(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(edit->style & EDIT_STYLE_CLIENT_BDR){
            move = gui_widget_draw_client_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(edit->style & EDIT_STYLE_STATIC_BDR){
            move = gui_widget_draw_subside_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(edit->style & EDIT_STYLE_TRANSPARENT){
            ;/* Transparent, do nothing here */
        } else if(!gdi_draw_box(x, y, x1, y1, bkcolor)){
            ;
        }
        x+=2; x1-=2; y+=1;
        gui_init_rect(inner_rect, x, y, (x1-x), (y1-y));
        t->text_old[0] = 0;
        if(t->blink_on){
            t->blink_state = 0;
            t->blink_counter = 0;
        }
    } else {
        INT16U __opt;
        INT08S * __text;
        int  i;

        __opt = DRAW_OPT_FIL_BG;
        if(edit->style & EDIT_ALIGN_CENTER)
            __opt |= DRAW_OPT_ALIGN_CENTER;
        if(edit->style & EDIT_ALIGN_RIGHT)
            __opt |= DRAW_OPT_ALIGN_RIGHT;

        if(edit->style & EDIT_STYLE_PASSWORD){
            __text = t->__text;
            for(i=0; i<(t->len); i++){
                if(!t->text[i])
                    break;
                __text[i] = '*';
            }
            __text[i] = 0;
        } else {
            __text = t->text;
        }

        if(t->blink_on){
            t->blink_counter++;
            if(t->blink_counter >= t->blink_speed){
                if(t->blink_state){ /* 已显示 */
                    __gui_edit_hide_blink(edit, t, edit->style);
                } else {            /* 已隐藏 */
                    __gui_edit_show_blink(edit, t, edit->style);
                }
            }
        }
        draw_font_for_widget(inner_rect->x, inner_rect->y, inner_rect->width, inner_rect->height, 
                             __text, t->text_old, edit->color, bkcolor, edit->font, __opt);
    }

    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/edit.c
 * 
**=======================================================================================*/


