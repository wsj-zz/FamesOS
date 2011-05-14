/***********************************************************************************************
 * �ļ�:    plc_api.c
 *
 * ˵��:    PLCģ��API��
 *
 * ����:    Jun
 *
 * ʱ��:    2010-12-19
***********************************************************************************************/
#define  FAMES_PLC_API_C
#include <includes.h>

#if FAMES_PLC_SVC_EN == 1 && FAMES_PLC_API_EN == 1
/*----------------------------------------------------------------------------------------------
 * ����:    do_plc_action_map()
 * 
 * ����:    ����PLC_ACTIONӳ��鵽ĳPLC
 *
 * ˵��:    �ر�ע��: һ��PLC_ACTIONӳ���ֻ�ܼ��ص�һ��PLC��!
**--------------------------------------------------------------------------------------------*/
BOOL apical do_plc_action_map(PLC * plc, struct plc_action_map_s plc_a_map[])
{
    BOOL   retval;
    struct plc_action_map_s * map;
    
    FamesAssert(plc);

    if(!plc)
        return fail;

    map = (struct plc_action_map_s *)plc_a_map;

    retval = fail;

    while(map){
        if(map->map_option == PLC_A_MAP_OPT_LAST)
            break;
        retval = plc_set_action(&map->map_action,
                    map->map_id,
                    map->map_cmd,
                    map->map_addr,
                    map->map_variable,
                    map->map_var_num,
                    map->map_timer,
                    map->map_finish,
                    map->map_disabled
                   );
        if(!retval)
            break;        
        retval = do_plc_action(plc, &map->map_action, PLC_ACTION_FLAG_LINK);
        if(!retval)
            break;
        map++;
    }
    
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * ����:    plc_rw()
 * 
 * ����:    PLC��д
 *
 * ˵��:    �˺����ᵼ������˯��
**--------------------------------------------------------------------------------------------*/
void apical plc_rw(PLC * plc, int cmd, void * addr, void * associated, int number)
{
    PLC_ACTION action;
    int retval;

    FamesAssert(plc);
    FamesAssert(addr);
    FamesAssert(associated);
    FamesAssert(number > 0);

    if(!plc || !addr || !associated || number <= 0)
        return;

    retval = plc_set_action(&action,
                            0, 
                            cmd, 
                            (INT08S *)addr, 
                            associated, 
                            number, 
                            0, 
                            NULL, 
                            0);
    if(!retval)
        return;

    retval = do_plc_action(plc, &action, PLC_ACTION_FLAG_ONCE);

    if(!retval)
        return;
}

/*----------------------------------------------------------------------------------------------
 * ����:    plc_rw_ensure()
 * 
 * ����:    PLC��д(�����Ŭ����д�ɹ�)
 *
 * ˵��:    �˺����ᵼ������˯��
**--------------------------------------------------------------------------------------------*/
static int ____rw_success = 0;

static void ____finish(int id, BOOL success)
{
    id = id;
    if(success)
        ____rw_success = 1;
}

void apical plc_rw_ensure(PLC * plc, int cmd, void * addr, void * associated, int number)
{
    PLC_ACTION action;
    int retval, tries;
    static int __lock = 0;

    FamesAssert(plc);
    FamesAssert(addr);
    FamesAssert(associated);
    FamesAssert(number > 0);

    if(!plc || !addr || !associated || number <= 0)
        return;

    os_mutex_lock(__lock);

    tries = 0;
    ____rw_success = 0;

    retval = plc_set_action(&action,
                            0,
                            cmd, 
                            (INT08S *)addr, 
                            associated, 
                            number, 
                            0, 
                            ____finish, 
                            0);
    if(!retval)
        goto out;

    do {
        do_plc_action(plc, &action, PLC_ACTION_FLAG_ONCE);
        if(____rw_success)
            break;
    } while(++tries < 3);

    os_mutex_unlock(__lock);

out:
    return;
}

/*----------------------------------------------------------------------------------------------
 * ����:    plc_action_enable_id()
 * 
 * ����:    ͨ��IDʹ��һ��ACTION
**--------------------------------------------------------------------------------------------*/
BOOL apical plc_action_enable_id(PLC * plc, int action_id)
{
    BOOL retval;
    PLC_ACTION * action;
    
    FamesAssert(plc);
    if(!plc)
        return fail;

    if(!(plc->flag & PLC_FLAG_OPEN)){ 
        return fail;  /* not opened */
    }

    retval = fail;
    
    lock_kernel();
    action = plc->action_list;
    while(action){
        if(action->id == action_id){
            action->disabled = 0;
            retval = ok;
            break;
        }
        action = action->next;
    }
    unlock_kernel();

    return retval;
}

/*----------------------------------------------------------------------------------------------
 * ����:    plc_action_disable_id()
 * 
 * ����:    ͨ��ID����һ��ACTION
**--------------------------------------------------------------------------------------------*/
BOOL apical plc_action_disable_id(PLC * plc, int action_id)
{
    BOOL retval;
    PLC_ACTION * action;
    
    FamesAssert(plc);
    if(!plc)
        return fail;

    if(!(plc->flag & PLC_FLAG_OPEN)){ 
        return fail;  /* not opened */
    }

    retval = fail;
    
    lock_kernel();
    action = plc->action_list;
    while(action){
        if(action->id == action_id){
            action->disabled = 1;
            retval = ok;
            break;
        }
        action = action->next;
    }
    unlock_kernel();

    return retval;
}

/*----------------------------------------------------------------------------------------------
 * ����:    plc_action_id_set_number()
 * 
 * ����:    ͨ��ID����һ��ACTION�ı�������
**--------------------------------------------------------------------------------------------*/
BOOL apical plc_action_id_set_number(PLC * plc, int action_id, int number)
{
    BOOL retval;
    PLC_ACTION * action;
    
    FamesAssert(plc);
    FamesAssert(number > 0 && number <= 32);

    if(!plc || !(number > 0 && number <= 32))
        return fail;

    if(!(plc->flag & PLC_FLAG_OPEN)){ 
        return fail;  /* not opened */
    }

    retval = fail;
    
    lock_kernel();
    action = plc->action_list;
    while(action){
        if(action->id == action_id){
            action->number = number;
            retval = ok;
            break;
        }
        action = action->next;
    }
    unlock_kernel();

    return retval;
}

#endif /* _EN && _EN */

/*==============================================================================================
 * 
 * ���ļ�����: plc_api.c
 * 
**============================================================================================*/

