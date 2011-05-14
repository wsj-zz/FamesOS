/*************************************************************************************
 * �ļ�: net.c
 *
 * ˵��: ����ģ��(��̫��)
 *
 * ����: Jun
 *
 * ����: ����ģ��ĳ�ʼ�����˳�, �������ݰ��Ľ����봦��
 *       ����Э��ջ�Ĵ���, ����
*************************************************************************************/
#define  FAMES_NET_C
#include <includes.h>

/*------------------------------------------------------------------------------------
 * 
 * ���������
 * 
**----------------------------------------------------------------------------------*/
void   __daemon NetworkDaemon(void * data);
void   __exit   OnExit_network(void);

static const  INT16U NetworkDaemonStackSize = 4096; /* �����������Ķ�ջ��С */
static        HANDLE NetworkDaemonHandle = InvalidHandle;

static BOOL   network_open_state = NO;

static void * nic_rx_buffer  = NULL;

#define       NIC_RX_BUF_LEN      2048

/*------------------------------------------------------------------------------------
 * ����:    InitNetService()
 *
 * ����:    ��ʼ���������ģ��
 *
 * ˵��:    ���嶯����: ��ʼ���������ݰ�����
 *                      ɨ��ϵͳ�п�֧�ֵ�������������
 *                      ע������ģ��OnExit����
 *                      ���������������, 
 *                      ������, �����������ʼ����
**----------------------------------------------------------------------------------*/
void apical __init InitNetService(void)
{
    BOOL retval = fail;

    network_open_state  = NO;
    NetworkDaemonHandle = InvalidHandle;
    
    arp_initialize();           /* initialize arp protocol               */
    set_local_ip(0xFFFFFFFFuL); /* initialize ip addr to 255.255.255.255 */

    nic_rx_buffer = (void *)mem_alloc((INT32U)NIC_RX_BUF_LEN);
    if(!nic_rx_buffer){
        sys_print("InitNetService: failed to allocate memory!\n");
        SetErrorCode(ERROR_NO_MEM);
        return;
    }

    InitNetworkHook();          /* initialize network hooks              */
    udp_port_initialize();      /* initialize udp.port manager           */

    if( scan_all_nic() && setup_working_nic() ){
        RegisterOnExit(OnExit_network); /* register OnExit routine */
        NetworkDaemonHandle = TaskCreate(NetworkDaemon, NULL, 
                                         "network", 
                                         NULL, 
                                         NetworkDaemonStackSize, 
                                         PRIO_NET, TASK_CREATE_OPT_NONE);
        if(NetworkDaemonHandle == InvalidHandle){
            retval = fail;      /* failed to create network daemon       */
            sys_print("InitNetService: failed to create NetworkDaemon!\n");
        } else if(!nic_open()){
            retval = fail;      /* failed to open nic                    */
            sys_print("InitNetService: nic can not open\n");
        }
    } else {
        sys_print("InitNetService: There is no nic supported\n");
    }

    retval = retval; /* prevent warning */

    return;
}

/*------------------------------------------------------------------------------------
 * ����:    OnExit_network()
 *
 * ����:    ����ģ��OnExit����
**----------------------------------------------------------------------------------*/
void __exit OnExit_network(void)
{
    if(NetworkDaemonHandle != InvalidHandle){
        if(!StopNetwork()){;}
        if(!TaskDelete(NetworkDaemonHandle)){;}
    }
    if(nic_rx_buffer){
        mem_free(nic_rx_buffer);
    }
}

/*------------------------------------------------------------------------------------
 * ����:    OpenNetwork()
 *
 * ����:    ���������
 *
 * ˵��:    ���嶯����: ���´�����, ��ʹ��������NetworkDaemon()�ָ�����
**----------------------------------------------------------------------------------*/
BOOL apical OpenNetwork(void)
{
    if(network_open_state)
        return fail;
    nic_open();
    TaskResume(NetworkDaemonHandle);
    
    network_open_state = YES;
    
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    StopNetwork()
 *
 * ����:    �ر��������
 *
 * ˵��:    ���嶯����: �ر�����, �����������NetworkDaemon()
**----------------------------------------------------------------------------------*/
BOOL apical StopNetwork(void)
{
    if(!network_open_state)
        return fail;
    nic_stop();
    TaskSuspend(NetworkDaemonHandle);
    
    network_open_state = NO;
    
    return ok;
}

/*------------------------------------------------------------------------------------
 * ����:    RestartNetwork()
 *
 * ����:    ���������������
 *
 * ˵��:    ���嶯����: �ر�����, �����´�
**----------------------------------------------------------------------------------*/
BOOL apical RestartNetwork(void)
{

    return (StopNetwork() && OpenNetwork());

}

/*------------------------------------------------------------------------------------
 * ����:    NetworkDaemon()
 *
 * ����:    �����������, ��ȡ�������ݰ�������
 *
 * ˵��:    �����ջ��С��Ϊ1024
**----------------------------------------------------------------------------------*/
void __daemon NetworkDaemon(void * data)
{
    INT16S rx_len;
    static INT16S run_flag=0;

    data = data;

    lock_kernel();
    if(run_flag){ /* ������ֻ�ܱ�����һ�� */
        unlock_kernel();
        task_return;
    } else {
        run_flag = 1;
    }
    unlock_kernel();

    for(;;){
        if(nic_poll(nic_rx_buffer, &rx_len)){
            eth_dispatcher(nic_rx_buffer, rx_len);
        }
        TaskSleep(5L); /* ���С��� */
    }
}


/*====================================================================================
 * 
 * ���ļ�����: net.c
 * 
**==================================================================================*/


