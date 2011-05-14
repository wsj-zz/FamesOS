COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc

dstfile     = os_ver.lib

Lib         = $(dstfile)
LibList     =  os_ver.obj
LibFile     = +os_ver.obj

os_ver.obj: os_ver.c $(HEADFILE)
	$(COMPILER) os_ver.c

$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



