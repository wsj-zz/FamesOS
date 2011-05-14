COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc

dstfile     = monitor.lib

Lib         = $(dstfile)
LibList     =  mntr_svc.obj  stack.obj
LibFile     = +mntr_svc.obj +stack.obj


mntr_svc.obj: mntr_svc.c $(HEADFILE)
	$(COMPILER) mntr_svc.c

stack.obj: stack.c $(HEADFILE)
	$(COMPILER) stack.c

$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



