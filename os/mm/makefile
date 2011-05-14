COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc

dstfile     = mm.lib

Lib         = $(dstfile)
LibList     =  mm_svc.obj
LibFile     = +mm_svc.obj


mm_svc.obj: mm_svc.c $(HEADFILE)
	$(COMPILER) mm_svc.c

$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



