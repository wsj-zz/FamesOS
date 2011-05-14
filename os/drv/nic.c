/*************************************************************************************
 * �ļ�: nic.c
 *
 * ����: FamesOS����������
 *
 * ����: Jun
 *
 * ʱ��: 2010-7-21
 *
 * ˵��: �������������ο���Linux-2.6.13
*************************************************************************************/
#define  FAMES_NIC_C
#include "includes.h"

/*------------------------------------------------------------------------------------
 * ��������ָ��
**----------------------------------------------------------------------------------*/
static BOOL (*current_nic_init)(void)                           = NULL;
static BOOL (*current_nic_open)(void)                           = NULL;                         
static BOOL (*current_nic_stop)(void)                           = NULL;                        
static BOOL (*current_nic_xmit)(void * srcbuf, INT16S buflen)   = NULL;
static BOOL (*current_nic_poll)(void * dstbuf, INT16S * buflen) = NULL;
static BOOL (*current_nic_control)(INT16S cmd, void * data)     = NULL;  
static BOOL (*current_nic_get_status)(void * status)            = NULL;         
static BOOL (*current_nic_islink)(void)                         = NULL;
static void (*current_nic_watchdog)(void *, INT16S)             = NULL;

static BOOL nic_open_state = NO; /* ������״̬ */

/*------------------------------------------------------------------------------------
 * ɨ��ϵͳ�е�����
**----------------------------------------------------------------------------------*/
BOOL scan_all_nic(void)
{
    INT16S bus_no;
    INT16S dev_func_no;
    
    nr_of_nic=0;

    for(;;){
        if(pci_find_device_class(PCI_CLASS_ETHERNET, &bus_no, &dev_func_no, nr_of_nic)){
            nic_in_system[nr_of_nic].BusNo=bus_no;
            nic_in_system[nr_of_nic].DeviceFuncNo=dev_func_no;
            nic_in_system[nr_of_nic].Supported=NO;
            nic_in_system[nr_of_nic].VendorID=pci_read_cfg_word(bus_no, dev_func_no, PCI_VENDOR_ID);
            nic_in_system[nr_of_nic].DeviceID=pci_read_cfg_word(bus_no, dev_func_no, PCI_DEVICE_ID);
            nic_in_system[nr_of_nic].SubVendorID=pci_read_cfg_word(bus_no, dev_func_no, PCI_SUBSYSTEM_VENDOR_ID);
            nic_in_system[nr_of_nic].SubDeviceID=pci_read_cfg_word(bus_no, dev_func_no, PCI_SUBSYSTEM_ID);
            nr_of_nic++;
            if(nr_of_nic>=NIC_MAX_NR){
                break;
            }
        } else {
            break;
        }
    }
    if(nr_of_nic>0){
        return ok;
    } else {
        return fail;
    }
}

/*------------------------------------------------------------------------------------
 * Ѱ��һ���ֱ��֧�ֵ�����(8139), ��������Ϊ��ǰ����
**----------------------------------------------------------------------------------*/
BOOL setup_working_nic_native(void)
{
    INT16S  i;
    NIC_INFO_BLOCK * ptrNIB;

    CurrentNIC=NULL;
    
    for(i=0; i<nr_of_nic; i++){
        nic_in_system[i].Supported=NO;
        ptrNIB=NICS;
        while(ptrNIB->VendorID!=0x0){
            if( nic_in_system[i].VendorID==ptrNIB->VendorID &&
                nic_in_system[i].DeviceID==ptrNIB->DeviceID ){ 
                nic_in_system[i].Supported=YES;     /* ֧��������� */
                if(CurrentNIC==NULL){
                    CurrentNIC=ptrNIB;
                    CurrentNIC->NicDriver->VendorID=nic_in_system[i].VendorID;
                    CurrentNIC->NicDriver->DeviceID=nic_in_system[i].DeviceID;
                    CurrentNIC->NicDriver->BusNo   =nic_in_system[i].BusNo;
                    CurrentNIC->NicDriver->DeviceFuncNo=nic_in_system[i].DeviceFuncNo;
                    CurrentNIC->NicDriver->NicName=ptrNIB->NicName;
                }
                break;
            }
            ptrNIB++;
        }
    }
    if(CurrentNIC!=NULL){      /* �ҵ���һ����õ����� */
        current_nic_init       =CurrentNIC->NicDriver->init;
        current_nic_open       =CurrentNIC->NicDriver->open;
        current_nic_stop       =CurrentNIC->NicDriver->stop;
        current_nic_xmit       =CurrentNIC->NicDriver->xmit;
        current_nic_poll       =CurrentNIC->NicDriver->poll;
        current_nic_control    =CurrentNIC->NicDriver->control;
        current_nic_get_status =CurrentNIC->NicDriver->get_status;
        current_nic_islink     =CurrentNIC->NicDriver->islink;
        current_nic_watchdog   =CurrentNIC->NicDriver->watchdog;
        nic_driver_type        =NIC_DRIVER_TYPE_NATIVE;
        return nic_init();
    } else {                   /* û���ҵ����õ�����   */
        current_nic_init       =NULL;
        current_nic_open       =NULL;
        current_nic_stop       =NULL;
        current_nic_xmit       =NULL;
        current_nic_poll       =NULL;
        current_nic_control    =NULL;
        current_nic_get_status =NULL;
        current_nic_islink     =NULL;
        current_nic_watchdog   =NULL;
        return fail;
    }
}

/*------------------------------------------------------------------------------------
 * Ѱ��һ���֧�ֵ�����(PacketDriver)
**----------------------------------------------------------------------------------*/
#if NIC_PKTDRV_EN==1
BOOL setup_working_nic_pktdrv(void)
{
    BOOL retval;

    retval = pktdrv_setup();

    if(retval){      /* �ҵ���ĳ������PKT���� */
        current_nic_init       =PKTDRV_DRV.init;
        current_nic_open       =PKTDRV_DRV.open;
        current_nic_stop       =PKTDRV_DRV.stop;
        current_nic_xmit       =PKTDRV_DRV.xmit;
        current_nic_poll       =PKTDRV_DRV.poll;
        current_nic_control    =PKTDRV_DRV.control;
        current_nic_get_status =PKTDRV_DRV.get_status;
        current_nic_islink     =PKTDRV_DRV.islink;
        current_nic_watchdog   =PKTDRV_DRV.watchdog;
        nic_driver_type        =NIC_DRIVER_TYPE_PKTDRV;
        CurrentNIC             =&PKTDRV_NICS;
        return nic_init();
    } else {         /* û���ҵ�PKT���� */
        current_nic_init       =NULL;
        current_nic_open       =NULL;
        current_nic_stop       =NULL;
        current_nic_xmit       =NULL;
        current_nic_poll       =NULL;
        current_nic_control    =NULL;
        current_nic_get_status =NULL;
        current_nic_islink     =NULL;
        current_nic_watchdog   =NULL;
        return fail;
    }
}
#endif

/*------------------------------------------------------------------------------------
 * Ѱ��һ���֧�ֵ�����(NDIS)
**----------------------------------------------------------------------------------*/
BOOL setup_working_nic_ndis(void)
{
    BOOL retval;

    retval = fail;
    
    return retval;
}

/*------------------------------------------------------------------------------------
 * Ѱ��һ���֧�ֵ�����(����Native, PacketDriver, NDIS)
**----------------------------------------------------------------------------------*/
BOOL setup_working_nic(void)
{
    BOOL retval;

    retval = setup_working_nic_ndis();
    
    #if NIC_PKTDRV_EN==1
    if(!retval)
        retval = setup_working_nic_pktdrv();
    #endif
    
    if(!retval)
        retval = setup_working_nic_native();

    return retval;
}

/*------------------------------------------------------------------------------------
 * ������ʼ��, ������дPCI���ÿռ��
**----------------------------------------------------------------------------------*/
BOOL nic_init(void)
{   
    /*
    char ts[32];
    prt_string(20,5, INT16toSTR(ts, sizeof(NICS)/sizeof(NIC_INFO_BLOCK)-1,CHG_OPT_DEC|CHG_OPT_END),0x70);
    */
    nic_open_state = NO;
    
    if(current_nic_init){
        return (*current_nic_init)();
    }
    return fail;
}

/*------------------------------------------------------------------------------------
 * ������, ʹ�俪ʼ����
**----------------------------------------------------------------------------------*/
BOOL nic_open(void)
{
    BOOL retval;
    
    if(nic_open_state){
        return fail;
    }

    retval = fail;
    
    if(current_nic_open){
        retval = (*current_nic_open)();
    }

    if(retval){
        nic_open_state = YES;
        ExecuteNetworkHooks(NULL, NETWORK_HOOK_OPEN);
    }
    
    return retval;
}

/*------------------------------------------------------------------------------------
 * �ر�����
**----------------------------------------------------------------------------------*/
BOOL nic_stop(void)
{
    BOOL retval;
    
    if(!nic_open_state){
        return fail;
    }

    retval = fail;
    
    if(current_nic_stop){
        retval = (*current_nic_stop)();
    }

    if(retval){
        nic_open_state = NO;
        ExecuteNetworkHooks(NULL, NETWORK_HOOK_STOP);
    }
    
    return retval;
}

/*------------------------------------------------------------------------------------
 * ����һ�����ݰ�
**----------------------------------------------------------------------------------*/
BOOL nic_xmit(void * srcbuf, INT16S buflen)
{
    if(!nic_open_state)return fail;
    
    ExecuteNetworkHooks(srcbuf, NETWORK_HOOK_XMIT);
    
    if(current_nic_xmit){
        return (*current_nic_xmit)(srcbuf, buflen);
    }
    return fail;
}

/*------------------------------------------------------------------------------------
 * ��ѯ�Ƿ������ݰ��յ�
**----------------------------------------------------------------------------------*/
BOOL nic_poll(void * dstbuf, INT16S * buflen)
{
    BOOL retval;
    #if 0
    char testbuf[]="\x11\x22\x33\x44\x55\x66\x01\x02\x03\x04\x05\x06\x08\x00"
                   "\x45\x00\x00\x34\x33\x44\x00\x00\x80\x01\x0E\x04\x44\x00"
                   "\xA8\xC0\x64\x00\xA8\xC0\x08\x00\x0C\x14\x23\x23\x23\x23"
                   "\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23"
                   "\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23";
    if(0){
        memcpy(dstbuf, testbuf, sizeof(testbuf));
        * buflen = sizeof(testbuf);
        return ok;
    }
    #endif
    if(!nic_open_state)return fail;

    retval = fail;
    
    if(current_nic_poll){
        retval = (*current_nic_poll)(dstbuf, buflen);
    }
    if(retval)
        ExecuteNetworkHooks(dstbuf, NETWORK_HOOK_RECV);
    
    return retval;
}

/*------------------------------------------------------------------------------------
 * ��������
**----------------------------------------------------------------------------------*/
BOOL nic_control(INT16S cmd, void * data)
{
    if(!nic_open_state)return fail;
    if(current_nic_control){
        return (*current_nic_control)(cmd, data);
    }
    return fail;
}

/*------------------------------------------------------------------------------------
 * ������״̬
**----------------------------------------------------------------------------------*/
BOOL nic_get_status(void * status)
{
    if(!nic_open_state)return fail;
    if(current_nic_get_status){
        return (*current_nic_get_status)(status);
    }
    return fail;
}

/*------------------------------------------------------------------------------------
 * �����Ƿ�������
**----------------------------------------------------------------------------------*/
BOOL nic_islink(void)
{
    if(!nic_open_state)return fail;
    if(current_nic_islink){
        return (*current_nic_islink)();
    }
    return fail;
}


/*------------------------------------------------------------------------------------
 * �������Ź���ʱ����
**----------------------------------------------------------------------------------*/
void nic_watchdog(void * data, INT16S times)
{
    if(!nic_open_state)return;
    if(current_nic_watchdog){
        (*current_nic_watchdog)(data, times);
    }
}


/*
*���ļ�����: nic.c ===================================================================
*/

