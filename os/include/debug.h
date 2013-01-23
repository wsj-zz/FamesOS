/***********************************************************************************************
 * �ļ�: debug.h
 *
 * ˵��: ����(Assertion)
 *
 * ����: Jun
 *
 * ʱ��: 2010-8-16
***********************************************************************************************/
#ifndef FAMES_ASSERT_H
#define FAMES_ASSERT_H

/*----------------------------------------------------------------------------------------------
 * ����(Assertion)����
**--------------------------------------------------------------------------------------------*/
#define FamesAssertionEnable 1

/*----------------------------------------------------------------------------------------------
 * ����(Assertion)�궨��
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
                       /* ������ʧ��ʱ, �˳�ϵͳ�ᶪʧ������Ϣ */    \
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
 * ���ļ�����: debug.h
 * 
**============================================================================================*/


