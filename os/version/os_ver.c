/*************************************************************************************
 * �ļ�:    os_ver.c
 *
 * ˵��:    FamesOS�汾��
 *
 * ����:    Jun
 *
 * ʱ��:    2011-2-19
*************************************************************************************/
#define FAMES_OS_VER_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 *                  FamesOS�汾��(VERSION)
 * 
 * ˵��: �汾����3�������:
 *
 *       1) major:  ���汾��, ���汾�ŵı仯����FamesOS���˽ϴ�Ļ��߽ϱ��ʵĸı�
 *       2) minor:  �ΰ汾��, �ΰ汾�ŵı仯����FamesOS�ڲ��ֹ�����������ǿ��ı�
 *       3) serial: �汾���, ��仯�����˶�FamesOS��ÿһ���޸�
**----------------------------------------------------------------------------------*/
static char FamesOS_NAME[]   =      "FamesOS";
static char FamesOS_AUTHOR[] =      "Jun";

#define FamesOS_VersionMajor          0
#define FamesOS_VersionMinor          7
#define FamesOS_VersionSerial         3

#define FamesOS_VersionString       "0.7.3"

INT32U apical os_get_version(void)
{
    INT32U ver=0L;

    ver |=(INT32U)FamesOS_VersionMajor&0xFFL;
    ver<<=8L;
    ver |=(INT32U)FamesOS_VersionMinor&0xFFL;
    ver<<=16L;
    ver |=(INT32U)FamesOS_VersionSerial&0xFFFFL;

    return ver;
}

STRING apical os_get_version_string(void)
{
    return FamesOS_VersionString;
}

STRING apical os_get_name(void)
{
    return FamesOS_NAME;
}

STRING apical os_get_author(void)
{
    return FamesOS_AUTHOR;
}

STRING apical os_get_buildtime(void)
{
    return __DATE__" "__TIME__;
}

STRING apical os_get_description(void)
{
    return "FamesOS Version "FamesOS_VersionString" "
           "Jun(wsj20369@yahoo.com.cn) "
           "Build "__DATE__" "__TIME__" "
           "";
}

/*------------------------------------------------------------------------------------
 *                  ����
 * 
 * Name:    Jun(��)
 * Email:   FamesOS@126.com
**----------------------------------------------------------------------------------*/


/*====================================================================================
 * 
 * ���ļ�����: os_ver.c
 * 
**==================================================================================*/

