/******************************************************************************************
 * 文件:    gui/widget/sys_mntr.c
 *
 * 描述:    系统监控控件(System Monitor)
 *
 * 作者:    Jun
******************************************************************************************/
#define  FAMES_GUI_WIDGET_SYS_MNTR_C
#include "includes.h"



long get_free_mem(void);

static INT08S __line[] = "-------------------------------------------"
                         "-------------------------------------------"
                         "----------------------";

/*-----------------------------------------------------------------------------------------
 * 
 *      SYS_MNTR私有结构
 * 
**---------------------------------------------------------------------------------------*/
struct gui_sys_mntr_private_s {
    INT08S title[48];       /* System Monitor */
    INT08S field[100];      /* handle buffer bottom...*/
    INT08S lines[2][sizeof(__line)+2];
    INT08S data[MAX_TASKS][128];
    INT08S statistics[128];
    INT08S os_info[64];
    INT16S tasks_old;
    INT08S running_time[64];
};

typedef struct gui_sys_mntr_private_s gui_sys_mntr_private;

/*-----------------------------------------------------------------------------------------
 * 
 * 
 * 
**---------------------------------------------------------------------------------------*/
BOOL guical gui_sys_mntr_init_private(gui_widget * sys_mntr)
{
    int bytes;
    INT08S * buf;
    gui_sys_mntr_private * t, * t2;;
    
    FamesAssert(sys_mntr);

    if(!sys_mntr)
        return fail;

    FamesAssert(sys_mntr->type == GUI_WIDGET_SYS_MNTR);

    if(sys_mntr->type != GUI_WIDGET_SYS_MNTR)
        return fail;

    bytes = (int)sizeof(gui_sys_mntr_private);

    buf = (INT08S *)mem_alloc((INT32U)(INT32S)bytes);

    if(buf){
        MEMSET(buf, 0, bytes);
        t = (gui_sys_mntr_private *)buf;/*lint !e826*/
        if(sys_mntr->private_data){
            lock_kernel();
            t2 = sys_mntr->private_data;
            sys_mntr->private_data = NULL;
            unlock_kernel();
            mem_free(t2);
        }
        lock_kernel();
        sys_mntr->private_data = (void *)t;
        unlock_kernel();
        return ok;
    } else {
        return fail;
    }
}

static char * ___get_state_str(INT16U state)
{
    if(state & TASK_STATE_DORMANT)
        return "DORMT";

    if(state & TASK_STATE_SUSPEND)
        return "SUSPD";

    if(state & TASK_STATE_WAIT)
        return "WAIT ";

    if(state & TASK_STATE_SLEEP)
        return "SLEEP";

    return "     ";
}

void gui_draw_sys_mntr(gui_widget * sys_mntr)
{
    int i, x, y, x1, y1, move;
    COLOR bkcolor, color;
    gui_sys_mntr_private * t;
    RECT  *inner_rect;
    INT08S str[128];

    FamesAssert(sys_mntr);

    if(!sys_mntr)
        return;

    t = (gui_sys_mntr_private *)sys_mntr->private_data;
    if(!t)
        return;

    inner_rect = &sys_mntr->inner_rect;
    color   = sys_mntr->color;
    bkcolor = sys_mntr->bkcolor;
    if(bkcolor==0)
        bkcolor = WIDGET_BKCOLOR;

    lock_kernel();
    if(sys_mntr->flag & GUI_WIDGET_FLAG_NEED_REFRESH || (t->tasks_old!=NumberOfTasks)){
        if(sys_mntr->real_rect.width < 780)
            sys_mntr->real_rect.width = 780; /* 780与640是一个经验值, 可修改 */
        if(sys_mntr->real_rect.height < 640)
            sys_mntr->real_rect.height = 640;
        x  = sys_mntr->real_rect.x;
        y  = sys_mntr->real_rect.y;
        x1 = __gui_make_x2(x, sys_mntr->real_rect.width);
        y1 = __gui_make_y2(y, sys_mntr->real_rect.height);
        if(sys_mntr->style & SYS_MNTR_STYLE_TRANSPARENT){
            goto goto1;
        }        
        if(sys_mntr->style & SYS_MNTR_STYLE_MODAL_FRAME){
            move = gui_widget_draw_modal_frame(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(sys_mntr->style & SYS_MNTR_STYLE_CLIENT_BDR){
            move = gui_widget_draw_client_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(sys_mntr->style & SYS_MNTR_STYLE_STATIC_BDR){
            move = gui_widget_draw_static_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        } 
        if(sys_mntr->style & SYS_MNTR_STYLE_NO_BORDER){
            ;/* Transparent, do nothing here */
        } else {
            move = gui_widget_draw_subside_bdr(x, y, x1, y1);
            ___gui_widget_xy_move(move);
        
        }
        if(!gdi_draw_box(x, y, x1, y1, bkcolor)){
            ;
        }
    goto1:
        gui_init_rect(inner_rect, x, y, (x1-x)+1, (y1-y)+1);
        MEMSET((INT08S *)t, 0, sizeof(*t));
        t->tasks_old = NumberOfTasks;
    } else {
        INT32S s[6];
        
        x  = sys_mntr->real_rect.x + 12;
        y  = sys_mntr->real_rect.y + 5;
        sprintf(str, "System Monitor");
        draw_font_for_widget(x+180, y, 0, 0, str, t->title, color, bkcolor, 2, DRAW_OPT_FIL_BG);
        y += 64;
        sprintf(str, "handle  buffer     bottom     pointer      total    used     remain   state   prio   name ");
        draw_font_for_widget(x, y, 0, 0, str, t->field, color, bkcolor, 0, DRAW_OPT_FIL_BG);
        y += 14;
        sprintf(str, __line);
        draw_font_for_widget(x, y, 0, 0, str, t->lines[0], color, bkcolor, 0, DRAW_OPT_FIL_BG);
        y += 14;
        for(i=0; i<MAX_TASKS; i++){
            if(TCBS[i].TcbUsed != TCB_USED_YES)continue;
            s[0] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_GET_BUF);
            s[1] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_GET_BTM);
            s[2] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_GET_PTR);
            s[3] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_TOTAL);
            s[4] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_USED);
            s[5] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_REMAIN);
            sprintf(str, "  %2d    %p  %p  %p    %-6ld   %-6ld %c %-6ld   %s   %-6d %s", 
                         i, (void *)s[0], (void *)s[1], (void *)s[2], s[3], s[4], ((s[5]<32L)?'*':' '), s[5], 
                         ___get_state_str(TCBS[i].TaskState),
                         TCBS[i].Priority, TCBS[i].TaskName);
            draw_font_for_widget(x, y, 0, 0, str, t->data[i], color, bkcolor, 0, DRAW_OPT_FIL_BG);
            y += 14;
        }
        y += 6;
        sprintf(str, "tasks: %-4d  cpu: %2d%%    free-memory: %-8ld  switches: %-8ld  milli-seconds: %ld  ",
                     NumberOfTasks, CPU_USED, get_free_mem(), TaskSwitches, SecondsFromStart);
        draw_font_for_widget(x, y, 0, 0, str, t->statistics, color, bkcolor, 0, DRAW_OPT_FIL_BG);
        y += 14;
        sprintf(str, __line);
        draw_font_for_widget(x, y, 0, 0, str, t->lines[1], color, bkcolor, 0, DRAW_OPT_FIL_BG);
        y += 26;
        sprintf(str, "%s-%s", os_get_name(), os_get_version_string());
        draw_font_for_widget(x, y, 0, 0, str, t->os_info, 206, bkcolor, 2, DRAW_OPT_FIL_BG);
        y += 70;
        if(1){ /*lint !e506 !e774*/
            int day, hour, minute, second;
            INT32U v;
            prepare_atomic()
            in_atomic();
            v = SecondsFromStart;
            out_atomic();
            v /= 1000L;              /* 毫秒 => 秒 */
            second = (int)(v%60L);   /* 秒数       */
            v /= 60L;                /* 秒 => 分   */
            minute = (int)(v%60L);   /* 分钟数     */
            v /= 60L;                /* 分 => 时   */
            hour   = (int)(v%24L);   /* 小时数     */
            v /= 24L;                /* 小时 => 天 */
            day    = (int)v;
            sprintf(str, "Running:  %d Day(s)  %d:%02d %02d", 
                    day, hour, minute, second);
            draw_font_for_widget(x, y, sys_mntr->real_rect.width - 64, 0, str, t->running_time, color, bkcolor, 2, DRAW_OPT_FIL_BG);
            y += 32;
        }
    }
    unlock_kernel();
}


/*=========================================================================================
 * 
 * 本文件结束: gui/widget/sys_mntr.c
 * 
**=======================================================================================*/


