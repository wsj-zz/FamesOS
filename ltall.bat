echo. > ltall.txt

cd os
call ltall.bat
cd ..

lint  options.lnt -Idemo demo\*.c    >> ltall.txt

notepad ltall.txt

