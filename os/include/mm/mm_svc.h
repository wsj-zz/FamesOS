/*************************************************************************************
 * 文件: mm_svc.h
 *
 * 说明: 存储管理服务
 *
 * 注意: 此文件必需以大模式编译(如: TCC -1 -B -ml ###.C)
 *
 * 作者: Jun
 *
 * 时间: 2010-12-8, 2010-12-15
*************************************************************************************/
#ifndef FAMES_MM_SVC_H
#define FAMES_MM_SVC_H


/*------------------------------------------------------------------------------------
 *
 * 内存空间的申请与回收
 *
**----------------------------------------------------------------------------------*/
void * apical __mem_alloc(INT32U nbytes);
void   apical __mem_free(void * block);

#define mem_alloc(x)   __mem_alloc((x))
#define mem_free(x)    __mem_free((x))

/*------------------------------------------------------------------------------------
 * 
 * 申请一段内存, 并在申请失败时执行指定语句
 * 
**----------------------------------------------------------------------------------*/
/*lint -save -emacro(717, allocate_buffer)*/
#define allocate_buffer(name, type, nbytes, statement_on_failure)  \
            do{ name = (type)mem_alloc((nbytes));                  \
                if(name == NULL){ statement_on_failure; }          \
            } while(0) 
/*lint -restore*/


/*------------------------------------------------------------------------------------
 * 函数:    InitMemService()
 *
 * 描述:    内存管理服务器初始化
**----------------------------------------------------------------------------------*/
void apical __init InitMemService(void);


#endif /* #ifndef FAMES_MM_SVC_H */

/*====================================================================================
 * 
 * 本文件结束: mm_svc.h
 * 
**==================================================================================*/
