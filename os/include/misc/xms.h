/*************************************************************************************
 * �ļ�: xms.h
 *
 * ����: ��չ�ڴ�(XMS)��ض���
 *
 * ����: Jun
 *
 * ʱ��: 2010-03-03
 *
 * �汾: V0.1 (2010-03-03, ����İ汾)
*************************************************************************************/
#ifndef FAMES_XMS_H
#define FAMES_XMS_H

/*
*XMS���ÿ���--------------------------------------------------------------------------
*/
#define XMS_EN   1                            /* 1=����, ��������                   */

#if    XMS_EN == 1
/*
*���涨��XMS��صĽṹ������----------------------------------------------------------
*/
#define XMSHANDLE  INT16U

struct EMB {                                  /* EMB�ṹ                            */
    INT32S  Length;
    INT16U  SourceHandle;
    INT32S  SourceOfs;
    INT16U  DestinationHandle;
    INT32S  DestinationOfs;
};
#ifdef FAMES_XMS_C
static INT16U XMSOK=0;                        /* XMS��Ч��־                        */
INT16U (* XMSaddr)(void);                     /* XMS����������ڵ�ַ                */
#endif                                        /* #ifdef  FAMES_XMS_C                */

/*
*����ΪXMS��غ���������--------------------------------------------------------------
*/
INT16U  XMSinit(void);                        /* XMS��ʼ��                          */
INT16U  XMSfreesize(void);                    /* ȡ��ǰ������չ�ڴ��ܵĴ�С         */
INT16U  XMSlargestblock(void);                /* ȡ��ǰ������չ�ڴ��Ĵ�С       */
INT16U  XMS(INT16U size);                     /* ����һ����չ�ڴ�                   */
INT16U  _XMS(INT16U handle);                  /* �ͷ���չ�ڴ��                     */
INT16U  XMSrealloc(INT16U handle,             /* ���µĳߴ����·�����չ�ڴ�         */
                   INT16U size);
INT16U  XMSmove(struct EMB *emb);             /* �ƶ��ڴ��                         */
INT16U  XMSput(INT16U handle, void * dp,      /* д��չ�ڴ�                         */
               void * sp, INT32S length);
INT16U  XMSget(INT16U handle, void * dp,      /* ����չ�ڴ�                         */
               void * sp, INT32S length);

XMSHANDLE apical XMSalloc(INT16U size);
void      apical XMSfree(XMSHANDLE handle);
BOOL apical __init init_xms_service(void);


#endif                                        /* #if     XMS_EN == 1                */

#endif                                        /* #ifndef FAMES_XMS_H                */

/*====================================================================================
 * 
 * ���ļ�����: xms.h
 * 
**==================================================================================*/


