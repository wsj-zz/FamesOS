/*************************************************************************************
 * �ļ�: monitor\stack.c
 *
 * ����: ϵͳ���֮��ջ���
 *
 * ����: Jun
 *
 * ʱ��: 2010-12-6
 *
 * �汾: ��1��
 *
 * ˵��: �ռ�ϵͳ����������Ķ�ջ��Ϣ��������ļ�stack.txt
*************************************************************************************/
#define  FAMES_MONITOR_STACK_C
#include "includes.h"

#if  FAMES_MONITOR_STACK_EN == 1
/*------------------------------------------------------------------------------------
 *      ��ض���
**----------------------------------------------------------------------------------*/
static INT08S * output_file = "stack.txt";
static INT08S * servic_name = "monitor_stack";

    
/*------------------------------------------------------------------------------------
 * ����: stack_monitor()
 *
 * ˵��: �����ջ������
**----------------------------------------------------------------------------------*/
void stack_monitor(void)
{
    FILE * fp;
    INT32S s[12];
    INT16S i;
    long get_free_mem(void);
    
    fp = fopen(output_file, "a+t");
    if(fp){
        fprintf(fp, "handle  buf        btm        ptr          total    used     remain   state   prio   name \n");
        for(i=0; i<MAX_TASKS; i++){
            if(TCBS[i].TcbUsed != TCB_USED_YES)continue;
            s[0] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_GET_BUF);
            s[1] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_GET_BTM);
            s[2] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_GET_PTR);
            s[3] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_TOTAL);
            s[4] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_USED);
            s[5] = (INT32S)TaskStackCheck(i, STACK_CHECK_OPT_REMAIN);
            fprintf(fp, "  %-6d%p  %p  %p    %-6ld   %-6ld %c %-6ld   %s   %-6d %s\n", i, (void*)s[0], (void*)s[1], (void*)s[2], s[3], s[4], ((s[5]<32L)?'*':' '), s[5], ((TCBS[i].TaskState==0)?"     ":"SLEEP"),TCBS[i].Priority, TCBS[i].TaskName);
        }
        fprintf(fp, "\ntasks: %-4d free-memory: %-8ld  switches: %-8ld  seconds-from-start: %ld\n",
                     NumberOfTasks, get_free_mem(), TaskSwitches, SecondsFromStart);
        fprintf(fp, "------------------------------------------------------------------------------------------------------\n");
        fclose(fp); 
    } else {
        printf("!!! stack_monitor: file open error! !!!\n");
    }
}

/*------------------------------------------------------------------------------------
 * ����: monitor_open_stack_chk()
 *
 * ˵��: �������ջ������
**----------------------------------------------------------------------------------*/
void apical monitor_open_stack_chk(void)
{
    if(!RegisterMonitorService(servic_name, stack_monitor, 1000uL)){
        sys_print("monitor_open_stack_chk: failed to register stack-chk svc\n");
    }
}


#endif /* #if FAMES_MONITOR_STACK_EN==1 */

/*====================================================================================
 * 
 * ���ļ�����: monitor\stack.c
 * 
**==================================================================================*/

