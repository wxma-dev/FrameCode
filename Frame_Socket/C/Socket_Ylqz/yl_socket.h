

typedef unsigned short u_port_t;

int u_ignore_sigpipe();
int u_open( u_port_t port );
int yl_listen( int fd, char *hostn );
int u_close( int fd );
int CloseSocket( int iSocket );
ssize_t u_read( int fd, void *buf, size_t size );
ssize_t u_write( int fd, void *buf, size_t size );
int u_connect_ext( u_port_t port, char *hostn );
int CUPSRecvMsg( int communfd, char *pczTranBuf, int *piBuflen, int iDelayTime );
void ConnectTimeout( int iSig );
int TranClientByShortTcpIp_ext( short iClientPort, char *pczHostName,
                                int iConnectTimes, int iConnectDelay );
void WriteMsgTimeout( int iSig );
int CUPSSendZeroToSocket( int communfd, int iDelayTime);
int SendMsgToSocket( int communfd, char *pczTranBuf, 
                      int iBuflen, int iDelayTime, int iHeadFlag );
