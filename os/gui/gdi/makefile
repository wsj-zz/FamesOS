COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\..\include
HEADFILE    = ..\..\include\checkinc

dstfile     = gdi.lib

Lib         = $(dstfile)
LibList     =  gdi.obj  vesa.obj 
LibFile     = +gdi.obj +vesa.obj 


gdi.obj: gdi.c $(HEADFILE)
	$(COMPILER) gdi.c

vesa.obj: vesa.c $(HEADFILE)
	$(COMPILER) vesa.c


$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



