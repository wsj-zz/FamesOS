/*************************************************************************************
 * �ļ�:    init.h
 *
 * ����:    ��ʼ��
 *
 * ����:    Jun
 *
 * ʱ��:    2010-7-5
 *
 * ˵��:    FamesOS�ڴ˴���ʼ��, ��������һ���û�����(start)
*************************************************************************************/
#ifndef FAMES_INIT_H
#define FAMES_INIT_H

/*------------------------------------------------------------------------------------
 * 
 *          ����
 * 
**----------------------------------------------------------------------------------*/
#ifdef  FAMES_INIT_C
void interrupt (far *DOS_TimeTick)(void);   /* DOSԭ����ʱ���ж�����               */
static jmp_buf   JumpBuf;                   /* ����FamesOS����ǰ��ϵͳ״̬         */
static BOOL      ExitAppFlag=NO;            /* Ӧ���˳���־                        */
static INT16U    ElapsedOverhead=0;         /* ����ʱ��У��ֵ                      */
#endif 

/*------------------------------------------------------------------------------------
 * 
 *          ��������
 * 
**----------------------------------------------------------------------------------*/
void   apical InitFamesOS(void);            /* ��ʼ��FamesOS                       */
void   apical StartOS(void);                /* ��ʼ����                            */
void   apical ExitFamesOS(void);            /* ��ȫ�˳�FamesOS������,����DOS       */
void   apical InitApplication(void);        /* ��ʼ��Ӧ�ó���(���û�����)          */
void   apical ExitApplication(void);        /* �˳�Ӧ�ó��������                  */
void   apical InitTimeTick(void);           /* ��װʱ�ӵδ�                        */
void   apical StopTimeTick(void);           /* �ر�ʱ�ӵδ�                        */
void   interrupt TimeTick(void);            /* ʱ�ӵδ�ISR                         */
void   apical ElapsedInit(void);            /* ��������ʱ���ʼ��                  */
void   apical ElapsedStart(void);           /* ��������ʱ�俪ʼ                    */
INT16U apical ElapsedStop(void);            /* ��������ʱ�����                    */
void   apical FamesDelay(INT32U);           /* FamesOSӲ��ʱ                       */

void   apical FamesSetVect(INT16S interruptno, void interrupt (far *isr)());
INT32U apical __FamesGetVect(INT16S interruptno);

#define       FamesGetVect(interruptno)  \
                    (void interrupt (far *)(void))(__FamesGetVect((interruptno)))

#define DELAY(us) FamesDelay(us)

/*------------------------------------------------------------------------------------
 * 
 *  �û���ʼ����, ��init����, ��Ӧ�ó���ʵ��
 * 
**----------------------------------------------------------------------------------*/
void __task start(void *data);  


#endif /* #ifndef FAMES_INIT_H */
/*====================================================================================
 * 
 * ���ļ�����: init.h
 * 
**==================================================================================*/


