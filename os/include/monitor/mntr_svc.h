/*************************************************************************************
 * 文件: mntr_svc.h
 *
 * 描述: 系统监测
 *
 * 作者: Jun
 *
 * 时间: 2010-12-6
 *
 * 版本: 第1版
 *
 * 说明: 系统监测是基于调试的目的, 它的功能由一个后台任务完成, 这个任务收集
 *       系统中的一些信息, 适当加工处理后输出到文件或者其它设备
*************************************************************************************/
#ifndef FAMES_MONITOR_SVC_H
#define FAMES_MONITOR_SVC_H

/*------------------------------------------------------------------------------------
 *      系统监测模块启用开关:  1=启用, 0=关闭
**----------------------------------------------------------------------------------*/
#define FAMES_MONITOR_EN       0


/*------------------------------------------------------------------------------------
 *      相关定义
**----------------------------------------------------------------------------------*/
typedef  void (* mntr_f)(void);       /* 系统监测函数类型         */
typedef  char  * mntr_nm;             /* 系统监测服务名           */
typedef  signed int    mntr_cmd;      /* 系统监测服务控制命令     */
typedef  unsigned long mntr_opt;      /* 选项                     */


#define  MONITOR_MAX_NUM       8      /* 可注册的监测服务最大个数 */

/*------------------------------------------------------------------------------------
 *      系统监测控制命令
**----------------------------------------------------------------------------------*/
#define  MNTR_CTRL_SVC_START   1   /* 针对全局, 也就是所有的监测服务 */
#define  MNTR_CTRL_SVC_STOP    2

#define  MNTR_CTRL_SVC_OPEN    3   /* 只针对个别监测服务             */
#define  MNTR_CTRL_SVC_CLOSE   4


/*------------------------------------------------------------------------------------
 *      API函数
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
 * 本文件结束: mntr_svc.h
 * 
**==================================================================================*/

