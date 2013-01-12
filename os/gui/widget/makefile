COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\..\include
HEADFILE    = ..\..\include\checkinc

dstfile     = gui_wid.lib

tmpfile1    = _tmp1.lib
tmpfile2    = _tmp2.lib

Lib         = $(dstfile)
LibList1    =  pool.obj  widget.obj  common.obj  usr_wid.obj  sys_mntr.obj  dashed.obj  groupbox.obj
LibFile1    = +pool.obj +widget.obj +common.obj +usr_wid.obj +sys_mntr.obj +dashed.obj +groupbox.obj
LibList2    =  form.obj  label.obj  edit.obj  button.obj  progress.obj  picture.obj  view.obj  desktop.obj
LibFile2    = +form.obj +label.obj +edit.obj +button.obj +progress.obj +picture.obj +view.obj +desktop.obj

pool.obj: pool.c $(HEADFILE)
	$(COMPILER) pool.c 

widget.obj: widget.c $(HEADFILE)
	$(COMPILER) widget.c 

common.obj: common.c $(HEADFILE)
	$(COMPILER) common.c

usr_wid.obj: usr_wid.c $(HEADFILE)
	$(COMPILER) usr_wid.c

form.obj: form.c $(HEADFILE)
	$(COMPILER) form.c

label.obj: label.c $(HEADFILE)
	$(COMPILER) label.c

edit.obj: edit.c $(HEADFILE)
	$(COMPILER) edit.c

button.obj: button.c $(HEADFILE)
	$(COMPILER) button.c

groupbox.obj: groupbox.c $(HEADFILE)
	$(COMPILER) groupbox.c

progress.obj: progress.c $(HEADFILE)
	$(COMPILER) progress.c

picture.obj: picture.c $(HEADFILE)
	$(COMPILER) picture.c

dashed.obj: dashed.c $(HEADFILE)
	$(COMPILER) dashed.c

sys_mntr.obj: sys_mntr.c $(HEADFILE)
	$(COMPILER) sys_mntr.c

view.obj: view.c $(HEADFILE)
	$(COMPILER) view.c

desktop.obj: desktop.c $(HEADFILE)
	$(COMPILER) desktop.c

$(tmpfile1): $(LibList1)
	del  $(tmpfile1)
	tlib $(tmpfile1) $(LibFile1)

$(tmpfile2): $(LibList2)
	del  $(tmpfile2)
	tlib $(tmpfile2) $(LibFile2)

$(Lib):  $(LibList1) $(LibList2)
	del  $(Lib)
	tlib $(Lib) +$(tmpfile1) +$(tmpfile2)
	copy $(Lib) ..



