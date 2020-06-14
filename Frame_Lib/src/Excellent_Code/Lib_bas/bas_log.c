/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_log.c                                                   */
/* DESCRIPTIONS: 系统日志接口                                                */
/* AUTHOR      : Robert Peng                                                 */
/* CREATE DATE : 2008-06-23                                                  */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

#include "dbs/dbs_basopr.h"
#include "bas/bas_global.h"

static const char *mpcaId = "$Id: bas_log.c,v 1.5 2013/04/09 05:06:40 wanglp Exp $";

static  int                 iLogRcdNum = 0;     /*日志切片计数*/

/*****************************************************************************/
/* FUNC:   char* basGetLogFileName()                                         */
/* PARAMS: 无                                                                */
/* RETURN: 无                                                                */
/* DESC:   返回日志文件名                                                    */
/*****************************************************************************/
BAS_EXPORT char* basGetLogFileName(void)
{
    static  char    mcaLogFile[BAS_FILENAME_LEN];
    char            *pcaAppHome;
    time_t          tTime;
    struct tm       *ptmNow;

    if ((pcaAppHome = getenv("LOGPATH")) == NULL)
    {
        return NULL;
    }
    if ((tTime = time(NULL)) == -1)
    {
        return NULL;
    }
    if ((ptmNow = localtime(&tTime)) == NULL)
    {
        return NULL;
    }
    sprintf(mcaLogFile, "%s%s/%s/trace.%s.%04d%02d%02d.%d.%02d.log",
            pcaAppHome, BAS_LOGPATH, gcaModuleName, gcaProgramName,
            ptmNow -> tm_year + 1900, ptmNow -> tm_mon + 1, ptmNow -> tm_mday,
            getpid(),
            iLogRcdNum);

//  printf("------pcaAppHome=[%s],[%s],[%s],[%s] \n",pcaAppHome,  BAS_LOGPATH,gcaModuleName,gcaProgramName);

    return mcaLogFile;
}

/*全局变量，日志句柄*/

static   FILE            *ps_fp = NULL;


/*****************************************************************************/
/* FUNC:   void basLog1(const char *pcaFile, const int iLine,                 */
/*                     const int iErrCode, const int iSqlCode, const int iErrno,*/
/*                     const char *pcaFmt, ...)                              */
/* PARAMS: iErrCode - 应用的错误代码                                         */
/*         iSqlCode - 数据库SQLCODE, 如果是0则忽略 (I)                       */
/*         iErrno   - 操作系统的errno, 如果是0则忽略 (I)                     */
/*         pcaFmt   - 错误信息的格式串 (I)                                   */
/*         ...      - 错误信息的变参部分 (I)                                 */
/* RETURN: 无                                                                */
/* DESC:   系统日志接口                                                      */
/*****************************************************************************/
BAS_EXPORT void basLog(const char *pcaFile, int iLine,
                       int iErrCode, int iSqlCode, int iErrno,
                       const char *pcaFmt, ...)
{
    int             fd  = -1;
    static  char    caPrevLogFileName[BAS_FILENAME_LEN] = "";
    struct stat     tStat;
    va_list         vlVar;
    char            *pcaLogLevel;
    char            *pcaLogFile;
    time_t          tTime;
    struct tm       *ptmNow;
    char            caTimeString[256];
    char caBuff[256];
    struct timeval tTimeVal;
    int             iLevel = iErrCode / 100000;
	int iRet = 0;

    char      caEncryptBuf[51200] = "";
    char      caOutBuf[51200] = "";
    int       iOutBufLen  = 0;

	
	/*判断日志级别*/
    if ((pcaLogLevel = getenv("LOGLEVEL")) != NULL)
    {
        if ((iLevel >= 4) && (atoi(pcaLogLevel) < iLevel))
        {
            return;
        }
    }   
	
	/*得到文件名*/
	if ((pcaLogFile = basGetLogFileName()) == NULL)
	{
		return;
	}
	

    if (stat(pcaLogFile, &tStat) < 0)
    {
        if ((ps_fp = fopen( pcaLogFile, "ab+" ) ) == NULL)
            return;
    }
    else
    {
        int iLogSize = 0;
        iLogSize = atoi(getenv("LOGSIZE"));
        if (iLogSize == 0)
        {
           iLogSize = 100*1000*1000;
        }
        
        if (tStat.st_size > iLogSize)
        {
             fclose(ps_fp);
             ps_fp = NULL;
             
            /*达到日志大小上限，另起一日志*/
            if (++iLogRcdNum > 99)
                iLogRcdNum = 0;
            
            /*重新得到文件名*/
            if ((pcaLogFile = basGetLogFileName()) == NULL)
            {
                return;
            }
        }

        if (ps_fp == NULL)
        {
            ps_fp = fopen(pcaLogFile, "ab+" );
            if (ps_fp == NULL)
                return;
        }
    }

sprintf( caOutBuf + strlen(caOutBuf), "------------------------------------------------------------------------------\n");

    switch (iLevel)
    {
        case 1:
            sprintf( caOutBuf + strlen(caOutBuf), "INFO :");
            break;
        case 2:
            sprintf( caOutBuf + strlen(caOutBuf), "FATAL:");
            break;
        case 3:
            sprintf( caOutBuf + strlen(caOutBuf), "ERROR:");
            break;
        case 4:
            sprintf( caOutBuf + strlen(caOutBuf), "WARN :");
            break;
        case 5:
            sprintf( caOutBuf + strlen(caOutBuf), "DEBUG:");
            break;
        default:
            sprintf( caOutBuf + strlen(caOutBuf), "-----:");
            break;
    }

    tTime = time(NULL);
    ptmNow = localtime(&tTime);

    //取日志时间, 精确到毫秒
    memset(caTimeString, 0, sizeof(caTimeString));
    memset(caBuff, 0, sizeof(caBuff));
    gettimeofday(&tTimeVal, NULL);
    strftime(caBuff, sizeof(caBuff), "TIME:%Y-%m-%d_%H:%M:%S",
             localtime(&tTimeVal.tv_sec));
    snprintf(caTimeString, sizeof(caTimeString), "%s.%03ld",
             caBuff, tTimeVal.tv_usec / 1000);

    va_start(vlVar, pcaFmt);
    sprintf( caOutBuf + strlen(caOutBuf), "%s:%s[%d]:%s,%d:\n%s[%d]:",
            caTimeString,
            gcaProgramName,
            getpid(),
            pcaFile,
            iLine,
            basGetErrDesc(iErrCode),
            iErrCode);
    if (iErrno != 0)
    {
        sprintf( caOutBuf + strlen(caOutBuf), "OS[%d,%s]:", iErrno, strerror(iErrno));
    }
    if (iSqlCode != 0)
    {
        sprintf( caOutBuf + strlen(caOutBuf), "DB[%d,%s]:", iSqlCode, dbsGetErrMsg());
    }
    vsprintf( caOutBuf + strlen(caOutBuf), pcaFmt, vlVar);
    sprintf( caOutBuf + strlen(caOutBuf), "\n");
    va_end(vlVar); 

     //加密：
    iOutBufLen = lencodelog(caEncryptBuf,sizeof(caEncryptBuf),caOutBuf,strlen(caOutBuf));
    fwrite(caEncryptBuf,iOutBufLen,1,ps_fp);
    fflush(ps_fp); 

    fclose(ps_fp);
    ps_fp = NULL;
}

/*****************************************************************************/
/* FUNC:   void basBatLog(const char *pcaFmt, ...)                           */
/* PARAMS:                                                                   */
/*                                                                           */
/*         pcaFmt   - 错误信息的格式串 (I)                                   */
/*         ...      - 错误信息的变参部分 (I)                                 */
/* RETURN: 无                                                                */
/* DESC:   批量专用日志接口                                                  */
/*****************************************************************************/
BAS_EXPORT void basBatLog(const char *pcaFmt, ...)
{
    FILE *fp;
    va_list    vlVar;
    char       *pcaLogFile;

    char      caEncryptBuf[51200] = "";
    char      caOutBuf[51200] = "";
    int       iOutBufLen  = 0;

    if ((pcaLogFile = basGetLogFileName()) == NULL)
    {
        return;
    }

    if ((fp = fopen(pcaLogFile, "ab+")) == NULL)
    {
        return;
    }

    va_start(vlVar, pcaFmt);
    vsprintf( caOutBuf + strlen(caOutBuf), pcaFmt, vlVar);
    va_end(vlVar);

     //加密：
    iOutBufLen = lencodelog(caEncryptBuf,sizeof(caEncryptBuf),caOutBuf,strlen(caOutBuf));
    fwrite(caEncryptBuf,iOutBufLen,1,fp);

    fclose(fp);
}

BAS_EXPORT void basDebugString(const char *pcFile, const int iLine,
                               const char *psBuf, const int iLength)
{
    char    filename[BAS_FILENAME_LEN+1], bakfilename[BAS_FILENAME_LEN+1];
    struct  stat statbuf;
    int     cur_env_set_level;
    long    base_overflow;
    char    *ptr;
    register int i,j=0;
    char    s[100], temp[5];
    time_t  systime;
    char    st[128];
    char    *pcaLogFile;
    FILE    *fp=NULL;
    int     cnt;
    int     nRet;
    struct tm *ptmNow;
    char       *pcaAppHome;

    if ((pcaLogFile = basGetLogFileName()) == NULL)
    {
        return;
    }

    systime=time(NULL);
    ptmNow=localtime(&systime);

    /********************************************************************/
    /*          get     current     system  time    to  register        */
    /********************************************************************/
    memset(st, 0, sizeof(st));


    strftime(st, sizeof(st),"%Y-%m-%d %H:%M:%S|%a %b %d %H:%M:%S %Y", ptmNow);

    /********************************************************************/
    /*          set     logical     file    name                        */
    /********************************************************************/
    memset(filename, 0, sizeof(filename));  /* Log File Name */
    if (strlen(pcaLogFile) == 0 )    /* filename is "" */
    {
        strcpy(filename, "./ht.log");
    }
    else /* filename is normal string */
    {
        strcpy(filename, pcaLogFile);
    }

    if ( fp != stderr )
    {
        if ( ( fp = fopen(filename, "a+") ) == NULL )
            fp = stderr;
        else
        {
            memset(&statbuf, 0, sizeof(statbuf));
            nRet = stat(filename, &statbuf);
            /*
            if ( nRet == 0 && statbuf.st_size >= 1024 * 1024 * 5 )
            {
                fclose(fp);
                memset(bakfilename, 0, sizeof(bakfilename));
                sprintf(bakfilename, "%s.old", filename);
                rename(filename, bakfilename);
                if ( ( fp = fopen(filename, "a+") ) == NULL )
                    fp = stderr;
            }
            */
        }
    }

    fprintf(fp, "------------------------------------------------------------------------------\n");

    cnt=fprintf(fp, "Date:[%s]\n", st);
    cnt=fprintf(fp, "File:[%s] Line:[%d] Lenth:[%d]\n",
            pcFile, iLine, iLength);

    for (i=0; i<iLength; i++)
    {
        if (j==0)
        {
            memset( s, ' ', sizeof(s));
            sprintf(temp,   "%04d:",i );
            memcpy( s, temp, 5);
            sprintf(temp,   ":%04d",i+15 );
            memcpy( &s[72], temp, 5);
        }
        sprintf( temp, "%02X ", (unsigned char)psBuf[i]);
        memcpy( &s[j*3+5+(j>7)], temp, 3);
        if ( isprint( psBuf[i]))
        {
            s[j+55+(j>7)]=psBuf[i];
        }
        else
        {
            s[j+55+(j>7)]='.';
        }
        j++;
        if ( j==16)
        {
            s[77]=0;
            fprintf(fp, "%s\n", s);
            j=0;
        }
    }

    if (j)
    {
          s[77]=0;
          fprintf(fp, "%s\n\n", s);
    }

    fflush(fp);

    if ( fp != stderr )
        fclose(fp);

    return;
}
/*****************************************************************************/
/* FUNC:   char* basGetPLogFileName()                                         */
/* PARAMS: 无                                                                */
/* RETURN: 无                                                                */
/* DESC:   返回日志文件名                                                    */
/*****************************************************************************/
BAS_EXPORT char* basGetPLogFileName(void)
{
    static  char    mcaLogFile[BAS_FILENAME_LEN];
    char            *pcaAppHome;
    time_t          tTime;
    struct tm       *ptmNow;

    if ((pcaAppHome = getenv("LOGPATH")) == NULL)
    {
        return NULL;
    }
    if ((tTime = time(NULL)) == -1)
    {
        return NULL;
    }
    if ((ptmNow = localtime(&tTime)) == NULL)
    {
        return NULL;
    }
    sprintf(mcaLogFile, "%s%s/%s/trace.%s.%04d%02d%02d.%d.%02d.log",
            pcaAppHome, BAS_LOGPATH, gcaModuleName, gcaProgramName,
            ptmNow -> tm_year + 1900, ptmNow -> tm_mon + 1, ptmNow -> tm_mday,
            getppid(),
            iLogRcdNum);

//  printf("------pcaAppHome=[%s],[%s],[%s],[%s] \n",pcaAppHome,  BAS_LOGPATH,gcaModuleName,gcaProgramName);

    return mcaLogFile;
}
/*****************************************************************************/
/* FUNC:   void basPLog(const char *pcaFile, const int iLine,                 */
/*                     const int iErrCode, const int iSqlCode, const int iErrno,*/
/*                     const char *pcaFmt, ...)                              */
/* PARAMS: iErrCode - 应用的错误代码                                         */
/*         iSqlCode - 数据库SQLCODE, 如果是0则忽略 (I)                       */
/*         iErrno   - 操作系统的errno, 如果是0则忽略 (I)                     */
/*         pcaFmt   - 错误信息的格式串 (I)                                   */
/*         ...      - 错误信息的变参部分 (I)                                 */
/* RETURN: 无                                                                */
/* DESC:   系统日志接口                                                      */
/*****************************************************************************/
BAS_EXPORT void basPLog(const char *pcaFile, int iLine,
                       int iErrCode, int iSqlCode, int iErrno,
                       const char *pcaFmt, ...)
{
    int             fd  = -1;
    static  char    caPrevLogFileName[BAS_FILENAME_LEN] = "";
    struct stat     tStat;
    va_list         vlVar;
    char            *pcaLogLevel;
    char            *pcaLogFile;
    time_t          tTime;
    struct tm       *ptmNow;
    char            caTimeString[256];
    char caBuff[256];
    struct timeval tTimeVal;
    int             iLevel = iErrCode / 100000;
	int iRet = 0;

    char      caEncryptBuf[51200] = "";
    char      caOutBuf[51200] = "";
    int       iOutBufLen  = 0;

	
	/*判断日志级别*/
    if ((pcaLogLevel = getenv("LOGLEVEL")) != NULL)
    {
        if ((iLevel >= 4) && (atoi(pcaLogLevel) < iLevel))
        {
            return;
        }
    }   
	
	/*得到文件名*/
	if ((pcaLogFile = basGetPLogFileName()) == NULL)
	{
		return;
	}
	

    if (stat(pcaLogFile, &tStat) < 0)
    {
        if ((ps_fp = fopen( pcaLogFile, "ab+" ) ) == NULL)
            return;
    }
    else
    {
        int iLogSize = 0;
        iLogSize = atoi(getenv("LOGSIZE"));
        if (iLogSize == 0)
        {
           iLogSize = 100*1000*1000;
        }
        
        if (tStat.st_size > iLogSize)
        {
             fclose(ps_fp);
             ps_fp = NULL;
             
            /*达到日志大小上限，另起一日志*/
            if (++iLogRcdNum > 99)
                iLogRcdNum = 0;
            
            /*重新得到文件名*/
            if ((pcaLogFile = basGetLogFileName()) == NULL)
            {
                return;
            }
        }

        if (ps_fp == NULL)
        {
            ps_fp = fopen(pcaLogFile, "ab+" );
            if (ps_fp == NULL)
                return;
        }
    }

sprintf( caOutBuf + strlen(caOutBuf), "------------------------------------------------------------------------------\n");

    switch (iLevel)
    {
        case 1:
            sprintf( caOutBuf + strlen(caOutBuf), "INFO :");
            break;
        case 2:
            sprintf( caOutBuf + strlen(caOutBuf), "FATAL:");
            break;
        case 3:
            sprintf( caOutBuf + strlen(caOutBuf), "ERROR:");
            break;
        case 4:
            sprintf( caOutBuf + strlen(caOutBuf), "WARN :");
            break;
        case 5:
            sprintf( caOutBuf + strlen(caOutBuf), "DEBUG:");
            break;
        default:
            sprintf( caOutBuf + strlen(caOutBuf), "-----:");
            break;
    }

    tTime = time(NULL);
    ptmNow = localtime(&tTime);

    //取日志时间, 精确到毫秒
    memset(caTimeString, 0, sizeof(caTimeString));
    memset(caBuff, 0, sizeof(caBuff));
    gettimeofday(&tTimeVal, NULL);
    strftime(caBuff, sizeof(caBuff), "TIME:%Y-%m-%d_%H:%M:%S",
             localtime(&tTimeVal.tv_sec));
    snprintf(caTimeString, sizeof(caTimeString), "%s.%03ld",
             caBuff, tTimeVal.tv_usec / 1000);

    va_start(vlVar, pcaFmt);
    sprintf( caOutBuf + strlen(caOutBuf), "%s:%s[%d]:%s,%d:\n%s[%d]:",
            caTimeString,
            gcaProgramName,
            getpid(),
            pcaFile,
            iLine,
            basGetErrDesc(iErrCode),
            iErrCode);
    if (iErrno != 0)
    {
        sprintf( caOutBuf + strlen(caOutBuf), "OS[%d,%s]:", iErrno, strerror(iErrno));
    }
    if (iSqlCode != 0)
    {
        sprintf( caOutBuf + strlen(caOutBuf), "DB[%d,%s]:", iSqlCode, dbsGetErrMsg());
    }
    vsprintf( caOutBuf + strlen(caOutBuf), pcaFmt, vlVar);
    sprintf( caOutBuf + strlen(caOutBuf), "\n");
    va_end(vlVar); 

     //加密：
    iOutBufLen = lencodelog(caEncryptBuf,sizeof(caEncryptBuf),caOutBuf,strlen(caOutBuf));
    fwrite(caEncryptBuf,iOutBufLen,1,ps_fp);
    fflush(ps_fp); 

    fclose(ps_fp);
    ps_fp = NULL;
}
