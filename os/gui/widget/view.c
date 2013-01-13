/******************************************************************************************
 * 文件:    gui/widget/view.c
 *
 * 描述:    数据视图控件
 *
 * 作者:    Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_VIEW_C
#include "includes.h"

/*-----------------------------------------------------------------------------------------
 * 
 *      VIEW私有结构
 * 
**---------------------------------------------------------------------------------------*/
#define VIEW_MAX_FIELDS     32          /* 可同时支持的最大字段数              */
#define VIEW_MAX_RECORDS    64          /* 可同时显示的最大记录数(1页)         */
#define VIEW_MAX_OLD_SIZE   256         /* size_per_record_for_old的最大值     */

#define VIEW_EDITED_COLOR   COLOR_RED   /* 本编辑行中已修改过的内容, 字体颜色  */
#define VIEW_EDITED_BKCOLOR COLOR_BLUE  /* 应与__SELECT_ORDER_BK相同, 背景色   */

struct gui_view_private_s {
    int    me_size;
    view_fields_t  * fields;
    view_show_record_f       show_record;
    view_show_statistics_f   show_statistics;
    view_get_max_index_f     get_max_index;
    view_get_item_f          get_item;
    view_set_item_f          set_item;
    view_is_writable_f       is_writable;
    view_notifier_f          notifier_on_changed;
    COLOR  dashed_color;
    INT08U dashed_style;
    int    height_per_row;   /* 一行的显示高度 */
    int    data_x, data_y;
    int    edit_flag;
    gui_widget __edit;
    int    data_displayed;
    int    first_record_index;
    int    first_index_old;
    int    selected_index_old;
    int    selected_index_curr;
    COLOR  data_color;
    COLOR  data_bkcolor;
    int    nr_records_to_show;
    int    max_index_old;
    int    fields_nr;        /* 字段个数 */
    int    current_field;    /* 当前字段 */
    int    x_width_fields[VIEW_MAX_FIELDS];
    char * fields_caption[VIEW_MAX_FIELDS];
    char * statistics_caption;
    INT08S statistic_buf_old[VIEW_MAX_OLD_SIZE];
    void * records_buf_old;
    int    sizeof_records_buf_old;
    int    sizeof_old_buf_per_record;
};

typedef struct gui_view_private_s gui_view_private;

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_init_private()
 *
 * 描述:    实例初始化, 主要是给私有结构申请内存
**---------------------------------------------------------------------------------------*/
BOOL guical gui_view_init_private( gui_widget * view, 
                                   view_fields_t * fields,
                                   view_get_max_index_f get_max_index,
                                   view_get_item_f  get_item,
                                   view_set_item_f  set_item,
                                   view_is_writable_f  is_writable,
                                   view_show_record_f  show_record,
                                   char * statistics_caption,
                                   view_show_statistics_f show_statistics,
                                   view_notifier_f notifier_on_changed,
                                   COLOR data_color,
                                   COLOR data_bkcolor,
                                   int  height_per_row,
                                   BOOL alloc_memory)
{
    INT08S * buf;
    gui_view_private * t, * t2;
    INT32S bytes;
    int fields_nr, __temp;
    int x, width, font_width;
    BOOL retval;
    int field_old_buf_ptr;
    int size_per_record_for_old;
    view_fields_t * ___f;
    
    FamesAssert(view);
    FamesAssert(fields);
    FamesAssert(get_max_index);
    FamesAssert(height_per_row > 0 && height_per_row < 128);
    

    if(!view || !fields || !get_max_index ||
        height_per_row<=0 || height_per_row>=128)
        return fail;

    FamesAssert(view->type == GUI_WIDGET_VIEW);

    if(view->type != GUI_WIDGET_VIEW)
        return fail;

    font_width = get_font_width(view->font);

    size_per_record_for_old = 0;
    fields_nr = 0;
    field_old_buf_ptr = 0;
    for(___f = fields; ___f->caption; ___f++){
        FamesAssert(___f->bytes_for_width > 0);
        ___f->old_buf_ptr = field_old_buf_ptr;
        field_old_buf_ptr += (___f->bytes_for_width+8);
        size_per_record_for_old += (___f->bytes_for_width+8);
        if(size_per_record_for_old >= VIEW_MAX_OLD_SIZE)
            return fail;
        fields_nr++;
        if(fields_nr >= VIEW_MAX_FIELDS)
            return fail;
    }

    __temp = size_per_record_for_old * VIEW_MAX_RECORDS;
   
    bytes  = (long)sizeof(gui_view_private);
    bytes += ((long)__temp);

    FamesAssert(bytes < 32L*1024L); /* 不能超过32K */
    if(bytes >= 32L*1024L)
        return fail;

    retval = fail;

    lock_kernel();
    if(alloc_memory || (!view->private_data)){ /* 如果强制分配或尚未分配, 那么就分配吧 */
        buf = (INT08S *)mem_alloc((INT32U)bytes);
        alloc_memory = 1;
    } else {
        buf = (INT08S *)view->private_data;
        t = (gui_view_private *)buf;/*lint !e826*/
        bytes = (INT32S)t->me_size;
        alloc_memory = 0;
    }
    if(buf){
        gui_widget ___edit;
        t = (gui_view_private *)buf;/*lint !e826*/
        ___edit = t->__edit;
        MEMSET(buf, 0, (INT16S)bytes);
        t->me_size      = (int)bytes;
        t->fields        = fields;
        t->show_record    = show_record;        
        t->show_statistics = show_statistics;
        t->get_max_index  = get_max_index;
        t->get_item       = get_item;
        t->set_item       = set_item;
        t->is_writable    = is_writable;
        t->notifier_on_changed = notifier_on_changed;
        t->height_per_row = height_per_row;
        t->data_x         = view->real_rect.x;
        t->data_y         = view->real_rect.y;
        t->edit_flag      = 0;
        t->data_displayed = 0;
        if(alloc_memory){
            gui_init_widget(&t->__edit);
            gui_set_widget_type(&t->__edit, GUI_WIDGET_EDIT);
            gui_set_widget_color(&t->__edit, COLOR_WHITE);
            gui_set_widget_bkcolor(&t->__edit, COLOR_BROWN);
            gui_set_widget_font(&t->__edit, view->font);
            gui_set_widget_style(&t->__edit, EDIT_STYLE_IN_VIEW);
            gui_edit_init_private(&t->__edit, 256);
        } else {
            t->__edit = ___edit;
        }
        t->first_record_index = 0;
        t->first_index_old    = -1;
        t->selected_index_old = -1;
        t->selected_index_curr = 0;
        t->data_color         = data_color;
        t->data_bkcolor       = data_bkcolor;
        t->max_index_old      = 0;
        t->nr_records_to_show = 0;  /* 这个字段在刷新时会重新计算 */
        t->current_field      = 0;
        fields_nr = 0;
        x = 0;
        while(fields->caption){     /* 处理字段定义               */
            width = (fields->bytes_for_width * font_width);
            width += font_width; /* 加多几个象素 */
            t->x_width_fields[fields_nr] = width;
            t->fields_caption[fields_nr] = fields->caption;
            fields->offset_x = x;
            fields->width = width;
            fields->height = height_per_row;
            x += width;
            fields ++;
            fields_nr ++;
        }
        FamesAssert(fields_nr >= 2); /* 至少应该有2个字段吧 */
        t->fields_nr         = fields_nr;
        t->statistics_caption = statistics_caption;
        t->records_buf_old = (void *)(buf + sizeof(gui_view_private));
        t->sizeof_records_buf_old = __temp;
        t->sizeof_old_buf_per_record = size_per_record_for_old;
        /* 至此, gui_view_private结构初始化完成 */
        if(alloc_memory){
            if(view->private_data){
                t2 = view->private_data;
                view->private_data = NULL;
                mem_free(t2);
            }
        }
        view->private_data = (void *)t;
        gui_refresh_widget(view);
        retval = ok;
    }
    unlock_kernel();
    
    return retval;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_goto_top()
 *
 * 描述:    选中第一笔订单
**---------------------------------------------------------------------------------------*/
BOOL guical gui_view_goto_top(gui_widget *view)
{
    gui_view_private * t;

    FamesAssert(view);

    if(!view)
        return fail;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return fail;

    if(t->edit_flag) /* 正在编辑时, 不允许重新选择 */
        return fail;
    
    lock_kernel();
    t->first_record_index = 0;
    t->selected_index_curr = 0;
    unlock_kernel();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_goto_bottom()
 *
 * 描述:    选中最后一笔订单
**---------------------------------------------------------------------------------------*/
BOOL guical gui_view_goto_bottom(gui_widget *view)
{
    gui_view_private * t;
    int last_index;

    FamesAssert(view);

    if(!view)
        return fail;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return fail;

    if(t->edit_flag) /* 正在编辑时, 不允许重新选择 */
        return fail;

    lock_kernel();
    last_index = t->get_max_index();
    if(last_index < 0)
        last_index = 0;

    t->selected_index_curr = last_index;

    last_index -= (t->nr_records_to_show-1);
    if(last_index < 0)
        last_index = 0;
    
    t->first_record_index = last_index;
    if(last_index > t->selected_index_curr)
        t->first_record_index = t->selected_index_curr;
    unlock_kernel();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_move_up()
 *
 * 描述:    选中上一笔订单
**---------------------------------------------------------------------------------------*/
BOOL guical gui_view_move_up(gui_widget *view)
{
    gui_view_private * t;
    int index;

    FamesAssert(view);

    if(!view)
        return fail;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return fail;

    if(t->edit_flag) /* 正在编辑时, 不允许重新选择 */
        return fail;

    index = t->selected_index_curr;

    if(index <= 0)
        return fail;

    lock_kernel();
    index--;
    if(index < t->first_record_index)
        t->first_record_index = index;
    t->selected_index_curr = index;
    unlock_kernel();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_move_down()
 *
 * 描述:    选中下一笔订单
**---------------------------------------------------------------------------------------*/
BOOL guical gui_view_move_down(gui_widget *view)
{
    gui_view_private  * t;
    int last_index, index;

    FamesAssert(view);

    if(!view)
        return fail;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return fail;

    if(t->edit_flag) /* 正在编辑时, 不允许重新选择 */
        return fail;

    last_index = t->get_max_index();
    index = t->selected_index_curr;

    if(index >= last_index)
        return fail;

    lock_kernel();
    index++;
    if(index >= (t->first_record_index + t->nr_records_to_show))
        t->first_record_index++;
    t->selected_index_curr = index;
    unlock_kernel();


    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_page_up()
 *
 * 描述:    查看上一页订单
**---------------------------------------------------------------------------------------*/
BOOL guical gui_view_page_up(gui_widget *view)
{
    gui_view_private * t;
    int index;

    FamesAssert(view);

    if(!view)
        return fail;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return fail;

    if(t->edit_flag) /* 正在编辑时, 不允许重新选择 */
        return fail;

    lock_kernel();
    index = t->first_record_index;
    index -= t->nr_records_to_show;
    if(index < 0){
        t->first_record_index = 0;
        t->selected_index_curr = 0;
    } else {
        t->first_record_index = index;
        t->selected_index_curr -= t->nr_records_to_show;
    }
    unlock_kernel();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_page_down()
 *
 * 描述:    查看下一页订单
**---------------------------------------------------------------------------------------*/
BOOL guical gui_view_page_down(gui_widget *view)
{
    gui_view_private * t;
    int last_index, index;

    FamesAssert(view);

    if(!view)
        return fail;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return fail;

    if(t->edit_flag) /* 正在编辑时, 不允许重新选择 */
        return fail;

    lock_kernel();
    last_index = t->get_max_index();
    if(last_index < 0)
        last_index = 0;
    index = t->first_record_index;
    index += t->nr_records_to_show;
    if(index > last_index){
        t->selected_index_curr = last_index;
    } else {
        t->first_record_index = index;
        t->selected_index_curr += t->nr_records_to_show;
        if(t->selected_index_curr > last_index)
            t->selected_index_curr = last_index;
    }
    unlock_kernel();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_select_index()
 *
 * 描述:    选中某一笔记录
**---------------------------------------------------------------------------------------*/
BOOL guical gui_view_select_index(gui_widget *view, int index)
{
    gui_view_private * t;
    int last_index, ___t;

    FamesAssert(view);

    if(!view)
        return fail;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return fail;

    if(t->edit_flag) /* 正在编辑时, 不允许重新选择 */
        return fail;
    
    if(index <= 0){  /*  */
        return gui_view_goto_top(view);
    }
    if(index >= t->get_max_index()){
        return gui_view_goto_bottom(view);
    }
    lock_kernel();
    last_index = t->get_max_index();
    if(last_index < 0)
        last_index = 0;
    t->selected_index_curr = index;
    ___t = t->first_record_index;
    ___t += t->nr_records_to_show;
    if(___t <= index){
        t->first_record_index = index;
    }
    ___t = t->first_record_index;
    if(___t > index){
        t->first_record_index = index;
    }        
    unlock_kernel();

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_get_selected()
 *
 * 描述:    返回选中订单的序号
**---------------------------------------------------------------------------------------*/
int guical gui_view_get_selected(gui_widget *view)
{
    gui_view_private * t;
    int ret;

    FamesAssert(view);

    if(!view)
        return -1;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return -1;

    lock_kernel();
    ret = t->selected_index_curr;
    unlock_kernel();

    return ret;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_set_dashed()
 *
 * 描述:    设置虚线的颜色及样式
**---------------------------------------------------------------------------------------*/
BOOL guical gui_view_set_dashed(gui_widget *view, COLOR dashed_color, INT08U dashed_style)
{
    gui_view_private * t;

    FamesAssert(view);

    if(!view)
        return fail;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return fail;

    lock_kernel();
    t->dashed_color = dashed_color;
    t->dashed_style = dashed_style;
    unlock_kernel();
    gui_refresh_widget(view);

    return ok;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_view_editing()
 *
 * 描述:    VIEW的编辑方法
 *
 * 参数:    
**---------------------------------------------------------------------------------------*/
KEYCODE gui_view_editing(gui_widget *view, INT16U opt)
{
    gui_view_private * t;
    KEYCODE key;
    view_fields_t * f;
    char buf[256];
    view_get_item_f    get_item;
    view_set_item_f    set_item;
    view_is_writable_f is_writable;
    int x, y, w, h, i, index;
    int __y, __h, find_next;
    int continue_edit;
    char * old_buf;

    opt = opt;

    FamesAssert(view);

    key = NONEKEY;

    if(!view)
        goto out_directly;

    if(!(view->flag & GUI_WIDGET_FLAG_VISIBLE))
        goto out_directly;

    t = (gui_view_private *)view->private_data;
    if(!t)
        goto out_directly;

    lock_kernel();
    t->data_displayed = 0;
    unlock_kernel();
    do { /* 先等待VIEW将数据区显示出来 */
        int ___t;
        lock_kernel();
        ___t = t->data_displayed;
        unlock_kernel();
        if(___t)
            break;
        if(waitkey(20L) == ESC)
            return NONEKEY;
    } while(1); /*lint !e506 */

    index = gui_view_get_selected(view);
    
    get_item = t->get_item;
    set_item = t->set_item;
    is_writable = t->is_writable;

    FamesAssert(get_item);
    FamesAssert(set_item);
    FamesAssert(is_writable);
    if(!get_item || !set_item || !is_writable)
        goto out_directly;

    lock_kernel();
    t->edit_flag = 1; /* 打开编辑标志 */
    unlock_kernel();

    i = (t->selected_index_curr - t->first_record_index);
    if(i < 0)
        goto out;
    __h = (t->height_per_row);
    __y = (t->data_y) + (i * __h);
    old_buf = ((char *)t->records_buf_old+(i*(t->sizeof_old_buf_per_record))); /*lint !e679*/

    FamesAssert((i*(t->sizeof_old_buf_per_record)) < (VIEW_MAX_RECORDS*VIEW_MAX_OLD_SIZE));

    continue_edit = 1;
    i = t->current_field;
    find_next = 0;
    while(continue_edit){
        if(++find_next > (VIEW_MAX_FIELDS*2)){ /* 没有可编辑的字段 */
            break;
        }
        if(i < 0)
            i = 0;
        if(i >= t->fields_nr)
            i = t->fields_nr-1;
        f = &t->fields[i];
        if(is_writable(index, f->id, 0)){
            find_next = 0;
            lock_kernel();
            t->current_field = i;
            unlock_kernel();
        } else {
            switch(key){
                case LEFT:
                case END:
                    i--;
                    if(i < 0)
                        i = t->fields_nr-1;
                    break;
                default:
                    i++;
                    if(i >= t->fields_nr)
                        i = 0;
                    break;
            }
            continue;
        }
        
        h = __h;
        y = __y;
        w = (f->width -2);
        x = (t->data_x + 1) + f->offset_x;
        
        if(get_item){
            MEMSET(buf, 0, sizeof(buf));
            if(!get_item(index, f->id, buf, f->bytes, 0))
                buf[0] = 0; /* 如果没有数据, 那就清空buf */
            if((f == t->fields) && (view->style & VIEW_STYLE_MARK_BAR)){
                /* 第一个字段可能需要特殊显示 */
                x += 3;
                w -= 6;
                y += 1;
                h -= 2;
            }
            /* 显示一个编辑框 */
            x -= 0;
            y += 2;
            w -= 2;
            h -= 6;
            lock_kernel();
            gui_init_rect(&t->__edit.real_rect, x, y, w+1, h+1);
            gui_refresh_widget(&t->__edit);
            gdc_set_myself_window_from_widget(view);
            gui_draw_edit(&t->__edit); /* 这句的作用是重新计算光标的位置的(并不是真的需要显示它) */
            gdc_set_myself_window(NULL);
            t->edit_flag = 2;
            unlock_kernel();
            key = gui_edit_input(&t->__edit, buf, f->bytes, 0);
            if(set_item)
                (void)set_item(index, f->id, buf, f->bytes, key, 0);
            lock_kernel();
            MEMSET(old_buf+f->old_buf_ptr, -1, f->bytes_for_width); /* 清空对应的OLD缓冲 */
            (old_buf+f->old_buf_ptr)[f->bytes_for_width] = 0;
            t->edit_flag = 1;
            unlock_kernel();
            switch(key){
                case LEFT:
                case ENTER:
                case HOME:
                case END:
                default:
                    (void)get_item(index, f->id, buf, f->bytes, 0);
                    lock_kernel();
                    gdc_set_myself_window_from_widget(view);
                    if(view->style & VIEW_STYLE_NONE_SELECT){
                        gdi_draw_box(x, y, x+w, y+h, t->data_bkcolor);
                        draw_font_ex(x+1, y+1, w, h, buf, t->data_color, t->data_bkcolor, view->font, (f->draw_style & ~DRAW_OPT_FIL_BG));
                    } else {
                        gdi_draw_box(x, y, x+w, y+h, VIEW_EDITED_BKCOLOR);
                        draw_font_ex(x+1, y+1, w, h, buf, VIEW_EDITED_COLOR, VIEW_EDITED_BKCOLOR, view->font, (f->draw_style & ~DRAW_OPT_FIL_BG));
                    }
                    gdc_set_myself_window(NULL);
                    unlock_kernel();
                    break;
            }
            switch(key){
                case LEFT:
                    i--;
                    if(i < 0)
                        i = t->fields_nr-1;
                    break;
                case ENTER:
                    i++;
                    if(i >= t->fields_nr)
                        i = 0;
                    break;
                case HOME:
                    i = 0;
                    break;
                case END:
                    i = t->fields_nr-1;
                    break;
                default:
                    continue_edit = 0;
                    break;
            }
        } else {
            break;
        }
    }
    
out:
    lock_kernel();
    t->edit_flag = 0;
    t->data_displayed = 0;
    unlock_kernel();

out_directly:
    if(key == NONEKEY)
        key = waitkey(0L);

    return key;
}

/*-----------------------------------------------------------------------------------------
 * 函数:    ____draw_blank_for_records()
 *
 * 描述:    由订单的选中状态变化而引起的背景重绘
**---------------------------------------------------------------------------------------*/
static void ____draw_blank_for_records(int x, int y, int x2, int y2, gui_view_private * t,
                                       COLOR bkcolor, COLOR marker_bkcolor, INT16U style)
{
    int j, to_left, zoom_out;

    to_left  = 2;
    zoom_out = 2;

    if(marker_bkcolor == 0)
        marker_bkcolor = WIDGET_BKCOLOR;
    
    j = 0;
    if(style & VIEW_STYLE_MARK_BAR){
        /* 第一个比较特殊 */
        gdi_draw_box(x+3, y+3, (t->x_width_fields[j]-to_left)+(x-3), y2-3, marker_bkcolor);
        x += t->x_width_fields[j];
        j ++;
    }
    /* 第二个也稍微有点特殊 */
    y+=zoom_out, y2-=zoom_out;
    gdi_draw_box(x+1, y, (t->x_width_fields[j]-to_left)+x, y2, bkcolor);
    x += t->x_width_fields[j];
    j ++;
    /* 后面的字段~~~*/
    for(; j<(t->fields_nr); j++){
        int tmp_x2 = (t->x_width_fields[j]-to_left)+x;
        if (tmp_x2 > x2)
            tmp_x2 = x2;
        gdi_draw_box(x, y, tmp_x2, y2, bkcolor);
        x += t->x_width_fields[j];
    }
}

/*-----------------------------------------------------------------------------------------
 * 函数:    gui_draw_view()
 *
 * 描述:    绘图
**---------------------------------------------------------------------------------------*/
void gui_draw_view(gui_widget * view)
{
/*lint --e{534}*/
    int x, y, x1, y1, move;
    COLOR color, bkcolor;
    int   font;
    gui_view_private * t;
    RECT * inner_rect;
    int i, _x, _y, __x, __y;
    COLOR  dashed_color;
    INT08U dashed_style;
    int fields_nr, records_nr;
    int height_per_row, __height_per_row;
    view_fields_t * ___f;
    view_get_item_f         get_item;
    view_show_record_f      show_record;
    view_get_max_index_f    get_max_index;
    view_show_statistics_f  show_statistics;
    view_notifier_f         notifier_on_changed;

    FamesAssert(view);
    if(!view)
        return;

    t = (gui_view_private *)view->private_data;
    if(!t)
        return;

    FamesAssert(t->fields);
    if(!t->fields)
        return;
    
    inner_rect = &view->inner_rect;
    bkcolor = t->data_bkcolor;
    font = view->font;

    fields_nr = t->fields_nr;
    ___f      = t->fields;
    get_item  = t->get_item;
    show_record = t->show_record;
    get_max_index = t->get_max_index;
    show_statistics = t->show_statistics;
    notifier_on_changed = t->notifier_on_changed;
    height_per_row = t->height_per_row;
    __height_per_row = height_per_row - 1; /* 一笔记录显示的高度 */
    
    if(view->flag & GUI_WIDGET_FLAG_REFRESH){
        x  = view->real_rect.x;
        y  = view->real_rect.y;
        x1 = __gui_make_x2(x, view->real_rect.width);
        y1 = __gui_make_y2(y, view->real_rect.height);
        dashed_color = t->dashed_color;
        dashed_style = t->dashed_style;
        if(dashed_color == 0)
            dashed_color = WIDGET_COLOR_TITLE_NORMAL;
        if(dashed_style == 0)
            dashed_style = 0x66;
        if(view->style & VIEW_STYLE_XP_BORDER){
            move = gui_widget_draw_xp_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        }
        if(view->style & VIEW_STYLE_MODAL_FRAME){
            move = gui_widget_draw_modal_frame(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        }
        if(view->style & VIEW_STYLE_SUBSIDE){
            move = gui_widget_draw_subside_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(!gdi_draw_box(x, y, x1, y1, bkcolor)){
            ;
        }
        if(view->style & VIEW_STYLE_NO_BORDER){
            ;
        } else {
            move = 1;
            gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR4);
            gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR4);
            gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR2);
            gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR2);
            ___gui_widget_xy_move(move);
            gdi_draw_h_line(x,  y,  x1, WIDGET_COLOR_NORMAL_BDR5);
            gdi_draw_v_line(x,  y,  y1, WIDGET_COLOR_NORMAL_BDR5);
            gdi_draw_h_line(x,  y1, x1, WIDGET_COLOR_NORMAL_BDR1);
            gdi_draw_v_line(x1, y,  y1, WIDGET_COLOR_NORMAL_BDR1);
            ___gui_widget_xy_move(move);
        }
        if(view->style & VIEW_STYLE_STATIC_BDR){
            move = gui_widget_draw_static_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        }
        x+=0; x1-=0; y+=0;
        gui_init_rect(inner_rect, x, y, (x1-x), (y1-y));

        __x = x;
        __y = y;

        bkcolor = view->bkcolor;
        if(bkcolor==0)
            bkcolor = WIDGET_BKCOLOR;

        records_nr = ((y1-y)+1)/height_per_row; /* 共可显示的记录数 */
        if(view->style & VIEW_STYLE_FIELDS_TITLE)
            records_nr--;
        if(records_nr <= 0){
            sys_print("No sapce to display records, check it!\n");
            return;
        }
        if(view->style & VIEW_STYLE_STATISTIC_BAR){
            t->nr_records_to_show = records_nr-1;
        } else {
            t->nr_records_to_show = records_nr;
        }
        if(t->nr_records_to_show > VIEW_MAX_RECORDS){
            t->nr_records_to_show = VIEW_MAX_RECORDS;
        }

        for(i=0; i<=fields_nr; i++){ /* 栏位标题 */
            int ___t1, ___t2;
            if(i < fields_nr){
                _x = x+t->x_width_fields[i];
                _x --;
            } else {
                _x = x1;
            }
            if(x >= x1)
                break;
            if(_x > x1)
                _x = x1;
            ___t1 = y;
            if(view->style & VIEW_STYLE_FIELDS_TITLE){
                move = gui_widget_draw_normal_bdr(x, y, _x, y+__height_per_row);
                if(!gdi_draw_box(x+move, y+move, _x-move, y+__height_per_row-move, bkcolor)){
                    ;
                }
                if(i < fields_nr){
                    draw_font_ex(x, (y+1), (_x-x), __height_per_row, t->fields_caption[i], view->color, bkcolor, font, DRAW_OPT_ALIGN_CENTER);
                }
                ___t1 += height_per_row;
            }
            ___t2 = ((height_per_row * records_nr) + ___t1) - 1; /* 虚线的Y坐标之y2 */
            if(i > 0 || !(view->style & VIEW_STYLE_MARK_BAR)){
                if(i < (fields_nr)){
                    ___t2 -= (view->style & VIEW_STYLE_STATISTIC_BAR)?height_per_row:0;
                }
                draw_vertical_dashed_line(_x, ___t1, ___t2, dashed_color, dashed_style);
            }
            x = _x+1;
        }
        x = __x;
        y = __y;
        if(view->style & VIEW_STYLE_FIELDS_TITLE){
            y += height_per_row;
        }
        t->data_x = x;
        t->data_y = y;
        for(i=0; i<records_nr; i++){ /* 记录标记 */
            int ___tmp;
            _y = y+__height_per_row;
            if(_y > y1)
                break;
            _x = x;
            ___tmp = (t->x_width_fields[0])-1;
            if(view->style & VIEW_STYLE_MARK_BAR){
                _x = (x+___tmp);
                move = gui_widget_draw_normal_bdr(x, y, _x, _y);
                if(!gdi_draw_box(x+move, y+move, _x-move, _y-move, bkcolor)){
                    ;
                }
            }
            if(i == (records_nr-1)){ /* 显示统计栏的CAPTION */
                if((view->style & VIEW_STYLE_STATISTIC_BAR) && (view->style & VIEW_STYLE_MARK_BAR))
                    draw_font_ex(x, y, ___tmp, height_per_row, t->statistics_caption, view->color, bkcolor, font, DRAW_OPT_ALIGN_CENTER);
            }
            if((view->style & VIEW_STYLE_STATISTIC_BAR) && i >= (records_nr-2)){
                gdi_draw_h_line(_x+1, _y, x1, dashed_color);
            } else {
                draw_horizontal_dashed_line(_x+1, _y, x1, dashed_color, dashed_style);
            }
            y = _y+1;
        }
        /* 清空旧缓冲区 */
        t->first_index_old = -1;
        t->selected_index_old = -1;
        if(t->selected_index_curr >= (t->first_record_index + t->nr_records_to_show)){
            t->first_record_index = (t->selected_index_curr - t->nr_records_to_show)+1;
        }
        t->statistic_buf_old[0] = 0;
        MEMSET((INT08S *)t->records_buf_old, 0, t->sizeof_records_buf_old);
        gui_refresh_widget(&t->__edit);
    } else { /* if(view->flag & GUI_WIDGET_FLAG_REFRESH) */
        int  first_record_index;
        int  first_index_old;
        int  selected_index_old;
        int  selected_index_curr;
        int  ___old, ___cur;
        int  to_left, ___tmp;
        char * old_buf_for_records;
        int   sizeof_old_buf_per_record;
        COLOR marker_color, marker_bkcolor;

        ___tmp = get_max_index();
        if(___tmp < t->max_index_old){
            if(t->selected_index_curr > ___tmp){
                gui_view_goto_bottom(view);
            }
        }
        t->max_index_old = ___tmp;

        if(t->selected_index_curr < 0)
            t->selected_index_curr = 0;
        if(t->first_record_index < 0)
            t->first_record_index = 0;
        if(t->first_record_index > t->selected_index_curr)
            t->first_record_index = t->selected_index_curr;

        #define __FIRST_ORDER       COLOR_WHITE
        #define __SELECT_ORDER      COLOR_YELLOW
        #define __NORMAL_ORDER      t->data_color
        #define __FIRST_ORDER_BK    COLOR_RED
        #define __SELECT_ORDER_BK   COLOR_BLUE
        #define __NORMAL_ORDER_BK   t->data_bkcolor

        #define records_oldbuf(i)  (old_buf_for_records+(i*sizeof_old_buf_per_record))

        old_buf_for_records       = (char *)t->records_buf_old;
        sizeof_old_buf_per_record = t->sizeof_old_buf_per_record;

        FamesAssert(old_buf_for_records);
        if(!old_buf_for_records)
            return;

        bkcolor = view->bkcolor;
        if(bkcolor==0)
            bkcolor = __NORMAL_ORDER_BK;
        
        first_record_index  = t->first_record_index;
        first_index_old     = t->first_index_old;
        selected_index_curr = t->selected_index_curr;
        selected_index_old  = t->selected_index_old;

        if(selected_index_curr != selected_index_old){
            if(notifier_on_changed)
                notifier_on_changed(selected_index_curr, selected_index_old, 0);
        }

        ___cur = (selected_index_curr - first_record_index);

        if(selected_index_old >= 0){
            ___old = (selected_index_old - first_index_old);
            
        } else {
            ___old = -1;
        }
        
        t->first_index_old = first_record_index;
        t->selected_index_old = selected_index_curr;

        __x  = t->data_x + 1;
        __y  = t->data_y;
        x = __x;
        y = __y;
        x1 = (inner_rect->x + inner_rect->width) - 1;
        records_nr = t->nr_records_to_show;

        to_left = 2;

        if(___cur != ___old){  /* 选中了另一笔订单 */
            if(___old >= 0){
                if(___old == 0 && first_record_index == 0){ /* 第一笔订单 */
                    bkcolor = __FIRST_ORDER_BK;
                } else {
                    bkcolor = __NORMAL_ORDER_BK;
                }
                if(view->style & VIEW_STYLE_NONE_FIRST){
                    bkcolor = __NORMAL_ORDER_BK;
                }
                MEMSET(records_oldbuf(___old), 0, sizeof_old_buf_per_record); /*lint !e679*/
                x  -= 1; /* 左边界 */
                y  += (___old * height_per_row); /* 上边界 */
                y1  = (y + height_per_row) - 2; /* 下边界 */
                ____draw_blank_for_records(x, y, x1, y1, t, bkcolor, view->bkcolor, view->style);
            }
            MEMSET(records_oldbuf(___cur), 0, sizeof_old_buf_per_record); /*lint !e679*/
            x = __x;  /* 还原x,y的值 */
            y = __y;
            if(view->style & VIEW_STYLE_NONE_SELECT){
                bkcolor = __NORMAL_ORDER_BK;
            } else {
                bkcolor = __SELECT_ORDER_BK;
            }
            x  -= 1;  /* 左边界 */
            y  += (___cur * height_per_row); /* 上边界 */
            y1  = (y + height_per_row) - 2;  /* 下边界 */
            ____draw_blank_for_records(x, y, x1, y1, t, bkcolor, view->bkcolor, view->style);
        }

        if(view->style & VIEW_STYLE_NONE_FIRST)
            goto skip__first_index_changed;
        if(first_record_index == 0){ /* 换到了第1页??? */
            x = __x; /* 更换页首引起的重绘 */
            y = __y;
            if(first_index_old != 0 && first_record_index != selected_index_curr){
                bkcolor = __FIRST_ORDER_BK;
                MEMSET(old_buf_for_records, 0, sizeof_old_buf_per_record);
                x  -= 1; /* 左边界 */
                y1  = (y + height_per_row) - 2; /* 下边界 */
                ____draw_blank_for_records(x, y, x1, y1, t, bkcolor, view->bkcolor, view->style);
            }
        }
        if(first_index_old == 0){ /* 换之前是第1页??? */
            x = __x; /* 更换页首引起的重绘 */
            y = __y;
            if(first_record_index != 0 && first_record_index != selected_index_curr){
                bkcolor = __NORMAL_ORDER_BK;
                MEMSET(old_buf_for_records, 0, sizeof_old_buf_per_record);
                x  -= 1; /* 左边界 */
                y1  = (y + height_per_row) - 2; /* 下边界 */
                ____draw_blank_for_records(x, y, x1, y1, t, bkcolor, view->bkcolor, view->style);
            }
        }
        skip__first_index_changed:
            
        x = __x; /* 还原x,y的值 */
        y = __y;

        for(i=0; i<records_nr; i++){
            if(i == ___cur) { /* 被选中的订单 */
                color   = __SELECT_ORDER;
                bkcolor = __SELECT_ORDER_BK;
                if(view->style & VIEW_STYLE_NONE_SELECT){ /* 无选中状态, 无特殊显示 */
                    color   = __NORMAL_ORDER;
                    bkcolor = __NORMAL_ORDER_BK;
                }
            } else if(first_record_index == 0 && i == 0){ /* 第一笔订单 */
                color   = __FIRST_ORDER;
                bkcolor = __FIRST_ORDER_BK;
                if(view->style & VIEW_STYLE_NONE_FIRST){  /* 无首笔状态, 无特殊显示 */
                    color   = __NORMAL_ORDER;
                    bkcolor = __NORMAL_ORDER_BK;
                }
            } else {
                color   = __NORMAL_ORDER;
                bkcolor = __NORMAL_ORDER_BK;
            }
            if(view->style & VIEW_STYLE_MARK_BAR){
                marker_color   = view->color;
                marker_bkcolor = view->bkcolor;
                if(marker_bkcolor == 0)
                    marker_bkcolor = WIDGET_BKCOLOR;
            } else {
                marker_color   = color;
                marker_bkcolor = bkcolor;
            }
            if(!show_record || (t->edit_flag && (first_record_index+i == selected_index_curr))){
                /* 如果没有提供show_record或者是正在编辑这一行, 那么就用这样的方式显示 */
                view_fields_t * f;
                char buf[256];
                if((t->edit_flag == 2) && (first_record_index+i == selected_index_curr) &&
                    !(view->style & VIEW_STYLE_NONE_SELECT)){ /* 在有选择模式下, 如果正在编辑这一行, 那么这一行不会自动刷新 */
                    gui_draw_edit(&t->__edit);
                    t->__edit.flag &= ~GUI_WIDGET_FLAG_REFRESH;
                } else {
                    for(f=___f; f->caption; f++){
                        if((t->edit_flag == 2) && (first_record_index+i == selected_index_curr) &&
                           (___f[t->current_field].id == f->id)){ /* 正在编辑这个元素, 那么就不自动刷新 */
                            gui_draw_edit(&t->__edit);
                            t->__edit.flag &= ~GUI_WIDGET_FLAG_REFRESH;
                        } else if(get_item){
                            MEMSET(buf, 0, sizeof(buf));
                            if(!get_item(first_record_index+i, f->id, buf, f->bytes, 0))
                                buf[0] = 0; /* 如果没有数据, 那就清空buf */
                            if((f == ___f) && (view->style & VIEW_STYLE_MARK_BAR)){
                                /* 第一个字段可能需要特殊显示 */
                                draw_font_for_widget(x+f->offset_x+3, y, (f->width-to_left)-6, height_per_row, buf, 
                                                     records_oldbuf(i)+f->old_buf_ptr, /*lint !e679*/
                                                     marker_color, marker_bkcolor, font, f->draw_style);
                            } else {
                                draw_font_for_widget(x+f->offset_x, y, (f->width-to_left), height_per_row, buf, 
                                                     records_oldbuf(i)+f->old_buf_ptr, /*lint !e679*/
                                                     color, bkcolor, font, f->draw_style);
                            }
                        }
                    }
                }
            } else {
                /* 在这里, show_record!=NULL */
                show_record(first_record_index+i, i, x, y, to_left, height_per_row,
                            color, bkcolor, font, marker_color, marker_bkcolor,
                            records_oldbuf(i), t->x_width_fields, 0); /*lint !e679*/
            }
            y += height_per_row;
        }
        if(view->style & VIEW_STYLE_STATISTIC_BAR){
            if(view->style & VIEW_STYLE_MARK_BAR)
                x += (t->x_width_fields[0]+3);
            if(show_statistics){
                show_statistics(selected_index_curr, x, y, (x1-x), height_per_row, t->data_color, 
                                t->data_bkcolor, font, t->fields[t->current_field].comment, t->statistic_buf_old, 0);
            }
        }
        t->data_displayed = 1; /* 数据已显示过 */
    }

    return;
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/view.c
 * 
**=======================================================================================*/


