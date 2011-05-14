/*************************************************************************************
 * �ļ�: pktdrv.h
 *
 * ����: PacketDriver�������ӿ�
 *
 * ����: Jun
*************************************************************************************/
#ifndef FAMES_NIC_PKTDRV_H
#define FAMES_NIC_PKTDRV_H

/*------------------------------------------------------------------------------------
 *      PKTDRVģ��֧��, 1=֧��PKTDRVģ��, 0=��֧��
**----------------------------------------------------------------------------------*/
#define NIC_PKTDRV_EN   0


#if NIC_PKTDRV_EN == 1
/*------------------------------------------------------------------------------------
 *        ��������
**----------------------------------------------------------------------------------*/
#define PKTDRV_MAX_LEN  1600


/*------------------------------------------------------------------------------------
 *        �ڲ�����
**----------------------------------------------------------------------------------*/
#ifdef FAMES_NIC_PKTDRV_C
#endif


/*------------------------------------------------------------------------------------
 *        ��������
**----------------------------------------------------------------------------------*/
BOOL init_pktdrv(void);
BOOL open_pktdrv(void);
BOOL stop_pktdrv(void);
BOOL xmit_pktdrv(void * srcbuf, INT16S buflen);
BOOL poll_pktdrv(void * dstbuf, INT16S * buflen);
BOOL ctrl_pktdrv(INT16S cmd, void * data);
BOOL get_status_pktdrv(void * status);
BOOL islink_pktdrv(void);
void watchdog_pktdrv(void * data, INT16S nr);

BOOL   pktdrv_setup(void);
INT16S pktdrv_probe(void);

/*------------------------------------------------------------------------------------
 *        ������Ϣ��(PKTDRV)
**----------------------------------------------------------------------------------*/
#ifdef FAMES_NIC_PKTDRV_C
NIC_DRIVER_BLOCK PKTDRV_DRV=
       {            
          init_pktdrv,
          open_pktdrv,
          stop_pktdrv,
          xmit_pktdrv,
          poll_pktdrv,
          ctrl_pktdrv,
          get_status_pktdrv,
          islink_pktdrv,
          watchdog_pktdrv,
       };
#else
extern NIC_DRIVER_BLOCK PKTDRV_DRV;
#endif

#endif   /* #if NIC_PKTDRV_EN == 1     */

#endif   /* #ifndef FAMES_NIC_PKTDRV_H */

/*====================================================================================
 * 
 * ���ļ�����: pktdrv.h
 * 
**==================================================================================*/

