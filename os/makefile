
LIB_UTIL   = tlib

OS_LIST    =  knl.lib  os_ver.lib  mm.lib  drv.lib  net.lib  misc.lib  plc.lib  gui.lib  console.lib  monitor.lib
OS_FILE    = +knl.lib +os_ver.lib +mm.lib +drv.lib +net.lib +misc.lib +plc.lib +gui.lib +console.lib +monitor.lib


OS_LIB     =  os.lib



$(OS_LIB): $(OS_LIST)
	DEL $(OS_LIB)
	$(LIB_UTIL) $(OS_LIB) $(OS_FILE)
	COPY $(OS_LIB) ..


