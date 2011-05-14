COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\os\include
HEADFILE    = ..\os\include\checkinc

dstfile     = demo.lib

Lib         = $(dstfile)

LibList1    =  start.obj
LibFile1    = +start.obj

Libtemp1    = __tmp1.lib

start.obj: start.c $(HEADFILE)
	$(COMPILER) start.c


$(Libtemp1):  $(LibList1)
	del  $(Libtemp1)
	tlib $(Libtemp1) $(LibFile1)


$(Lib):  $(LibList1)
	del  $(Lib)
	tlib $(Lib) +$(Libtemp1)
	copy $(Lib) ..


