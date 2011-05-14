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


/*------------------------------------------------------------------------------------
 * 函数:    start()
 *
 * 描述:    应用程序启始例程
**----------------------------------------------------------------------------------*/
void __task start(void * data)
{
    data = data;

    TaskCreate(demo_task, (void *)1, "demo1", NULL, 512, 1, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)2, "demo2", NULL, 512, 2, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)3, "demo3", NULL, 512, 3, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)4, "demo4", NULL, 512, 4, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)5, "demo5", NULL, 512, 5, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)6, "demo6", NULL, 512, 6, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)7, "demo7", NULL, 512, 7, TASK_CREATE_OPT_NONE);
    TaskCreate(demo_task, (void *)8, "demo8", NULL, 512, 8, TASK_CREATE_OPT_NONE);

    OpenConsole();

    for(;;){
        KEYCODE key;

        key = waitkey(0L);

        lock_kernel();
        printf("Key Pressed: %04X\n", key);
        unlock_kernel();
        if(key == ESC)
            ExitApplication();
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

