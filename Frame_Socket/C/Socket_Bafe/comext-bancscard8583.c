#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Beai3Log.h>

#include "com-extension.h"

#define HEADERLEN (4)
#define REFIDLEN  (30)
#define MSGMAX    (262144)
#define SYSNAME   "BANCSC8583"

int GetMessageLength(char *message, int length)
{
	int  len;
	char fhh[32];
	char wwqdlsh[32];
	char refid[35 + 1];
	int  number;
	IMHEADER imheader;

	if(!message)
		return -1;

	if(length < HEADERLEN)
		return 0;

	memcpy(&len, message, 4);
/*
	if(len <= length)
	{
		memcpy(refid, message + HEADERLEN, REFIDLEN);

		PDEBUG("%s: refid = [%s]", SYSNAME, refid);
		if(RetrieveTransaction(SYSNAME, refid, &number, &imheader) < 0)
		{
			PERROR("%s: RetrieveTransaction() error.", SYSNAME);
		}
		else
			g_lSessionID = imheader.SessionID;
	}
*/
	return len >= 0 && len <= MSGMAX - HEADERLEN ? len + HEADERLEN: -1;
}

static char refid8583[REFIDLEN + 1];

int GetReferenceID(DictHandle handle, char *refid, int size)
{
	long applicationID;
	char idtmp[64];
	char buftmp[64];
	int iRet;
	
	/*第7域JYCSSJ，第11域XTGZH */
	if((iRet = GetDictString(handle, "JYCSSJ", idtmp, sizeof(idtmp))) < 0)
	{
	/*	applicationID = GetApplicationID();
		snprintf(refid, size, "%0*ld", REFIDLEN, applicationID);
		strncpy(refid8583, refid, sizeof(refid8583) - 1);
		refid8583[sizeof(refid8583) - 1] = '\0';
	*/
		PERROR("获取[JYCSSJ]失败!");
		return -1;
	}else
	{
		memcpy(buftmp, idtmp, iRet);
		buftmp[iRet]=0;
		if((iRet = GetDictString(handle, "XTGZH", idtmp, sizeof(idtmp))) < 0)
		{
			PERROR("获取[XTGZH]失败!");
			return -1;
		}
		idtmp[iRet]=0;
		sprintf(refid, "%010s%06s", buftmp, idtmp);
	}
	/* memcpy(refid, idtmp+2, 32);
	refid[32] = '\0';
	*/
	PINFO("%s: refid = [%s]", SYSNAME, refid);

	return 0;
}

int OutputMessageProcess(char **message, int length)
{
	int ilen;
	ilen = length-4;
	memcpy(*message, &ilen, sizeof(int));
	PDEBUG("%02X %02X %02X %02X", (*message)[0], (*message)[1], (*message)[2], (*message)[3]);
	return length;
}

