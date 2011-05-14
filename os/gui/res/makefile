COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\..\include
HEADFILE    = ..\..\include\checkinc

dstfile     = res.lib

Lib         = $(dstfile)
LibList     =  font.obj  bmp.obj 
LibFile     = +font.obj +bmp.obj 


font.obj: font.c $(HEADFILE)
	$(COMPILER) font.c 

bmp.obj: bmp.c $(HEADFILE)
	$(COMPILER) bmp.c


$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



