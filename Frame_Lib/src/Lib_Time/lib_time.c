/*************************************************************
*    FileName : Time.c                                       *
*    FileFunc : 时间、日期处理的定义实现的源文件             *
*    Version: V0.1                                           *
*    Author :                                                *
*    Date : 2016-12-25                                       *
*    Descp :                                                 *
*************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lib_time.h"


#if 0
/*************************************************************
*    Function:    getstrdate                                 *
*    Description: 获取字符串个数的日期(格式 %d%d%d )         *
*    Calls:                                                  *
*    Called By:                                              *
*    Table Accessed:                                         *
*    Table Updated:                                          *
*    Input:                                                  *
*    Output:                                                 *
*    Return:                                                 *
*    Others:                                                 *
*************************************************************/
int getstrdate( char *sOutStrTime )
{
    time_t timep; 
    struct tm *p; 
    
    memset( &timep, 0x00, sizeof( timep) );
    p = NULL;
    
    if( NULL == sOutStrTime )
    {
        return -1;
    }

    time( &timep );
    p = localtime( &timep );
    sprintf( sOutStrTime, "%d%d%d", (1900 + p->tm_year),(1 + p->tm_mon), p->tm_mday );
    
    return 0;
}


/*************************************************************
*    Function:    getstrtime                                 *
*    Description: 获取字符串个数的时间(格式：%02d%02d%02d )  *
*    Calls:                                                  *
*    Called By:                                              *
*    Table Accessed:                                         *
*    Table Updated:                                          *
*    Input:                                                  *
*    Output:                                                 *
*    Return:                                                 *
*    Others:                                                 *
*************************************************************/
int getstrtime( char *sOutStrTime )
{
    time_t timep; 
    struct tm *p; 
    
    memset( &timep, 0x00, sizeof( timep) );
    p = NULL;
    
    if( NULL == sOutStrTime )
    {
        return -1;
    }

    time( &timep );
    p = localtime( &timep );
    sprintf( sOutStrTime, "%02d%02d%02d", p->tm_hour, p->tm_min, p->tm_sec );
    
    return 0;
}


/*************************************************************
*    Function:    basAddMonths                               *
*    Description: YYYYMMDD日期往后延期几个月                 *
*    Calls:                                                  *
*    Called By:                                              *
*    Table Accessed:                                         *
*    Table Updated:                                          *
*    Input:       pcaBeginMonth    YYYYMMDD                  *
*    Output:      iMonths         往后延期几个月             *
*    Return:                                                 *
*    Others:                                                 *
*************************************************************/
char *basAddMonths(const char *pcaBeginMonth, int iMonths)
{
    static char caBuf[8 + 1];
    char caYear[5], caMonth[3], caDay[3];
    struct tm tTime;
    time_t tTimeVal;
    long lValue;

    memset(caBuf, 0, sizeof(caBuf));
    memset(caYear, 0, sizeof(caYear));
    memset(caMonth, 0, sizeof(caMonth));
    memset(caDay, 0, sizeof(caDay));

    memcpy(caYear, pcaBeginMonth, 4);
    memcpy(caMonth, pcaBeginMonth + 4, 2);
    if (pcaBeginMonth[6] == 0)
    {
        strcpy(caDay, "01");
    }
    else
    {
        memcpy(caDay, pcaBeginMonth + 6, 2);
    }

    lValue = atol(caYear) * 12 + atol(caMonth) + iMonths - 1;

    sprintf(caYear, "%04d", (int)((lValue) / 12));
    sprintf(caMonth, "%02d", (int)((lValue) % 12 + 1));

    while (1)
    {
        struct tm *ptTimeStruct;
        char caBufTmp[8 + 1];
    
        tTime.tm_year  = atoi(caYear) - 1900;
        tTime.tm_mon   = atoi(caMonth) - 1;
        tTime.tm_mday  = atoi(caDay);
        tTime.tm_hour  = 0;
        tTime.tm_min   = 0;
        tTime.tm_sec   = 1;
        tTime.tm_isdst = -1;
    
        sprintf(caBuf, "%s%s%s", caYear, caMonth, caDay);
    
        if ((tTimeVal = mktime(&tTime)) == -1)
        {
            memset(caBuf, 0, sizeof(caBuf));
            return caBuf;
        }
        if ((ptTimeStruct = localtime(&tTimeVal)) == NULL)
        {
            memset(caBuf, 0, sizeof(caBuf));
            return caBuf;
        }
        sprintf(caBufTmp, "%04d%02d%02d", ptTimeStruct->tm_year + 1900, ptTimeStruct->tm_mon + 1, ptTimeStruct->tm_mday);
        if (memcmp(caBuf, caBufTmp, sizeof(caBuf) - 1) == 0)
        {
            break;
        }
        sprintf(caDay, "%02d", (int)(atol(caDay) - 1));
    }

    if (pcaBeginMonth[6] == 0)
    {
        caBuf[6] = 0;
    }

    return caBuf;
}

#endif


#if 0
int main()
{
    char sDate[8+1];
    memset( sDate, 0x00, sizeof( sDate ) );
    memcpy( sDate, "20180131", 8);
    printf( "%s\n", basAddMonths(sDate, 1 ) );
    
    
}
#endif



