/***********************************************************************************************
 * 文件: debug.h
 *
 * 说明: 断言(Assertion)
 *
 * 作者: Jun
 *
 * 时间: 2010-8-16
***********************************************************************************************/
#ifndef FAMES_ASSERT_H
#define FAMES_ASSERT_H

/*----------------------------------------------------------------------------------------------
 * 断言(Assertion)开关
**--------------------------------------------------------------------------------------------*/
#define FamesAssertionEnable 1

/*----------------------------------------------------------------------------------------------
 * 断言(Assertion)宏定义
**--------------------------------------------------------------------------------------------*/
#if     FamesAssertionEnable
#ifdef  FAMES_GLOBAL
char    __assert_format[] = "Fames Assertion failed: { %s }, file %s, line %d\n%s\n";
#else
extern  char __assert_format[];
#endif
#define FamesAssert(p)    if(!(p)){DispatchLock();                   \
                          fprintf(stderr,  __assert_format,          \
                          #p, __FILE__, __LINE__,                    \
                          os_get_description());                     \
                       /* 当断言失败时, 退出系统会丢失断言消息 */    \
                          for(;;)getch();                            \
                       /* if(FamesOSStarted==YES)ExitApplication(); else abort(); */}
#undef  NDEBUG
#define SysAssert(p) assert(p)
#else
#define FamesAssert(p)
#define SysAssert(p)
#endif  



#endif
/*==============================================================================================
 * 
 * 本文件结束: debug.h
 * 
**============================================================================================*/


