COMPILER    = tcc -c -1 -B -S -ml -w -Ic:\tc\include -I.\..\include
HEADFILE    = ..\include\checkinc

dstfile     = net.lib

Lib         = $(dstfile)
LibList     =  net.obj  hook.obj  eth.obj  arp.obj  ip.obj  ipx.obj  icmp.obj  udp.obj  tcp.obj  udp_port.obj  tftpd.obj
LibFile     = +net.obj +hook.obj +eth.obj +arp.obj +ip.obj +ipx.obj +icmp.obj +udp.obj +tcp.obj +udp_port.obj +tftpd.obj

net.obj: net.c $(HEADFILE)
	$(COMPILER) net.c

hook.obj: hook.c $(HEADFILE)
	$(COMPILER) hook.c

eth.obj: eth.c $(HEADFILE)
	$(COMPILER) eth.c

arp.obj: arp.c $(HEADFILE)
	$(COMPILER) arp.c

ip.obj: ip.c $(HEADFILE)
	$(COMPILER) ip.c

ipx.obj: ipx.c $(HEADFILE)
	$(COMPILER) ipx.c

icmp.obj: icmp.c $(HEADFILE)
	$(COMPILER) icmp.c

udp.obj: udp.c $(HEADFILE)
	$(COMPILER) udp.c

tcp.obj: tcp.c $(HEADFILE)
	$(COMPILER) tcp.c

udp_port.obj: udp_port.c $(HEADFILE)
	$(COMPILER) udp_port.c

tftpd.obj: tftpd.c $(HEADFILE)
	$(COMPILER) tftpd.c

$(Lib):  $(LibList)
	del  $(Lib)
	tlib $(Lib) $(LibFile)
	copy $(Lib) ..



