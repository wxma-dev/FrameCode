/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_err.c                                                   */
/* DESCRIPTIONS: ������������                                              */
/* AUTHOR      : Robert Peng                                                 */
/* CREATE DATE : 2008-06-23                                                  */
/*****************************************************************************/

#include "bas/bas_const.h"
#include "bas/bas_ext.h"
#include "bas/bas_errcom.h"

static const char *mpcaId = "$Id: bas_err.c,v 1.2 2012/02/08 09:24:21 cardv5c Exp $";


/* ����ȫ�ֱ��� gtGlobal */
BAS_EXPORT T_Global     gtGlobal;

/* ��������밴����������� */
BAS_EXPORT T_ErrDesc gtaErrDesc[] =
{
	{ BAS_LOGINFO,                    "��Ϣ" },

	{ BAS_LOGFATAL,                   "��������" },

	{ BAS_LOGERROR,                   "����" },

	/* ����(ERROR) */
	{ BAS_ERR300002_ERRORTIME,        "ʱ��������" },
	{ BAS_ERR300003_ERRORENV,         "ȱ�ٻ�������" },
	{ BAS_ERR300004_ERROROPENFILE,    "���ļ���" },
	{ BAS_ERR300005_ERRORWRITEFILE,   "д�ļ���" },
	{ BAS_ERR300006_ERRORREADFILE,    "���ļ���" },
	{ BAS_ERR300007_ERRORDB,          "���ݿ����" },
	{ BAS_ERR300008_ERRORFUN,         "���ܴ���" },
	{ BAS_ERR300009_ERROROVERLIMIT,   "����ϵͳ����" },
	{ BAS_ERR300010_ERRORNOTFOUND,    "û���ҵ�ָ����¼" },
	{ BAS_ERR300011_ERRORARG,         "��������" },
	{ BAS_ERR300012_ERRORMEMOUT,      "�����ڴ�ʧ��" },
	{ BAS_ERR300013_ERRORFCNTL,       "�ļ�����fcntl����" },
	{ BAS_ERR300014_ERRORFSTAT,       "�ļ�״̬fstat����" },
	{ BAS_ERR300015_ERRORFRENAME,     "�ļ���������" },
	{ BAS_ERR300029_ERRORHOSTNAME,    "ȡ����������" },
	{ BAS_ERR300101_ERRORSYSPRM,      "ϵͳ���������" },
	{ BAS_ERR300102_ERRORNEXTSEQ,     "��ȡ���ݿ���һ���ʧ��" },
	{ BAS_ERR300103_ERRORBATCHNO,     "��Ч�����κ�" },
	{ BAS_ERR300104_ERRORCATINFO,     "���ҷ�����Ϣʧ��" },
	{ BAS_ERR300105_ERRORDATAFILE,    "�����ļ���ʽ����" },

	{ BAS_ERR300500_CARDNOTEXT,       "����Ϣ������" },
	{ BAS_ERR300501_CUSTNOTEXT,       "�ͻ���Ϣ������" },
	{ BAS_ERR300502_CARDSTAT,         "��״̬����" },
	{ BAS_ERR300503_BSCCUSTNOTEXT,    "�����ͻ���Ϣ������" },
	{ BAS_ERR300504_ACTIONTYPE,       "����Ĳ�������" },
	{ BAS_ERR300505_BSCARDNOTEXT,     "����������" },
	{ BAS_ERR300506_PRDGRPNOTEXT,     "��Ʒ�鲻����" },
	{ BAS_ERR300507_INDACCR,          "�����ʻ�����Ϣ������" },
	{ BAS_ERR300508_FINTBLR,          "�����˺ű���Ϣ������" },
	{ BAS_ERR300509_COPMSTR,          "��˾��������Ϣ������" },
	{ BAS_ERR300510_PRODUCTNOTEXT,    "��Ʒ��Ϣ������" },
	{ BAS_ERR300511_POSCNTRYERR,      "�����������" },
	{ BAS_ERR300512_COCARDFORBID,     "��˾ĸ����������" },
	{ BAS_ERR300513_PRDFRQR,          "��ȨƵ�ʱ����" },
	{ BAS_ERR300514_PRDFRQW,          "��ȨƵ�ʱ�д��" },
	{ BAS_ERR300515_CURRATE,          "���ʴ���" },
	{ BAS_ERR300516_PRDUSGR,          "��Ʒ��Ȩ�������ȡ����" },
	{ BAS_ERR300517_PRDCTRR,          "���Ҵ����ر��ȡ����" },
	{ BAS_ERR300518_PRDCRDR,          "��Ʒ״ָ̬ʾ���ȡ����" },
	{ BAS_ERR300519_PINERROR,         "�������" },
	{ BAS_ERR300520_MERCHANT,         "�޴��̻���Ϣ" },
	{ BAS_ERR300521_MERCHSTU,         "�̻�״̬����" },
	{ BAS_ERR300522_MERCHCAU,         "�̻��������" },
	{ BAS_ERR300523_RETCHQFREQ,       "������ۼ���Ʊ��������" },

	{ BAS_LOGWARNING,                 "����" },

	{ BAS_LOGDEBUG,                   "����" },

	/* ������¼ */
	{ 999999, "" }
};

/*****************************************************************************/
/* FUNC:   char *basGetErrDesc(int iErrCode)                                 */
/* PARAMS: iErrCode - �������                                               */
/* RETURN: ������Ϣ                                                          */
/* DESC:   �Էַ����Ҵ������                                                */
/*****************************************************************************/
BAS_EXPORT char *basGetErrDesc(int iErrCode)
{
	int n = 0;                                        /* С���±� */
	int m = sizeof(gtaErrDesc) / sizeof(T_ErrDesc);   /* ����±� */
	int p;                                            /* �е��±� */

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
			return "δ֪�Ĵ�����";
		}
	}
}
