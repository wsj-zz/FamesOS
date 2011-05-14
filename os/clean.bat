@echo off

del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt

cd  include
del chkinc*
cd ..

cd  knl
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd  ..

cd  mm
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd  ..

cd  drv
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd ..

cd  gui
call clean
cd ..

cd  net
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd ..

cd  misc
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd ..

cd  plc
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd ..

cd  console
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd ..

cd  monitor
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd ..

cd  version
del *.obj
del *.lib
del t.txt
del lint.txt
del ltall.txt
cd  ..

