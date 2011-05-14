/*************************************************************************************
 * 文件: keysvc.h
 *
 * 说明: 按键服务
 *
 * 作者: Jun
 *
 * 时间: 2010-04-14
 *
 * 版本: V0.1 (2010-04-14, 最初的版本)
*************************************************************************************/
#ifndef FAMES_KEY_SVC_H
#define FAMES_KEY_SVC_H

/*
*下面定义按键相关结构-----------------------------------------------------------------
*/
#define KEY_BUFFER_SIZE 32          /* 按键缓冲大小                                */

typedef struct{                     /* --------按键缓冲结构--------------          */
    KEYCODE key[KEY_BUFFER_SIZE];   /* 按键缓冲区                                  */
    INT16S  front;                  /* 指向下一个空位置, 用于PUT键码               */ 
    INT16S  rear;                   /* 指向下一个键码,   用于GET键码               */
    INT16S  num;                    /* 缓冲中的键码个数                            */
}KEY_BUFFER;                        /* ----------------------------------          */

#ifdef  FAMES_KEY_SVC_C             /* 只有keysvc.c可见下面定义                    */
KEY_BUFFER KEYBUF={{0},0,0,0};      /* FAMES按键缓冲                               */
#endif

/*
*特殊键相关定义-----------------------------------------------------------------------
*/
typedef  void (*KEYFUNC)(void);        /* 此类函数应尽可能精简, 且不允许睡眠 */
typedef  void (*KEYHOOK)(KEYCODE key); /* 此类函数应尽可能精简, 且不允许睡眠 */


/*
*下面定义键盘按键 --------------------------------------------------------------------
*/
#define F1              0x3B00
#define F2              0x3C00
#define F3              0x3D00
#define F4              0x3E00
#define F5              0x3F00
#define F6              0x4000
#define F7              0x4100
#define F8              0x4200
#define F9              0x4300
#define F10             0x4400

#define CTRL_F1         0x5E00
#define CTRL_F2         0x5F00
#define CTRL_F3         0x6000
#define CTRL_F4         0x6100
#define CTRL_F5         0x6200
#define CTRL_F6         0x6300
#define CTRL_F7         0x6400
#define CTRL_F8         0x6500
#define CTRL_F9         0x6600
#define CTRL_F10        0x6700

#define ALT_F1          0x6800
#define ALT_F2          0x6900
#define ALT_F3          0x6A00
#define ALT_F4          0x6B00
#define ALT_F5          0x6C00
#define ALT_F6          0x6D00
#define ALT_F7          0x6E00
#define ALT_F8          0x6F00
#define ALT_F9          0x7000
#define ALT_F10         0x7100

#define SHIFT_F1        0x5400
#define SHIFT_F2        0x5500
#define SHIFT_F3        0x5600
#define SHIFT_F4        0x5700
#define SHIFT_F5        0x5800
#define SHIFT_F6        0x5900
#define SHIFT_F7        0x5A00
#define SHIFT_F8        0x5B00
#define SHIFT_F9        0x5C00
#define SHIFT_F10       0x5D00

#define HOME            0x4700
#define END             0x4F00
#define PGUP            0x4900
#define PGDN            0x5100
#define INSERT          0x5200
#define DELETE          0x5300
#define LEFT            0x4B00
#define RIGHT           0x4D00
#define UP              0x4800
#define DOWN            0x5000

#define CTRL_HOME       0x7700
#define CTRL_END        0x7500
#define CTRL_PGUP       0x8400
#define CTRL_PGDN       0x7600
#define CTRL_LEFT       0x7300
#define CTRL_RIGHT      0x7400
#define CTRL_UP         0x8D00
#define CTRL_DOWN       0x9100

#define ESC             0x1B
#define ENTER           0xD
#define SPACE           0x20
#define BACKSPACE       0x8
#define TAB             0x9
#define SHIFT_TAB       0x0F00

#define RIGHT_SHIFT     0xF100
#define LEFT_SHIFT      0xF200
#define SHIFT           0xF300
#define CTRL            0xF400
#define ALT             0xF800
#define CTRLALT         0xFC00
#define NONE_KEY        0xF000

#define DUMMY_KEY       0xFFFF  /* 不可能有的按键值         */

#define ALT_X           0x2D00  /* ALT_X 超级命令引导键     */
#define ALT_K           0x2500  /* ALT_K 超级命令之强制退出 */
#define ALT_S           0x1F00  /* ALT_S 超级命令之刷新窗口 */

/*
*下面为按键服务函数声明 --------------------------------------------------------------
*/
void    apical __init InitKeyService(void);

BOOLEAN apical RegisterSpecialKey(KEYCODE keycode, KEYFUNC func);
BOOLEAN apical DeregisterSpecialKey(KEYCODE keycode);
BOOLEAN apical RegisterKeyHook(KEYHOOK hook);
BOOLEAN apical DeregisterKeyHook(KEYHOOK hook);

/*
*-------------------------------------------------------------------------------------
*/


#endif /* #ifndef FAMES_KEY_SVC_H               */

/*====================================================================================
 * 
 * 本文件结束: keysvc.h
 * 
**==================================================================================*/

