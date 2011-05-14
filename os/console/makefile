COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc

dstfile     = console.lib

Lib         = $(dstfile)
LibList     =  keysvc.obj  console.obj
LibFile     = +keysvc.obj +console.obj

keysvc.obj: keysvc.c $(HEADFILE)
	$(COMPILER) keysvc.c

console.obj: console.c $(HEADFILE)
	$(COMPILER) console.c

$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



