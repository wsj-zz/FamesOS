echo. > ltall.txt

lint  options.lnt -u knl\*.c     >> ltall.txt
lint  options.lnt -u mm\*.c      >> ltall.txt
lint  options.lnt -u drv\*.c     >> ltall.txt
lint  options.lnt -u version\*.c >> ltall.txt
cd gui
call  ltall.bat
cd ..
lint  options.lnt -u net\*.c     >> ltall.txt
lint  options.lnt -u misc\*.c    >> ltall.txt
lint  options.lnt -u plc\*.c     >> ltall.txt
lint  options.lnt -u console\*.c >> ltall.txt
lint  options.lnt -u monitor\*.c >> ltall.txt

notepad ltall.txt

