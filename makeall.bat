@echo off

REM  ����OS
CD   OS
CALL MAKEOS
CD   ..

REM  ����demo
CD   DEMO
CALL MAKEFILE
CD   ..

REM  ���� application
make makefile


ECHO ALL OK. 
