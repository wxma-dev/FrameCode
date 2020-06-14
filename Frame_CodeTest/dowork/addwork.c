#include "struct.h"

#include "addwork.h"

#define BUF_TMP_SIZE 1024

void addwork_thread_singnal_fun( int signo )
{
	DEBUG( "不执行任何动作，只为激活epoll_wait返回-1" );
	return;
}

int setnonblocking( int sockfd )
{
	if ( -1 == fcntl(sockfd, F_SETFL, fcntl( sockfd, F_GETFD, 0 ) | O_NONBLOCK ) )
	{
		return -1;
	}
	return 0;
}

extern void mq_send_info_ywjc();

/*********************************************************************
* Function：                                                         *
*    int thread_routine_addwork(void *(*process)(void *arg), void *arg)*
* Description：                                                      *
*    往CThread_worker链表中添加任务（函数，参数）                    *
* In Params:                                                         *
*                                                                    *
* Out Params:                                                        *
*                                                                    *
* Return:                                                            *
*                                                                    *
*********************************************************************/
void *thread_routine_addwork( void *(*process)(void *), void *arg )
{
	struct    sockaddr_in service_addr;          /* 服务端地址结构 */
	struct    sockaddr_in client_addr;           /* 客户端地址结构 */
	int       socket_fd;                         /* 网络描述符 */
	int       address_size;
	char    buf[MAXLINE];
	char    str[INET_ADDRSTRLEN];
	int    len;
	int    iRet;
	int    iPort; 
	int    listenq;
	int    epoll_fd;
	struct epoll_event ev;
	struct epoll_event events[MAXEPOLLSIZE];
	int    wait_count;
	int    i;
	int    connect_fd;
	int    cur_fd_count;
	socklen_t client_len;
	int   ctl_fd;
	ssize_t   iRet_Read;
	ssize_t   iRet_Write;
	char      buf_tmp[BUF_TMP_SIZE];
	int  iRead_Count;

	listenq = 1024;
	epoll_fd = 0;
	socket_fd = 0;
	address_size = 0;
	iPort = 9999;
	wait_count = 0;	
	connect_fd = 0;
	cur_fd_count = 0;
	client_len = sizeof( struct sockaddr_in );
	ctl_fd = 0;
	iRet_Read = 0;
	iRet_Write = 0;
	iRead_Count = 0;

	memset( &service_addr, 0x00, sizeof( service_addr ) );
	memset( &client_addr, 0x00, sizeof( client_addr ) );
	memset( buf, 0x00, sizeof( buf ) );
	memset( &ev, 0x00, sizeof( ev ) );
	memset( &events, 0x00, sizeof( events ) );
	memset( buf_tmp, 0x00, sizeof( buf_tmp ) );

	INFO( "thread of addwork [%d] start.", pthread_self() );
	
	signal( SIGUSR1, addwork_thread_singnal_fun );

	service_addr.sin_family = AF_INET;
	service_addr.sin_addr.s_addr = INADDR_ANY;
	service_addr.sin_port = htons( iPort );

	socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( -1 == socket_fd )
	{
		ERROR( "socket error." );
		return NULL;
	}
	INFO( "socket succ." );

	int opt = 1;
	setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) );

	if ( 0 > setnonblocking(socket_fd) ) 
	{
		ERROR( "setnonblocking error." );
		return NULL;
	}
	INFO( "setnonblocking succ." );

	iRet = bind( socket_fd, (struct sockaddr *)&service_addr, sizeof(service_addr) );
	if ( -1 == iRet )
	{
		ERROR( "bind error." );
		return NULL;
	}
	INFO( "bind succ." );

	if ( -1 == listen( socket_fd, listenq ) )
	{
		ERROR( "listen error." );
		return NULL;
	}
	INFO( "listen succ." );

	epoll_fd = epoll_create( 3024000 );
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = socket_fd;
	INFO( "epoll_create succ." );

	if ( 0 != epoll_ctl( epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev ) )
	{
		ERROR( "epoll_ctl error." );
		return NULL;
	}
	INFO( "epoll_ctrl succ" );

	cur_fd_count = 1;

	for( ; ; )
	{
		DEBUG( "really for epoll_wait." );
		wait_count = epoll_wait( epoll_fd, events, cur_fd_count, -1 );
		DEBUG( "wait_count is [%d].", wait_count );
		if ( -1 == wait_count ) 
		{
			if ( errno == EINTR )
			{
				DEBUG( "epoll_wait errno:EINTR" );
				/* 如果epoll_wati被信号中断，就在此分支 */

				if ( 1 == pool->shutdown )
				{
					/* 即将退出 */
					DEBUG( "addwork thread will exit!" );
					pthread_exit( NULL );
				}
			}
			else
			{
				ERROR( "epoll_wait error." );
				continue;
			}
		}
		
		for( i = 0; i < wait_count; i++ )
		{
			connect_fd = 0;
			ctl_fd = 0;

			if ( events[i].data.fd == socket_fd )
			{
				connect_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len );
				DEBUG( "new connection, new fd is [%d],accept form %s:%d\n", \
					connect_fd, inet_ntoa( client_addr.sin_addr ), client_addr.sin_port );
				if ( 0 > connect_fd )
				{
					ERROR( "accept error." );
					continue;
				}

				if ( 3002400 <= cur_fd_count )
				{
					DEBUG( "too many connection" );
					close( connect_fd );
					continue;
				}

				if ( 0 > setnonblocking( connect_fd ) )
				{
					ERROR( "setnonblocking error." );
					continue;
				}

				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = connect_fd;
				if ( 0 != epoll_ctl( epoll_fd, EPOLL_CTL_ADD, connect_fd, &ev ) )
				{
					ERROR( "epoll_ctl error" );
					return NULL;
				}
				
				cur_fd_count++;
				continue;
			}
			else if ( events[i].events & EPOLLIN )
			{
				DEBUG( "event read." );

				ctl_fd = events[i].data.fd;
				if ( 0 > ctl_fd )
				{
					DEBUG( "ctl_fd < 0,now continue" );
					continue;
				}
				DEBUG( "ctl_fd is [%d].", ctl_fd );

				memset( buf, 0x00, sizeof( buf ) );
				while( 1 )
				{
					memset( buf_tmp, 0x00, BUF_TMP_SIZE );
					iRet_Read = read( ctl_fd, buf_tmp, BUF_TMP_SIZE - 1 );

					/* 异常情况 */
					if ( 0 > iRet_Read )
					{
						DEBUG( "读取fd的返回之为负数" );
						
						/* 读取数据完毕 */
						if ( errno == EAGAIN || errno == EWOULDBLOCK )
						{
							{
								/* 将任务添加至任务列表 */
								INFO( buf );

								/* 分配任务内存 */
								CThread_worker *newworker = (CThread_worker *) malloc( sizeof(CThread_worker) );
								memset( newworker, 0x00, sizeof( newworker ) );

								/* 添加任务 */
								strcpy( newworker->worker_str_buf, buf );
								newworker->work_fd = ctl_fd;
								newworker->next = NULL;
								newworker->process = mq_send_info_ywjc;
								newworker->arg = newworker->worker_str_buf;

								/* 加锁 */
								pthread_mutex_lock( &(pool->queue_lock) );

								/* 插入链表尾部 */
								CThread_worker *member = pool->queue_head;
								if ( NULL != member )
								{
									while( NULL != member->next )
									{
										member = member->next;
										member->next = newworker;
									}
								}
								else
								{
									pool->queue_head = newworker;
								}
								++pool->cur_queue_size;

								/* 解锁 */
								pthread_mutex_unlock( &(pool->queue_lock) );

								/* 通知一个等待的线程 */
								pthread_cond_signal( &(pool->queue_ready) );
							}
							break;
						}
						else if ( errno == EINTR )
						{
							/* 中断唤醒 */
						}
						else
						{
							close( ctl_fd );

							ev.data.fd = ctl_fd;
							ev.events = EPOLLIN;
							if ( 0 != epoll_ctl( epoll_fd, EPOLL_CTL_DEL, ctl_fd, &ev ) )
							{
								ERROR( "epoll_ctl error." );
							}

							break;
						}
					}
					/* 读取无内容 */
					else if ( 0 == iRet_Read )
					{
						/* 此分支为客户端链接关闭了 */
						DEBUG( "读事件，而且读取的字符为空则认为客户端链接关闭" );
						close( ctl_fd );

						ev.data.fd = ctl_fd;
						ev.events = EPOLLIN;
						if ( 0 != epoll_ctl( epoll_fd, EPOLL_CTL_DEL, ctl_fd, &ev ) )
						{
							ERROR( "epoll_ctl error." );
						}

						break;
					}
					/* 读取有内容 */
					else
					{
						/* 统计读取字符串的个数 */
						iRead_Count += iRet_Read;
						
						/* 如果报文的长度大于40960则，报文丢弃，后续可以合理考虑将malloc用之 */
						if ( iRead_Count > MAXLINE )
						{
							ERROR( "message too big." );
							continue;
						}
						else
						{
							/* 内存拼接 */
							strcat( buf, buf_tmp );
						}
					}
				}

				memset( buf, 0x00, sizeof( buf ) );
			}
			else if ( events[i].events & EPOLLOUT )
			{
				DEBUG( "envent write." );
				ctl_fd = events[i].data.fd;

				iRet_Write = write(ctl_fd, buf, iRet_Read);
				if ( -1 == iRet_Write )
				{
					ERROR( "write error." );
					close( ctl_fd );

					ev.data.fd = ctl_fd;
					ev.events = EPOLLOUT;
					epoll_ctl( epoll_fd, EPOLL_CTL_DEL, ctl_fd, &ev );
				}
				else
				{
					
				}
				memset( buf, 0x00, sizeof( buf ) );
			}
		}
	}

	INFO( "thread of addwork [%d] end.", pthread_self() );
	
	return NULL;
}

