/*************************************************************************************
 * 文件: kbd.h
 *
 * 说明: 键盘驱动
 *
 * 作者: Jun
 *
 * 时间: 2010-04-14
 *
 * 版本: V0.1 (2010-04-14, 最初的版本)
*************************************************************************************/
#ifndef FAMES_KEYBOARD_H
#define FAMES_KEYBOARD_H

#define NONEKEY         0           /* 无效键码                                    */
#define KEYCODE         INT16U      /* 键码的数据类型                              */

/*------------------------------------------------------------------------------------
 *
 *      函数声明
 *
**----------------------------------------------------------------------------------*/
void    apical __init InitKeyboard(void);
void    apical __exit StopKeyboard(void);
BOOL    apical PutKeyToKeyBoard(KEYCODE);    /* 发送一个键码到键盘缓冲             */
KEYCODE apical GetKeyFromKeyBoard(void);     /* 取本地键盘按键的键码               */



#endif /* #ifndef FAMES_KEYBOARD_H */

/*====================================================================================
 * 
 * 本文件结束: kbd.h
 * 
**==================================================================================*/

