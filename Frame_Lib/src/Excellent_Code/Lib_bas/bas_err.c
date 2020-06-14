/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_err.c                                                   */
/* DESCRIPTIONS: 错误代码和描述                                              */
/* AUTHOR      : Robert Peng                                                 */
/* CREATE DATE : 2008-06-23                                                  */
/*****************************************************************************/

#include "bas/bas_const.h"
#include "bas/bas_ext.h"
#include "bas/bas_errcom.h"

static const char *mpcaId = "$Id: bas_err.c,v 1.2 2012/02/08 09:24:21 cardv5c Exp $";


/* 分配全局变量 gtGlobal */
BAS_EXPORT T_Global     gtGlobal;

/* 此数组必须按错误代码排序 */
BAS_EXPORT T_ErrDesc gtaErrDesc[] =
{
	{ BAS_LOGINFO,                    "信息" },

	{ BAS_LOGFATAL,                   "致命错误" },

	{ BAS_LOGERROR,                   "错误" },

	/* 错误(ERROR) */
	{ BAS_ERR300002_ERRORTIME,        "时间计算错误" },
	{ BAS_ERR300003_ERRORENV,         "缺少环境变量" },
	{ BAS_ERR300004_ERROROPENFILE,    "打开文件错" },
	{ BAS_ERR300005_ERRORWRITEFILE,   "写文件错" },
	{ BAS_ERR300006_ERRORREADFILE,    "读文件错" },
	{ BAS_ERR300007_ERRORDB,          "数据库错误" },
	{ BAS_ERR300008_ERRORFUN,         "功能错误" },
	{ BAS_ERR300009_ERROROVERLIMIT,   "超出系统限制" },
	{ BAS_ERR300010_ERRORNOTFOUND,    "没有找到指定记录" },
	{ BAS_ERR300011_ERRORARG,         "参数错误" },
	{ BAS_ERR300012_ERRORMEMOUT,      "分配内存失败" },
	{ BAS_ERR300013_ERRORFCNTL,       "文件控制fcntl错误" },
	{ BAS_ERR300014_ERRORFSTAT,       "文件状态fstat错误" },
	{ BAS_ERR300015_ERRORFRENAME,     "文件改名错误" },
	{ BAS_ERR300029_ERRORHOSTNAME,    "取主机名错误" },
	{ BAS_ERR300101_ERRORSYSPRM,      "系统参数表错误" },
	{ BAS_ERR300102_ERRORNEXTSEQ,     "读取数据库下一序号失败" },
	{ BAS_ERR300103_ERRORBATCHNO,     "无效的批次号" },
	{ BAS_ERR300104_ERRORCATINFO,     "查找分类信息失败" },
	{ BAS_ERR300105_ERRORDATAFILE,    "数据文件格式错误" },

	{ BAS_ERR300500_CARDNOTEXT,       "卡信息不存在" },
	{ BAS_ERR300501_CUSTNOTEXT,       "客户信息不存在" },
	{ BAS_ERR300502_CARDSTAT,         "卡状态错误" },
	{ BAS_ERR300503_BSCCUSTNOTEXT,    "主卡客户信息不存在" },
	{ BAS_ERR300504_ACTIONTYPE,       "错误的操作代码" },
	{ BAS_ERR300505_BSCARDNOTEXT,     "主卡不存在" },
	{ BAS_ERR300506_PRDGRPNOTEXT,     "产品组不存在" },
	{ BAS_ERR300507_INDACCR,          "个人帐户表信息不存在" },
	{ BAS_ERR300508_FINTBLR,          "财务账号表信息不存在" },
	{ BAS_ERR300509_COPMSTR,          "公司合作表信息不存在" },
	{ BAS_ERR300510_PRODUCTNOTEXT,    "产品信息不存在" },
	{ BAS_ERR300511_POSCNTRYERR,      "受理地区错误" },
	{ BAS_ERR300512_COCARDFORBID,     "公司母卡不允许交易" },
	{ BAS_ERR300513_PRDFRQR,          "授权频率表读错" },
	{ BAS_ERR300514_PRDFRQW,          "授权频率表写错" },
	{ BAS_ERR300515_CURRATE,          "汇率错误" },
	{ BAS_ERR300516_PRDUSGR,          "产品授权参数表读取错误" },
	{ BAS_ERR300517_PRDCTRR,          "国家代码监控表读取错误" },
	{ BAS_ERR300518_PRDCRDR,          "产品状态指示表读取错误" },
	{ BAS_ERR300519_PINERROR,         "密码错误" },
	{ BAS_ERR300520_MERCHANT,         "无此商户信息" },
	{ BAS_ERR300521_MERCHSTU,         "商户状态错误" },
	{ BAS_ERR300522_MERCHCAU,         "商户警告错误" },
	{ BAS_ERR300523_RETCHQFREQ,       "允许的累计退票笔数超限" },

	{ BAS_LOGWARNING,                 "警告" },

	{ BAS_LOGDEBUG,                   "调试" },

	/* 结束记录 */
	{ 999999, "" }
};

/*****************************************************************************/
/* FUNC:   char *basGetErrDesc(int iErrCode)                                 */
/* PARAMS: iErrCode - 错误代码                                               */
/* RETURN: 错误信息                                                          */
/* DESC:   对分法查找错误代码                                                */
/*****************************************************************************/
BAS_EXPORT char *basGetErrDesc(int iErrCode)
{
	int n = 0;                                        /* 小的下标 */
	int m = sizeof(gtaErrDesc) / sizeof(T_ErrDesc);   /* 大的下标 */
	int p;                                            /* 中点下标 */

	while (1)
	{
		p = (n + m) / 2;
		if (gtaErrDesc[p].iErrNo == iErrCode)
		{
			return gtaErrDesc[p].pcaErrDesc;
		}
		if (gtaErrDesc[n].iErrNo == iErrCode)
		{
			return gtaErrDesc[n].pcaErrDesc;
		}
		if (gtaErrDesc[m].iErrNo == iErrCode)
		{
			return gtaErrDesc[m].pcaErrDesc;
		}
		if (iErrCode > gtaErrDesc[p].iErrNo)
		{
			n = p;
		}
		else
		{
			m = p;
		}
		if (m - n <= 1)
		{
			return "未知的错误码";
		}
	}
}
