/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_log.c                                                   */
/* DESCRIPTIONS: ϵͳ��־�ӿ�                                                */
/* AUTHOR      : Robert Peng                                                 */
/* CREATE DATE : 2008-06-23                                                  */
/*****************************************************************************/

#include <time.h>
#include <sys/time.h>
#include "bas/bas_global.h"

static const char *mpcaId = "$Id: bas_date.c,v 1.2 2012/02/08 09:24:21 cardv5c Exp $";

/*****************************************************************************/
/* FUNC:   char *basGetSysJulianDate(void)                                   */
/* PARAMS: ��                                                                */
/* RETURN: ̫����,��ʽYYDDD                                                  */
/* DESC:   �õ�ϵͳ̫����                                                    */
/*****************************************************************************/
BAS_EXPORT char *basGetSysJulianDate(void)
{
	static char caTimeBuf[6];

	struct tm *ptTimeStruct;
	time_t tTimeVal;

	memset(caTimeBuf, 0, sizeof(caTimeBuf));
	memset(caTimeBuf, ' ', sizeof(caTimeBuf) - 1);

	if ((tTimeVal = time(NULL)) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetSysJulianDay:time����ʧ��");
		return caTimeBuf;
	}
	if ((ptTimeStruct = localtime(&tTimeVal)) == NULL)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetSysJulianDay:localtime����ʧ��");
		return caTimeBuf;
	}

	sprintf(caTimeBuf, "%02d%03d",
			ptTimeStruct->tm_year % 100,
			ptTimeStruct->tm_yday + 1);

	return caTimeBuf;
}

/*****************************************************************************/
/* FUNC:   char *basGetSysDate(void);                                        */
/* PARAMS: ��                                                                */
/* RETURN: ��ʽYYYYMMDD                                                      */
/* DESC:   ȡ��UNIX����                                                      */
/*****************************************************************************/
BAS_EXPORT char *basGetSysDate(void)
{
	static char caTimeBuf[8 + 1];

	struct tm *ptTimeStruct;
	time_t tTimeVal;

	memset(caTimeBuf, 0, sizeof(caTimeBuf));
	memset(caTimeBuf, ' ', sizeof(caTimeBuf) - 1);

	if ((tTimeVal = time(NULL)) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetSysDate:time����ʧ��");
		return caTimeBuf;
	}
	if ((ptTimeStruct = localtime(&tTimeVal)) == NULL)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetSysDate:localtime����ʧ��");
		return caTimeBuf;
	}

	sprintf(caTimeBuf, "%04d%02d%02d",
			ptTimeStruct->tm_year + 1900,
			ptTimeStruct->tm_mon + 1,
			ptTimeStruct->tm_mday);

	return caTimeBuf;
}

/*****************************************************************************/
/* FUNC:   char *basGetSysTime(void)                                         */
/* PARAMS: ��                                                                */
/* RETURN: ��ʽHHMMSS                                                        */
/* DESC:   ȡ��UNIXʱ��                                                      */
/*****************************************************************************/
BAS_EXPORT char *basGetSysTime(void)
{
	static char caTimeBuf[6 + 1];

	struct tm *ptTimeStruct;
	time_t tTimeVal;

	memset(caTimeBuf, 0, sizeof(caTimeBuf));
	memset(caTimeBuf, ' ', sizeof(caTimeBuf) - 1);

	if ((tTimeVal = time(NULL)) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetSysTime:time����ʧ��");
		return caTimeBuf;
	}
	if ((ptTimeStruct = localtime(&tTimeVal)) == NULL)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetSysTime:localtime����ʧ��");
		return caTimeBuf;
	}

	sprintf(caTimeBuf, "%02d%02d%02d",
			ptTimeStruct->tm_hour,
			ptTimeStruct->tm_min,
			ptTimeStruct->tm_sec);

	return caTimeBuf;
}

/*****************************************************************************/
/* FUNC:   char *basGetSysTimestamp(void)                                    */
/* PARAMS: ��                                                                */
/* RETURN: ��ʽHHMMSSMS (6λMS)                                              */
/* DESC:   ȡ��UNIXʱ��                                                      */
/*****************************************************************************/
BAS_EXPORT char *basGetSysTimestamp(void)
{
	static char caTimeBuf[12 + 1];

	struct tm *ptTimeStruct;
	struct timeval tTimeValue;
	time_t tTimeVal;

	memset(caTimeBuf, 0, sizeof(caTimeBuf));
	memset(caTimeBuf, ' ', sizeof(caTimeBuf) - 1);

	if (gettimeofday(&tTimeValue, NULL) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetSysTimestamp:gettimeofday����ʧ��");
		return caTimeBuf;
	}
	if ((ptTimeStruct = localtime(&(tTimeValue.tv_sec))) == NULL)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetSysTimestamp:localtime����ʧ��");
		return caTimeBuf;
	}

	sprintf(caTimeBuf, "%02d%02d%02d%06d",
			ptTimeStruct->tm_hour,
			ptTimeStruct->tm_min,
			ptTimeStruct->tm_sec,
			(int)tTimeValue.tv_usec);

	return caTimeBuf;
}

/*****************************************************************************/
/* FUNC:   char *basGetGmDate(void);                                         */
/* PARAMS: ��                                                                */
/* RETURN: ��ʽYYYYMMDD                                                      */
/* DESC:   ȡ��UNIX������������                                              */
/*****************************************************************************/
BAS_EXPORT char *basGetGmDate(void)
{
	static char caTimeBuf[8 + 1];

	struct tm *ptTimeStruct;
	time_t tTimeVal;

	memset(caTimeBuf, 0, sizeof(caTimeBuf));
	memset(caTimeBuf, ' ', sizeof(caTimeBuf) - 1);

	if ((tTimeVal = time(NULL)) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetGmDate:time����ʧ��");
		return caTimeBuf;
	}
	if ((ptTimeStruct = gmtime(&tTimeVal)) == NULL)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetGmDate:localtime����ʧ��");
		return caTimeBuf;
	}

	sprintf(caTimeBuf, "%04d%02d%02d",
			ptTimeStruct->tm_year + 1900,
			ptTimeStruct->tm_mon + 1,
			ptTimeStruct->tm_mday);

	return caTimeBuf;
}

/*****************************************************************************/
/* FUNC:   char *basGetGmTime(void)                                          */
/* PARAMS: ��                                                                */
/* RETURN: ��ʽHHMMSS                                                        */
/* DESC:   ȡ��UNIX��������ʱ��                                              */
/*****************************************************************************/
BAS_EXPORT char *basGetGmTime(void)
{
	static char caTimeBuf[6 + 1];

	struct tm *ptTimeStruct;
	time_t tTimeVal;

	memset(caTimeBuf, 0, sizeof(caTimeBuf));
	memset(caTimeBuf, ' ', sizeof(caTimeBuf) - 1);

	if ((tTimeVal = time(NULL)) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetGmTime:time����ʧ��");
		return caTimeBuf;
	}
	if ((ptTimeStruct = gmtime(&tTimeVal)) == NULL)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basGetGmTime:localtime����ʧ��");
		return caTimeBuf;
	}

	sprintf(caTimeBuf, "%02d%02d%02d",
			ptTimeStruct->tm_hour,
			ptTimeStruct->tm_min,
			ptTimeStruct->tm_sec);

	return caTimeBuf;
}

/*****************************************************************************/
/* FUNC:   int basCalDiffTwoDays(const char *pcaBeginDate,                   */
/*                               const char *pcaEndDate, int *piDays)        */
/* PARAMS: pcaBeginDate - ��ʼ������(YYYYMMDD) (I)                           */
/*         pcaEndDate   - ����������(YYYYMMDD) (I)                           */
/* RETURN: ����                                                              */
/* DESC:   ����caBeginDate��caEndDate֮�������                              */
/*****************************************************************************/
BAS_EXPORT int basCalDiffTwoDays(const char *pcaBeginDate, const char *pcaEndDate, int *piDays)
{
	char caYear[5], caMonth[3], caDay[3];
	struct tm	tTime1, tTime2;
	time_t		tTimeVal1, tTimeVal2, tTimeDummy;

	memset(caYear, 0, sizeof(caYear));
	memset(caMonth, 0, sizeof(caMonth));
	memset(caDay, 0, sizeof(caDay));

	/* ����date 1 */
	memcpy(caYear, pcaBeginDate, 4);
	memcpy(caMonth, pcaBeginDate + 4, 2);
	memcpy(caDay, pcaBeginDate + 6, 2);

	tTime1.tm_year  = atoi(caYear) - 1900;
	tTime1.tm_mon   = atoi(caMonth) - 1;
	tTime1.tm_mday  = atoi(caDay);
	tTime1.tm_hour  = 0;
	tTime1.tm_min   = 0;
	tTime1.tm_sec   = 1;
	tTime1.tm_isdst = -1;

	if ((tTimeVal1 = mktime(&tTime1)) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basCalDiffTwoDays:mktime����ʧ��:[%s]:[%s]", pcaBeginDate, pcaEndDate);
		return -1;
	}

	/* ����date 2 */
	memcpy(caYear, pcaEndDate, 4);
	memcpy(caMonth, pcaEndDate + 4, 2);
	memcpy(caDay, pcaEndDate + 6, 2);

	tTime2.tm_year  = atoi(caYear) - 1900;
	tTime2.tm_mon   = atoi(caMonth) - 1;
	tTime2.tm_mday  = atoi(caDay);
	tTime2.tm_hour  = 0;
	tTime2.tm_min   = 0;
	tTime2.tm_sec   = 1;
	tTime2.tm_isdst = -1;

	if ((tTimeVal2 = mktime(&tTime2)) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basCalDiffTwoDays:mktime����ʧ��:[%s]:[%s]", pcaBeginDate, pcaEndDate);
		return -1;
	}

	/* �������ڼ�� */
	if (tTimeVal2 > tTimeVal1)
	{
		*piDays = (int)(difftime(tTimeVal2, tTimeVal1) / 24 / 60 / 60 + 0.5);
	}
	else
	{
		*piDays = (int)(difftime(tTimeVal2, tTimeVal1) / 24 / 60 / 60 - 0.5);
	}
	/* +0.5 �������ϵͳ����������ʱ�����ܲ���1Сʱ��� */

	return 0;
}

/*****************************************************************************/
/* FUNC:   int basCalDiffTwoMonths(const char *pcaBeginMonth,                */
/*                                 const char *pcaEndMonth, int *piMonths)   */
/* PARAMS: pcaBeginMonth - ��ʼ����(YYYYMM) (I)                              */
/*         pcaEndMonth   - ��������(YYYYMM) (I)                              */
/* RETURN: ����                                                              */
/* DESC:   ����caBeginMonth��caEndMonth֮�������                            */
/*****************************************************************************/
BAS_EXPORT int basCalDiffTwoMonths(const char *pcaBeginMonth, const char *pcaEndMonth, int *piMonths)
{
	char caBeginYear[5], caEndYear[5];
	char caBeginMonth[3], caEndMonth[3];

	memset(caBeginYear, 0, sizeof(caBeginYear));
	memset(caEndYear, 0, sizeof(caEndYear));
	memset(caBeginMonth, 0, sizeof(caBeginMonth));
	memset(caEndMonth, 0, sizeof(caEndMonth));

	memcpy(caBeginYear, pcaBeginMonth, 4);
	memcpy(caEndYear, pcaEndMonth, 4);
	memcpy(caBeginMonth, pcaBeginMonth + 4, 2);
	memcpy(caEndMonth, pcaEndMonth + 4, 2);

	*piMonths = (atoi(caEndYear) - atoi(caBeginYear)) * 12 + atoi(caEndMonth) - atoi(caBeginMonth);

	return 0;
}

/*****************************************************************************/
/* FUNC:   char *basAddDays(const char *pcaBeginDate, int iDays)             */
/* PARAMS: pcaBeginDate - ��ʼ������(YYYYMMDD) (I)                           */
/*         iDays       - ����                                                */
/* RETURN: ��ǰ,��ʽYYYYMMDD                                                 */
/* DESC:   ����caBeginDay֮��iDay�������                                    */
/*****************************************************************************/
BAS_EXPORT char *basAddDays(const char *pcaBeginDate, int iDays)
{
	static char caTimeBuf[9];
	struct tm tTime, *ptTime;
	time_t    tTimeNow;
	char caYear[5], caMonth[3], caDay[3];

	memset(caTimeBuf, 0, sizeof(caTimeBuf));
	memset(caTimeBuf, ' ', sizeof(caTimeBuf) - 1);

	memset(caYear, 0, sizeof(caYear));
	memset(caMonth, 0, sizeof(caMonth));
	memset(caDay, 0, sizeof(caDay));

	memcpy(caYear, pcaBeginDate, 4);
	memcpy(caMonth, pcaBeginDate + 4, 2);
	memcpy(caDay, pcaBeginDate + 6, 2);

	tTime.tm_year  = atoi(caYear) - 1900;
	tTime.tm_mon   = atoi(caMonth) - 1;
	tTime.tm_mday  = atoi(caDay);
	tTime.tm_hour  = 3;    /* ��������ʱ��������� */
	tTime.tm_min   = 0;
	tTime.tm_sec   = 1;
	tTime.tm_isdst = -1;

	if ((tTimeNow = mktime(&tTime)) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basAddDays:mktime����ʧ��:[%s]:[%d]", pcaBeginDate, iDays);
		return caTimeBuf;
	}
	tTimeNow += iDays * (24 * 60 * 60);

	ptTime = localtime(&tTimeNow);

	sprintf(caTimeBuf, "%04d%02d%02d",
			ptTime->tm_year + 1900,
			ptTime->tm_mon + 1,
			ptTime->tm_mday);

	return caTimeBuf;
}

/*****************************************************************************/
/* FUNC:   char *basSubDays(const char *pcaBeginDate, int iDays)             */
/* PARAMS: pcaBeginDate - ��ʼ������(YYYYMMDD) (I)                           */
/*         iDays        - ����                                               */
/* RETURN: ��ǰ,��ʽYYYYMMDD                                                 */
/* DESC:   ����caBeginDay֮ǰiDay�������                                    */
/*****************************************************************************/
BAS_EXPORT char *basSubDays(const char *pcaBeginDate, int iDays)
{
	return basAddDays(pcaBeginDate, -iDays);
}

/*****************************************************************************/
/* FUNC:   char *basAddMonths(const char *pcaBeginMonth, int iMonths)        */
/* PARAMS: pcaBeginMonth - ��ʼ����(YYYYMM) (I)                              */
/*         iMonths       - ����                                              */
/* RETURN: ��ǰ,��ʽYYYYMM                                                   */
/* DESC:   ����caBeginMonth֮��iMonths�µ�����                               */
/*****************************************************************************/
BAS_EXPORT char *basAddMonths(const char *pcaBeginMonth, int iMonths)
{
	static char caBuf[8 + 1];
	char caYear[5], caMonth[3], caDay[3];
	struct tm tTime;
	time_t tTimeVal;
	long lValue;

	if (strcmp(pcaBeginMonth, "999999") == 0 ||
		basIsSpace(pcaBeginMonth, strlen(pcaBeginMonth)) == BAS_TRUE)
    {
		memset(caBuf, 0, sizeof(caBuf));
		strcpy(caBuf, "99999999");
        return caBuf;
	}

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

/*****************************************************************************/
/* FUNC:   int basIsLeapYear(const char *pcaYear)                            */
/* PARAMS: pcaYear - ��� (I)                                                */
/* RETURN:  1:����                                                           */
/*          0:������                                                         */
/* DESC:   ����caBeginDay֮ǰiDay�������                                    */
/*****************************************************************************/
BAS_EXPORT int basIsLeapYear(const char *pcaYear)
{
	int iYear = atoi(pcaYear);

	if ((iYear % 4 == 0 && iYear % 100 != 0 ) || iYear % 400 == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*****************************************************************************/
/* FUNC:   char *basDateToJulianDate(const char *pcaDate)                    */
/* PARAMS: pcaDate - ����(YYYYMMDD) (I)                                      */
/* RETURN: ̫���� (YYDDD)                                                    */
/* DESC:   ������ת��Ϊ̫���ո�ʽ                                            */
/*****************************************************************************/
BAS_EXPORT char *basJulianDateToDate(const long lDate)
{
	static char caTimeBuf[7];
	long lYear = 0, lDay = 0;
	struct tm	tTime;
	int i =0, j = 0;
	
	int monarr[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	
	
	memset(caTimeBuf, 0, sizeof(caTimeBuf));
	memset(caTimeBuf, ' ', sizeof(caTimeBuf) - 1);
	
	lDay = lDate % 1000;
	lYear  = lDate / 1000;
	if (((lYear % 4 == 0) && (lYear % 100 != 0)) || (lYear % 400 == 0))
	{// ����
		monarr[1] = 29;
	}
	
	for (j = 0, i = lDay; i > monarr[j]; j++)
	{
		i -= monarr[j];
	}
	
	// �жϸ������Ƿ�Ϸ�
	tTime.tm_year  = lYear - 1900;
	tTime.tm_mon   = j - 1;
	tTime.tm_mday  = i;
	tTime.tm_hour  = 0;
	tTime.tm_min   = 0;
	tTime.tm_sec   = 1;
	tTime.tm_isdst = -1;
	
	if (mktime(&tTime) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basJulianDateToDate:mktime����ʧ��:[%d]", lDate);
		return caTimeBuf;
	}
	
	sprintf(caTimeBuf, "%02d%02d%02d",
	        lYear % 100,
	        j + 1,
	        i);
	return caTimeBuf;
}

/*****************************************************************************/
/* FUNC:   char *basDateToJulianDate(const char *pcaDate)                    */
/* PARAMS: pcaDate - ����(YYYYMMDD) (I)                                      */
/* RETURN: ̫���� (YYDDD)                                                    */
/* DESC:   ������ת��Ϊ̫���ո�ʽ                                            */
/*****************************************************************************/
BAS_EXPORT char *basDateToJulianDate(const char *pcaDate)
{
	static char caTimeBuf[5];
	char caYear[6], caMonth[3], caDay[3];
	struct tm	tTime;
	struct tm 	*ptTimeStruct;
	time_t		tTimeVal;

	memset(caYear, 0, sizeof(caYear));
	memset(caMonth, 0, sizeof(caMonth));
	memset(caDay, 0, sizeof(caDay));

	memset(caTimeBuf, 0, sizeof(caTimeBuf));
	memset(caTimeBuf, ' ', sizeof(caTimeBuf) - 1);

	/* ����date */
	memcpy(caYear, pcaDate, 4);
	memcpy(caMonth, pcaDate + 4, 2);
	memcpy(caDay, pcaDate + 6, 2);

	tTime.tm_year  = atoi(caYear) - 1900;
	tTime.tm_mon   = atoi(caMonth) - 1;
	tTime.tm_mday  = atoi(caDay);
	tTime.tm_hour  = 0;
	tTime.tm_min   = 0;
	tTime.tm_sec   = 1;
	tTime.tm_isdst = -1;

	if ((tTimeVal = mktime(&tTime)) == -1)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basDateToJuliandate:mktime����ʧ��:[%s]", pcaDate);
		return caTimeBuf;
	}

	if ((ptTimeStruct = localtime(&tTimeVal)) == NULL)
	{
		BAS_LOG(BAS_ERR300002_ERRORTIME, 0, errno, "basDateToJuliandate:localtime����ʧ��");
		return caTimeBuf;
	}

	sprintf(caTimeBuf, "%02d%03d",
			ptTimeStruct->tm_year % 100,
			ptTimeStruct->tm_yday + 1);

	return caTimeBuf;
}

/*****************************************************************************/
/* FUNC:   char *basDatePattern(const char *pcaDate, const char *pcaPattern) */
/* PARAMS: pcaDate    - ת���� (YYYYMMDD) (I)                                */
/*         pcaPattern - ��ʽ�� (I)                                           */
/* RETURN: ���ش�                                                            */
/* DESC:   ��ģʽ��ת��                                                      */
/*****************************************************************************/
BAS_EXPORT char *basDatePattern(const char *pcaDate, const char *pcaPattern)
{
	static char caDate[21];
	int		i;
	int		j = 0;
	int		k = 0;
	int		m = 0;
	int		n = 0;
	int		iLen = 0;

	char	caYear[5];
	char	caMonth[3];
	char	caDay[3];
	char	caBuf[21];

	int     iYLength = 0;
	const char *iFindY = pcaPattern;

	memset(caDate, ' ', sizeof(caDate));
	memset(caYear, 0, sizeof(caYear));
	memset(caMonth, 0, sizeof(caMonth));
	memset(caDay, 0, sizeof(caDay));
	memset(caBuf, 0, sizeof(caBuf));

	memcpy(caYear, pcaDate, 4);
	memcpy(caMonth, pcaDate+4, 2);
	memcpy(caDay, pcaDate+6, 2);

	iLen = strlen(pcaPattern);
	if (iLen > 20)
	{
		BAS_LOG(BAS_LOGERROR, 0, 0, "basDatePattern:���ڸ�ʽ����");
		return caDate;
	}

	//��added by tianx for YY:MM:DD
	while(*iFindY)
	{
	    if('Y' == *iFindY++)
	    {
	        iYLength++;
	    }
	}
    if(4 != iYLength)
    {
        k += 2;
    }
	//��added by tianx for YY:MM:DD


	for (i = 0; i < iLen; i++)
	{
		if (pcaPattern[i] == '/' ||
            pcaPattern[i] == '-' ||
            pcaPattern[i] == ' ' ||
            pcaPattern[i] == 0xc4 ||
			pcaPattern[i] == 0xffffffc4 ||
            pcaPattern[i] == 0xea ||
            pcaPattern[i] == 0xffffffea ||
            pcaPattern[i] == 0xd4 ||
            pcaPattern[i] == 0xffffffd4 ||
            pcaPattern[i] == 0xc2 ||
            pcaPattern[i] == 0xffffffc2 ||
            pcaPattern[i] == 0xc8 ||
            pcaPattern[i] == 0xffffffc8 ||
            pcaPattern[i] == 0xd5  ||
            pcaPattern[i] == 0xffffffd5)
		{
			caBuf[j] = pcaPattern[i];
		}
		else if (pcaPattern[i] == 'Y' || pcaPattern[i] == 'y')
		{
			caBuf[j] = caYear[k];
			k++;
		}
		else if (pcaPattern[i] == 'M' || pcaPattern[i] == 'm')
		{
			caBuf[j] = caMonth[m];
			m++;
		}
		else if (pcaPattern[i] == 'D' || pcaPattern[i] == 'd')
		{
			caBuf[j] = caDay[n];
			n++;
		}
		else
		{
			BAS_LOG(BAS_LOGINFO, 0, 0, "basDatePattern:���ڸ�ʽ����[%s][%x][%d]", pcaPattern, pcaPattern[i], i);
			return caDate;
		}
		j++;
	}

	if (k > 4 || m > 2 || n > 2)
	{
		BAS_LOG(BAS_LOGINFO, 0, 0, "basDatePattern:���ڸ�ʽ����");
		return caDate;
	}

	caBuf[iLen] = 0;
	strcpy(caDate, caBuf);

	return caDate;
}

/*****************************************************************************/
/* FUNC:   char *basTimePattern(const char *pcaTime, const char *pcaPattern) */
/* PARAMS: pcaTime    - ת���� (HHMMSS) (I)                                  */
/*         pcaPattern - ��ʽ�� (I)                                           */
/* RETURN: ���ش�                                                            */
/* DESC:   ��ģʽ��ת��                                                      */
/*****************************************************************************/
BAS_EXPORT char *basTimePattern(const char *pcaTime, const char *pcaPattern)
{
	static char caTime[21];
	int		i;
	int		j = 0;
	int		k = 0;
	int		m = 0;
	int		n = 0;
	int		iLen = 0;

	char	caHour[3];
	char	caMinute[3];
	char	caSecond[3];
	char	caBuf[21];



	memset(caTime, ' ', sizeof(caTime));
	memset(caHour, 0, sizeof(caHour));
	memset(caMinute, 0, sizeof(caMinute));
	memset(caSecond, 0, sizeof(caSecond));
	memset(caBuf, 0, sizeof(caBuf));

	memcpy(caHour, pcaTime, 2);
	memcpy(caMinute, pcaTime+2, 2);
	memcpy(caSecond, pcaTime+4, 2);

	iLen = strlen(pcaPattern);
	if (iLen > 20)
	{
		BAS_LOG(BAS_LOGERROR, 0, 0, "basTimePattern:ʱ���ʽ����");
		return caTime;
	}

	for (i = 0; i < iLen; i++)
	{
		if (pcaPattern[i] == '/' ||
            pcaPattern[i] == '-' ||
            pcaPattern[i] == ' ' ||
            pcaPattern[i] == ':' )
		{
			caBuf[j] = pcaPattern[i];
		}
		else if (pcaPattern[i] == 'H' || pcaPattern[i] == 'h')
		{
			caBuf[j] = caHour[k];
			k++;
		}
		else if (pcaPattern[i] == 'M' || pcaPattern[i] == 'm')
		{
			caBuf[j] = caMinute[m];
			m++;
		}
		else if (pcaPattern[i] == 'S' || pcaPattern[i] == 's')
		{
			caBuf[j] = caSecond[n];
			n++;
		}
		else
		{
			BAS_LOG(BAS_LOGINFO, 0, 0, "basTimePattern:ʱ���ʽ����[%s][%x][%d]", pcaPattern, pcaPattern[i], i);
			return caTime;
		}
		j++;
	}

	if (k > 4 || m > 2 || n > 2)
	{
		BAS_LOG(BAS_LOGINFO, 0, 0, "basTimePattern:ʱ���ʽ����");
		return caTime;
	}

	caBuf[iLen] = 0;
	strcpy(caTime, caBuf);

	return caTime;
}


/*****************************************************************************/
/* FUNC:   int basGetLastDayOfMonth(char *pcaDate)                           */
/* PARAMS: pcaDate    - ���ڴ� (YYYYMMDD) (I/O)                              */
/* RETURN: ���ش�                                                            */
/* DESC:   �õ����µ����һ��                                                */
/*****************************************************************************/
BAS_EXPORT int basGetLastDayOfMonth(char *pcaDate)
{
	char caYear[5];
	char caMonth[3];

	memset(caYear, 0, sizeof(caYear));
	memset(caMonth, 0, sizeof(caMonth));

	memcpy(caYear, pcaDate, 4);
	memcpy(caMonth, pcaDate + 4, 2);

	switch (atoi(caMonth))
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			memcpy(pcaDate + 6, "31", 2);
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			memcpy(pcaDate + 6, "30", 2);
			break;
		case 2:
			if (basIsLeapYear(caYear))
			{
				memcpy(pcaDate + 6, "29", 2);
			}
			else
			{
				memcpy(pcaDate + 6, "28", 2);
			}
			break;
		default:
			BAS_LOG(BAS_LOGERROR, 0, 0, "basGetLastDayOfMonth:���ڸ�ʽ����");
			return -1;
	}
	return 0;
}

/*****************************************************************************/
/* FUNC:   char *basGetMonthName(int iMonth)                                 */
/* PARAMS: iMonth - �·�                                                     */
/* RETURN: ������дӢ����ĸ                                                  */
/* DESC:   �õ��·���д                                                      */
/*****************************************************************************/
BAS_EXPORT char *basGetMonthName(int iMonth)
{
	switch (iMonth)
	{
		case 1:
			return "JAN";
		case 2:
			return "FEB";
		case 3:
			return "MAR";
		case 4:
			return "APR";
		case 5:
			return "MAY";
		case 6:
			return "JUN";
		case 7:
			return "JUL";
		case 8:
			return "AUG";
		case 9:
			return "SEP";
		case 10:
			return "OCT";
		case 11:
			return "NOV";
		case 12:
			return "DEC";
		default:
			return "   ";
	}
}

/*****************************************************************************/
/* FUNC:   int basIsValidDate(const char *pcaDate)                           */
/* PARAMS: pcaDate - ����(YYYYMMDD) (I)                                      */
/* RETURN:  1:�Ϸ�����                                                       */
/*          0:�Ƿ�����                                                       */
/* DESC:   �ж�����ʮ�ֺϷ�                                                  */
/*****************************************************************************/
BAS_EXPORT int basIsValidDate(const char *pcaDate)
{
	char caYear[5], caMonth[3], caDay[3];
    static int iaMonthDaysOfMapLeapYear[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static int iaMonthDaysOfMapNoLeapYear[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int iYear = 0;
    int iMonth = 0;
    int iDay = 0;

	memset(caYear, 0, sizeof(caYear));
	memset(caMonth, 0, sizeof(caMonth));
	memset(caDay, 0, sizeof(caDay));

	memcpy(caYear, pcaDate, 4);
	memcpy(caMonth, pcaDate + 4, 2);
	memcpy(caDay, pcaDate + 6, 2);

    iYear = atoi(caYear);
	iMonth = atoi(caMonth);
	iDay  = atoi(caDay);

    if (iMonth > 12 || iMonth < 1 || iYear < 1 || iDay < 1 || 
        (iDay > (basIsLeapYear(caYear) ? iaMonthDaysOfMapLeapYear[iMonth] : iaMonthDaysOfMapNoLeapYear[iMonth])))
    {
        return BAS_FALSE;
    }

	return BAS_TRUE;
}

/*****************************************************************************/
/* FUNC:   char *basGetFormatTimestamp(void)                                 */
/* PARAMS: ��                                                                */
/* RETURN: ��ʽYYYY-MM-DD.HH.MM.SS.mmmmm                                     */
/* DESC:   ȡ��ָ����ʽ��ʱ���                                              */
/*****************************************************************************/
BAS_EXPORT char *basGetFormatTimestamp(void)
{
	static char caTimestamp[25 + 1];
	char	caBuf[30];

	memset(caTimestamp, 0, sizeof(caTimestamp));
	memset(caBuf, 0, sizeof(caBuf));

	memcpy(caTimestamp, basDatePattern(basGetSysDate(), "YYYY-MM-DD"), 10);
	memcpy(caBuf, basStringPattern(basGetSysTimestamp(), "##:##:##:#####"), 14);
	sprintf(caTimestamp+10, " %14.14s", caBuf);
	basRTrim(caTimestamp, sizeof(caTimestamp) - 1);

	return caTimestamp;
}
/*****************************************************************************/
/* FUNC:   char *basGetFormatTimestamp(void)                                 */
/* PARAMS: ��                                                                */
/* RETURN: ��ʽYYYY-MM-DD.HH.MM.SS.mmmmm                                     */
/* DESC:   ȡ��ָ����ʽ��ʱ���                                              */
/*****************************************************************************/
BAS_EXPORT char *basGetDateTimeFromFormatTimestamp(char *pTimestamp, char *pDateTime)
{

       memcpy(pDateTime, pTimestamp, 4);/*YYYY*/
       memcpy(pDateTime+4, pTimestamp+5, 2);/*MM*/
       memcpy(pDateTime+6, pTimestamp +8, 2);/*DD*/
       memcpy(pDateTime+8, pTimestamp +11, 2);/*HH*/
       memcpy(pDateTime+10, pTimestamp +14, 2);/*MM*/
       memcpy(pDateTime+12, pTimestamp +17, 2);/*SS*/
	   
	return pDateTime;
}

/*****************************************************************************/
/* FUNC:   time_t basMakeTime(char *caTime)                                  */
/* PARAMS: caTime - ��ǰʱ��, ��ʽMMDDHHMMSS/YYMMDDHHMMSS/YYYYMMDDHHMMSS     */
/* RETURN: timeֵ                                                            */
/* DESC:   �����ַ���ʱ�����ɱ�׼ʱ��                                        */
/*****************************************************************************/
BAS_EXPORT time_t basMakeTime(const char *pcaTime)
{
    int     iLen;
    struct  tm  tmTime, *ptmNow;
    time_t  tTime;


    memset(&tmTime, '\0', sizeof(tmTime));
    iLen = strlen(pcaTime);
    switch (iLen)
    {
    case 10:
        sscanf(pcaTime, "%02d%02d%02d%02d%02d",
               &tmTime.tm_mon, &tmTime.tm_mday,
               &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);
        tTime = time(NULL);
        ptmNow = localtime(&tTime);
        tmTime.tm_year = ptmNow->tm_year;
        break;
    case 12:
        sscanf(pcaTime, "%02d%02d%02d%02d%02d%02d",
               &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday,
               &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);
        tmTime.tm_year = 2000 + tmTime.tm_year - 1900;
        break;
    case 14:
    default:
        sscanf(pcaTime, "%04d%02d%02d%02d%02d%02d",
               &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday,
               &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);
        tmTime.tm_year = tmTime.tm_year - 1900;
        break;
    }
    return mktime(&tmTime);
}

/*****************************************************************************/
/* FUNC:   char* basDiffTime(const char *pcaTime1,                           */
/*                            const char *pcaTime2)                          */
/* PARAMS: caTime1 - ʱ��1, ��ʽMMDDHHMMSS/YYMMDDHHMMSS/YYYYMMDDHHMMSS       */
/* PARAMS: caTime2 - ʱ��2, ��ʽMMDDHHMMSS/YYMMDDHHMMSS/YYYYMMDDHHMMSS       */
/* RETURN: ����ʱ��ֵ����ʽHH:MM:SS                                        */
/* DESC:   �����ַ���ʱ������ֵ                                            */
/*****************************************************************************/
BAS_EXPORT char* basDiffTime(const char *pcaTime1, const char *pcaTime2)
{
    static  char    caBuf[100];
    time_t  tTime1, tTime2;
    long    lHour=0, lMin=0, lSec=0;

    tTime1 = basMakeTime(pcaTime1);
    tTime2 = basMakeTime(pcaTime2);

    if (tTime1 < tTime2)
    {
        time_t  tTimeTmp = tTime1;
        tTime1 = tTime2;
        tTime2 = tTimeTmp;
    }

    memset(caBuf, '\0', sizeof(caBuf));
    lSec = difftime(tTime1, tTime2);
    lMin = lSec / 60;
    lSec = lSec % 60;
    lHour = lMin / 60;
    lMin = lMin % 60;

    sprintf(caBuf, "%02ld:%02ld:%02ld", lHour, lMin, lSec);
    return caBuf;
}

/*****************************************************************************/
/* FUNC:   char* basDiffTime(const char *pcaTime1,                           */
/*                            const char *pcaTime2)                          */
/* PARAMS: caTime1 - ʱ��1, ��ʽMMDDHHMMSS/YYMMDDHHMMSS/YYYYMMDDHHMMSS       */
/* PARAMS: caTime2 - ʱ��2, ��ʽMMDDHHMMSS/YYMMDDHHMMSS/YYYYMMDDHHMMSS       */
/* RETURN: ����ʱ��ֵ����ʽHH:MM:SS                                        */
/* DESC:   �����ַ���ʱ������ֵ                                            */
/*****************************************************************************/
BAS_EXPORT int basGetWeekDay(int iYear, int iMonth, int iDay)
{
	int     iWeek;
    int     iWeekDay;		

	
   if (iMonth == 1 || iMonth == 2)
   {
      iMonth += 12;
      iYear--;
   }

   iWeek = (iDay + 2 * iMonth + 3 * (iMonth + 1) / 5+ iYear +iYear/ 4 - iYear / 100 + iYear / 400) % 7;

   switch(iWeek)
   {
      case 0:
          iWeekDay = 1;
          break;
      case 1:
          iWeekDay = 2;
          break;
      case 2:
          iWeekDay = 3;
          break;
      case 3:
          iWeekDay = 4;
          break;
      case 4:
          iWeekDay = 5;
          break;
      case 5:
          iWeekDay = 6;
          break;
      case 6:
          iWeekDay = 0;
          break;
   }

   return iWeekDay;
}

BAS_EXPORT int basGetWeekDayByDate(const char *pcaDate)
{
    char caYear[4 + 1], caMonth[2 + 1], caDay[2 + 1];
    
    memset(caYear, 0, sizeof(caYear));
    memcpy(caYear, pcaDate, 4);
    memset(caMonth,0, sizeof(caMonth));
    memcpy(caMonth, pcaDate + 4, 2);
    memset(caDay, 0, sizeof(caDay));
    memcpy(caDay, pcaDate + 4 + 2, 2);
    
    return basGetWeekDay(atoi(caYear), atoi(caMonth), atoi(caDay));
}
