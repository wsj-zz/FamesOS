@echo off

del *.obj
del *.lib
del t.txt
del ltall.txt

cd  res
del *.obj
del *.lib
del t.txt
cd ..

cd  gdi
del *.obj
del *.lib
del t.txt
cd ..

cd  widget
del *.obj
del *.lib
del t.txt
cd ..

cd  dialog
del *.obj
del *.lib
del t.txt
cd ..

