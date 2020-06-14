/*  Copyright 2006, Robert Peng(robertpeng1116@gmail.com).  All right reserved.
 *
 *  THIS IS UNPUBLISHED  PROPRIETARY  SOURCE  CODE  OF Robert Peng.
 *  THE CONTENTS OF THIS FILE MAY  NOT  BE  DISCLOSED TO THIRD
 *  PARTIES, COPIED OR DUPLICATED IN ANY FORM, IN WHOLE OR IN PART,
 *  WITHOUT THE PRIOR WRITTEN PERMISSION OF Robert Peng.
 *
 *
 *  Edit History:
 *    2006/04/28 created by  Robert Peng(robertpeng1116@gmail.com)
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

#include "bas/bas_global.h"
#include "bas/bas_usrlog.h"

#define N_MAX_PATH_LEN      256
#define N_MAX_NAME_LEN      256

static  int                 iLogRcdNum = 0;     /*日志切片计数*/
//#define N_MAX_LOG_SIZE_TEST 1000                /*日志文件大小测试值*/ 

/*全局变量，日志句柄*/
static   FILE            *pFileAsc = NULL;
static   FILE            *pFileHex = NULL;

BAS_EXPORT void _vWriteLogAsc(char const *vspFileName, char const *vspFuncName, int vtLine,
                              char *vspLogName, char *vspFmt, ... )
{
    va_list                 strArgPtr;
    struct tm               *strpTm;
	struct timeval tv;
	char                    saTraceLogFileName[ N_MAX_PATH_LEN + N_MAX_NAME_LEN + 1 ];
    char                    saFileName[ N_MAX_NAME_LEN + 1 ];
    struct stat             strFstat;
    char                    *psAppHome;

    char      caEncryptBuf[51200] = "";
    char      caOutBuf[51200] = "";
    int       iOutBufLen  = 0;

    /*得到当前时间*/
	gettimeofday(&tv,NULL);	

	tv.tv_sec += 8 * 60 * 60;	/*取的格林威治时间，我们在东8区，所以加上*/
	strpTm = gmtime((time_t *) &tv.tv_sec);

    memset(saTraceLogFileName, 0, sizeof(saTraceLogFileName));

    sprintf(saFileName, "%s.%s.%04d%02d%02d.%-d.%02d",
            vspLogName,
            gcaProgramName,
            strpTm->tm_year+1900,
            strpTm->tm_mon+1,
            strpTm->tm_mday,
            getpid( ),
            iLogRcdNum);

    psAppHome = getenv("LOGPATH");
    if (psAppHome == NULL) {
        sprintf(saTraceLogFileName, "%s", saFileName);
    }
    else {
        sprintf(saTraceLogFileName, "%s/log/%s/%s", psAppHome, gcaModuleName, saFileName);
    }

    if (stat(saTraceLogFileName, &strFstat) < 0) {
        if ((pFileAsc = fopen( saTraceLogFileName, "ab+" ) ) == NULL )
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
     
        if (strFstat.st_size > iLogSize)
        {
             fclose(pFileAsc);
             pFileAsc = NULL;
             
            /*达到日志大小上限，另起一日志*/
            if (++iLogRcdNum > 99)
            {
                iLogRcdNum = 0;
            }
            
            sprintf(saFileName, "%s.%04d%02d%02d.%-d.%02d",
            vspLogName,
            strpTm->tm_year+1900,
            strpTm->tm_mon+1,
            strpTm->tm_mday,
            getpid( ),
            iLogRcdNum);
            sprintf(saTraceLogFileName, "%s/log/%s/%s", psAppHome, gcaModuleName, saFileName);

        }
        
        if (pFileAsc == NULL)
        {
            if ((pFileAsc = fopen( saTraceLogFileName, "ab+" ) ) == NULL)
                return;
        }
    }

    va_start( strArgPtr, vspFmt );

    sprintf( caOutBuf + strlen(caOutBuf), "[%02d:%02d:%02d:%03ld]",
            strpTm->tm_hour,
            strpTm->tm_min,
            strpTm->tm_sec ,
            tv.tv_usec/1000);

   sprintf( caOutBuf + strlen(caOutBuf),"[%s][%s][%d]",
            vspFileName,
            vspFuncName,
            vtLine );
   sprintf( caOutBuf + strlen(caOutBuf),"[%-d]",
            getpid( ));

    vsprintf(caOutBuf + strlen(caOutBuf), vspFmt, strArgPtr );
    va_end( strArgPtr );

    //加密：
    iOutBufLen = lencodelog(caEncryptBuf,sizeof(caEncryptBuf),caOutBuf,strlen(caOutBuf));

    fwrite(caEncryptBuf,iOutBufLen,1,pFileAsc);
    fflush(pFileAsc);
    
    fclose(pFileAsc);
    pFileAsc = NULL;
}

BAS_EXPORT void _vWriteLogHex(char const *vspFileName, char const *vspFuncName,int vtLine,
                              char *vspLogName, void *vvpRawBuf, int vtLen)
{
    struct tm               *strpTm;
	struct timeval tv;
    char                    saTraceLogFileName[ N_MAX_PATH_LEN + N_MAX_NAME_LEN + 1 ];
    char                    saFileName[ N_MAX_NAME_LEN + 1 ];
    register unsigned int   i, j, k;
    unsigned char           *spRawBuf;
    char                    saTmpBuf[17];
    struct stat             strFstat;
    char                    *psAppHome;

    char      caEncryptBuf[51200] = "";
    char      caOutBuf[51200] = "";
    int       iOutBufLen  = 0;

    spRawBuf = vvpRawBuf;
    
	/*得到当前时间*/
	gettimeofday(&tv,NULL);	

	tv.tv_sec += 8 * 60 * 60;	/*取的格林威治时间，我们在东8区，所以加上*/
	strpTm = gmtime((time_t *) &tv.tv_sec);
	
    sprintf(saFileName, "%s.%s.%04d%02d%02d.%-d.%02d",
            vspLogName,
            gcaProgramName,
            strpTm->tm_year+1900,
            strpTm->tm_mon+1,
            strpTm->tm_mday,
			         getpid( ),
			         iLogRcdNum);

    psAppHome = getenv("LOGPATH");
    if (psAppHome == NULL) {
        sprintf(saTraceLogFileName, "%s", saFileName);
    }
    else {
        sprintf(saTraceLogFileName, "%s/log/%s/%s", psAppHome, gcaModuleName, saFileName);
    }

    if (stat(saTraceLogFileName, &strFstat) < 0) {           
        if ((pFileHex = fopen( saTraceLogFileName, "ab+" ) ) == NULL )
            return;
    }
    else {
        int iLogSize = 0;
        iLogSize = atoi(getenv("LOGSIZE"));
        if (iLogSize == 0)
        {
            iLogSize = 100*1000*1000;
        }
     
        if (strFstat.st_size > iLogSize)
        {
             fclose(pFileHex);
             pFileHex = NULL;
             
            /*达到日志大小上限，另起一日志*/
            if (++iLogRcdNum > 99)
            {
                iLogRcdNum = 0;
            }
            
            sprintf(saFileName, "%s.%04d%02d%02d.%-d.%02d",
            vspLogName,
            strpTm->tm_year+1900,
            strpTm->tm_mon+1,
            strpTm->tm_mday,
            getpid( ),
            iLogRcdNum);
            sprintf(saTraceLogFileName, "%s/log/%s/%s", psAppHome, gcaModuleName, saFileName);
         
        }
        if (pFileHex == NULL)
        {
            if ((pFileHex = fopen( saTraceLogFileName, "ab+" ) ) == NULL )
                return;
        }
    }

    sprintf( caOutBuf + strlen(caOutBuf),"[%02d:%02d:%02d:%03ld]",
            strpTm->tm_hour,
            strpTm->tm_min,
            strpTm->tm_sec ,
			tv.tv_usec/1000);

    sprintf( caOutBuf + strlen(caOutBuf),"[%s][%s][%d]",
            vspFileName,
            vspFuncName,
            vtLine );
    sprintf( caOutBuf + strlen(caOutBuf),"[%-d]length[%d]\n",
            getpid( ),
            vtLen);
    j = vtLen/16;
    for (i = 0; i < j; i++) {
        sprintf( caOutBuf + strlen(caOutBuf),"%06x: ", i*16);
        for (k = 0; k < 16; k++) {
            sprintf( caOutBuf + strlen(caOutBuf),"%02X ", spRawBuf[i*16 + k]);
            if (k == 7)
                sprintf( caOutBuf + strlen(caOutBuf)," ");
        }

        sprintf( caOutBuf + strlen(caOutBuf),"; ");

        for (k = 0; k < 16; k++) {
            if (isprint(spRawBuf[i*16 + k]))
                sprintf( caOutBuf + strlen(caOutBuf),"%c", spRawBuf[i*16 + k]);
            else
                sprintf( caOutBuf + strlen(caOutBuf),".");
            if (k == 7)
                sprintf( caOutBuf + strlen(caOutBuf)," ");
        }
        sprintf( caOutBuf + strlen(caOutBuf)," :%06x\n", i*16+15);
    }
    if (vtLen%16) {
        sprintf( caOutBuf + strlen(caOutBuf),"%06x: ", j*16);
        for ( i = 0; i < 16; i++) {
            if (i + j*16 < vtLen)
                sprintf( caOutBuf + strlen(caOutBuf),"%02X ", spRawBuf[i+j*16]);
            else
                sprintf( caOutBuf + strlen(caOutBuf),"   ");
            if ( i == 7)
                sprintf( caOutBuf + strlen(caOutBuf)," ");
        }
       sprintf( caOutBuf + strlen(caOutBuf),"; ");
        for ( i = 0; i < 16; i++) {
            if (i + j*16 < vtLen) {
                if (isprint(spRawBuf[i+j*16]))
                    sprintf( caOutBuf + strlen(caOutBuf),"%c", spRawBuf[i+j*16]);
                else
                    sprintf( caOutBuf + strlen(caOutBuf),".");
            }
            else
                sprintf( caOutBuf + strlen(caOutBuf)," ");
            if ( i%16 == 7)
                sprintf( caOutBuf + strlen(caOutBuf)," ");
        }
        sprintf( caOutBuf + strlen(caOutBuf)," :%06x\n", j+16+15);
    }
   sprintf( caOutBuf + strlen(caOutBuf),"\n\n");

     //加密：
    iOutBufLen = lencodelog(caEncryptBuf,sizeof(caEncryptBuf),caOutBuf,strlen(caOutBuf));
    fwrite(caEncryptBuf,iOutBufLen,1,pFileHex);
    fflush(pFileHex);

    fclose( pFileHex );
    pFileHex = NULL;
}

BAS_EXPORT int _tAssertFailedLine(char const *vspFileName, char const *vspFuncName,int vtLine)
{
    _vWriteLogAsc(vspFileName,  vspFuncName, vtLine, "TRCLOG", "ASSERT Failure!\n");

    return 0;
}

BAS_EXPORT void _vDebugBreak(void)
{
    usrlog("DEBUG Break!\n");
    exit(-1);
}
