COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc

dstfile     = drv.lib

Lib         = $(dstfile)
LibList     =  rs232.obj  pci.obj  nic.obj  rtl8139.obj  pktdrv.obj  kbd.obj
LibFile     = +rs232.obj +pci.obj +nic.obj +rtl8139.obj +pktdrv.obj +kbd.obj


rs232.obj: rs232.c $(HEADFILE)
	$(COMPILER) rs232.c

pci.obj: pci.c $(HEADFILE)
	$(COMPILER) pci.c

nic.obj: nic.c $(HEADFILE)
	$(COMPILER) nic.c

rtl8139.obj: rtl8139.c $(HEADFILE)
	$(COMPILER) rtl8139.c

pktdrv.obj: pktdrv.c $(HEADFILE)
	$(COMPILER) pktdrv.c

kbd.obj: kbd.c $(HEADFILE)
	$(COMPILER) kbd.c

$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



