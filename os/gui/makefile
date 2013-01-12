COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc
LIB_UTIL    = tlib
LIB_LIST    =  gdi.lib  gdc.lib  res.lib  gui_wid.lib  gui_dlg.lib  gui_svc.obj  gui.obj  window.obj
LIB_FILE    = +gdi.lib +gdc.lib +res.lib +gui_wid.lib +gui_dlg.lib +gui_svc.obj +gui.obj +window.obj


gui_svc.obj: gui_svc.c $(HEADFILE)
	$(COMPILER) gui_svc.c

gui.obj: gui.c $(HEADFILE)
	$(COMPILER) gui.c

window.obj: window.c $(HEADFILE)
	$(COMPILER) window.c


LIB_NAME    =  gui.lib


$(LIB_NAME): $(LIB_LIST)
	DEL $(LIB_NAME)
	$(LIB_UTIL) $(LIB_NAME) $(LIB_FILE)
	COPY $(LIB_NAME) ..


