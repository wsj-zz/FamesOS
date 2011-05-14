COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc

dstfile     = misc.lib

Lib         = $(dstfile)
LibList     =  tool.obj  xms.obj
LibFile     = +tool.obj +xms.obj


tool.obj: tool.c $(HEADFILE)
	$(COMPILER) tool.c

xms.obj: xms.c $(HEADFILE)
	$(COMPILER) xms.c

$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



