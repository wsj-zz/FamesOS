@echo off

REM  检测头文件是否有更新
CD   INCLUDE
CALL MAKEFILE
CD   ..

REM  编译kernel
CD   knl
CALL MAKEFILE
CD   ..

REM  编译version
CD   version
CALL MAKEFILE
CD   ..

REM  编译mm
CD   mm
CALL MAKEFILE
CD   ..

REM  编译drv
CD   drv
CALL MAKEFILE
CD   ..

REM  编译gui
CD   gui
CALL MAKEFILE
CD   ..

REM  编译net
CD   net
CALL MAKEFILE
CD   ..

REM  编译misc
CD   misc
CALL MAKEFILE
CD   ..

REM  编译plc
CD   plc
CALL MAKEFILE
CD   ..

REM  编译monitor
CD   monitor
CALL MAKEFILE
CD   ..

REM  编译console
CD   console
CALL MAKEFILE
CD   ..

REM  组合OS模块
make makefile


ECHO OS OK. 
