/***********************************************************************************************
** 文件: pci.c
** 说明: PCI相关函数定义
** 作者: Jun
** 时间: 2010-7-12
***********************************************************************************************/
#define  FAMES_PCI_C
#include "includes.h"

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_find_device_id()
 *
 * 说明:    通过ID号查找设备并确定其在PCI总线上的位置
 *
 * 输入:    1) ven_id         Vendor ID
 *          2) dev_id         Device ID
 *          3) bus            Bus Number
 *          4) devfn          Device/Function Number
 *          5) index          Index of the device to find
 *
 * 输出:    ok when found,  fail when not found!
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
                        break;   /* 扫描下一个设备 */
                    }
                }
            }
        }
    }
    return fail;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_find_device_class()
 *
 * 说明:    通过设备类型码查找设备并确定其在PCI总线上的位置
 *
 * 输入:    1) pci_class      PCI Class Number
 *          2) bus            Bus Number
 *          3) devfn          Device/Function Number
 *          4) index          Index of the device to find
 *
 * 输出:    ok when found,  fail when not found!
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
                        break;   /* 扫描下一个设备 */
                    }
                }
            }
        }
    }
    return fail;
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_write_cfg_byte()
 *
 * 说明:    向PCI配置空间写入一个字节
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *          4) val            Value to write
 *
 * 输出:    ok when success,  fail when not success!  hehe~~~
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
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
 * 函数:    pci_write_cfg_word()
 *
 * 说明:    向PCI配置空间写入一个字
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *          4) val            Value to write
 *
 * 输出:    ok when success,  fail when not success!  hehe~~~
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
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
 * 函数:    pci_write_cfg_dword()
 *
 * 说明:    向PCI配置空间写入一个双字
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *          4) val            Value to write
 *
 * 输出:    ok when success,  fail when not success!  hehe~~~
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
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
 * 函数:    pci_read_cfg_byte()
 *
 * 说明:    从PCI配置空间中读取一个字节
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *
 * 输出:    读取到的字节
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
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
 * 函数:    pci_read_cfg_word()
 *
 * 说明:    从PCI配置空间中读取一个字
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *
 * 输出:    读取到的字
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
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
 * 函数:    pci_read_cfg_dword()
 *
 * 说明:    从PCI配置空间中读取一个双字
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *          3) where          Register Index
 *
 * 输出:    读取到的双字
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
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
 * 函数:    pci_get_base_addr_io()
 *
 * 说明:    在6个基地址中取一个有效的I/O基地址
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 输出:    I/O基地址/0x0(查找失败时)
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
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
 * 函数:    pci_get_base_addr_mem()
 *
 * 说明:    在6个基地址中取一个有效的MEMORY基地址
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 输出:    MEM基地址/0x0L(查找失败时)
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
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
 * 函数:    pci_get_irq_line()
 *
 * 说明:    读取设备的IRQ-LINE
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 输出:    IRQ中断线(0xFF代表IRQ没有连接)
 *
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
**--------------------------------------------------------------------------------------------*/
INT08S pci_get_irq_line(INT16S bus, INT16S devfn)
{
    return (INT08S)pci_read_cfg_byte(bus, devfn, PCI_INTERRUPT_LINE);
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_enable_device()
 *
 * 说明:    设备使能
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
**--------------------------------------------------------------------------------------------*/
BOOL pci_enable_device(INT16S bus, INT16S devfn)
{
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, 
                PCI_COMMAND_MASTER|PCI_COMMAND_IO|PCI_COMMAND_MEMORY);
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_disable_device()
 *
 * 说明:    设备禁止
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
**--------------------------------------------------------------------------------------------*/
BOOL pci_disable_device(INT16S bus, INT16S devfn)
{
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, 0);
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_set_master()
 *
 * 说明:    设置为MASTER设备
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
**--------------------------------------------------------------------------------------------*/
BOOL pci_set_master(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg|PCI_COMMAND_MASTER);
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_clr_master()
 *
 * 说明:    取消设备的MASTER属性
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
**--------------------------------------------------------------------------------------------*/
BOOL pci_clr_master(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg&(~PCI_COMMAND_MASTER));
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_enable_io()
 *
 * 说明:    I/O存取使能
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
**--------------------------------------------------------------------------------------------*/
BOOL pci_enable_io(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg|PCI_COMMAND_IO);
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_disable_io()
 *
 * 说明:    I/O存取禁止
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
**--------------------------------------------------------------------------------------------*/
BOOL pci_disable_io(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg&(~PCI_COMMAND_IO));
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_enable_memory()
 *
 * 说明:    Memory存取使能
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
**--------------------------------------------------------------------------------------------*/
BOOL pci_enable_memory(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg|PCI_COMMAND_MEMORY);
}

/*----------------------------------------------------------------------------------------------
 * 函数:    pci_disable_memory()
 *
 * 说明:    Memory存取禁止
 *
 * 输入:    1) bus            Bus Number
 *          2) devfn          Device/Function Number
 *
 * 注意:    为了提高效率, 此函数并不检查参数的有效性
**--------------------------------------------------------------------------------------------*/
BOOL pci_disable_memory(INT16S bus, INT16S devfn)
{
    INT16U cmd_reg;

    cmd_reg = pci_read_cfg_word(bus, devfn, PCI_COMMAND);
    return pci_write_cfg_word(bus, devfn, PCI_COMMAND, cmd_reg&(~PCI_COMMAND_MEMORY));
}

/*
*本文件结束: pci.c =============================================================================
*/


