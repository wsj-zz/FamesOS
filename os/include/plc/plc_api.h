/***********************************************************************************************
 * 文件:    plc_api.h
 *
 * 说明:    PLC模块API层
 *
 * 作者:    Jun
 *
 * 时间:    2010-12-19
***********************************************************************************************/
#ifndef FAMES_PLC_API_H
#define FAMES_PLC_API_H

/*----------------------------------------------------------------------------------------------
 * 
 *      编译开关
 * 
**--------------------------------------------------------------------------------------------*/
#define FAMES_PLC_API_EN  1 

#if FAMES_PLC_SVC_EN == 1 && FAMES_PLC_API_EN == 1
/*----------------------------------------------------------------------------------------------
 * 
 *      PLC_ACTION映射结构
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
 *      PLC_ACTION映射定义宏
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
 * 函数:    do_plc_action_map()
 * 
 * 描述:    加载PLC_ACTION映射块到某PLC
 *
 * 说明:    特别注意: 一个PLC_ACTION映射块只能加载到一个PLC上!
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
 * 本文件结束: plc_api.h
 * 
**============================================================================================*/

