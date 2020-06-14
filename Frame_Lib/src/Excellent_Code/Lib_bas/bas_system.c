/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_system.c                                                */
/* DESCRIPTIONS: Unix系统数据接口                                            */
/* AUTHOR      : Robert Peng                                                 */
/* CREATE DATE : 2008-06-23                                                  */
/*****************************************************************************/

#include <unistd.h>
#include "bas/bas_global.h"

static const char *mpcaId = "$Id: bas_system.c,v 1.2 2012/02/08 09:24:22 cardv5c Exp $";

/*****************************************************************************/
/* FUNC:   char *basGetUserId(void);                                         */
/* PARAMS: 无                                                                */
/* RETURN: 用户名                                                            */
/* DESC:   取得UNIX日期                                                      */
/*****************************************************************************/
BAS_EXPORT char *basGetUserId(void)
{
	if (getenv("SYS_USER_ID") == NULL)
	{
		BAS_LOG(BAS_ERR300003_ERRORENV, 0, 0, "basGetUserId:环境变量SYS_USER_ID");
		return "";
	}
	return getenv("SYS_USER_ID");
}

/*****************************************************************************/
/* FUNC:   char *basGetUserId(void);                                         */
/* PARAMS: 无                                                                */
/* RETURN: 用户名                                                            */
/* DESC:   取得UNIX日期                                                      */
/*****************************************************************************/
BAS_EXPORT char *basGetHostName(void)
{
	static char caHostName[512];

	memset(caHostName, 0, sizeof(caHostName));
	if (gethostname(caHostName, sizeof(caHostName)))
	{
		BAS_LOG(BAS_ERR300029_ERRORHOSTNAME, 0, errno, "basGetHostName:gethostname调用错误");
	}

	return caHostName;
}

/*****************************************************************************/
/* FUNC:   char *basGetGlblSrlNo(void);                                         */
/* PARAMS: 无                                                                */
/* RETURN: 用户名                                                            */
/* DESC:   获取全局流水号                                                    */
/*****************************************************************************/
BAS_EXPORT char *basGetGlblSrlNo(void)
{
    /*全局流水号*/
    char caGlblSrlNo[28 + 1];
    memset(caGlblSrlNo, 0, sizeof(caGlblSrlNo));
    //日期（8位）+应用系统编码 （6位，3位应用代码+3位子应用编码）+集群编码（2位）+流水序号（12位）
    sprintf(caGlblSrlNo, "%8.8s056000%2.2s%12.12s", basGetSysDate(), getenv("MACHAPP"), basGetSysTimestamp());

	return caGlblSrlNo;
}
