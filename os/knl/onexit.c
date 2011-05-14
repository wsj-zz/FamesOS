/***********************************************************************************************
** �ļ�:    onexit.c
**
** ˵��:    FamesOS�˳����̴���
**
** ����:    Jun
**
** ʱ��:    2010-6-28(����)
**
** ��ע:    �˳�������ָһ��ģ����ϵͳ�˳�ʱӦ��ִ�еĶ���, ��OnExit�ṩ��
**          ��֯������(����)��Щ�����Ĺ���
***********************************************************************************************/
#define  FAMES_ON_EXIT_C
#include "includes.h"

#if FAMES_ON_EXIT_EN == 1
/*----------------------------------------------------------------------------------------------
 *   OnExit���ݽṹ��������������:
 *
 *   onexit_s:       OnExit���ݽṹ
 *   onexit_buf:     OnExit���̻�����
 *   onexit_free:    OnExit�����б�
 *   onexit_list:    OnExitע���б�
**--------------------------------------------------------------------------------------------*/
struct onexit_s {
    onexit_func  func;
    struct onexit_s *next;
};

static struct onexit_s  onexit_buf[ON_EXIT_NUM];

static struct onexit_s *onexit_free = NULL;
static struct onexit_s *onexit_list = NULL;

/*----------------------------------------------------------------------------------------------
 * ����:    InitializeOnExit()
 *
 * ˵��:    ��ʼ��OnExitģ��
**--------------------------------------------------------------------------------------------*/
void apical InitializeOnExit(void)
{
    int  i;

    onexit_free = NULL;
    onexit_list = NULL;
    
    for(i=0; i<ON_EXIT_NUM; i++){
        onexit_buf[i].func =  NULL;
        onexit_buf[i].next =  onexit_free;
        onexit_free        = &onexit_buf[i];
    }
}

/*----------------------------------------------------------------------------------------------
 * ����:    RegisterOnExit()
 *
 * ˵��:    ע��OnExit����
**--------------------------------------------------------------------------------------------*/
BOOL apical RegisterOnExit(onexit_func onexit)
{
    BOOL   retval;
    struct onexit_s *oe;
    
    FamesAssert(onexit);

    if(!onexit){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(onexit_free){
        oe = onexit_free;       /* allocate a free one */
        onexit_free = oe->next;

        oe->next = onexit_list; /* link to onexit_list */
        oe->func = onexit;
        onexit_list = oe;      

        retval = ok;
    }
    unlock_kernel();

    return retval;
}

/*----------------------------------------------------------------------------------------------
 * ����:    DeregisterOnExit()
 *
 * ˵��:    ע��һ������
**--------------------------------------------------------------------------------------------*/
BOOL apical DeregisterOnExit(onexit_func onexit)
{
    BOOL   retval;
    struct onexit_s **oe, *t;
    
    FamesAssert(onexit);

    if(!onexit){
        return fail;
    }

    retval = fail;
    lock_kernel();
    if(onexit_list){
        for(oe = &onexit_list; *oe;){
            if((*oe)->func == onexit){
                t = (*oe);
               (*oe) = (*oe)->next; /* disconnect */
                t->next = onexit_free; /* link to free list */
                t->func = NULL;
                onexit_free = t;
                retval = ok;
                break;
            }
            oe = &(*oe)->next;
        }
    }
    unlock_kernel();

    return retval;
}

/*----------------------------------------------------------------------------------------------
 * ����:    ProcessOnExit()
 *
 * ˵��:    ִ��OnExit����, �˺���ֻ����ExitFamesOS()����
**--------------------------------------------------------------------------------------------*/
void apical ProcessOnExit(void)
{
    struct onexit_s *oe;

    lock_kernel();
    oe = onexit_list;
    while(oe){
        if(oe->func){
            (*oe->func)();
        }
        oe = oe->next;
    }
    unlock_kernel();
}

#endif /*#if FAMES_ON_EXIT_EN == 1*/

/*==============================================================================================
 * 
 * ���ļ�����: onexit.c
 * 
**============================================================================================*/

