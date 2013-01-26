@echo off
rem ===== this file is used to generate os_ver.lib =====
echo   Generating FamesOS Version information...
tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include os_ver.c > nul
del os_ver.lib
tlib os_ver.lib +os_ver.obj
copy os_ver.lib ..

