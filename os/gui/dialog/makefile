COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\..\include
HEADFILE    = ..\..\include\checkinc

dstfile     = gui_dlg.lib

tmpfile1    = _tmp1.lib
tmpfile2    = _tmp2.lib

Lib         = $(dstfile)
LibList1    =  dlg_inpt.obj
LibFile1    = +dlg_inpt.obj

dlg_inpt.obj: dlg_inpt.c $(HEADFILE)
	$(COMPILER) dlg_inpt.c 

$(tmpfile1): $(LibList1)
	del  $(tmpfile1)
	tlib $(tmpfile1) $(LibFile1)

$(Lib):  $(LibList1)
	del  $(Lib)
	tlib $(Lib) +$(tmpfile1)
	copy $(Lib) ..



