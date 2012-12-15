@echo off

CALL MAKEALL > t.txt

ECHO ALL OK.

notepad t.txt
del t.txt
