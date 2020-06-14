#include "../include/lib_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>

/************************************************************
* Copyright (c) 2015 wxma                                   *
* Source File:    Get_FileSize                              *
* Description:    获取文件大小(字节)                        *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             filename 文件名(绝对路径)                 *
*                 valuename_str 配置字段名称                *
                  value_str 配置字段内容                    *
* Out:            0 非数字                                  *
*                 1 数字                                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/

static int SetBlocking(int blocking) {
    int flags;

    /* Set the socket nonblocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(c->fd, F_GETFL)) == -1) {
        return -1;
    }

    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;

    if (fcntl(c->fd, F_SETFL, flags) == -1) {
        return -1;
    }
    return 0;
}




#define _TEST_LIE_FILE_ 0
#if _TEST_LIE_FILE_
int main()
{
	char * sFileName = "1.txt";
	char  * pTmp;


	FILE * fp = fopen( sFileName, "r" );
	int i = 0;
	for( i = 0 ; i < 20 ; i++ )
	{
		int iRet = GetFileLine_Malloc( fp, &pTmp );
		if ( -2 == iRet ) break;
	
		printf( "[iRet=%d],%s\n", iRet, pTmp );
		GetFileLine_Free( pTmp );
		pTmp = NULL;

		if ( -1 == iRet )
		{
			break;
		}
	}

}
#endif


