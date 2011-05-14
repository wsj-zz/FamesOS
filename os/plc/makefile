COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include 
HEADFILE    = ..\include\checkinc

dstfile     = plc.lib

Lib         = $(dstfile)
LibList     =  plc_svc.obj  plc.obj  plc_api.obj  fatek.obj  siemens.obj
LibFile     = +plc_svc.obj +plc.obj +plc_api.obj +fatek.obj +siemens.obj

plc_svc.obj: plc_svc.c $(HEADFILE)
	$(COMPILER) plc_svc.c

plc.obj: plc.c $(HEADFILE)
	$(COMPILER) plc.c

plc_api.obj: plc_api.c $(HEADFILE)
	$(COMPILER) plc_api.c

fatek.obj: fatek.c $(HEADFILE)
	$(COMPILER) fatek.c

siemens.obj: siemens.c $(HEADFILE)
	$(COMPILER) siemens.c

$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



