/***********************************************************************************************
 * 文件: tcp.h
 * 说明: TCP(Transmission Control Protocol)
 * 作者: Jun
 * 特别: 尚未完成(参考自Linux/tcp)
***********************************************************************************************/
#ifndef FAMES_TCP_H
#define FAMES_TCP_H

/*----------------------------------------------------------------------------------------------
 *
 * TCP帧首部结构及常数
 *
**--------------------------------------------------------------------------------------------*/
#define HEADER_SIZE    64        /* maximum header size        */

struct tcphdr {
    INT16U  source;
    INT16U  dest;
    INT32U  seq;
    INT32U  ack_seq;
    INT16U  res1:4;
    INT16U  doff:4;
    INT16U  fin:1;
    INT16U  syn:1;
    INT16U  rst:1;
    INT16U  psh:1;
    INT16U  ack:1;
    INT16U  urg:1;
    INT16U  res2:2;
    INT16U  window;
    INT16U  check;
    INT16U  urg_ptr;
};

enum {
    TCP_ESTABLISHED = 1,
    TCP_SYN_SENT,
    TCP_SYN_RECV,
#if 0
    TCP_CLOSING, /* not a valid state, just a seperator so we can use
          < tcp_closing or > tcp_closing for checks. */
#endif
    TCP_FIN_WAIT1,
    TCP_FIN_WAIT2,
    TCP_TIME_WAIT,
    TCP_CLOSE,
    TCP_CLOSE_WAIT,
    TCP_LAST_ACK,
    TCP_LISTEN
};

#if 0
参考Linux1.0/tcp.*
#define MAX_SYN_SIZE    44 + sizeof (struct sk_buff) + MAX_HEADER
#define MAX_FIN_SIZE    40 + sizeof (struct sk_buff) + MAX_HEADER
#define MAX_ACK_SIZE    40 + sizeof (struct sk_buff) + MAX_HEADER
#define MAX_RESET_SIZE    40 + sizeof (struct sk_buff) + MAX_HEADER
#define MAX_WINDOW    4096
#define MIN_WINDOW    2048
#define MAX_ACK_BACKLOG    2
#define MIN_WRITE_SPACE    2048
#define TCP_WINDOW_DIFF    2048

/* urg_data states */
#define URG_VALID    0x0100
#define URG_NOTYET    0x0200
#define URG_READ    0x0400

#define TCP_RETR1    7    /*
                 * This is howmany retries it does before it
                 * tries to figure out if the gateway is
                 * down.
                 */

#define TCP_RETR2    15    /*
                 * This should take at least
                 * 90 minutes to time out.
                 */

#define TCP_TIMEOUT_LEN    (15*60*HZ) /* should be about 15 mins        */
#define TCP_TIMEWAIT_LEN (60*HZ) /* how long to wait to sucessfully 
                  * close the socket, about 60 seconds    */
#define TCP_ACK_TIME    3000    /* time to delay before sending an ACK    */
#define TCP_DONE_TIME    250    /* maximum time to wait before actually
                 * destroying a socket            */
#define TCP_WRITE_TIME    3000    /* initial time to wait for an ACK,
                     * after last transmit            */
#define TCP_CONNECT_TIME 2000    /* time to retransmit first SYN        */
#define TCP_SYN_RETRIES    5    /* number of times to retry openning a
                 * connection                 */
#define TCP_PROBEWAIT_LEN 100    /* time to wait between probes when
                 * I've got something to write and
                 * there is no window            */

#define TCP_NO_CHECK    0    /* turn to one if you want the default
                 * to be no checksum            */

#define TCP_WRITE_QUEUE_MAGIC 0xa5f23477

/*
 *    TCP option
 */
 
#define TCPOPT_NOP        1
#define TCPOPT_EOL        0
#define TCPOPT_MSS        2

/*
 * The next routines deal with comparing 32 bit unsigned ints
 * and worry about wraparound (automatic with unsigned arithmetic).
 */
static inline int before(unsigned long seq1, unsigned long seq2)
{
        return (long)(seq1-seq2) < 0;
}

static inline int after(unsigned long seq1, unsigned long seq2)
{
    return (long)(seq1-seq2) > 0;
}


/* is s2<=s1<=s3 ? */
static inline int between(unsigned long seq1, unsigned long seq2, unsigned long seq3)
{
    return (after(seq1+1, seq2) && before(seq1, seq3+1));
}


/*
 * List all states of a TCP socket that can be viewed as a "connected"
 * state.  This now includes TCP_SYN_RECV, although I am not yet fully
 * convinced that this is the solution for the 'getpeername(2)'
 * problem. Thanks to Stephen A. Wood <saw@cebaf.gov>  -FvK
 */
static inline const int
tcp_connected(const int state)
{
  return(state == TCP_ESTABLISHED || state == TCP_CLOSE_WAIT ||
     state == TCP_FIN_WAIT1   || state == TCP_FIN_WAIT2 ||
     state == TCP_SYN_RECV);
}
#endif

/*----------------------------------------------------------------------------------------------
 *
 * 函数声明
 *
**--------------------------------------------------------------------------------------------*/
void apical tcp_dispatcher(INT08S * buf, INT16S buf_len);


#endif                          /* #ifndef FAMES_TCP_H          */

/*==============================================================================================
 * 
 * 本文件结束: tcp.h
 * 
**============================================================================================*/


