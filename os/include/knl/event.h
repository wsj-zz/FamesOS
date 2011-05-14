/*************************************************************************************
** �ļ�: event.h
** ˵��: FamesOS�¼�����
** ����: Jun
** ʱ��: 2010-6-28
*************************************************************************************/
#ifndef FAMES_EVENT_H
#define FAMES_EVENT_H

/*------------------------------------------------------------------------------------
*                   �¼����ƿ�(EventControlBlock, ECB)
* 
* ˵��: һ��ECB�ʹ���һ���¼�
*
*       1) Available:  �¼�����Ч��־
*       2) TaskWait:   �ȴ����¼�������
**----------------------------------------------------------------------------------*/
typedef struct EventControlBlock {          
    BOOL      Available;                       
    TASK_LIST TaskWait;              
}ECB;

#define EVENT_PENDING      0                  /* �¼�״̬: �ȴ�                    */
#define EVENT_AVAILABLE    1                  /* �¼�״̬: ��Ч                    */

/*
*�������� ----------------------------------------------------------------------------
*/
BOOL apical EventInit(ECB * event);
BOOL apical EventSet(ECB * event);
BOOL apical EventReset(ECB * event);
BOOL apical EventTest(ECB * event);
BOOL apical EventTaskWait(ECB *, HANDLE htask);
BOOL apical EventTaskReady(ECB * event);


#endif                                        /* #ifndef FAMES_EVENT_H             */

/*
*���ļ�����: event.h =================================================================
*/

