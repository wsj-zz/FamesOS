COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\..\include
HEADFILE    = ..\..\include\checkinc

dstfile     = gdc.lib

Lib         = $(dstfile)
LibList     =  gdc.obj
LibFile     = +gdc.obj


gdc.obj: gdc.c $(HEADFILE)
	$(COMPILER) gdc.c


$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



