
CHECKINC        = checkinc

HEADFILE_GLOBAL  = all.h debug.h includes.h os_ver.h
HEADFILE_CONFIG  = config\type.h config\hdware.h
HEADFILE_DRV     = drv\rs232.h drv\pci.h drv\nic.h drv\pci_ids.h drv\rtl8139.h drv\pktdrv.h drv\kbd.h
HEADFILE_KERNEL  = knl\core.h knl\init.h knl\task.h knl\lock.h knl\event.h knl\irq.h knl\timer.h knl\message.h knl\onexit.h
HEADFILE_NET     = net\net.h net\hook.h net\arp.h net\eth.h net\ip.h net\ipx.h net\icmp.h net\udp.h net\tcp.h net\udp_port.h net\tftpd.h
HEADFILE_MISC    = misc\tool.h misc\xms.h misc\misc.h misc\edge.h
HEADFILE_LOG     = log\error.h
HEADFILE_CRYPT   = crypt\crypt.h
HEADFILE_GUI     = gui\common.h gui\gui.h gui\gui_svc.h gui\gdi.h gui\vesa.h gui\bmp.h gui\font.h gui\palette.h gui\color.h gui\usr_wid.h gui\root_wid.h gui\pool.h
HEADFILE_GUI_CTL = gui\form.h gui\label.h gui\edit.h gui\button.h gui\progress.h gui\widget.h gui\picture.h gui\sys_mntr.h gui\dashed.h gui\view.h gui\groupbox.h
HEADFILE_GUI_DLG = gui\dlg_inpt.h
HEADFILE_PLC     = plc\plc_svc.h plc\plc.h plc\plc_api.h plc\fatek.h plc\siemens.h
HEADFILE_MNTR    = monitor\mntr_svc.h monitor\stack.h
HEADFILE_CON     = console\keysvc.h console\console.h


HEADFILE        = chkinc1 chkinc2 chkinc3 chkinc4 chkinc5 chkinc6 chkinc7 chkinc8 chkinc9 chkinc10 chkinc11 chkinc12

chkinc1: $(HEADFILE_GLOBAL)
	echo generate chkinc1        
	echo tempfile, can be deleted! > chkinc1
 
chkinc2: $(HEADFILE_CONFIG)
	echo generate chkinc2
	echo tempfile, can be deleted! > chkinc2

chkinc3: $(HEADFILE_DRV)
	echo generate chkinc3
	echo tempfile, can be deleted! > chkinc3

chkinc4: $(HEADFILE_KERNEL)
	echo generate chkinc4
	echo tempfile, can be deleted! > chkinc4

chkinc5: $(HEADFILE_NET)
	echo generate chkinc5
	echo tempfile, can be deleted! > chkinc5

chkinc6: $(HEADFILE_MISC)
	echo generate chkinc6
	echo tempfile, can be deleted! > chkinc6

chkinc7: $(HEADFILE_CRYPT)
	echo generate chkinc7
	echo tempfile, can be deleted! > chkinc7

chkinc8: $(HEADFILE_GUI) $(HEADFILE_GUI_CTL) $(HEADFILE_GUI_DLG)
	echo generate chkinc8
	echo tempfile, can be deleted! > chkinc8

chkinc9: $(HEADFILE_PLC)
	echo generate chkinc9
	echo tempfile, can be deleted! > chkinc9

chkinc10: $(HEADFILE_LOG)
	echo generate chkinc10
	echo tempfile, can be deleted! > chkinc10

chkinc11: $(HEADFILE_MNTR)
	echo generate chkinc11
	echo tempfile, can be deleted! > chkinc11

chkinc12: $(HEADFILE_CON)
	echo generate chkinc12
	echo tempfile, can be deleted! > chkinc12

$(CHECKINC): $(HEADFILE)
	echo checkinc is magic! > $(CHECKINC)


