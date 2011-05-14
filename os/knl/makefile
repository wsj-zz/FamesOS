COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc

dstfile     = knl.lib

Lib         = $(dstfile)
LibList     =  init.obj  task.obj  event.obj  irq.obj  timer.obj  message.obj  onexit.obj
LibFile     = +init.obj +task.obj +event.obj +irq.obj +timer.obj +message.obj +onexit.obj


init.obj: init.c $(HEADFILE)
	$(COMPILER) init.c

task.obj: task.c $(HEADFILE)
	$(COMPILER) task.c

event.obj: event.c $(HEADFILE)
	$(COMPILER) Event.c

irq.obj: irq.c $(HEADFILE)
	$(COMPILER) irq.c

timer.obj: timer.c $(HEADFILE)
	$(COMPILER) timer.c

message.obj: message.c $(HEADFILE)
	$(COMPILER) message.c

onexit.obj: onexit.c $(HEADFILE)
	$(COMPILER) onexit.c

$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



