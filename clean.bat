@echo off

del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt

cd  os
call clean
cd ..

cd  demo
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd ..



