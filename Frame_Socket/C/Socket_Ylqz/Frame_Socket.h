
#define    MSG_SIZE                4096
          
#define    CONFIG_FILE             "socket.cfg"

#define    CUPS_MAX_DATA_LEN       1846

#define    MAXBACKLOG              64

#define    CUPS_HEAD_LEN           46

/*   缓冲区空间大小 */
#define    MAX_DATA_LEN            102400

/* 接收链接超时断开时间 */
#define    CHECK_LINK_RECV_TIMEOUT 301

/* 发送空包间隔时间 */
#define    CHECK_LINK_SEND_TIMEOUT 20

/*   对报文长度头的定义   */
#define    NO_HEAD                 0
#define    HEXBYTE_TWO_HAS_HEAD    1
#define    HEXBYTE_TWO_NONE_HEAD   2
#define    ASCII_FOUR_HAS_HEAD     3
#define    ASCII_FOUR_NONE_HEAD    4
#define    ASCII_FIVE_NONE_HEAD    6
#define    ASCII_NINE_NONE_HEAD    7
#define    FF_ASCII_THREE_NONE_HEAD 5
#define    ASCII_EIGHT_NONE_HEAD   8

typedef struct{
    int       seq;               /* 顺序号         */
    char      ip[20];            /* 通讯服务器IP   */
    int       port;              /* 服务器通讯端口 */
    int       shmkey;            /* 共享内存key    */
    key_t     msgrecvid;         /* 发送消息队列id */
    key_t     msgsendid;         /* 接收消息队列id */
    key_t     msgsendidnet;      /* 接收消息队列id */
} YLCFG;

/*银联通讯共享内存-管理4进4出状态*/
typedef struct
{
    int      seq;        /*进程编号*/
    int      type;       /*类型 0-接收 1-发送*/
    long     pid;        /*进程号*/
    int      status;     /*标志 0-不正常 1-正常*/
    long     time;       /*活动时间戳*/
    char     msg[32];    /*状态*/
} YLCOMMMSG;

/* 消息队列结构 */
typedef struct {
    long            mtype;
    unsigned char   mtext[MSG_SIZE];
} MSGSTRUCT;

