
#define    MSG_SIZE                4096
          
#define    CONFIG_FILE             "socket.cfg"

#define    CUPS_MAX_DATA_LEN       1846

#define    MAXBACKLOG              64

#define    CUPS_HEAD_LEN           46

/*   �������ռ��С */
#define    MAX_DATA_LEN            102400

/* �������ӳ�ʱ�Ͽ�ʱ�� */
#define    CHECK_LINK_RECV_TIMEOUT 301

/* ���Ϳհ����ʱ�� */
#define    CHECK_LINK_SEND_TIMEOUT 20

/*   �Ա��ĳ���ͷ�Ķ���   */
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
    int       seq;               /* ˳���         */
    char      ip[20];            /* ͨѶ������IP   */
    int       port;              /* ������ͨѶ�˿� */
    int       shmkey;            /* �����ڴ�key    */
    key_t     msgrecvid;         /* ������Ϣ����id */
    key_t     msgsendid;         /* ������Ϣ����id */
    key_t     msgsendidnet;      /* ������Ϣ����id */
} YLCFG;

/*����ͨѶ�����ڴ�-����4��4��״̬*/
typedef struct
{
    int      seq;        /*���̱��*/
    int      type;       /*���� 0-���� 1-����*/
    long     pid;        /*���̺�*/
    int      status;     /*��־ 0-������ 1-����*/
    long     time;       /*�ʱ���*/
    char     msg[32];    /*״̬*/
} YLCOMMMSG;

/* ��Ϣ���нṹ */
typedef struct {
    long            mtype;
    unsigned char   mtext[MSG_SIZE];
} MSGSTRUCT;

