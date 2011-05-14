/***********************************************************************************************
 * �ļ�:    plc_api.h
 *
 * ˵��:    PLCģ��API��
 *
 * ����:    Jun
 *
 * ʱ��:    2010-12-19
***********************************************************************************************/
#ifndef FAMES_PLC_API_H
#define FAMES_PLC_API_H

/*----------------------------------------------------------------------------------------------
 * 
 *      ���뿪��
 * 
**--------------------------------------------------------------------------------------------*/
#define FAMES_PLC_API_EN  1 

#if FAMES_PLC_SVC_EN == 1 && FAMES_PLC_API_EN == 1
/*----------------------------------------------------------------------------------------------
 * 
 *      PLC_ACTIONӳ��ṹ
 * 
**--------------------------------------------------------------------------------------------*/
struct plc_action_map_s {
    int           map_option;
    int           map_id;
    PLC_ACTION    map_action;
    int           map_cmd;
    INT08S      * map_addr;
    void        * map_variable;
    int           map_var_num;
    int           map_timer;
    int           map_disabled;
    void       (* map_finish)(int id, BOOL success);
};

#define PLC_A_MAP_OPT_AUTO   1
#define PLC_A_MAP_OPT_LAST  -1

/*----------------------------------------------------------------------------------------------
 * 
 *      PLC_ACTIONӳ�䶨���
 * 
**--------------------------------------------------------------------------------------------*/
/*lint -save -emacro(785, END_PLC_ACTION_MAP)*/
#define BEGIN_PLC_ACTION_MAP(map_name)  struct plc_action_map_s map_name[] = {
#define END_PLC_ACTION_MAP()            { PLC_A_MAP_OPT_LAST, } }
#define PLC_MAP_LINK(id, addr, var, var_nm, cmd, timer, finish_routine, disabled)    \
                                        { PLC_A_MAP_OPT_AUTO, id, {0}, cmd, addr,   \
                                          var, var_nm, timer, disabled, finish_routine},
/*lint -restore*/

/*----------------------------------------------------------------------------------------------
 * ����:    do_plc_action_map()
 * 
 * ����:    ����PLC_ACTIONӳ��鵽ĳPLC
 *
 * ˵��:    �ر�ע��: һ��PLC_ACTIONӳ���ֻ�ܼ��ص�һ��PLC��!
**--------------------------------------------------------------------------------------------*/
BOOL apical do_plc_action_map(PLC * plc, struct plc_action_map_s plc_a_map[]);
void apical plc_rw(PLC * plc, int cmd, void * addr, void * associated, int number);
void apical plc_rw_ensure(PLC * plc, int cmd, void * addr, void * associated, int number);
BOOL apical plc_action_enable_id(PLC * plc, int action_id);
BOOL apical plc_action_disable_id(PLC * plc, int action_id);
BOOL apical plc_action_id_set_number(PLC * plc, int action_id, int number);



#endif /* _EN && _EN */

#endif /* #ifndef FAMES_PLC_API_H */
/*==============================================================================================
 * 
 * ���ļ�����: plc_api.h
 * 
**============================================================================================*/

