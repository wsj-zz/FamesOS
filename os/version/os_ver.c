/*************************************************************************************
 * 文件:    os_ver.c
 *
 * 说明:    FamesOS版本号
 *
 * 作者:    Jun
 *
 * 时间:    2011-2-19
*************************************************************************************/
#define FAMES_OS_VER_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 *                  FamesOS版本号(VERSION)
 * 
 * 说明: 版本号由3部分组成:
 *
 *       1) major:  主版本号, 主版本号的变化代表FamesOS有了较大的或者较本质的改变
 *       2) minor:  次版本号, 次版本号的变化代表FamesOS在部分功能上有所加强或改变
 *       3) serial: 版本序号, 其变化代表了对FamesOS的每一次修改
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
 *                  作者
 * 
 * Name:    Jun(军)
 * Email:   FamesOS@126.com
**----------------------------------------------------------------------------------*/


/*====================================================================================
 * 
 * 本文件结束: os_ver.c
 * 
**==================================================================================*/

