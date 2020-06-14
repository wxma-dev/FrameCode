#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <setjmp.h>

#include "Frame_Socket.h"

#include "yl_socket.h"

static int _gSock=-1;

jmp_buf connect_timeout;
jmp_buf write_msg_timeout;

int u_ignore_sigpipe( )
{
  struct sigaction act;

  if( sigaction( SIGPIPE, ( struct sigaction * )NULL, &act ) < 0 )
    return( 1 );
  if( act.sa_handler == SIG_DFL ) {
    act.sa_handler = SIG_IGN;
    if ( sigaction( SIGPIPE, &act, ( struct sigaction * )NULL ) < 0 )
      return( 1 );
  }
  return( 0 );
}


int u_open( u_port_t port )
{
  int sock;
  struct sockaddr_in server;
  int iOn;
  struct linger stLinger = { 1, 2 };

  if( ( u_ignore_sigpipe( ) != 0 ) ||
      ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) )
    return( -1 );

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( ( short ) port );

  /*   曹春光 20010808  begin         */
  errno = 0;
  if( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &iOn, sizeof( int ) ) < 0 )
  {
    printf( "setsockopt SO_REUSEADDR errno is %d errmsg is [%s]\n",
            errno, strerror( errno ) );
  }

  errno = 0;

#ifndef __linux__
  if( setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &iOn, sizeof( int ) ) < 0 )
  {
    printf( "setsockopt SO_REUSEPORT errno is %d errmsg is [%s]\n",
            errno, strerror( errno ) );
  }
#endif

  errno = 0;
  if( setsockopt(sock, SOL_SOCKET, SO_LINGER, &stLinger,
             sizeof( struct linger ) ) < 0 ){
    printf( "setsockopt SO_LINGER errno is %d errmsg is [%s]\n", 
           errno, strerror( errno ) );
  }

  /*   曹春光 20010808  end         */


  if( ( bind( sock, ( struct sockaddr * )&server, sizeof( server ) ) < 0 ) ||
      ( listen( sock, MAXBACKLOG ) < 0 ) ) 
    return( -1 );
  return sock;
}



int yl_listen( int fd, char *hostn )
{
  struct sockaddr_in net_client;
#if defined(_AIX)
  socklen_t len = sizeof( struct sockaddr );
#else
  int len = sizeof( struct sockaddr );
#endif
  int retval;
  struct hostent *hostptr;
  
  sigset(SIGPIPE,SIG_IGN); /* Add by CGB */

  while( ( ( retval = 
      accept( fd, ( struct sockaddr * ) ( &net_client ), &len ) ) == -1 ) &&
      ( errno == EINTR ) )
  ;
  
  if( retval == -1 ) 
    return( retval );

  //memset( hostn,0,sizeof(hostn));
#if 0
  sprintf( hostn,"%s",inet_ntoa(net_client.sin_addr));
#endif


  return( retval );
}


int u_close( int fd )
{
  return close( fd );
}

int CloseSocket( int iSocket )
{
   return( u_close( iSocket ) );
}

ssize_t u_read( int fd, void *buf, size_t size )
{
  ssize_t retval;

  while( retval = read( fd, buf, size ), retval == -1 && errno == EINTR )
  ;

  return retval;
}

ssize_t u_write( int fd, void *buf, size_t size )
{
  ssize_t retval;

  while( retval = write( fd, buf, size ), retval == -1 && errno == EINTR )
  ;

  return retval;
}

int u_connect_ext( u_port_t port, char *hostn )
{
  struct sockaddr_in BFEclient;
  struct sockaddr_in server;
  struct hostent *hp;
  int sock;
  int retval;

  char aczBFEip[23+1];

  memset(aczBFEip, 0x00, sizeof(aczBFEip));

  if( ( u_ignore_sigpipe( ) != 0 ) ||
      !( hp = gethostbyname( hostn ) ) || 
      ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) ) 
  return( -1 );

#if 0
  if( getenv("Union_IP") == NULL ) {
     return -2;
  }

  sprintf( aczBFEip, "%s", getenv("Union_IP") );
  //sprintf( aczBFEip, "%s", "10.242.134.75" );

  BFEclient.sin_addr.s_addr = inet_addr(aczBFEip);
  
  if( bind( sock, ( struct sockaddr * )&BFEclient, sizeof(BFEclient) ) != 0 ) {
     return( -1 );
  }
#endif

  memcpy( ( char * )&server.sin_addr, hp->h_addr_list[0], hp->h_length );

  server.sin_port = htons( ( short ) port );
  server.sin_family = AF_INET;  

  _gSock = sock;
  while( ( ( retval = 
      connect( sock, ( struct sockaddr * )&server, sizeof(server) ) ) == -1 )
      && ( errno == EINTR ) )
;
  if( retval == -1 ) {
    close( sock );
    return( -1 );
  }
  return sock;
}

int CUPSRecvMsg( int communfd, char *pczTranBuf, int *piBuflen, int iDelayTime ) 
{
	int iAllLen = -1,iLen = -1;
	char pczTmpBuf[ 2048+1 ];
	char aczPkgLen[5];
	char *ptr;

	memset( pczTmpBuf, 0x00, sizeof( pczTmpBuf ) );
	memset( aczPkgLen, 0x00, sizeof( aczPkgLen ) );
	if( ( iLen = u_read( communfd, aczPkgLen, 4 ) ) < 0 ) return( -1104 );

	iAllLen = atoi(aczPkgLen);
	if ( memcmp ( aczPkgLen, "0000" ,4 )  == 0 )
		return 0;
	else if ( iAllLen < CUPS_HEAD_LEN ) 
	{
	  return( -1107 );
	}

	if( ( iLen = u_read( communfd, pczTmpBuf, iAllLen ) ) < 0 ) return( -1105 );
	if ( iLen != iAllLen ) return( -1106 );

	*piBuflen = iAllLen ;
	memcpy( pczTranBuf, pczTmpBuf, *piBuflen );

	return( iAllLen );
}


void ConnectTimeout( int iSig )
{
   signal( iSig, SIG_IGN );

   longjmp( connect_timeout, 1 ); 

}


/*********************************************************************
 *     Function : TranClientByShortTcpIp_ext 
 *-------------------------------------------------------------------*
 *  Description : TranClientByShortTcpIp_ext()函数
 *                client方式与主机server进行连接，在循环连接
 *                若干次未成功返回失败。如果成功，调用交易处理
 *                进程进行交易处理。
 *-------------------------------------------------------------------*
 *        Input : 1. short iClientPort  作为 Client 连接端口
 *                2. char *pczHostname  主机名称
 *                3. int iConnectTimes  最多连接次数
 *                4. int iConnectDelay  每次试图连接间隔的秒数
 *-------------------------------------------------------------------*
 *       Output : None
 *-------------------------------------------------------------------*
 * Return Value : 成功后返回大于0的Sochet端口号
 *                错误返回小于零的数
 *********************************************************************/

int TranClientByShortTcpIp_ext( short iClientPort, char *pczHostName,
                                int iConnectTimes, int iConnectDelay )
{
   /*   --- some ---   */
   int         iNowTimes;
   int         iCommunfd=-1;
 
   /*
   **   Check IP_PORT and open the socket
   if( iClientPort > MAX_IP_PORT || iClientPort < MIN_IP_PORT )
      return ( -2301 );
   */
   

   /*
   **   main loop of wait client connect 
   **   fork a child to communte with client
   */
   /*
   **    use the alarm to control the socket read 
   */
   signal( SIGALRM, ConnectTimeout );
   
   
   if( setjmp( connect_timeout ) == 0 )
   {
      alarm( iConnectTimes * iConnectDelay + 5 );

      for( iNowTimes = 1; iNowTimes <= iConnectTimes; iNowTimes ++ )
      {
      
         iCommunfd = u_connect_ext( iClientPort, pczHostName );
         if( iCommunfd != -1 && iCommunfd != -2 )
         { 
         	alarm( 0 );
            return( iCommunfd );
         }/*   end of the if */   

         sleep( iConnectDelay );
      	
      }/* end of for */
      
      alarm( 0 );
      if( iCommunfd == -2 ) {
         return( -2311 );
      }
      else {
         return( -2302 );
      }
   }
   else
   {
      if( _gSock >= 0 ) {
         close( _gSock );
         _gSock = -1;
      }
      close( iCommunfd );
      return( -2300 );
   }
}  /* end of the function */

void WriteMsgTimeout( int iSig )
{
   signal( iSig, SIG_IGN );
   
   longjmp( write_msg_timeout, 1 ); 

}

int CUPSSendZeroToSocket( int communfd, int iDelayTime)
{
        char sTemp[5]="0000";
        int iLen = 0;

        signal( SIGALRM, WriteMsgTimeout );
        alarm( iDelayTime );
        iLen = u_write( communfd,sTemp,4);
/**	iLen = send ( communfd,sTemp,4,0); **/
        alarm( 0 );

        if ( iLen < 4 ) return -1;
        else return 0;
}


int SendMsgToSocket( int communfd, char *pczTranBuf, 
                      int iBuflen, int iDelayTime, int iHeadFlag ) 
{
   int iLen, iAll, iLeft;
   int iHeadLen=0;
   
   /*   the head of the transaction which will be send    */
   char pczTmpBuf[ MAX_DATA_LEN ];
   char *ptr;

   if( iDelayTime < 0 )
      return( -1301 );
      
   if( iHeadFlag < 0 || iHeadFlag > 9 )
      return( -1302 );
   
   if( iBuflen <= 0 )
      return( -1312 );
   
   /*
   **    use the alarm to control the socket read 
   */
   signal( SIGALRM, WriteMsgTimeout );
   if( setjmp( write_msg_timeout ) == 0 )
   {
      alarm( iDelayTime );
   
      memset( pczTmpBuf, 0x00, sizeof( pczTmpBuf ) );
      iAll = iBuflen;

      if( iHeadFlag == NO_HEAD )
      {
         if( iBuflen < 0 || iBuflen > MAX_DATA_LEN )
         {
      	   alarm( 0 );
      	   return( -1303 );
         }
         
         /*
         **   send the head to client
         */
         iLen = u_write( communfd, pczTranBuf, iBuflen );
    
         if( iLen != iBuflen )
         {
      	    alarm( 0 );
      	    return ( -1304 );
         }
         else
         {
            alarm( 0 );
            return( iBuflen );
         }
      }
      else if( iHeadFlag == HEXBYTE_TWO_HAS_HEAD )
      {
         if( iBuflen < 0 || iBuflen > 256 * 256 - 3  )
         {
      	   alarm( 0 );
      	   return( -1305 );
         }
         
         iHeadLen = 2;
         pczTmpBuf[ 0 ] = ( iBuflen + 2 ) / 256;
         pczTmpBuf[ 1 ] = ( iBuflen + 2 ) % 256;
         
      }
      else if( iHeadFlag == HEXBYTE_TWO_NONE_HEAD )
      {
         if( iBuflen < 0 || iBuflen > 256 * 256 - 1 )
         {
      	   alarm( 0 );
      	   return( -1306 );
         }
         
         iHeadLen = 2;
         pczTmpBuf[ 0 ] = iBuflen / 256;
         pczTmpBuf[ 1 ] = iBuflen % 256;

      }
      else if( iHeadFlag == ASCII_FOUR_HAS_HEAD )
      {
         if( iBuflen < 0 || iBuflen > 9999 - 4 )
         {
      	   alarm( 0 );
      	   return( -1307 );
         }
         
         sprintf( pczTmpBuf, "%04d", iAll + 4 );

         iHeadLen = 4;
      }
      else if( iHeadFlag == ASCII_FOUR_NONE_HEAD )
      {
         if( iBuflen < 0 || iBuflen > 9999 )
         {
      	   alarm( 0 );
      	   return( -1308 );
         }
         
         sprintf( pczTmpBuf, "%04d", iAll );

         iHeadLen = 4;
      }
      else if( iHeadFlag == ASCII_FIVE_NONE_HEAD )
      {
         if( iBuflen < 0 || iBuflen > 99999 )
         {
      	   alarm( 0 );
      	   return( -1308 );
         }
         
         sprintf( pczTmpBuf, "%05d", iAll );

         iHeadLen = 5;
      }
      else if( iHeadFlag == ASCII_EIGHT_NONE_HEAD )
      {
         if( iBuflen < 0 || iBuflen > 99999999 )
         {
           alarm( 0 );
           return( -1308 );
         }
         sprintf( pczTmpBuf, "%08d", iAll );

         iHeadLen = 8;
      }
      else if( iHeadFlag == ASCII_NINE_NONE_HEAD )
      {
         if( iBuflen < 0 || iBuflen > 999999999 )
         {
      	   alarm( 0 );
      	   return( -1308 );
         }
         
         sprintf( pczTmpBuf, "%09d", iAll );

         iHeadLen = 9;
      }
      else if( iHeadFlag == FF_ASCII_THREE_NONE_HEAD )
      {
         if (iBuflen < 0 || iBuflen > 999)
         {
      	   alarm( 0 );
      	   return( -1309 );
         }
        pczTmpBuf[ 0 ] = 0xFF;
         sprintf (pczTmpBuf + 1, "%03d", iAll);

         iHeadLen = 4;
     }
    
      /*
      **   send the head to client
      */
      iLen = u_write( communfd, pczTmpBuf, iHeadLen );
    
      if( iLen != iHeadLen )
      {
      	 alarm( 0 );
      	 return ( -1310 );
      }
   
      /*
      **    read the socket, obtain the transaction 
      **    if the read_length is not equal to transaction_length
      **    read the socket again 
      */
      iLeft = iAll;
      ptr = pczTranBuf;

      /*
      **   begin the loop to write to socket until all has sent
      */
      do
      {
         /*
         **   write the left letter to socket
         */
         iLen = u_write( communfd, ptr, iLeft );
         if( iLen <= 0 ) 
         {
            alarm( 0 );    
            return ( -1311 );
         }

         /*
         **   how many letter left
         */
         iLeft = iLeft - iLen;
         ptr = ptr + iLen;

      }
      while( iLeft > 0 );    /*   all has sent */
   
      /*
      **    read the socket of transaction is ok 
      */
      alarm( 0 ); 
      return( iAll );
   }
   else
   {
      return( -1300 );
   }
   
}
