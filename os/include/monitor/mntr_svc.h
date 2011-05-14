/*************************************************************************************
 * �ļ�: mntr_svc.h
 *
 * ����: ϵͳ���
 *
 * ����: Jun
 *
 * ʱ��: 2010-12-6
 *
 * �汾: ��1��
 *
 * ˵��: ϵͳ����ǻ��ڵ��Ե�Ŀ��, ���Ĺ�����һ����̨�������, ��������ռ�
 *       ϵͳ�е�һЩ��Ϣ, �ʵ��ӹ������������ļ����������豸
*************************************************************************************/
#ifndef FAMES_MONITOR_SVC_H
#define FAMES_MONITOR_SVC_H

/*------------------------------------------------------------------------------------
 *      ϵͳ���ģ�����ÿ���:  1=����, 0=�ر�
**----------------------------------------------------------------------------------*/
#define FAMES_MONITOR_EN       0


/*------------------------------------------------------------------------------------
 *      ��ض���
**----------------------------------------------------------------------------------*/
typedef  void (* mntr_f)(void);       /* ϵͳ��⺯������         */
typedef  char  * mntr_nm;             /* ϵͳ��������           */
typedef  signed int    mntr_cmd;      /* ϵͳ�������������     */
typedef  unsigned long mntr_opt;      /* ѡ��                     */


#define  MONITOR_MAX_NUM       8      /* ��ע��ļ����������� */

/*------------------------------------------------------------------------------------
 *      ϵͳ����������
**----------------------------------------------------------------------------------*/
#define  MNTR_CTRL_SVC_START   1   /* ���ȫ��, Ҳ�������еļ����� */
#define  MNTR_CTRL_SVC_STOP    2

#define  MNTR_CTRL_SVC_OPEN    3   /* ֻ��Ը��������             */
#define  MNTR_CTRL_SVC_CLOSE   4


/*------------------------------------------------------------------------------------
 *      API����
**----------------------------------------------------------------------------------*/
#if  FAMES_MONITOR_EN == 1
void apical __init InitMonitorService(void);
BOOL apical        MonitorControl(mntr_nm svc_name, mntr_cmd cmd, void * data);
BOOL apical        RegisterMonitorService(mntr_nm svc_name, mntr_f func, mntr_opt opt);
BOOL apical        DeregisterMonitorService(mntr_nm svc_name);
#else 
#define            InitMonitorService()
#define            MonitorControl(mntr_nm, mntr_cmd, voidptr) (fail)
#define            RegisterMonitorService(mntr_nm, mntr_f, mntr_opt) (fail)
#define            DeregisterMonitorService(mntr_nm) (fail)
#endif /* FAMES_MONITOR_EN == 1 */

#endif /* #ifndef FAMES_MONITOR_SVC_H */

/*====================================================================================
 * 
 * ���ļ�����: mntr_svc.h
 * 
**==================================================================================*/

