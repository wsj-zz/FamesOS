/*************************************************************************************
** 文件: misc.h
** 说明: 一些杂项的定义
** 作者: Jun
** 时间: 2010-03-28
** 版本: V0.1 (2010-03-28, 最初的版本)
*************************************************************************************/
#ifndef FAMES_MISC_H
#define FAMES_MISC_H

/*
*位掩码-------------------------------------------------------------------------------
*/
#define INT8_BIT0       (0x01)
#define INT8_BIT1       (0x02) 
#define INT8_BIT2       (0x04) 
#define INT8_BIT3       (0x08) 
#define INT8_BIT4       (0x10) 
#define INT8_BIT5       (0x20) 
#define INT8_BIT6       (0x40) 
#define INT8_BIT7       (0x80) 

#define INT8_MSK0       (~INT8_BIT0)
#define INT8_MSK1       (~INT8_BIT1)
#define INT8_MSK2       (~INT8_BIT2)
#define INT8_MSK3       (~INT8_BIT3)
#define INT8_MSK4       (~INT8_BIT4)
#define INT8_MSK5       (~INT8_BIT5)
#define INT8_MSK6       (~INT8_BIT6)
#define INT8_MSK7       (~INT8_BIT7)

#define INT16_BIT0      (0x0001)
#define INT16_BIT1      (0x0002)
#define INT16_BIT2      (0x0004)
#define INT16_BIT3      (0x0008)
#define INT16_BIT4      (0x0010)
#define INT16_BIT5      (0x0020)
#define INT16_BIT6      (0x0040)
#define INT16_BIT7      (0x0080)
#define INT16_BIT8      (0x0100)
#define INT16_BIT9      (0x0200)
#define INT16_BIT10     (0x0400)
#define INT16_BIT11     (0x0800)
#define INT16_BIT12     (0x1000)
#define INT16_BIT13     (0x2000)
#define INT16_BIT14     (0x4000)
#define INT16_BIT15     (0x8000)

#define INT16_MSK0      (~INT16_BIT0 )
#define INT16_MSK1      (~INT16_BIT1 )
#define INT16_MSK2      (~INT16_BIT2 )
#define INT16_MSK3      (~INT16_BIT3 )
#define INT16_MSK4      (~INT16_BIT4 )
#define INT16_MSK5      (~INT16_BIT5 )
#define INT16_MSK6      (~INT16_BIT6 )
#define INT16_MSK7      (~INT16_BIT7 )
#define INT16_MSK8      (~INT16_BIT8 )
#define INT16_MSK9      (~INT16_BIT9 )
#define INT16_MSK10     (~INT16_BIT10)
#define INT16_MSK11     (~INT16_BIT11)
#define INT16_MSK12     (~INT16_BIT12)
#define INT16_MSK13     (~INT16_BIT13)
#define INT16_MSK14     (~INT16_BIT14)
#define INT16_MSK15     (~INT16_BIT15)

#define INT32_BIT0      (0x00000001L)
#define INT32_BIT1      (0x00000002L)
#define INT32_BIT2      (0x00000004L)
#define INT32_BIT3      (0x00000008L)
#define INT32_BIT4      (0x00000010L)
#define INT32_BIT5      (0x00000020L)
#define INT32_BIT6      (0x00000040L)
#define INT32_BIT7      (0x00000080L)
#define INT32_BIT8      (0x00000100L)
#define INT32_BIT9      (0x00000200L)
#define INT32_BIT10     (0x00000400L)
#define INT32_BIT11     (0x00000800L)
#define INT32_BIT12     (0x00001000L)
#define INT32_BIT13     (0x00002000L)
#define INT32_BIT14     (0x00004000L)
#define INT32_BIT15     (0x00008000L)
#define INT32_BIT16     (0x00010000L)
#define INT32_BIT17     (0x00020000L)
#define INT32_BIT18     (0x00040000L)
#define INT32_BIT19     (0x00080000L)
#define INT32_BIT20     (0x00100000L)
#define INT32_BIT21     (0x00200000L)
#define INT32_BIT22     (0x00400000L)
#define INT32_BIT23     (0x00800000L)
#define INT32_BIT24     (0x01000000L)
#define INT32_BIT25     (0x02000000L)
#define INT32_BIT26     (0x04000000L)
#define INT32_BIT27     (0x08000000L)
#define INT32_BIT28     (0x10000000L)
#define INT32_BIT29     (0x20000000L)
#define INT32_BIT30     (0x40000000L)
#define INT32_BIT31     (0x80000000L)

#define INT32_MSK0      (~INT32_BIT0 )
#define INT32_MSK1      (~INT32_BIT1 )
#define INT32_MSK2      (~INT32_BIT2 )
#define INT32_MSK3      (~INT32_BIT3 )
#define INT32_MSK4      (~INT32_BIT4 )
#define INT32_MSK5      (~INT32_BIT5 )
#define INT32_MSK6      (~INT32_BIT6 )
#define INT32_MSK7      (~INT32_BIT7 )
#define INT32_MSK8      (~INT32_BIT8 )
#define INT32_MSK9      (~INT32_BIT9 )
#define INT32_MSK10     (~INT32_BIT10)
#define INT32_MSK11     (~INT32_BIT11)
#define INT32_MSK12     (~INT32_BIT12)
#define INT32_MSK13     (~INT32_BIT13)
#define INT32_MSK14     (~INT32_BIT14)
#define INT32_MSK15     (~INT32_BIT15)
#define INT32_MSK16     (~INT32_BIT16)
#define INT32_MSK17     (~INT32_BIT17)
#define INT32_MSK18     (~INT32_BIT18)
#define INT32_MSK19     (~INT32_BIT19)
#define INT32_MSK20     (~INT32_BIT20)
#define INT32_MSK21     (~INT32_BIT21)
#define INT32_MSK22     (~INT32_BIT22)
#define INT32_MSK23     (~INT32_BIT23)
#define INT32_MSK24     (~INT32_BIT24)
#define INT32_MSK25     (~INT32_BIT25)
#define INT32_MSK26     (~INT32_BIT26)
#define INT32_MSK27     (~INT32_BIT27)
#define INT32_MSK28     (~INT32_BIT28)
#define INT32_MSK29     (~INT32_BIT29)
#define INT32_MSK30     (~INT32_BIT30)
#define INT32_MSK31     (~INT32_BIT31)


/*
*其它---------------------------------------------------------------------------------
*/
#define fames_print    printf
#define print print_null

#define sys_print      printf
#define sys_exit()     exit(0)


#endif                                      /* #ifndef FAMES_MISC_H                 */

/*
*本文件结束: misc.h ==================================================================
*/

