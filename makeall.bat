@echo off

REM  ±‡“ÎOS
CD   OS
CALL MAKEOS
CD   ..

REM  ±‡“Îdemo
CD   DEMO
CALL MAKEFILE
CD   ..

REM  ¡¨Ω” application
make makefile


ECHO ALL OK. 
