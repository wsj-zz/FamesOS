/*************************************************************************************
 * 文件: net.c
 *
 * 说明: 网络模块(以太网)
 *
 * 作者: Jun
 *
 * 描述: 网络模块的初始化及退出, 网络数据包的接收与处理
 *       网络协议栈的处理, 网络
*************************************************************************************/
#define  FAMES_NET_C
#include <includes.h>

/*------------------------------------------------------------------------------------
 * 
 * 函数或变量
 * 
**----------------------------------------------------------------------------------*/
void   __daemon NetworkDaemon(void * data);
void   __exit   OnExit_network(void);

static const  INT16U NetworkDaemonStackSize = 4096; /* 网络服务任务的堆栈大小 */
static        HANDLE NetworkDaemonHandle = InvalidHandle;

static BOOL   network_open_state = NO;

static void * nic_rx_buffer  = NULL;

#define       NIC_RX_BUF_LEN      2048

/*------------------------------------------------------------------------------------
 * 函数:    InitNetService()
 *
 * 描述:    初始化网络服务模块
 *
 * 说明:    具体动作有: 初始化网络数据包钩子
 *                      扫描系统中可支持的网卡并打开网卡
 *                      注册网络模块OnExit例程
 *                      创建网络服务任务, 
 *                      打开网卡, 网络服务器开始运行
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
 * 函数:    OnExit_network()
 *
 * 描述:    网络模块OnExit例程
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
 * 函数:    OpenNetwork()
 *
 * 描述:    打开网络服务
 *
 * 说明:    具体动作有: 重新打开网卡, 并使服务任务NetworkDaemon()恢复运行
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
 * 函数:    StopNetwork()
 *
 * 描述:    关闭网络服务
 *
 * 说明:    具体动作有: 关闭网卡, 挂起服务任务NetworkDaemon()
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
 * 函数:    RestartNetwork()
 *
 * 描述:    重新启动网络服务
 *
 * 说明:    具体动作有: 关闭网卡, 再重新打开
**----------------------------------------------------------------------------------*/
BOOL apical RestartNetwork(void)
{

    return (StopNetwork() && OpenNetwork());

}

/*------------------------------------------------------------------------------------
 * 函数:    NetworkDaemon()
 *
 * 描述:    网络服务任务, 读取网卡数据包并处理
 *
 * 说明:    建议堆栈大小设为1024
**----------------------------------------------------------------------------------*/
void __daemon NetworkDaemon(void * data)
{
    INT16S rx_len;
    static INT16S run_flag=0;

    data = data;

    lock_kernel();
    if(run_flag){ /* 此任务只能被创建一次 */
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
        TaskSleep(5L); /* 打个小盹儿 */
    }
}


/*====================================================================================
 * 
 * 本文件结束: net.c
 * 
**==================================================================================*/


