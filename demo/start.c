/*************************************************************************************
 * 文件:    start.c
 *
 * 说明:    应用程序启始例程
 *
 * 作者:    Jun
 *
 * 时间:    2011-2-19
*************************************************************************************/
#define  APP_START_C
#include <FamesOS.h>


void __task demo_task(void * data)
{
    int i;

    i = (int)data;

    for(;;){
        lock_kernel();
        printf("Task Data: %d\n", i);
        unlock_kernel();
        TaskDelay(100*i);
    }
}

static FONTINFO font_asc_16 = {NO, FONT_TYPE_ASCII,   0,  8, 16, 1,  0,  "ASC16",     NULL};
static FONTINFO font_hz_16  = {NO, FONT_TYPE_GB2312,  0, 16, 16, 2,  0,  "HZK16",     NULL};
static FONTINFO font_asc_48 = {NO, FONT_TYPE_ASCII,   0, 24, 48, 3, 32,  "ASC48",     NULL};

int font16, font48;

void load_fonts(void)
{
    load_font(&font_asc_16);
    load_font(&font_hz_16);
    load_font(&font_asc_48);

    font16 = register_font(&font_hz_16, &font_asc_16);
    font48 = register_font(NULL, &font_asc_48);
}

enum {
    __id_name1,
    __id_sex1,
    __id_old1,
    __id_sss1,
    __id_name2,
    __id_sex2,
    __id_old2,
    __id_sss2,
    __id_name3,
    __id_sex3,
    __id_old3,
    __id_sss3,
};

DECLARE_INPUT_DIALOG_START(abcde)
INPUT_DIALOG_ITEM("姓名: ", __id_name1, 8, 10,   40, 120, 32, 0, 0, "student name")
INPUT_DIALOG_ITEM("性别: ", __id_sex1,  8, 10,   80, 120, 32, 0, 0, "sex: M or F")
INPUT_DIALOG_ITEM("年龄: ", __id_old1,  8, 10,  120, 120, 32, 0, 0, "years old?")
INPUT_DIALOG_ITEM("姓名: ", __id_name2, 8, 150,  40, 120, 32, 0, 0, "student name")
INPUT_DIALOG_ITEM("性别: ", __id_sex2,  8, 150,  80, 120, 32, 0, 0, "M/F")
INPUT_DIALOG_ITEM("年龄: ", __id_old2,  8, 150, 120, 120, 32, 0, 0, "how old are you?")
INPUT_DIALOG_ITEM("姓名: ", __id_name3, 8, 280,  40, 160, 32, 0, 0, "what's your name")
INPUT_DIALOG_ITEM("性别: ", __id_sex3,  8, 280,  80, 160, 32, 0, 0, "boy or girl? M/F")
INPUT_DIALOG_ITEM("年龄: ", __id_old3,  8, 280, 120, 160, 32, 0, 0, "how old are you?")
INPUT_DIALOG_SET(abcde, "输入对话框演示", NULL, 64, 283, 500, 250, 0, 0, 0, FORM_STYLE_TITLE|FORM_STYLE_XP_BORDER);
DECLARE_INPUT_DIALOG_ENDED(abcde)

gui_widget * form;
gui_widget * edit;
gui_widget * test;
gui_widget * view;

gui_widget * label;
gui_widget * button;

gui_widget * progress1;

gui_widget * dialog;

gui_window_t * window1 = NULL;
gui_window_t * window2 = NULL;
gui_window_t * window3 = NULL;

BMPINFO icon;
    
INT16U style = 0;
COLOR  color = 0, bkcolor = 0;

enum {
    __id_name,
    __id_sex,
    __id_old,
    __id_sss,
};

#define __style DRAW_OPT_ALIGN_CENTER|DRAW_OPT_FIL_BG

view_fields_t fields[] = {
    { "姓名", __id_name, 8,  9, 0, __style, "学生的姓名，4个汉字",},
    { "性别", __id_sex,  1,  5, 0, __style, "M/F",},
    { "年龄", __id_old,  3,  6, 0, __style, "",},
    { "备注", __id_sss, 28, 30, 0, DRAW_OPT_FIL_BG, "学生备注",},
    { NULL, },
};

struct student
{
    char name[10];
    char sex;
    int  old;
    char comment[128];
    struct student * next;
};

struct student * all_students = NULL;

int get_max_index(void)
{
    int i = -1;
    struct student * t;

    t = all_students;

    lock_kernel();
    while(t){
        i++;
        t = t->next;
    }
    unlock_kernel();

    return i;
}

BOOL get_item(int index, int field_id, char *buf, int buf_len, INT16U option)
{
    struct student * t;
    int i;

    FamesAssert(buf);

    if(index > get_max_index())
        return fail;

    t = all_students;
    for(i=0; i<index; i++){
        if(t == NULL)
            return fail;
        t = t->next;
    }
    if(t == NULL)
            return fail;

    switch(field_id){
        case __id_name:
            strcpy(buf, t->name);
            break;
        case __id_sex:
            sprintf(buf, "%c", t->sex);
            break;
        case __id_old:
            sprintf(buf, "%d", t->old);
            break;
        case __id_sss:
            sprintf(buf, "随机数: %d", rand());
            break;
        default:
            break;
    }

    return ok;
}

BOOL set_item(int index, int field_id, char *buf, int buf_len, KEYCODE key, INT16U option)
{
    struct student * t;
    int i;

    FamesAssert(buf);

    if(index > get_max_index())
        return fail;

    t = all_students;
    for(i=0; i<index; i++){
        if(t == NULL)
            return fail;
        t = t->next;
    }
    
    if(t == NULL)
            return fail;

    switch(field_id){
        case __id_name:
            strcpy(t->name, buf);
            break;
        case __id_sex:
            t->sex = buf[0];
            break;
        case __id_old:
            t->old = atoi(buf);
            break;
        case __id_sss:
            sprintf(t->comment, "%s", buf);
            break;
        default:
            break;
    }

    return ok;
}

BOOL is_writable(int index, int field_id, INT16U option)
{
    return ok;
}

void show_status(int index, int x, int y, int width, int height, 
                                    int color, int bkcolor, int font,
                                    INT08S *field_comment,
                                    INT08S *old, INT16U option)
{
    char s[64];

    sprintf(s, "编号: %d, 栏位: %s", index+1, field_comment);
    
    draw_font_for_widget(x, y, width, height, s, old, 189, bkcolor, font, DRAW_OPT_FIL_BG);
}

void notifier(int index, int prev_index, INT16U option)
{
    int i;
    long i32;

    if(get_max_index() < 0)
        return;

    i = index+1;

    i32 = (long)i * 100; /* 这里必须用long型数据, 不然当i>327的时候就会变成负数 */
    i = (int)(i32/(get_max_index()+1));

    gui_progress_set_value(progress1, i);

    gui_edit_set_text(edit, "又修改了一次");
    
}

void init_view(gui_widget * view)
{
    gui_view_init_private(view, 
                          fields, 
                          get_max_index, 
                          get_item, 
                          set_item, 
                          is_writable, 
                          NULL, 
                          "统计",
                          show_status,
                          notifier,
                          90,
                          66,
                          30,
                          1);
}

void dialog_prepare(int id, char * buf, void * data, INT16U opt)
{
    struct student * t;
    int index, i;

    switch (id) {
        case __id_name1:
        case __id_sex1:
        case __id_old1:
        case __id_sss1:
            index = 0;
            break;
        case __id_name2:
        case __id_sex2:
        case __id_old2:
        case __id_sss2:
            index = 1;
            break;
        case __id_name3:
        case __id_sex3:
        case __id_old3:
        case __id_sss3:
            index = 2;
            break;
        default:
            buf[0] = 0; /* clear the output buf */
            return;
    }

    t = all_students;   

    for(i=0; i<index; i++){
        if(t == NULL)
            break;
        t = t->next;
    }
    if(t == NULL) {
        buf[0] = 0;
        return;
    }

    switch (id) {
        case __id_name1:
        case __id_name2:
        case __id_name3:
            strcpy(buf, t->name);
            break;
        case __id_sex1:
        case __id_sex2:
        case __id_sex3:
            sprintf(buf, "%c", t->sex);
            break;
        case __id_old1:
        case __id_old2:
        case __id_old3:
            sprintf(buf, "%d", t->old);
            break;
        case __id_sss1:
        case __id_sss2:
        case __id_sss3:
            sprintf(buf, "%s", t->comment);
            break;
        default:
            break;
    }

    return;
}

int dialog_finish(int id, char * buf, void * data, KEYCODE key)
{
    struct student * t;
    int index, i;

    switch (id) {
        case __id_name1:
        case __id_sex1:
        case __id_old1:
        case __id_sss1:
            index = 0;
            break;
        case __id_name2:
        case __id_sex2:
        case __id_old2:
        case __id_sss2:
            index = 1;
            break;
        case __id_name3:
        case __id_sex3:
        case __id_old3:
        case __id_sss3:
            index = 2;
            break;
        default:
            return 1;
    }

    t = all_students;   

    for(i=0; i<index; i++){
        if(t == NULL)
            break;
        t = t->next;
    }
    if(t == NULL)
            return 1;

    switch (id) {
        case __id_name1:
        case __id_name2:
        case __id_name3:
            strcpy(t->name, buf);
            break;
        case __id_sex1:
        case __id_sex2:
        case __id_sex3:
            t->sex = toupper(buf[0]);
            if(t->sex != 'M' && t->sex != 'F')
                return 0;
            break;
        case __id_old1:
        case __id_old2:
        case __id_old3:
            t->old = atoi(buf);
            break;
        case __id_sss1:
        case __id_sss2:
        case __id_sss3:
            strcpy(t->comment, buf);
            break;
        default:
            break;
    }

    return 1;
}

void demo_init_gui(void)
{
    InitBMPINFO(&icon);
    LoadBmp(&icon, "icon.bmp");
    load_fonts();

    gui_desktop_enable();
    gui_desktop_set_color(CLRSCR_COLOR2);
    
    form = gui_create_widget(GUI_WIDGET_FORM, 180, 120, 632, 500, 0, 0, 0, FORM_STYLE_XP_BORDER|FORM_STYLE_TITLE);
    if(!form)
        goto err;
    gui_form_init_private(form, 128);
    gui_form_set_caption(form, "控件演示");
    gui_form_set_icon(form, &icon);

    edit = gui_create_widget(GUI_WIDGET_EDIT, 80, 270, 460, 48, color, bkcolor, 0, style);
    if(!edit)
        goto err;
    gui_edit_init_private(edit, 128);
    gui_edit_set_text(edit, "这是一个文本框");

    button = gui_create_widget(GUI_WIDGET_BUTTON, 235+156, 142+100, 236, 32, COLOR_YELLOW, 64, font16, 0); /*BUTTON_STYLE_CLIENT_BDR*/
    if(!edit)
        goto err;
    gui_button_init_private(button, 128);
    gui_button_set_caption(button, "将那个Label盖住");

    test = gui_create_widget(GUI_WIDGET_EDIT, 80, 330, 460, 32, 0, 0, 0, 0);
    if(!test)
        goto err;
    gui_edit_init_private(test, 128);
    gui_edit_set_text(test, "");

    label = gui_create_widget(GUI_WIDGET_LABEL, 80, 380, 460, 64, COLOR_WHITE, 66, font48, LABEL_STYLE_SUBSIDE);
    if(!edit)
        goto err;
    gui_label_init_private(label, 128);
    gui_label_set_text(label, "Startting...");

    view = gui_create_widget(GUI_WIDGET_VIEW, 80, 48, 460, 200, 0, 0, 0, VIEW_STYLE_NONE_FIRST|VIEW_STYLE_FIELDS_TITLE|VIEW_STYLE_STATISTIC_BAR);
    if(!view)
        goto err;
    init_view(view);

    progress1 = gui_create_widget(GUI_WIDGET_PROGRESS, 80, 251, 460, 14, 0, 0, 0, PROGRESS_STYLE_BORDER);
    if(!progress1)
        goto err;
    gui_progress_init_private(progress1, 100);

    dialog = input_dialog_initialize(&abcde);
    if(!dialog)
        goto err;

    gui_widget_link(NULL, form);
    gui_widget_link(form, view);
    gui_widget_link(form, edit);
    gui_widget_link(form, label);
    gui_widget_link(form, progress1);
    gui_widget_link(form, test);

    gui_widget_link(NULL, dialog);

    gui_widget_link(NULL, button);

    window1 = gui_create_window(form);
    gui_show_window(window1);

    window2 = gui_create_window(dialog);
    gui_show_window(window2);

    window3 = gui_create_window(button);
    gui_show_window(window3);

    gui_form_set_caption(form, os_get_description());

    StartGUI();
    
err:
    return;
}

int moveto_x, moveto_y, moveto_now = 0;

void __task refresh_task(void * data)
{
    long count = 0;
    char buf[64];
    int flag = 0;

    data = data;

    for (;;) {
        sprintf(buf, "[%d%%] %ld $ %ld", CPU_USED, count, 50000-count*3);
        count++;
        if (count >= 5000)
            count = 1;
        gui_label_set_text(label, buf);

        #if 0
        gui_form_set_caption(form, buf);
        #endif

        sprintf(buf, "[%d%%] %ld 将那个Label盖住", CPU_USED, count*3);
        gui_button_set_caption(button, buf);

        if (0) {
            static int f = 0, i = 0;
            static KEYCODE key, test_keys[] = {'d', ENTER, ESC, 'f', ENTER, 'h', ENTER, ESC};

            if (--f <= 0) {
                f = 1;
                /* key = test_keys[i++]; */
                key = (random(128) * random(128)) % 128;
                if (key < 0x10 && key != ESC) {
                    key += 0x10;
                }
                putkey(key);
                if (i >= sizeof(test_keys))
                    i = 0;
            }
        }

        if (1) {
            static int f = 0, x = 0, y = 0;

            if (--f <= 0) {
                f = 20;
                x = random(900);
                y = random(700);
                gui_window_moveto(window3, x, y);
            }
        }        
	
        if (1) {
            static int f = 0, x = 0;
            char buf[32];

            if (--f <= 0) {
                f = 10;
                x = random(20000);
                sprintf(buf, "random() = %d", x);
                gui_edit_set_text(edit, buf);
            }
        }

        if (moveto_now) {
            moveto_now = 0;
            gui_window_moveto(window3, moveto_x, moveto_y);
        }

        TaskSleep(20);
    }
}

void move_button(void)
{
    static int x, y, flag = 0;

    if (flag) {
        moveto_x = x;
        moveto_y = y;
        moveto_now = 1;
    } else {
        x = window3->root_widget->real_rect.x;
        y = window3->root_widget->real_rect.y;
        moveto_x = 100;
        moveto_y = 640;
        moveto_now = 1;
    }

    flag = !flag;
}

/*------------------------------------------------------------------------------------
 * 函数:    start()
 *
 * 描述:    应用程序启始例程
**----------------------------------------------------------------------------------*/
void __task start(void * data)
{
    data = data;

    /*
    TaskCreate(demo_task, (void *)1, "demo1", NULL, 512, 1, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)2, "demo2", NULL, 512, 2, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)3, "demo3", NULL, 512, 3, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)4, "demo4", NULL, 512, 4, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)5, "demo5", NULL, 512, 5, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)6, "demo6", NULL, 512, 6, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)7, "demo7", NULL, 512, 7, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)8, "demo8", NULL, 512, 8, TASK_CREATE_OPT_NONE);
    */

    demo_init_gui();
    TaskCreate(refresh_task, NULL, "refresh", NULL, 2048, 8, TASK_CREATE_OPT_NONE);

    OpenConsole();

    RegisterSpecialKey('x', move_button);

    for(;;){
        KEYCODE key;
        char buf[64] = {0, };

        key = gui_edit_input(test, buf, 32, 0);
        /* use superkey alt-x-k to quit! */
        switch(key){
            case UP:
                gui_view_move_up(view);
                break;
            case DOWN:
                gui_view_move_down(view);
                break;
            case PGUP:
                gui_view_page_up(view);
                break;
            case PGDN:
                gui_view_page_down(view);
                break;
        }
        switch(buf[0]){
            default:
                break;
            case 'c':
            case 'C':
                color = atoi(&buf[1]);
                gui_set_widget_color(edit, color);
                break;
            case 'b':
            case 'B':
                bkcolor = atoi(&buf[1]);
                gui_set_widget_bkcolor(edit, bkcolor);
                break;
            case 's':
            case 'S':
                style = STRtoINT16(&buf[1], 0);
                gui_set_widget_style(edit, style);
                break;
            case 'p':
            case 'P':
                /*
                gui_progress_set_value(progress1, atoi(&buf[1]));
                */
                break;
            case 'a':
            case 'A':
                {
                     struct student * t;

                     t = mem_alloc(sizeof(*t));
                     if(!t)
                        break;
                     sprintf(t->name, "Name");
                     t->sex = '-';
                     t->old = 0;
                     sprintf(t->comment, "");
                     lock_kernel();
                     t->next = all_students;
                     all_students = t;
                     unlock_kernel();
                }
                break;
            case 'd':
            case 'D':
                /*gui_show_window(window2);*/
                input_dialog_method(&abcde, dialog_prepare, dialog_finish, NULL, 1);
                {
                    static COLOR tmp = 67;
                    gui_desktop_set_color(tmp);
                    tmp += 77;
                }
                TaskSleep(10);
                /*gui_hide_window(window2);*/
                break;
            case 'h':
            case 'H':
                gui_hide_window(window3);
                break;
            case 'f':
            case 'F':
                gui_show_window(window3);
                break;
            case 'm':
            case 'M':
                while (1) {
                    gui_window_t * w;
                    KEYCODE move;
                    if (buf[0] == 'm')
                        w = window2;
                    else
                        w = window3;
                    move = getkey();
                    switch (move) {
                        case ESC:
                            goto out_while;
                        case LEFT:
                            gui_window_move(w, -23, 0);
                            break;
                        case RIGHT:
                            gui_window_move(w, 13, 0);
                            break;
                        case UP:
                            gui_window_move(w, 0, -11);
                            break;
                        case DOWN:
                            gui_window_move(w, 0, 23);
                            break;
                        case CTRL_LEFT:
                            gui_window_resize(w, -23, 0);
                            break;
                        case CTRL_RIGHT:
                            gui_window_resize(w, 13, 0);
                            break;
                        case CTRL_UP:
                            gui_window_resize(w, 0, -11);
                            break;
                        case CTRL_DOWN:
                            gui_window_resize(w, 0, 23);
                            break;
                    }
                }
                out_while:
                break;
            case 'e':
            case 'E':
                while(1){
                    key = gui_view_editing(view, 0);
                    if(key == ESC)
                        break;
                    switch(key){
                        case UP:
                            gui_view_move_up(view);
                            break;
                        case DOWN:
                            gui_view_move_down(view);
                            break;
                        default:
                            break;
                    }
                }
                break;
        }
    }
}

/*------------------------------------------------------------------------------------
 * 函数: TaskSwitchHook()
 *
 * 说明: 任务切换钩子
 *
 * 特别: 此函数运行在中断服务程序之中,应特别注意其执行效率
**----------------------------------------------------------------------------------*/
void apical TaskSwitchHook(void)
{
}

void quit(void)
{
    ExitApplication();
}

long get_free_mem(void)
{
    long mem;

    lock_kernel();
    mem = (long)coreleft();
    unlock_kernel();

    return mem;
}

/*------------------------------------------------------------------------------------
 *  取系统的时间日期
**----------------------------------------------------------------------------------*/
void GetDateTime (INT08S *s)
{
    struct time now;
    struct date today;

    lock_kernel();
    gettime(&now);
    getdate(&today);
    unlock_kernel();
    sprintf(s, "%02d-%02d-%02d  %02d:%02d:%02d",
            today.da_mon,
            today.da_day,
            today.da_year,
            now.ti_hour,
            now.ti_min,
            now.ti_sec);
}


/*====================================================================================
 * 
 * 本文件结束: start.c
 * 
**==================================================================================*/

