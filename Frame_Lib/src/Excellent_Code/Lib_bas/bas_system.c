/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_system.c                                                */
/* DESCRIPTIONS: Unixϵͳ���ݽӿ�                                            */
/* AUTHOR      : Robert Peng                                                 */
/* CREATE DATE : 2008-06-23                                                  */
/*****************************************************************************/

#include <unistd.h>
#include "bas/bas_global.h"

static const char *mpcaId = "$Id: bas_system.c,v 1.2 2012/02/08 09:24:22 cardv5c Exp $";

/*****************************************************************************/
/* FUNC:   char *basGetUserId(void);                                         */
/* PARAMS: ��                                                                */
/* RETURN: �û���                                                            */
/* DESC:   ȡ��UNIX����                                                      */
/*****************************************************************************/
BAS_EXPORT char *basGetUserId(void)
{
	if (getenv("SYS_USER_ID") == NULL)
	{
		BAS_LOG(BAS_ERR300003_ERRORENV, 0, 0, "basGetUserId:��������SYS_USER_ID");
		return "";
	}
	return getenv("SYS_USER_ID");
}

/*****************************************************************************/
/* FUNC:   char *basGetUserId(void);                                         */
/* PARAMS: ��                                                                */
/* RETURN: �û���                                                            */
/* DESC:   ȡ��UNIX����                                                      */
/*****************************************************************************/
BAS_EXPORT char *basGetHostName(void)
{
	static char caHostName[512];

	memset(caHostName, 0, sizeof(caHostName));
	if (gethostname(caHostName, sizeof(caHostName)))
	{
		BAS_LOG(BAS_ERR300029_ERRORHOSTNAME, 0, errno, "basGetHostName:gethostname���ô���");
	}

	return caHostName;
}

/*****************************************************************************/
/* FUNC:   char *basGetGlblSrlNo(void);                                         */
/* PARAMS: ��                                                                */
/* RETURN: �û���                                                            */
/* DESC:   ��ȡȫ����ˮ��                                                    */
/*****************************************************************************/
BAS_EXPORT char *basGetGlblSrlNo(void)
{
    /*ȫ����ˮ��*/
    char caGlblSrlNo[28 + 1];
    memset(caGlblSrlNo, 0, sizeof(caGlblSrlNo));
    //���ڣ�8λ��+Ӧ��ϵͳ���� ��6λ��3λӦ�ô���+3λ��Ӧ�ñ��룩+��Ⱥ���루2λ��+��ˮ��ţ�12λ��
    sprintf(caGlblSrlNo, "%8.8s056000%2.2s%12.12s", basGetSysDate(), getenv("MACHAPP"), basGetSysTimestamp());

	return caGlblSrlNo;
}
