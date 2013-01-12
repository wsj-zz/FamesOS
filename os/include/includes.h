/*************************************************************************************
 * 文件: includes.h
 *
 * 说明: 总包含文件
 *
 * 作者: Jun
 *
 * 时间: 2010-02-07
*************************************************************************************/
#ifndef FAMES_INCLUDES_H
#define FAMES_INCLUDES_H

#include  <stdio.h>            /* standard */
#include  <stdlib.h>
#include  <dos.h>
#include  <io.h>
#include  <fcntl.h>
#include  <stat.h>
#include  <mem.h>
#include  <alloc.h>
#include  <bios.h>
#include  <conio.h>
#include  <assert.h>
#include  <setjmp.h>

#include  "config\hdware.h"
#include  "config\type.h"      /* config */

#include  "all.h"              /* common */
#include  "debug.h"            /* debug/FamesOS Assert */
#include  "os_ver.h"           /* os version */

#include  "log\error.h"

#include  "knl\lock.h"

#include  "knl\core.h"         /* kernel */
#include  "knl\init.h"
#include  "knl\message.h"
#include  "knl\Task.h"
#include  "knl\irq.h"
#include  "knl\timer.h"
#include  "knl\Event.h"
#include  "knl\onexit.h"

#include  "mm\mm_svc.h"        /* memory  */

#include  "drv\rs232.h"        /* drivers */
#include  "drv\pci.h"
#include  "drv\nic.h"
#include  "drv\kbd.h"

#include  "net\eth.h"          /* net */
#include  "net\arp.h"
#include  "net\ipx.h"
#include  "net\ip.h"
#include  "net\icmp.h"
#include  "net\tcp.h"
#include  "net\udp.h"
#include  "net\tftpd.h"
#include  "net\httpd.h"
#include  "net\net.h"
#include  "net\hook.h"
#include  "net\udp_port.h"

#include  "net\fames.h"        /* fames */

#include  "misc\tool.h"        /* misc */
#include  "misc\xms.h"
#include  "misc\misc.h"
#include  "misc\edge.h"

#include  "gui\common.h"       /* gui */
#include  "gui\color.h"
#include  "gui\vesa.h"
#include  "gui\font.h"
#include  "gui\bmp.h"
#include  "gui\gdi.h"
#include  "gui\gui_svc.h"
#include  "gui\gui.h"
#include  "gui\widget.h"
#include  "gui\window.h"
#include  "gui\gdc.h"
#include  "gui\pool.h"
#include  "gui\desktop.h"
#include  "gui\form.h"
#include  "gui\label.h"
#include  "gui\edit.h"
#include  "gui\button.h"
#include  "gui\groupbox.h"
#include  "gui\progress.h"
#include  "gui\picture.h"
#include  "gui\dashed.h"
#include  "gui\usr_wid.h"
#include  "gui\sys_mntr.h"
#include  "gui\view.h"
#include  "gui\dlg_inpt.h"

#include  "plc\plc_svc.h"      /* plc    */
#include  "plc\plc.h"
#include  "plc\fatek.h"
#include  "plc\siemens.h"
#include  "plc\plc_api.h"

#include  "console\keysvc.h"   /* console */
#include  "console\console.h"

#include  "monitor\mntr_svc.h" /* monitor */
#include  "monitor\stack.h"

#endif

/*====================================================================================
 * 
 * 本文件结束: includes.h
 * 
**==================================================================================*/


