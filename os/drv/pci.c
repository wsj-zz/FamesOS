/***********************************************************************************************
** �ļ�: pci.c
** ˵��: PCI��غ�������
** ����: Jun
** ʱ��: 2010-7-12
***********************************************************************************************/
#define  FAMES_PCI_C
#include "includes.h"

/*----------------------------------------------------------------------------------------------
 * ����:    pci_find_device_id()
 *
 * ˵��:    ͨ��ID�Ų����豸��ȷ������PCI�����ϵ�λ��
 *
 * ����:    1) ven_id         Vendor ID
 *          2) dev_id         Device ID
 *          3) bus            Bus Number
 *          4) devfn          Device/Function Number
 *          5) index          Index of the device to find
 *
 * ���:    ok when found,  fail when not found!
**--------------------------------------------------------------------------------------------*/
BOOL pci_find_device_id(INT16S ven_id, INT16S dev_id, INT16S * bus, INT16S * devfn, INT16S index)
{
    INT16S tempbus, tempdev, tempfunc;
    INT32U tempval, dstid;

    if(!bus || !devfn || index<0){
        return fail;
    }
    dstid=((INT32U)(INT32S)dev_id<<16UL)+(INT32U)(INT32S)ven_id;
    if(dstid==0xFFFFFFFFUL) {
        return fail;
    }
    for(tempbus=0; tempbus<PCI_MAX_BUS_NUM; tempbus++) {
        for(tempdev=0; tempdev<PCI_MAX_DEV_NUM; tempdev++) {
            for(tempfunc=0; tempfunc<PCI_MAX_FUNC_NUM; tempfunc++) {
                tempval=GET_PCI_CFG_ADDR((INT32S)tempbus,(INT32S)tempdev,(INT32S)tempfunc);
                DispatchLock();
                outportdword(PCI_CFG_IDX_PORT, tempval); 
                tempval=inportdword(PCI_CFG_DAT_PORT);
                DispatchUnlock();
                if(tempval==dstid) {
                    if(index==0) {
                        *bus   = tempbus;
                        *devfn =(tempdev*8+tempfunc);
                        return ok;
                    } else {
                        index--;
                        break;   /* ɨ����һ���豸 */
                    }
                }
            }
        }
    }
    return fail;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_find_device_class()
 *
 * ˵��:    ͨ���豸����������豸��ȷ������PCI�����ϵ�λ��
 *
 * ����:    1) pci_class      PCI Class Number
 *          2) bus            Bus Number
 *          3) devfn          Device/Function Number
 *          4) index          Index of the device to find
 *
 * ���:    ok when found,  fail when not found!
**--------------------------------------------------------------------------------------------*/
BOOL pci_find_device_class(INT16S pci_class, INT16S * bus, INT16S * devfn, INT16S index)
{
    INT16S tempbus, tempdev, tempfunc;
    INT32U tempval;

    if(!bus || !devfn || index<0){
        return fail;
    }
    for(tempbus=0; tempbus<PCI_MAX_BUS_NUM; tempbus++) {
        for(tempdev=0; tempdev<PCI_MAX_DEV_NUM; tempdev++) {
            for(tempfunc=0; tempfunc<PCI_MAX_FUNC_NUM; tempfunc++) {
                tempval=GET_PCI_CFG_ADDR((INT32S)tempbus,(INT32S)tempdev,(INT32S)tempfunc);
                DispatchLock();
                outportdword(PCI_CFG_IDX_PORT, tempval+(PCI_CLASS_DEVICE&(~0x3)) ); 
                tempval=(INT32U)inportdword(PCI_CFG_DAT_PORT);
                DispatchUnlock();
                tempval>>=16;
                if(pci_class==(INT16S)tempval) {
                    if(index==0) {
                        *bus   = tempbus;
                        *devfn =(tempdev*8+tempfunc);
                        return ok;
                    } else {
                        index--;
                        break;   /* ɨ����һ���豸 */
                    }
                }
            }
        }
    }
    return fail;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_write_cfg_byte()
 *
 * ˵��:    ��PCI���ÿռ�д��һ���ֽ�
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *          4) val            Value to write
 *
 * ���:    ok when success,  fail when not success!  hehe~~~
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_write_cfg_byte(INT16S bus, INT16S devfn, INT16S where, INT08U val)
{
    INT32U  tempval;

    tempval=GET_PCI_CFG_ADDR2((INT32S)bus, (INT32S)devfn);
    tempval+=(INT32U)(where&(~0x3));
    DispatchLock();
    outportdword(PCI_CFG_IDX_PORT, tempval);
    outportbyte(PCI_CFG_DAT_PORT+(where&0x3), val);
    DispatchUnlock();
    return ok;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_write_cfg_word()
 *
 * ˵��:    ��PCI���ÿռ�д��һ����
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *          4) val            Value to write
 *
 * ���:    ok when success,  fail when not success!  hehe~~~
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_write_cfg_word (INT16S bus, INT16S devfn, INT16S where, INT16U val)
{
    INT32U  tempval;

    tempval=GET_PCI_CFG_ADDR2((INT32S)bus, (INT32S)devfn);
    tempval+=(INT32U)(where&(~0x3));
    DispatchLock();
    outportdword(PCI_CFG_IDX_PORT, tempval);
    outportword(PCI_CFG_DAT_PORT+(where&0x2), val);
    DispatchUnlock();
    return ok;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_write_cfg_dword()
 *
 * ˵��:    ��PCI���ÿռ�д��һ��˫��
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *          4) val            Value to write
 *
 * ���:    ok when success,  fail when not success!  hehe~~~
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_write_cfg_dword(INT16S bus, INT16S devfn, INT16S where, INT32U val)
{
    INT32U tempval;

    tempval=GET_PCI_CFG_ADDR2((INT32S)bus, (INT32S)devfn);
    tempval+=(INT32U)(INT32S)where;
    DispatchLock();
    outportdword(PCI_CFG_IDX_PORT, tempval);
    outportdword(PCI_CFG_DAT_PORT, val);
    DispatchUnlock();
    return ok;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_read_cfg_byte()
 *
 * ˵��:    ��PCI���ÿռ��ж�ȡһ���ֽ�
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *
 * ���:    ��ȡ�����ֽ�
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
INT08U pci_read_cfg_byte(INT16S bus, INT16S devfn, INT16S where)
{
    INT32U tempval;
    INT08U retval;

    tempval=GET_PCI_CFG_ADDR2((INT32S)bus, (INT32S)devfn);
    tempval+=(INT32U)(where&(~0x3));
    DispatchLock();
    outportdword(PCI_CFG_IDX_PORT, tempval);
    retval=inportbyte(PCI_CFG_DAT_PORT+(where&0x3));
    DispatchUnlock();
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_read_cfg_word()
 *
 * ˵��:    ��PCI���ÿռ��ж�ȡһ����
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *
 * ���:    ��ȡ������
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
INT16U pci_read_cfg_word(INT16S bus, INT16S devfn, INT16S where)
{
    INT32U tempval;
    INT16U retval;

    tempval=GET_PCI_CFG_ADDR2((INT32S)bus, (INT32S)devfn);
    tempval+=(INT32U)(where&(~0x3));
    DispatchLock();
    outportdword(PCI_CFG_IDX_PORT, tempval);
    retval=inportword(PCI_CFG_DAT_PORT+(where&0x2));
    DispatchUnlock();
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_read_cfg_dword()
 *
 * ˵��:    ��PCI���ÿռ��ж�ȡһ��˫��
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *
 * ���:    ��ȡ����˫��
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
INT32U pci_read_cfg_dword (INT16S bus, INT16S devfn, INT16S where)
{
    INT32U tempval;
    INT32U retval;

    tempval=GET_PCI_CFG_ADDR2((INT32S)bus, (INT32S)devfn);
    where&=(~0x3);
    tempval+=(INT32U)(INT32S)where;
    DispatchLock();
    outportdword(PCI_CFG_IDX_PORT, tempval);
    retval=inportdword(PCI_CFG_DAT_PORT);
    DispatchUnlock();
    return retval;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_get_base_addr_io()
 *
 * ˵��:    ��6������ַ��ȡһ����Ч��I/O����ַ
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ���:    I/O����ַ/0x0(����ʧ��ʱ)
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
INT16S pci_get_base_addr_io(INT16S bus, INT16S devfn)
{
    INT16S iobaseaddr;
    INT16S i;

    for(i=0; i<6; i++) {
        iobaseaddr=(INT16S)pci_read_cfg_dword(bus, devfn, PCI_BASE_ADDRESS_0+i*4);
        if((iobaseaddr&PCI_BASE_ADDRESS_SPACE)==PCI_BASE_ADDRESS_SPACE_IO ) {
            iobaseaddr&=(~0x3);
            if(iobaseaddr!=0) {
                return iobaseaddr;
            }
        }
    }
    return 0x0;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_get_base_addr_mem()
 *
 * ˵��:    ��6������ַ��ȡһ����Ч��MEMORY����ַ
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ���:    MEM����ַ/0x0L(����ʧ��ʱ)
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
INT32S pci_get_base_addr_mem(INT16S bus, INT16S devfn)
{
    INT32S membaseaddr;
    INT16S i;

    for(i=0; i<6; i++) {
        membaseaddr=(INT32S)pci_read_cfg_dword(bus, devfn, PCI_BASE_ADDRESS_0+i*4);
        if((membaseaddr&(INT32S)PCI_BASE_ADDRESS_SPACE)==(INT32S)PCI_BASE_ADDRESS_SPACE_MEMORY  ) {
            membaseaddr&=(INT32S)PCI_BASE_ADDRESS_MEM_MASK;
            if(membaseaddr!=0L) {
                return membaseaddr;
            }
        }
    }
    return 0x0L;
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_get_irq_line()
 *
 * ˵��:    ��ȡ�豸��IRQ-LINE
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ���:    IRQ�ж���(0xFF����IRQû������)
 *
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
INT08S pci_get_irq_line(INT16S bus, INT16S devfn)
{
    return (INT08S)pci_read_cfg_byte(bus, devfn, PCI_INTERRUPT_LINE);
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_enable_device()
 *
 * ˵��:    �豸ʹ��
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_enable_device(INT16S bus, INT16S devfn)
{
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, 
                PCI_COMMAND_MASTER|PCI_COMMAND_IO|PCI_COMMAND_MEMORY);
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_disable_device()
 *
 * ˵��:    �豸��ֹ
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_disable_device(INT16S bus, INT16S devfn)
{
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, 0);
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_set_master()
 *
 * ˵��:    ����ΪMASTER�豸
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_set_master(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg|PCI_COMMAND_MASTER);
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_clr_master()
 *
 * ˵��:    ȡ���豸��MASTER����
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_clr_master(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg&(~PCI_COMMAND_MASTER));
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_enable_io()
 *
 * ˵��:    I/O��ȡʹ��
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_enable_io(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg|PCI_COMMAND_IO);
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_disable_io()
 *
 * ˵��:    I/O��ȡ��ֹ
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_disable_io(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg&(~PCI_COMMAND_IO));
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_enable_memory()
 *
 * ˵��:    Memory��ȡʹ��
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_enable_memory(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg|PCI_COMMAND_MEMORY);
}

/*----------------------------------------------------------------------------------------------
 * ����:    pci_disable_memory()
 *
 * ˵��:    Memory��ȡ��ֹ
 *
 * ����:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * ע��:    Ϊ�����Ч��, �˺�����������������Ч��
**--------------------------------------------------------------------------------------------*/
BOOL pci_disable_memory(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg&(~PCI_COMMAND_MEMORY));
}

/*
*���ļ�����: pci.c =============================================================================
*/


