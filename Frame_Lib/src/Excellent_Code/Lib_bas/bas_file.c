/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_file.c                                                  */
/* DESCRIPTIONS: 文件操作接口                                                */
/* AUTHOR      : Robert Peng                                                 */
/* CREATE DATE : 2008-06-23                                                  */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "bas/bas_global.h"

static const char *mpcaId = "$Id: bas_file.c,v 1.4 2013/02/26 04:40:48 cardv5c Exp $";

#define BAS_MAX_DBLOG_BYTES         10000000L

/*****************************************************************************/
/* FUNC:   int basFileLock(FILE *fpFile)                                     */
/* PARAMS: fpFile - 文件句柄                                                 */
/* RETURN: 1 - 文件已经被锁                                                  */
/*         0 - 锁成功                                                        */
/*        -1 - 错误                                                          */
/* DESC:   文件加锁                                                          */
/*****************************************************************************/
BAS_EXPORT int basFileLock(FILE *fpFile)
{
    int fd;
    struct flock tLock;

    fd = fileno(fpFile);

    tLock.l_type   = F_WRLCK;
    tLock.l_whence = 0;
    tLock.l_start  = 0L;
    tLock.l_len    = 0L;

    if (fcntl(fd, F_SETLK, &tLock) < 0)
    {
        if ((errno == EAGAIN) || (errno == EACCES))
        {
            return 1;
        }
        else
        {
            BAS_LOG(BAS_ERR300013_ERRORFCNTL, 0, errno, "basFileLock:锁文件失败");
            return -1;
        }
    }

    return 0;
}
