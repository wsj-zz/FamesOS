@echo off

REM  ���ͷ�ļ��Ƿ��и���
CD   INCLUDE
CALL MAKEFILE
CD   ..

REM  ����kernel
CD   knl
CALL MAKEFILE
CD   ..

REM  ����version
CD   version
CALL MAKEFILE
CD   ..

REM  ����mm
CD   mm
CALL MAKEFILE
CD   ..

REM  ����drv
CD   drv
CALL MAKEFILE
CD   ..

REM  ����gui
CD   gui
CALL MAKEFILE
CD   ..

REM  ����net
CD   net
CALL MAKEFILE
CD   ..

REM  ����misc
CD   misc
CALL MAKEFILE
CD   ..

REM  ����plc
CD   plc
CALL MAKEFILE
CD   ..

REM  ����monitor
CD   monitor
CALL MAKEFILE
CD   ..

REM  ����console
CD   console
CALL MAKEFILE
CD   ..

REM  ���OSģ��
make makefile


ECHO OS OK. 
