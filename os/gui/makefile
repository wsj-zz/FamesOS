COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc
LIB_UTIL    = tlib
LIB_LIST    =  gdi.lib  res.lib  gui_wid.lib  gui_dlg.lib  gui_svc.obj  gui.obj  root_wid.obj
LIB_FILE    = +gdi.lib +res.lib +gui_wid.lib +gui_dlg.lib +gui_svc.obj +gui.obj +root_wid.obj


gui_svc.obj: gui_svc.c $(HEADFILE)
	$(COMPILER) gui_svc.c

gui.obj: gui.c $(HEADFILE)
	$(COMPILER) gui.c

root_wid.obj: root_wid.c $(HEADFILE)
	$(COMPILER) root_wid.c


LIB_NAME    =  gui.lib


$(LIB_NAME): $(LIB_LIST)
	DEL $(LIB_NAME)
	$(LIB_UTIL) $(LIB_NAME) $(LIB_FILE)
	COPY $(LIB_NAME) ..


