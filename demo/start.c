/*************************************************************************************
 * �ļ�:    start.c
 *
 * ˵��:    Ӧ�ó�����ʼ����
 *
 * ����:    Jun
 *
 * ʱ��:    2011-2-19
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
 * ����:    start()
 *
 * ����:    Ӧ�ó�����ʼ����
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
 * ����: TaskSwitchHook()
 *
 * ˵��: �����л�����
 *
 * �ر�: �˺����������жϷ������֮��,Ӧ�ر�ע����ִ��Ч��
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
 *  ȡϵͳ��ʱ������
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
 * ���ļ�����: start.c
 * 
**==================================================================================*/

