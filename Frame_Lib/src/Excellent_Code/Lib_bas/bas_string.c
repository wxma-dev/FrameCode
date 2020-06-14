/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_string.c                                                */
/* DESCRIPTIONS: �ַ���������������                                          */
/* AUTHOR      : Robert Peng                                                 */
/* CREATE DATE : 2008-06-23                                                  */
/*****************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>
#include <regex.h>

#ifdef __IBMC__
#if (__xlC__ >= 0x0600) /* VAC version 6 and above */
/* get rid of "rtld: 0712-001 Symbol __ceil was referenced" error */
#ifdef floor
#undef floor
#endif
#ifdef ceil
#undef ceil
#endif
#endif /* __xlC__ >= 0x0600 */
#endif /* __IBMC__ */

#include "bas/bas_global.h"

#define BAS_CMP_DELTA 0.000001

static const char *mpcaId = "$Id: bas_string.c,v 1.6 2013/04/07 05:19:29 lus Exp $";
//R20170324000008 -���ÿ�����ϵͳ�����޸����� �����޸�cfg����
#define MAXLINEOFCFGFILE 1000
typedef struct pled{
  char code[20];
  char tranfrom[9];
  char tranto[9];
} tppled;

/*****************************************************************************/
/* FUNC:   void basDbl2Char(double dValue, int iFix, int iFloat,             */
/*                          char *pcaBuf)                                    */
/* PARAMS: dValue - �����ֵ (I)                                             */
/*         iFix   - С����ǰ������λ�� (I)                                   */
/*         iFloat - С����λ�� (I)                                           */
/*         pcaBuf - ת��������ַ��� (O)                                     */
/* RETURN: ת��������ַ���                                                  */
/* DESC:   ����תΪ�ַ���                                                    */
/*****************************************************************************/
BAS_EXPORT char *basDbl2Char(double dValue, int iFix, int iFloat, char *pcaBuf)
{
    sprintf(pcaBuf, "%0*.0f", iFix + iFloat, dValue * pow(10, iFloat));

    return pcaBuf;
}

/*****************************************************************************/
/* FUNC:   double basRound(double dAmount, int iPoint)                       */
/* PARAMS: dAmount - ��ֵ (I)                                                */
/*         iPoint  - ����С��λ�� (I)                                        */
/* RETURN: �����ֵ                                                          */
/* DESC:   ��������                                                          */
/*****************************************************************************/
BAS_EXPORT double basRound(double dAmount, int iPoint)
{
    return floor(dAmount * pow(10, iPoint) + 0.5001) / pow(10, iPoint);  /* ���ڴ��ڸ�������Ҫ���0.0001 */
}

/*****************************************************************************/
/* FUNC:   double basFloor(double dAmount)                                   */
/* PARAMS: dAmount - ��ֵ (I)                                                */
/* RETURN: �����ֵ                                                          */
/* DESC:   ȡ��                                                              */
/*****************************************************************************/
BAS_EXPORT double basFloor(double dAmount)
{
    if (dAmount < 0)
    {
        return -floor(-dAmount);
    }
    else
    {
        return floor(dAmount);
    }
}

/*****************************************************************************/
/* FUNC:   double basFloor2(double dAmount, int iPoint)                      */
/* PARAMS: dAmount - ��ֵ (I)                                                */
/*         iPoint  - ����С��λ�� (I)                                        */
/* RETURN: �����ֵ                                                          */
/* DESC:   ȡ��                                                              */
/*****************************************************************************/
BAS_EXPORT double basFloor2(double dAmount, int iPoint)
{
    if (dAmount < 0)
    {
        return -floor(-dAmount * pow(10, iPoint) + 0.0001) / pow(10, iPoint);
    }
    else
    {
        return floor(dAmount * pow(10, iPoint)  + 0.0001) / pow(10, iPoint);
    }
}

/*****************************************************************************/
/* FUNC:   double basCeil(double dAmount)                                    */
/* PARAMS: dAmount - ��ֵ (I)                                                */
/* RETURN: �����ֵ                                                          */
/* DESC:   ȡ��                                                              */
/*****************************************************************************/
BAS_EXPORT double basCeil(double dAmount)
{
    if (dAmount < 0)
    {
        return -ceil(-dAmount);
    }
    else
    {
        return ceil(dAmount);
    }
}

/*****************************************************************************/
/* FUNC:   double basCeil2(double dAmount, int iPoint)                      */
/* PARAMS: dAmount - ��ֵ (I)                                                */
/*         iPoint  - ����С��λ�� (I)                                        */
/* RETURN: �����ֵ                                                          */
/* DESC:   ȡ��                                                              */
/*****************************************************************************/
BAS_EXPORT double basCeil2(double dAmount, int iPoint)
{
    if (dAmount < 0)
    {
        return -ceil(-dAmount * pow(10, iPoint)) / pow(10, iPoint);
    }
    else
    {
        return ceil(dAmount * pow(10, iPoint)) / pow(10, iPoint);
    }
}

/*****************************************************************************/
/* FUNC:   char *basRSpace(char *pcaString, int iLen)                        */
/* PARAMS: pcaString - ת���� (I/O)                                          */
/*         iLen      - ���� (I)                                              */
/* RETURN: �����                                                            */
/* DESC:   �Ҳ��ո�                                                          */
/*****************************************************************************/
BAS_EXPORT char *basRSpace(char *pcaString, int iLen)
{
    char caBuffer[1024];

    sprintf(caBuffer, "%-*s", iLen, pcaString);
    strncpy(pcaString, caBuffer, iLen);

    return pcaString;
}

/*****************************************************************************/
/* FUNC:   int basIsNumeric(const char *psBuf)                               */
/* PARAMS: psBuf - ���봮 (I)                                                */
/* RETURN: 0 - ����                                                          */
/*         1 - ��                                                            */
/* DESC:   �ж��Ƿ�Ϊȫ���ִ�                                                */
/*****************************************************************************/
BAS_EXPORT int basIsNumeric(const char *psBuf)
{
    int i;

    for (i = 0; i < strlen(psBuf); i++)
    {
        if ((psBuf[i] < '0') || (psBuf[i] > '9'))
        {
            return 0;
        }
    }
    return 1;
}

/*****************************************************************************/
/* FUNC:   int basIsAlphabet(const char *psBuf)                              */
/* PARAMS: psBuf - ���봮 (I)                                                */
/* RETURN: 0 - ����                                                          */
/*         1 - ��                                                            */
/* DESC:   �ж��Ƿ�Ϊȫ��ĸ��                                                */
/*****************************************************************************/
BAS_EXPORT int basIsAlphabet(const char *psBuf)
{
    int i;

    for (i = 0; i < strlen(psBuf); i++)
    {
        if ((psBuf[i] < 'A') ||
            ((psBuf[i] > 'Z') && (psBuf[i] < 'a')) ||
            (psBuf[i] > 'z'))
        {
            return 0;
        }
    }
    return 1;
}

/*****************************************************************************/
/* FUNC:   int basIsSpace(const char *pcaBuf, int iLen)                      */
/* PARAMS: psBuf - ���봮 (I)                                                */
/*         iLen  - ����                                                      */
/* RETURN: 0 - ����                                                          */
/*         1 - ��                                                            */
/* DESC:   �Ƿ�ȫ�ո�                                                        */
/*****************************************************************************/
BAS_EXPORT int basIsSpace(const char *pcaBuf, int iLen)
{
    int i;

	if (strlen(pcaBuf) == 0)
	{
		return 1;
	}
	else
	{
		if (strlen(pcaBuf) < iLen )
		{
			iLen = strlen(pcaBuf);
		}
	}

    for (i = 0; i < iLen; i++)
    {
        if (pcaBuf[i] != ' ')
        {
            return 0;
        }
    }
    return 1;
}

/*****************************************************************************/
/* FUNC:   int basIsSpaceOrY(const char *pcaBuf, int iLen)                   */
/* PARAMS: psBuf - ���봮 (I)                                                */
/*         iLen  - ����                                                      */
/* RETURN: 0 - ����                                                          */
/*         1 - ��                                                            */
/* DESC:   �Ƿ�ȫ�ǿո�͡�Y��                                                 */
/*****************************************************************************/
BAS_EXPORT int basIsSpaceOrY(const char *pcaBuf, int iLen)
{
    int i;

	if (strlen(pcaBuf) == 0)
	{
		return 1;
	}
	else
	{
		if (strlen(pcaBuf) < iLen )
		{
			iLen = strlen(pcaBuf);
		}
	}

    for (i = 0; i < iLen; i++)
    {
        if (pcaBuf[i] != ' ' &&  pcaBuf[i] != 'Y')
        {
            return 0;
        }
    }
    return 1;
}

/*****************************************************************************/
/* FUNC:   int basIsZero(const char *pcaBuf, int iLen)                       */
/* PARAMS: psBuf - ���봮 (I)                                                */
/*         iLen  - ����                                                      */
/* RETURN: 0 - ����                                                          */
/*         1 - ��                                                            */
/* DESC:   �Ƿ�ȫ��                                                          */
/*****************************************************************************/
BAS_EXPORT int basIsZero(const char *pcaBuf, int iLen)
{
    int i;

    for (i = 0; i < iLen; i++)
    {
        if (pcaBuf[i] != '0')
        {
            return 0;
        }
    }
    return 1;
}

/*****************************************************************************/
/* FUNC:   int basIsNull(const char *pcaBuf, int iLen)                       */
/* PARAMS: psBuf - ���봮 (I)                                                */
/*         iLen  - ����                                                      */
/* RETURN: 0 - ����                                                          */
/*         1 - ��                                                            */
/* DESC:   �Ƿ�ȫ���ȫ�ո�                                                  */
/*****************************************************************************/
BAS_EXPORT int basIsNull(const char *pcaBuf, int iLen)
{
    if ((basIsSpace(pcaBuf, iLen) == 1) || (basIsZero(pcaBuf, iLen) == 1))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************/
/* FUNC:   char *basRTrim(char *pcaBuf, int iMaxLen)                         */
/* PARAMS: pcaBuf  - ת���� (I/O)                                            */
/*         iMaxLen - ��󳤶� (I)                                            */
/* RETURN: ת�������                                                        */
/* DESC:   ȥ���ұ߿ո�                                                      */
/*****************************************************************************/
BAS_EXPORT char *basRTrim(char *pcaBuf, int iMaxLen)
{
    int i;

    if ((strlen(pcaBuf) <= iMaxLen) && (strlen(pcaBuf) > 0))
    {
        iMaxLen = strlen(pcaBuf);
    }

    for (i = iMaxLen - 1; i >= 0 ; i--)
    {
        if (pcaBuf[i] == ' ')
        {
            pcaBuf[i] = 0;
        }
        else
        {
            break;
        }
    }

    return pcaBuf;
}

/*****************************************************************************/
/* FUNC:   char *basLTrim(char *pcaBuf, int iMaxLen)                         */
/* PARAMS: pcaBuf  - ���봮 (I)                                              */
/*         iMaxLen - ��󳤶� (I)                                            */
/* RETURN: ת�������                                                        */
/* DESC:   ȥ����߿ո�                                                      */
/*****************************************************************************/
BAS_EXPORT char *basLTrim(char *pcaBuf, int iMaxLen)
{
    int i = 0, j = 0;
    int iFlag = 0;

    if (pcaBuf[0] != ' ')
    {
        return pcaBuf;
    }

    while ((pcaBuf[j] != '\0') && j < iMaxLen)
    {
        if (iFlag)
        {
            pcaBuf[i] = 0;
        }
        if (pcaBuf[j] == ' ' && iFlag == 0)
        {
            j++;
        }
        else
        {
            iFlag = 1;
            pcaBuf[i] = pcaBuf[j];
            i++;
            j++;
        }
    }
    pcaBuf[i] = 0;
    return pcaBuf;
}

/*****************************************************************************/
/* FUNC:   char *basRemoveSpace(char *pcaBuf, int iMaxLen)                   */
/* PARAMS: pcaBuf  - ���봮 (I)                                              */
/*         iMaxLen - ��󳤶� (I)                                            */
/* RETURN: �����                                                            */
/* DESC:   ȥ���ַ����еĿո�                                                */
/*****************************************************************************/
BAS_EXPORT char *basRemoveSpace(char *pcaBuf, int iMaxLen)
{
    int i = 0, j = 0;

    if ((strlen(pcaBuf) <= iMaxLen) && (strlen(pcaBuf) > 0))
    {
        iMaxLen = strlen(pcaBuf);
    }

    for (i = 0; i < iMaxLen; i++)
    {
        if (pcaBuf[i] == ' ')
        {
            continue;
        }
        pcaBuf[j++] = pcaBuf[i];
    }
    for (i = j; i < iMaxLen; i++)
    {
        pcaBuf[i] = 0;
    }

    return pcaBuf;
}

/*****************************************************************************/
/* FUNC:   char *basToUpper(char *pcaBuf, int iLen)                          */
/* PARAMS: psBuf - ת���� (I/O)                                              */
/*         iLen  - ����                                                      */
/* RETURN: ���ش�                                                            */
/*         1 - ��                                                            */
/* DESC:   ת��Ϊȫ��д                                                      */
/*****************************************************************************/
BAS_EXPORT char *basToUpper(char *pcaBuf, int iLen)
{
    int i;

    for (i = 0; i < iLen; i++)
    {
        if (((unsigned char)pcaBuf[i] >= 0x81) && ((unsigned char)pcaBuf[i] <= 0xfe))
        {
            /* ����Ǻ��ֵ�ǰ����ַ��������������ֲ���ת�� */
            i++;
        }
        else
        {
            pcaBuf[i] = toupper(pcaBuf[i]);
        }
    }
    return pcaBuf;
}

/*****************************************************************************/
/* FUNC:   char *basToLower(char *pcaBuf, int iLen)                          */
/* PARAMS: psBuf - ת���� (I/O)                                              */
/*         iLen  - ����                                                      */
/* RETURN: ���ش�                                                            */
/*         1 - ��                                                            */
/* DESC:   ת��ΪȫСд                                                      */
/*****************************************************************************/
BAS_EXPORT char *basToLower(char *pcaBuf, int iLen)
{
    int i;

    for (i = 0; i < iLen; i++)
    {
        if (((unsigned char)pcaBuf[i] >= 0x81) && ((unsigned char)pcaBuf[i] <= 0xfe))
        {
            /* ����Ǻ��ֵ�ǰ����ַ��������������ֲ���ת�� */
            i++;
        }
        else
        {
            pcaBuf[i] = tolower(pcaBuf[i]);
        }
    }
    return pcaBuf;
}

/*****************************************************************************/
/* FUNC:   int basDoubleCmp(double dVal1, double dVal2)                      */
/* PARAMS: dVal1 - �Ƚ�ֵ1 (I)                                               */
/*         dVal2 - �Ƚ�ֵ2 (I)                                               */
/* RETURN: 0  : dVal1 = dVal2                                                */
/*         <0 : dVal1 < dVal2                                                */
/*         >0 : dVal1 > dVal2                                                */
/* DESC:   ȥ����߿ո�                                                      */
/*****************************************************************************/
BAS_EXPORT int basDoubleCmp(double dVal1, double dVal2)
{
    if (fabs(dVal1 - dVal2) < BAS_CMP_DELTA)
    {
        return 0;
    }
    if (dVal1 > dVal2)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

/*****************************************************************************/
/* FUNC:   int basDoubleCmpByScale(const double dVal1,                       */
/*								   const double dVal2,                       */
/*								   const int iScale)                         */
/* PARAMS: dVal1 - �Ƚ�ֵ1 (I)                                               */
/*         dVal2 - �Ƚ�ֵ2 (I)                                               */
/* RETURN: 0  : dVal1 = dVal2                                                */
/*         <0 : dVal1 < dVal2                                                */
/*         >0 : dVal1 > dVal2                                                */
/* DESC:   ȥ����߿ո�                                                      */
/*****************************************************************************/
BAS_EXPORT int basDoubleCmpByScale(const double dVal1, const double dVal2, 
								   const int iScale)
{
	double ddiff = 1.00;
	int i = 0;

	for (i = 0; i < iScale; i++)
	{
		ddiff *=0.1;
	}

    if (fabs(dVal1 - dVal2) < ddiff)
    {
        return 0;
    }
    if (dVal1 > dVal2)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

/*****************************************************************************/
/* FUNC:   int basChkActBit(char *pcaActno, int iCardLen)                    */
/* PARAMS: pcaActno - �ʺ� (I/O)                                             */
/*         iCardLen -  (I)                                                   */
/* RETURN: 1  : У��λ��ȷ                                                   */
/*         0  : У��λ����                                                   */
/* DESC:   ����/���У��λ                                                   */
/*****************************************************************************/
BAS_EXPORT int basChkActBit(char *pcaActno, int iCardLen)
{
    int    i, t;
    int    iSum = 0;
    char   cBit, cOrgBit;

    for (i = 0; i < iCardLen; i++)
    {
        if ((pcaActno[i] == ' ') || (pcaActno[i] == 0))
        {
            iCardLen = i;
            break;
        }
    }

    for (i = iCardLen - 2; i >= 0; i-=2)
    {
        t = (pcaActno[i] - '0') * 2;
        iSum += t / 10 + t % 10;
        if (i - 1 >= 0)
        {
            iSum += (pcaActno[i - 1] - '0');
        }
    }

    if (iSum % 10 == 0)
    {
        cBit = '0';
    }
    else
    {
        cBit = '0' + 10 - iSum % 10;
    }

    cOrgBit = pcaActno[iCardLen - 1];
    pcaActno[iCardLen - 1] = cBit;

    if (cOrgBit == cBit)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************/
/* FUNC:   char *basStringPattern(char *pcaString, const char *pcaPattern)   */
/* PARAMS: pcaString  - ת���� (I/O)                                         */
/*         pcaPattern - ��ʽ�� (I)                                           */
/* RETURN: ���ش�                                                            */
/* DESC:   ��ģʽ��ת��                                                      */
/*****************************************************************************/
BAS_EXPORT char *basStringPattern(char *pcaString, const char *pcaPattern)
{
    char caBuf[1024];
    int i, j;
    int iCFlag = 0; /* ʹ�� ZZZ,ZZZģʽ�ı�־ */
    int iZFlag = 0;
    int iNFlag = 0; /* ������־ */

    j = 0;
    memset(caBuf, 0, sizeof(caBuf));

    for (i = 0; i < strlen(pcaPattern); i++)
    {
        if (pcaString[j] == 0)
        {
            break;
        }
        switch (pcaPattern[i])
        {
            case 'U': /* ת��д */
                caBuf[i] = toupper(pcaString[j]);
                j++;
                break;
            case 'L': /* תСд */
                caBuf[i] = tolower(pcaString[j]);
                j++;
                break;
            case '#':
                if ((pcaString[j] == ' ') && (iCFlag == 1))
                {
                    caBuf[i] = '0';
                }
                else
                {
                    caBuf[i] = pcaString[j];
                }
                j++;
                break;
            case '9'://added by tianx
            case 'Z':
                iCFlag = 1;
                if (iNFlag == 1)
                {
                    if ((caBuf[i - 1] == '-') && (pcaString[j] == '0'))
                    {
                        caBuf[i - 1] = ' ';
                        caBuf[i] = '-';
                        j++;
                        break;
                    }
                    iNFlag = 0;
                }
                if (pcaString[j] == '-')
                {
                    iNFlag = 1;
                }
                if (iZFlag == 1)
                {
                    caBuf[i] = pcaString[j];
                }
                else
                {
                    if ((pcaString[j] != '0') && (pcaString[j] != ' '))
                    {
                        iZFlag = 1;
                        caBuf[i] = pcaString[j];
                    }
                    else
                    {
                        caBuf[i] = ' ';
                    }
                }
                j++;
                break;
            case ',':
                if (iNFlag == 1)
                {
                    caBuf[i - 1] = ' ';
                    caBuf[i] = '-';
                    break;
                }
                if (iZFlag == 1)
                {
                    caBuf[i] = ',';
                }
                else
                {
                    caBuf[i] = ' ';
                }
                break;
            default:
                caBuf[i] = pcaPattern[i];
        }
    }

    strcpy(pcaString, caBuf);

    return pcaString;
}

/*****************************************************************************/
/* FUNC:   char *basFloatToSection(double dValue, int iPointLen)             */
/* PARAMS: dValue    - ��ֵ (I)                                              */
/*         iPointLen - ��ʽ�� (I)                                            */
/* RETURN: ���ش�                                                            */
/* DESC:   ���طֽڷ��ĸ�ʽ                                                  */
/*****************************************************************************/
BAS_EXPORT char *basFloatToSection(double dValue, int iPointLen)
{
    static char caBuf[100];
    char caPattern[30];
    int  i;

    strcpy(caPattern, "ZZ,ZZZ,ZZZ,ZZZ,ZZZ,ZZZ,ZZ#");
    if (iPointLen > 0)
    {
        strcat(caPattern, ".");
        for (i = 0; i < iPointLen; i++)
        {
            strcat(caPattern, "#");
        }
    }
    sprintf(caBuf, "%0*.f", 20 + iPointLen, dValue * pow(10, iPointLen));
    basStringPattern(caBuf, caPattern);

    return basLTrim(caBuf, sizeof(caBuf));
}



/*****************************************************************************/
/* FUNC:   char *basFloatToSection(double dValue, int iPointLen)             */
/* PARAMS: dValue    - ��ֵ (I)                                              */
/*         iPointLen - ��ʽ�� (I)                                            */
/* RETURN: ���ش�                                                            */
/* DESC:   �����޷ֽڷ��ĸ�ʽ                                                  */
/*****************************************************************************/
BAS_EXPORT char *basFloatToSectionNoDot(double dValue, int iPointLen)
{
    static char caBuf[100];
    char caPattern[30];
    int  i;

    strcpy(caPattern, "ZZZZZZZZZZZZZZZZZZZ#");
    if (iPointLen > 0)
    {
        strcat(caPattern, ".");
        for (i = 0; i < iPointLen; i++)
        {
            strcat(caPattern, "#");
        }
    }
    sprintf(caBuf, "%0*.f", 20 + iPointLen, dValue * pow(10, iPointLen));
    basStringPattern(caBuf, caPattern);

    return basLTrim(caBuf, sizeof(caBuf));
}

/*****************************************************************************/
/* FUNC:   char *basAddString(char *pcaBuf, int iLen, int iVal)              */
/* PARAMS: pcaBuf - ���ִ� (I/O)                                             */
/*         iLen   - ���ִ��ĳ��� (I)                                         */
/*         iVal   - ������Ŀǰֻ֧��1 (I)                                    */
/* RETURN: ���ش�                                                            */
/* DESC:   ���ִ��ӷ�����                                                    */
/*****************************************************************************/
BAS_EXPORT char *basAddString(char *pcaBuf, int iLen, int iVal)
{
    int i = 0;

    for (i = iLen - 1; i >=0; i --)
    {
        if (pcaBuf[i] < '9')
        {
            pcaBuf[i]++;
            break;
        }
        pcaBuf[i] = '0';
    }
    if (i < 0)
    {
        memset(pcaBuf, '0', iLen);
    }
    return pcaBuf;
}


/*****************************************************************************/
/* FUNC:   int basMemcmp(const char *pcaBuf1, const char *pcaBuf2, int iLen) */
/* PARAMS: pcaBuf1 - ����1 (I)                                               */
/*         pcaBuf2 - ����2 (I)                                               */
/*         iLen    - ����                                                    */
/* RETURN: 0  : pcaBuf1 = pcaBuf2                                            */
/*         -1 : pcaBuf1 < pcaBuf2                                            */
/*         1  : pcaBuf1 > pcaBuf2                                            */
/* DESC:   �����ұ߿ո���ڴ�Ƚ�                                            */
/*****************************************************************************/
BAS_EXPORT int basMemcmp(const char *pcaBuf1, const char *pcaBuf2, int iLen)
{
    int i;
    int iFlag1 = 0, iFlag2 = 0;
    for (i = 0; i < iLen; i++)
    {
        if ((iFlag1 == 0) && (pcaBuf1[i] == 0))
        {
            iFlag1 = 1;
        }
        if ((iFlag2 == 0) && (pcaBuf2[i] == 0))
        {
            iFlag2 = 1;
        }
        if (iFlag1 == 1 && iFlag2 == 1)
        {
            break;
        }
        if (iFlag1 == 1)
        {
            if (pcaBuf2[i] != ' ')
            {
                return -1;
            }
            continue;
        }
        if (iFlag2 == 1)
        {
            if (pcaBuf1[i] != ' ')
            {
                return 1;
            }
            continue;
        }
        if (pcaBuf1[i] > pcaBuf2[i])
        {
            return 1;
        }
        if (pcaBuf1[i] < pcaBuf2[i])
        {
            return -1;
        }
    }
    return 0;
}

/*****************************************************************************/
/* FUNC:   int basCountString(const char *pcaBuf, char cChar, int iLen)      */
/* PARAMS: pcaBuf  - �ַ��� (I)                                              */
/*         pcaBuf2 - �ַ� (I)                                                */
/*         iLen    - ����                                                    */
/* RETURN: cChar������                                                       */
/* DESC:   �����ַ�����ָ�����ַ�������                                      */
/*****************************************************************************/
BAS_EXPORT int basCountString(const char *pcaBuf, char cChar, int iLen)
{
    int i;
    int iCount = 0;
    for (i = 0; i < iLen; i++)
    {
        if (pcaBuf[i] == 0)
        {
            break;
        }
        if (pcaBuf[i] == cChar)
        {
            iCount++;
        }
    }

    return iCount;
}

/*****************************************************************************/
/* FUNC:   double basAtof(const char *pcaBuf, int iLen)                      */
/* PARAMS: pcaBuf  - �ַ��� (I)                                              */
/*         iLen    - ����                                                    */
/* RETURN: ת������ֵ                                                        */
/* DESC:   �ַ���ת��Ϊdouble��ֵ                                            */
/*****************************************************************************/
BAS_EXPORT double basAtof(const char *pcaBuf, int iLen)
{
    char caTmp[256];

    memset(caTmp, 0, sizeof(caTmp));
    memcpy(caTmp, pcaBuf, BAS_MIN(iLen, sizeof(caTmp)));

    return atof(caTmp);
}

/*****************************************************************************/
/* FUNC:   long basAtol(const char *pcaBuf, int iLen)                        */
/* PARAMS: pcaBuf  - �ַ��� (I)                                              */
/*         iLen    - ����                                                    */
/* RETURN: ת������ֵ                                                        */
/* DESC:   �ַ���ת��Ϊlong��ֵ                                              */
/*****************************************************************************/
BAS_EXPORT long basAtol(const char *pcaBuf, int iLen)
{
    char caTmp[256];

    memset(caTmp, 0, sizeof(caTmp));
    memcpy(caTmp, pcaBuf, BAS_MIN(iLen, sizeof(caTmp)));

    return atol(caTmp);
}

/*****************************************************************************/
/* FUNC:    basAtoiHex                                                       */
/* PARAMS:  pcaBuf  - �ַ��� (I)                                             */
/*          iLen    - ����                                                   */
/* RETURN:  ת������ֵ                                                       */
/* DESC:    �ַ���ת��Ϊlong��ֵ(֧��0x,0X��ͷ��16�����ַ���)                */
/*****************************************************************************/
BAS_EXPORT long basAtolHex(const char *pcaBuf, int iLen)
{
    char caTmp[256];
    long lConvertedValue = 0;

    memset(caTmp, 0, sizeof(caTmp));
    memcpy(caTmp, pcaBuf, BAS_MIN(iLen, sizeof(caTmp)));
   
    if ((memcmp(caTmp, "0x", 2) == 0) ||
        (memcmp(caTmp, "0X", 2) == 0)) 
    {   
        sscanf(caTmp + 2, "%lx", &lConvertedValue);
    }   
    else
    {   
        lConvertedValue = atol(caTmp);
    }    

    return lConvertedValue;
}

/*****************************************************************************/
/* FUNC:   int basCountStr(const char *pcaBuf, const char *pStr)             */
/* PARAMS: pcaBuf  - �ַ��� (I)                                              */
/*         pStr    - �ַ��� (I)                                              */
/* RETURN: pStr������                                                        */
/* DESC:   �����ַ����а���ָ���ַ���������                                  */
/*****************************************************************************/
BAS_EXPORT int basCountStr(const char *pcaBuf, const char *pStr)
{
    int     iLen1;
    int     iLen2;
    int     iCnt = 0;
    int     i;

    iLen1 = strlen(pcaBuf);
    iLen2 = strlen(pStr);

    if (iLen2 > iLen1)
        return 0;

    for (i = 0; i < iLen1; )
    {
        if(memcmp(pcaBuf+i, pStr, iLen2) == 0)
        {
            iCnt++;
            i += iLen2;
        }
        else
        {
            i++;
        }

    }
    return iCnt;
}

/*****************************************************************************/
/* FUNC:   int basLocateStr(const char *pcaBuf, const char *pStr)            */
/* PARAMS: pcaBuf  - �ַ��� (I)                                              */
/*         pStr    - �ַ��� (I)                                              */
/* RETURN: pStr���״γ���λ��                                                */
/* DESC:   �����ַ�����ָ���ַ�����λ��                                      */
/*****************************************************************************/
BAS_EXPORT int basLocateStr(const char *pcaBuf, const char *pStr)
{
    int     iLen1;
    int     iLen2;
    int     i;

    iLen1 = strlen(pcaBuf);
    iLen2 = strlen(pStr);

    if (iLen2 > iLen1)
        return -1;

    for (i = 0; i < iLen1; i++ )
    {
        if(memcmp(pcaBuf+i, pStr, iLen2) == 0)
        {
            break;
        }
    }

    if (i == iLen1)
    {
        return -1;
    }

    return i;
}

/*****************************************************************************/
/* FUNC:   int basLocateSpace(const char *pcaBuf)                            */
/* PARAMS: pcaBuf  - �ַ��� (I)                                              */
/* RETURN: �ո����TAB���״γ���λ��                                         */
/* DESC:   �����ַ������׸��ո����TAB��λ��                                 */
/*****************************************************************************/
BAS_EXPORT int basLocateSpace(const char *pcaBuf)
{
    int     iLen1;
    int     iLen2;
    int     i;

    iLen1 = strlen(pcaBuf);

    for (i = 0; i < iLen1; i++ )
    {
        if(pcaBuf[i] == ' ' || pcaBuf[i] == '\t')
        {
            break;
        }
    }

    if (i == iLen1)
    {
        return -1;
    }

    return i;
}

/*****************************************************************************/
/* FUNC:   void basInitSpaces(char *pcaBuf, int iLen)                        */
/* PARAMS: pcaBuf  - �ַ��� (I)                                              */
/*         iLen    - ����                                                    */
/* RETURN:                                                                   */
/* DESC:   ���ַ�����ʼ��Ϊ�ո�                                              */
/*****************************************************************************/
BAS_EXPORT void basInitSpaces(char *pcaBuf, int iLen)
{
    int     i;

    for (i = 0; i < iLen; i++ )
    {
        pcaBuf[i] = ' ';
    }

    return ;
}

/*****************************************************************************/
/* FUNC:   void basInitZeros(char *pcaBuf, int iLen)                         */
/* PARAMS: pcaBuf  - �ַ��� (I)                                              */
/*         iLen    - ����                                                    */
/* RETURN:                                                                   */
/* DESC:   ���ַ�����ʼ��Ϊ'0'                                               */
/*****************************************************************************/
BAS_EXPORT void basInitZeros(char *pcaBuf, int iLen)
{
    int     i;

    for (i = 0; i < iLen; i++ )
    {
        pcaBuf[i] = '0';
    }

    return ;
}

/*****************************************************************************/
/* FUNC:   void basRTrimChar(char *pcaBuf, char cStr)                        */
/* PARAMS: pcaBuf  - �ַ��� (I)                                              */
/*         cStr    - �ַ�                                                    */
/* RETURN:                                                                   */
/* DESC:   ȥ���ַ�������ĵ�һ��cStr�ַ�                                    */
/*****************************************************************************/
BAS_EXPORT void basRTrimChar(char *pcaBuf, int cStr)
{
    int     i;
    int     iLen;

    iLen = strlen(pcaBuf);

    for (i = iLen; i >= 0; i--)
    {
        if(pcaBuf[i] == cStr)
        {
            pcaBuf[i] = 0;
            break;
        }
    }

    return;
}

/*****************************************************************************/
/* FUNC:   int basIsSameIdno(const char *pcaIdno1, const char *pcaIdno2)     */
/* PARAMS: pcaIdno1  - ���֤��1 (I)                                         */
/*         pcaIdno2  - ���֤��2 (I)                                         */
/* RETURN: 0 : ��ͬ                                                          */
/*         1 : ��ͬ                                                          */
/* DESC:   �Ƚ��������֤���루֧��15λ��18λ�ıȽϣ�                        */
/*****************************************************************************/
BAS_EXPORT int basIsSameIdno(const char *pcaIdtype1, const char *pcaIdno1, const char *pcaIdtype2, const char *pcaIdno2)
{
    //char caTmp1[18 + 1], caTmp2[18 + 1];
    char caTmp1[25 + 1], caTmp2[25 + 1];

    memset(caTmp1, 0, sizeof(caTmp1));
    memset(caTmp2, 0, sizeof(caTmp2));

    memcpy(caTmp1, pcaIdno1, sizeof(caTmp1) - 1);
    memcpy(caTmp2, pcaIdno2, sizeof(caTmp2) - 1);

    basRTrim(caTmp1, sizeof(caTmp1));
    basRTrim(caTmp2, sizeof(caTmp2));

	if (pcaIdtype1[0] != pcaIdtype2[0])
	{
		return 0;
	}

	if (pcaIdtype1[0] == BAS_ID_TYPE_IDENTITY)
	{
    	if (strlen(caTmp1) == 15)
    	{
        	basIdno15To18(caTmp1);
    	}
    	if (strlen(caTmp2) == 15)
    	{
        	basIdno15To18(caTmp2);
    	}
	    /*sr1263 add by lush 20130117*/
		if (caTmp1[17]=='x') 
		{
			caTmp1[17] = toupper(caTmp1[17]);
		}
		if (caTmp2[17]=='x') 
		{
			caTmp2[17] = toupper(caTmp2[17]);
		}
	}

    if (strcmp(caTmp1, caTmp2) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************/
/* FUNC:   int basIsSameIdnoEcif(const char *pcaIdno1, const char *pcaIdno2) */
/* PARAMS: pcaIdno1  - ���֤��1 (I)                                         */
/*         pcaIdno2  - ���֤��2 (I)                                         */
/* RETURN: 0 : ��ͬ                                                          */
/*         1 : ��ͬ                                                          */
/* DESC:   �Ƚ��������֤���루֧��15λ��18λ�ıȽϣ�                        */
/*****************************************************************************/
BAS_EXPORT int basIsSameIdnoEcif(const char *pcaIdtype1, const char *pcaIdno1, const char *pcaIdtype2, const char *pcaIdno2)
{
    //char caTmp1[18 + 1], caTmp2[18 + 1];
    char caTmp1[25 + 1], caTmp2[25 + 1];
    char caType1[ 5 + 1], caType2[5 +1];

    memset(caTmp1, 0, sizeof(caTmp1));
    memset(caTmp2, 0, sizeof(caTmp2));
    memset(caType1, 0, sizeof(caType1));
    memset(caType2, 0, sizeof(caType2));

    memcpy(caTmp1, pcaIdno1, sizeof(caTmp1) - 1);
    memcpy(caTmp2, pcaIdno2, sizeof(caTmp2) - 1);

    memcpy(caType1, pcaIdtype1, sizeof(caType1) - 1);
    memcpy(caType2, pcaIdtype2, sizeof(caType2) - 1);

    basRTrim(caTmp1, sizeof(caTmp1));
    basRTrim(caTmp2, sizeof(caTmp2));

    basRTrim(caType1, sizeof(caType1));
    basRTrim(caType2, sizeof(caType2));
    
	  if (memcmp(caType1,  caType2, 5) != 0)
	  {
	  	return 0;
	  }
    
	  if ( memcmp(caType1, "10100", 5) == 0)
	  {
      	if (strlen(caTmp1) == 15)
      	{
          	basIdno15To18(caTmp1);
      	}
      	if (strlen(caTmp2) == 15)
      	{
          	basIdno15To18(caTmp2);
      	}
	      /*sr1263 add by lush 20130117*/
	  	  if (caTmp1[17]=='x') 
	  	  {
	  	  	caTmp1[17] = toupper(caTmp1[17]);
	  	  }
	  	  if (caTmp2[17]=='x') 
	  	  {
	  	  	caTmp2[17] = toupper(caTmp2[17]);
	  	  }
	  }

    if (strcmp(caTmp1, caTmp2) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*****************************************************************************/
/* FUNC:   char *basIdno18To15(char *pcaIdno)                                */
/* PARAMS: pcaIdno  - 18λ���֤�� (I/O)                                     */
/* RETURN: 15λ���֤��                                                      */
/* DESC:   ��18λ���֤��ת��Ϊ15λ                                          */
/*****************************************************************************/
BAS_EXPORT char *basIdno18To15(char *pcaIdno)
{
    char caTmp[15 + 1];

    memset(caTmp, 0, sizeof(caTmp));
    memcpy(caTmp, pcaIdno, 6);
    memcpy(caTmp + 6, pcaIdno + 8, 6);
    memcpy(caTmp + 12, pcaIdno + 14, 3);

    strcpy(pcaIdno, caTmp);

    return pcaIdno;
}

/*****************************************************************************/
/* FUNC:   char *basIdno15To18(char *pcaIdno)                                */
/* PARAMS: pcaIdno  - 15λ���֤�� (I/O)                                     */
/* RETURN: 18λ���֤��                                                      */
/* DESC:   ��15λ���֤��ת��Ϊ18λ                                          */
/*****************************************************************************/
BAS_EXPORT char *basIdno15To18(char *pcaIdno)
{
    char caTmp[18 + 1];

    memset(caTmp, 0, sizeof(caTmp));
    memcpy(caTmp, pcaIdno, 6);
    caTmp[6] = '1';
    caTmp[7] = '9';
    memcpy(caTmp + 8, pcaIdno + 6, 9);

    basIdno18GenCheckVal(caTmp);
    strcpy(pcaIdno, caTmp);

    return pcaIdno;
}

/*****************************************************************************/
/* FUNC:   char basIdno18GenCheckVal(char *pcaIdno)                          */
/* PARAMS: pcaIdno  - 18λ���֤�� (I/O)                                     */
/* RETURN: 18λ���֤��У��λ                                                */
/* DESC:   Ϊ18λ���֤��������У��λ                                        */
/*****************************************************************************/
BAS_EXPORT char basIdno18GenCheckVal(char *pcaIdno)
{
    static int miaWi[] = { 7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2, 1 };
    static char mcaVer[] = {'1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2', '\0'};
    int iSum = 0;
    int i;

    for(i = 0; i < 17; i++)
    {
        iSum += (pcaIdno[i] - '0') * miaWi[i];
    }

    pcaIdno[17] = mcaVer[iSum % 11];

    return pcaIdno[17];
}

/*****************************************************************************/
/* FUNC:   int basIdno18IsValid(const char *pcaIdno)                         */
/* PARAMS: pcaIdno  - 18λ���֤�� (I)                                       */
/* RETURN: 1 - У��ͨ�� 0 - У��ʧ��                                         */
/* DESC:   Ϊ18λ���֤������֤��Ч��                                        */
/*****************************************************************************/
BAS_EXPORT int basIdno18IsValid(const char *pcaIdno)
{
    char caTmp[18 + 1];

    strcpy(caTmp, pcaIdno);
    basIdno18GenCheckVal(caTmp);

    if (caTmp[17] == pcaIdno[17])
    {
        return BAS_TRUE;
    }
    else
    {
        return BAS_FALSE;
    }
}

/*****************************************************************************/
/* FUNC:   void basSprintf(char *pcaBuf, int iLen, const char *pcaFmt, ...)  */
/* PARAMS: pcaBuf   - ���صĴ�(�����\0) (O)                                 */
/*         iLen     - ���صĳ��� (I)                                         */
/*         pcaFmt   - ��ʽ����ʽͬprintf (I)                                 */
/*         ...      - ��β��� (I)                                           */
/* RETURN: ��                                                                */
/* DESC:   ��ʽ�������ָ����󴮳��ȣ�                                      */
/*****************************************************************************/
BAS_EXPORT void basSprintf(char *pcaBuf, int iLen, const char *pcaFmt, ...)
{
    va_list    vlVar;
    char       caTmp[2560];

    va_start(vlVar, pcaFmt);
    vsprintf(caTmp, pcaFmt, vlVar);
    va_end(vlVar);

    strncpy(pcaBuf, caTmp, iLen);
}

/*****************************************************************************/
/* FUNC:   char *basLFill(char *pcaBuf, int iLen, char cChar)                */
/* PARAMS: pcaBuf   - ת���� (I/O)                                           */
/*         iLen     - �ܳ��� (I)                                             */
/*         cChar    - �����ַ� (I)                                           */
/*         ...      - ��β��� (I)                                           */
/* RETURN: ת����Ĵ�                                                        */
/* DESC:   �����ַ�                                                        */
/*****************************************************************************/
BAS_EXPORT char *basLFill(char *pcaBuf, int iLen, char cChar)
{
    char caBuf[512];

    int iOrgLen = strlen(pcaBuf);
    if (iOrgLen >= iLen)
    {
        return pcaBuf;
    }

    memset(caBuf, 0, sizeof(caBuf));
    memset(caBuf, cChar, iLen - iOrgLen);
    strcat(caBuf, pcaBuf);
    memcpy(pcaBuf, caBuf, iLen);

    return pcaBuf;
}

/*****************************************************************************/
/* FUNC:   int basChar2FloatOfAmt(char *pcaBuf, double *dAmt)                */
/* PARAMS: pcaBuf    - ת���� (I)                                            */
/*         pcaOutBuf - ת���� (O)                                            */
/* RETURN: 0 - �ɹ�                                                          */
/*        -1 - ʧ��                                                          */
/* DESC:   �ַ������ת��Ϊ���                                              */
/*****************************************************************************/
BAS_EXPORT int basChar2FloatOfAmt(char *pcaBuf, double *dAmt)
{
    char *temp;
    int len, i,j;
    int iCnt;
    char    caStr[2000];

    memset(caStr, 0, sizeof(caStr));
    iCnt = 0;

    temp = pcaBuf;
    while ((' ' == *temp) || (',' == *temp))
    {
        temp++;
    }

    len = strlen(temp);
    if (0 == len)
    {
        *dAmt = 0;
        return 0;
    }

    while (' ' == temp[len-1] || ',' == temp[len-1] || '\n' == temp[len-1])
    {
        len--;
    }
    temp[len] = '\0';

    j = 0;
    for (i=0; i<=len; i++)
    {
        if (temp[i] == ' ' || temp[i] == ',')
            continue;

        if (temp[i] == '.')
            iCnt++;

        if (iCnt > 1)
        {
            return -1;
        }

        if ((i != len) && (temp[i] != '.') && (temp[i] < '0' || temp[i] > '9'))
        {
            return -1;
        }

        caStr[j] = temp[i];
        j++;
    }

    *dAmt = atof(caStr);

    return 0;
}

/*****************************************************************************/
/* FUNC:   char *basLtoa(long lValue, char *pcaBuf, int iDec)                */
/* PARAMS: lValue    - ��ֵ   (I)                                            */
/*         pcaBuf    - ���ش� (O)                                            */
/*         iDec      - ����   (I) ֻ֧��10                                   */
/* RETURN: 0 - �ɹ�                                                          */
/*        -1 - ʧ��                                                          */
/* DESC:   ������תΪ�ַ���                                                  */
/*****************************************************************************/
BAS_EXPORT char *basLtoa(long lValue, char *pcaBuf, int iDec)
{
    sprintf(pcaBuf, "%ld", lValue);
    return pcaBuf;
}

/*****************************************************************************/
/* FUNC:   char *basTransVertical(const char *pcaBuf)                        */
/* PARAMS: pcaBuf    - ���봮 (I)                                            */
/* RETURN: ת����                                                            */
/* DESC:   ת�����е�"|"Ϊ"\|"                                               */
/*****************************************************************************/
BAS_EXPORT char *basTransVertical(const char *pcaBuf, int iLen)
{
    static char caTmpBuf[1024 + 1];
    char caTmpBuf2[1024 + 1];
    int i = 0, j = 0;

    memset(caTmpBuf, 0, sizeof(caTmpBuf));
    memset(caTmpBuf2, 0, sizeof(caTmpBuf2));

    if (iLen > sizeof(caTmpBuf2) - 1)
    {
        iLen = sizeof(caTmpBuf2) - 1;
    }
    sprintf(caTmpBuf2, "%-*.*s", iLen, iLen, pcaBuf);

    for (i = 0; i < strlen(caTmpBuf2); i++)
    {
        if (j > (sizeof(caTmpBuf) - 2))
        {
            break;
        }
        if (caTmpBuf2[i] == '|')
        {
            caTmpBuf[j] = '\\';
            j++;
            caTmpBuf[j] = '|';
            j++;
            continue;
        }
        if (caTmpBuf2[i] == '\\')
        {
            caTmpBuf[j] = '\\';
            j++;
            caTmpBuf[j] = '\\';
            j++;
            continue;
        }
        if (((unsigned char)caTmpBuf2[i] >= 0x81) && ((unsigned char)caTmpBuf2[i] <= 0xfe))
        {
            caTmpBuf[j] = caTmpBuf2[i];
            j++;
            i++;
            caTmpBuf[j] = caTmpBuf2[i];
            j++;
            continue;
        }
        caTmpBuf[j] = caTmpBuf2[i];
        j++;
    }

    return caTmpBuf;
}

/*����߿�ʼ��ȡ�ַ��� */
BAS_EXPORT char *basLeftStr(char *dst, char *src, int n, int m)
{
	char *p = src;
	char *q = dst;
	int len = strlen(src);
	if (n > len)
	{
		n=len;
	}

	while (n--)
	{
		*(q++)=*(p++);
	}
	*(q++) = '\0';

	return dst;
}
/* ���ұ߿�ʼ��ȡ*/
BAS_EXPORT char *basRightStr(char *dst, char *src, int n, int m)
{
	char *p = src;
	char *q = dst;
	int len = strlen(src);

	if (n > len)
	{
		n=len;
	}
	p += (len - n);

	while(*(q++)=*(p++));

	return dst;
}
/*���в���ȡ�ַ��� */
BAS_EXPORT char *basMidStr(char *dst, char *src, int n, int m)
{
	char *p = src;
	char *q = dst;
	int len = strlen(src);
	if (n > len)
	{
		n = len - m;
	}

	if (m > 0)
	{
		return NULL;
	}

	p += m;
	while(n--) *(q++) = *(p++);

	*(q++)='\0';

	return dst;
}
BAS_EXPORT char *basSubstr(char *dst, char *src, int len, int start)
{
	char *p = dst;
	char *q = src;
	int length = strlen(src);

	if (start >= length || start < 0)
	{
		return NULL;
	}

	if ( len > length)
	{
		len = length - start;
	}
	q += start;

	while (len--)
	{
		*(p++)=*(q++);
	}
	*(p++)='\0';

	return dst;
}

/*****************************************************************************/
/* FUNC: int SetAlign(char cType, long lFmtLength,                           */
/*                    const char *pcaSrc, char *pcaDes)		     	         */
/* PARAMS: cType - ���뷽ʽ (I)  		        		                     */
/*         lFmtLength  - ��ʽ������(I)    		       		                 */
/*         pcaSrc  - ��ʽ�������ַ���(I)           			                 */
/*         pcaDes  - ��ʽ������ַ���(O)         	  		                 */
/* RETURN  0 - �ɹ�                                                          */
/*        -1 - ʧ��                                                          */
/* DESC:   ���ö��뷽ʽ							                             */
/*****************************************************************************/
BAS_EXPORT int basSetAlign(char cType, long lFmtLength, const char *pcaSrc, char *pcaDes)
{
    char *pcaTemp;

	long lSrcLen = strlen(pcaSrc);
	long lMidPos = 0;
	if(lSrcLen > lFmtLength)
	{
        pcaTemp = (char *)malloc(lFmtLength + 1);
        memset(pcaTemp, 0, lFmtLength + 1);
        memcpy(pcaTemp, pcaSrc, lFmtLength);

//		BAS_LOG(BAS_LOGINFO, 0, 0,
//		"SetAlign:��ʽ�������ַ������ڸ�ʽ�����ȳ���");
//		return -1;
	}
	else
	{
	    pcaTemp = (char *)malloc(lSrcLen + 1);
        memset(pcaTemp, 0, lSrcLen + 1);
        memcpy(pcaTemp, pcaSrc, lSrcLen);
	}
	
	switch (cType)
	{
	case 'L':
		sprintf(pcaDes, "%-*s",(int)lFmtLength, pcaTemp);
		break;		
	case 'M':
		lMidPos = (lFmtLength - lSrcLen)/2;
		if(0 == lMidPos)
		{
		    memcpy(pcaDes, pcaTemp, lSrcLen);
            memset(pcaDes+lSrcLen,  ' ',  lFmtLength-lSrcLen);
		}
		else
		{
    		memset(pcaDes, ' ',  lMidPos);
    		lFmtLength -= lMidPos;
    		memcpy(pcaDes+lMidPos, pcaTemp, lSrcLen);
    		lFmtLength -= lSrcLen;
    		memset(pcaDes+lMidPos+lSrcLen,  ' ',  lFmtLength);
        }
		break;
	case 'R':
		sprintf(pcaDes, "%*s",(int)lFmtLength, pcaTemp);
		break;
	default:
		BAS_LOG(BAS_LOGINFO, 0, 0,
			"SetAlign:���뷽ʽ����");
		return -1;
	}
	return 0;
}

/*****************************************************************************/
/* FUNC:   int basIsValidMobileNo(const char *pcaBuf)                        */
/* PARAMS: psBuf - ���봮 (I)                                                */
/*         iLen  - ����                                                      */
/* RETURN: 0 - ����                                                          */
/*         1 - ��                                                            */
/* DESC:   �Ƿ���Ч�ֻ�����                                                  */
/*****************************************************************************/
BAS_EXPORT int basIsValidMobileNo(const char *pcaBuf)
{
    char caBuf[100];
    char caBuf1[100];
    regex_t tPreg;
    size_t nmatch  = 1;
    int iResult = 0;
    regmatch_t pm; 
    memset(&pm, 0, sizeof(regmatch_t));
    memset(caBuf1, 0, sizeof(caBuf1));
	strcpy(caBuf1, pcaBuf);
	basRTrim(caBuf1, strlen(caBuf1));
    memset(caBuf, 0, sizeof(caBuf));
    memset(&tPreg, 0, sizeof(tPreg));
	/* �ֻ���13*********,15*********,18********* */
    strcpy(caBuf, "^(\\+86|0)?1[3,5,8][0-9][0-9]{8}$");

    regcomp(&tPreg, caBuf, REG_EXTENDED);

    if (regexec(&tPreg, caBuf1, nmatch, &pm, 0) != 0)
    {
        return 0;
    }
    regfree (&tPreg); 
    return 1;
}
/*****************************************************************************/
/* FUNC:   int basIsValidTeleNo(const char *pcaBuf)                          */
/* PARAMS: psBuf - ���봮 (I)                                                */
/* RETURN: 0 - ����                                                          */
/*         1 - ��                                                            */
/* DESC:   �Ƿ���Ч�绰����                                                  */
/*****************************************************************************/
BAS_EXPORT int basIsValidTeleNo(const char *pcaBuf)
{
    char caBuf[100];
    regex_t tPreg;
    size_t nmatch  = 1;
    int iResult = 0;
    regmatch_t pm; 
    memset(&pm, 0, sizeof(regmatch_t));
    memset(caBuf, 0, sizeof(caBuf));
    memset(&tPreg, 0, sizeof(tPreg));
	/* ���Ҵ���-����-����-�ֻ���[3-4λ] */
    strcpy(caBuf, "^(\\+86|0)?(([0-9][0-9][-_])?[0-9]{3,4}[-_])?([0-9]{7,8})([-_][0-9]{3,4})?$");

    regcomp(&tPreg, caBuf, REG_EXTENDED);

    if (regexec(&tPreg, pcaBuf, nmatch, &pm, 0) != 0)
    {
        return 0;
    }
    regfree (&tPreg); 

    return 1;
}
/*****************************************************************************/
/* FUNC:   int basIsValidEmail(const char *pcaBuf)                           */
/* PARAMS: psBuf - ���봮 (I)                                                */
/* RETURN: 0 - ����                                                          */
/*         1 - ��                                                            */
/* DESC:   �Ƿ���ЧEmail��ʽ                                                 */
/*****************************************************************************/
BAS_EXPORT int basIsValidEmail(const char *pcaBuf)
{
    char caBuf[100];
    char caBuf1[100];
    regex_t tPreg;
    size_t nmatch  = 1;
    int iResult = 0;
    regmatch_t pm; 
    memset(&pm, 0, sizeof(regmatch_t));
    memset(caBuf1, 0, sizeof(caBuf1));
	strcpy(caBuf1, pcaBuf);
	basRTrim(caBuf1, strlen(caBuf1));
    memset(caBuf, 0, sizeof(caBuf));
    memset(&tPreg, 0, sizeof(tPreg));
    strcpy(caBuf, "^[a-z0-9]+([.][_a-z0-9]+)*@[a-z0-9]+([.][a-z0-9]+)*$");

    regcomp(&tPreg, caBuf, REG_EXTENDED);

    if (regexec(&tPreg, caBuf1, nmatch, &pm, 0) != 0)
    {
        return 0;
    }
    regfree (&tPreg); 

    return 1;
}

/*****************************************************************************/
/* FUNC:   char *out8100IsHalfChsChar(char *pcaBuf, int iPos)                */
/* PARAMS: psBuf - ת���� (I/O)                                              */
/*         iPos  - ���жϵ��ַ���λ��                                        */
/* RETURN: ���ش�                                                            */
/*         1 - ��                                                            */
/* DESC:   �ж��ַ������ض�λ���Ƿ��ǰ�����֣�����ǵĻ�����                */
/*****************************************************************************/
BAS_EXPORT int basTrimHalfChsChar(char *pcaBuf, int iPos)
{
    int iHalfChsChar = 0;           /* ��¼����ַ��ĸ��� */
	int iCharFlag = 0;				/*��¼ǰһ���ַ��Ƿ���*/

    while(iHalfChsChar < iPos)
    {
        /* ����ǰ������Ӧ���ǳɶԳ���*/
        if((unsigned char)pcaBuf[iHalfChsChar] >= 0x80)
        {
			iHalfChsChar += 2;
		}
		else
		{
			iHalfChsChar++;
		}		

    }
	if ((iHalfChsChar == iPos+1) && ((unsigned char)pcaBuf[iPos-1] >= 0x80))
	{
		pcaBuf[iPos - 1] = 0x20;
	}

    return iHalfChsChar;
}

/*****************************************************************************/
/* FUNC:   basStrlcpy                                                        */
/* PARAMS: char *caDest      - Ŀ���ַ���(������)(Out)                       */
/*         const char *caSrc - Դ�ַ���(In)                                  */
/*         size_t destSize   - Ŀ�껺������С(In)                            */
/* RETURN: strlen(caSrc)                                                     */
/* DESC:   strlcpy()��һ��ʵ��, ʵ�ְ�ȫ��Ч���ַ�������                     */
/* Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>             */
/*****************************************************************************/
BAS_EXPORT size_t basStrlcpy(char *caDest, const char *caSrc, size_t nDestSize)
{
    char *pcDest = caDest;
    const char *pcSrc = caSrc;
    size_t n = nDestSize;

    /* ���������ܶ���ֽ� */
    if (n != 0)
    {
        while(--n != 0)
        {
            if ((*pcDest++ = *pcSrc++) == '\0')
                break;
        }
    }

    /* ���Ŀ�껺����û��ʣ��ռ�, ����������NUL������, Դ�ַ������ض� */
    if (n == 0)
    {
        if (nDestSize != 0)
            *pcDest = '\0';
        while (*pcSrc++)
            ;
    }

    /* ������NUL������ */
    return (pcSrc - caSrc - 1);
}

/*******************************************************************************/
/* FUNC:   basStrlcat                                                          */
/* PARAMS: char *caDest      - Ŀ���ַ���(������)(Out)                         */
/*         const char *caSrc - Դ�ַ���(In)                                    */
/*         size_t destSize   - Ŀ�껺������С(In)                              */
/* RETURN: strlen(caSrc) + MIN(strlen(initial caDest), destSize)               */
/* DESC:   strlcat()��һ��ʵ��, ʵ�ְ�ȫ��Ч���ַ�������                       */
/* Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>               */
/*******************************************************************************/
BAS_EXPORT size_t basStrlcat(char *caDest, const char *caSrc, size_t nDestSize)
{
    char *pcDest = caDest;
    const char *pcSrc = caSrc;
    size_t n = nDestSize;
    size_t nDestStrLen;

    /* Ѱ��Ŀ���ַ����Ľ�β, ����ʣ���ֽ��� */
    while (n-- != 0 && *pcDest != '\0')
    {
        pcDest++;
    }
    nDestStrLen = pcDest - caDest;
    n = nDestSize - nDestStrLen;

    /* û��ʣ��ռ� */
    if (n == 0)
    {
        return (nDestStrLen + strlen(caSrc));
    }

    while(*pcSrc != '\0')
    {
        if (n != 1)
        {
            *pcDest++ = *pcSrc; 
            n--;
        }
        pcSrc++;
    }
    *pcDest = '\0';

    /* ������NUL������ */
    return (nDestStrLen + (pcSrc - caSrc));
}

/*****************************************************************************/
/* FUNC:   basFillField                                                      */
/* PARAMS: char *caDest       - Ŀ���ֶ�                                     */
/*         const char *caSrc  - Դ�ַ���(In)                                 */
/*         size_t fieldLength - Ŀ���ֶγ���                                 */
/* DESC:   ����ֶ�                                                          */
/*****************************************************************************/
BAS_EXPORT char *basFillField(char *caDest, const char *caSrc, size_t nFieldLength)
{
    memcpy(caDest, caSrc, BAS_MIN(strlen(caSrc), nFieldLength));

    return caDest;
}

/*****************************************************************************/
/* FUNC:    basConvAmtFieldFormat                                            */
/* PARAMS:  char *caToAmt(Out)              - ת����������ֶ�               */ 
/*          const char *caFromAmt(In)       - ת��ǰ�������ֶ�               */
/*          const int iToAmtScale(In)       - ת�������ֵ�С��λ��           */
/*          const int iFromAmtScale(In)     - ת��ǰ���ֵ�С��λ��           */
/*          const size_t nToAmtLength(In)   - ת���������ֶεĳ���           */
/*          const size_t nFromAmtLength(In) - ת��ǰ�����ֶεĳ���           */
/*          const int iFillerflag(In)       - ������־                     */  
/*                AMT_FIELD_FILLER_SPACE    - ��ո�                         */
/*                AMT_FIELD_FILLER_ZERO     - ����                           */
/* RETURN:  NULL - �ֶ����                                                  */
/* DESC:    ת���Ҷ���������ֶθ�ʽ(��С����)                               */
/*****************************************************************************/
BAS_EXPORT char *basConvAmtFieldFormat(char *caToAmt, const char *caFromAmt, 
                                       const int iToAmtScale, const int iFromAmtScale,
                                       const size_t nToAmtLength, const size_t nFromAmtLength,
                                       const int iFillerflag)
{
    char caBuff[BAS_LDESC_LEN + 1];

    double dAmt = basAtof(caFromAmt, nFromAmtLength) / pow(10, iFromAmtScale);

    memset(caBuff, 0, sizeof(caBuff));
    if (AMT_FIELD_FILLER_SPACE == iFillerflag)
    {
        sprintf(caBuff, "%*.0lf", nToAmtLength, dAmt * pow(10, iToAmtScale)); 
    }
    else
    {
        sprintf(caBuff, "%0*.0lf", nToAmtLength, dAmt * pow(10, iToAmtScale)); 
    }

    if (strlen(caBuff) > nToAmtLength)
    {
        return NULL;
    }
    else
    {
        memcpy(caToAmt, caBuff, nToAmtLength);
        return caToAmt;
    }
}

/*****************************************************************************/
/* FUNC:   basGetMinValue                                                    */
/* PARAMS: long nValue1 ��������ֵ1                                          */
/*         long nValue2 ��������ֵ2                                          */
/* DESC:   ��ȡ��Сֵ                                                        */
/*****************************************************************************/
BAS_EXPORT long basGetMinValue(long nValue1, long nValue2)
{
    return (nValue1 < nValue2 ? nValue1 : nValue2);
}

/*****************************************************************************/
/* FUNC:   basGetMaxValue                                                    */
/* PARAMS: long nValue1 ��������ֵ1                                          */
/*         long nValue2 ��������ֵ2                                          */
/* DESC:   ��ȡ���ֵ                                                        */
/*****************************************************************************/
BAS_EXPORT long basGetMaxValue(long nValue1,long nValue2)
{
    return (nValue1 > nValue2 ? nValue1 : nValue2);
}

/*****************************************************************************/
/* FUNC:   basStrcpy                                                         */
/* PARAMS: char *caOutDest      - Ŀ���ַ���(������)(Out)                    */
/*         char *caInSrc        - Դ�ַ���(In)                               */
/*         size_t nDestSize     - Ŀ���ַ�������Ĵ�С(����������\0)         */
/* RETURN: caOutDest                                                         */
/* DESC:   �ַ�������                                                        */
/*****************************************************************************/
BAS_EXPORT void basStrcpy(char *caOutDest, char *caInSrc, size_t nDestSize)
{
    unsigned int uiSrcLen = strlen(caInSrc);
    caOutDest[nDestSize - 1] = '\0';
    if (uiSrcLen + 1 > nDestSize)
    {
        memcpy(caOutDest, caInSrc, nDestSize - 1);
    }
    else
    {
        memcpy(caOutDest, caInSrc, uiSrcLen);
        caOutDest[uiSrcLen] = 0x00;
    }
}
/*****************************************************************************/
/* FUNC:   char *basRemove0D0A(char *pcaBuf, int iMaxLen)                    */
/* PARAMS: pcaBuf  - ���봮 (I)                                              */
/*         iMaxLen - ��󳤶� (I)                                            */
/* RETURN: �����                                                            */
/* DESC:   ȥ���ַ����еĻس��ͻ��з�                                        */
/*****************************************************************************/
BAS_EXPORT char *basRemove0D0A(char *pcaBuf, int iMaxLen)
{
    int i = 0, j = 0;

    if ((strlen(pcaBuf) <= iMaxLen) && (strlen(pcaBuf) > 0))
    {
        iMaxLen = strlen(pcaBuf);
    }

    for (i = 0; i < iMaxLen; i++)
    {
        if (pcaBuf[i] == 13 || pcaBuf[i] == 10)
        {
            continue;
        }
        pcaBuf[j++] = pcaBuf[i];
    }
    for (i = j; i < iMaxLen; i++)
    {
        pcaBuf[i] = ' ';
    }

    return pcaBuf;
}


/*****************************************************************************/
/* FUNC:   char *basRTrimLeftOne(char *pcaBuf, int iMaxLen)             */
/* PARAMS: pcaBuf  - ת���� (I/O)                                            */
/*         iMaxLen - ��󳤶� (I)                                            */
/* RETURN: ת�������                                                        */
/* DESC:   ȥ���ұ߿ո�Ϊ���ַ���ʱ����һ���ո�                                  */
/*****************************************************************************/
BAS_EXPORT char *basRTrimLeftOne(char *pcaBuf, int iMaxLen)
{
    int i;
	int iBuflen = 0; 
	iBuflen = iMaxLen;

    if( iMaxLen < 1 ) /* Ҫ����С����Ϊ1(������������) */
    	return pcaBuf;
    if ((strlen(pcaBuf) <= iMaxLen) && (strlen(pcaBuf) >= 0))
    {
        iMaxLen = strlen(pcaBuf);
    }
   if( 0 == iMaxLen )/* ���ַ���ʱ��һ���ո� */
   {
	   memset(pcaBuf, '\0', iBuflen);
	   pcaBuf[0] = ' ';
	   return pcaBuf;
   }
   /* ���һλ���Ƚ� */
    for (i = iMaxLen - 1; i > 0 ; i--)
    {
        if (pcaBuf[i] == ' ')
        {
            pcaBuf[i] = '\0';
        }
        else
        {
            break;
        }
    }
    return pcaBuf;
}

//R20170324000008 -���ÿ�����ϵͳ�����޸����� add by yuyang 20170405 start
//****************************************************************************/
// FUNC:    preiconv()                                                       */
// PARAM:   cpEncodeFrom -- ��ת���ı���                                     */
//          cpInput -- ��ת�����ַ���                                        */
//          output -- �滻����ַ���                                         */
//          ioutput -- �滻�󳤶�                                            */
// RETURN:  0 - �ɹ� -1 - �쳣                                               */
// DESC:    �滻�ַ�����ָ���������ַ���                                     */
//****************************************************************************/
BAS_EXPORT int preiconv(char *cpEncodeFrom, char *cpInput, char *output, size_t *ioutput)
{
    static int preiconvloaded=0;
    static tppled *pled;
    static int  pledtot=0;

    char *pchfrom, *pchto, *pchend;
    int k;

    if(preiconvloaded==0) {
      int i=0, rightline[MAXLINEOFCFGFILE], rightlinecnt=0;
      char *fcfg, *pch;
      FILE *fpcfg;
      char line[200];
      char s1[100], s2[100], s3[100], s4[100];

      if( (fcfg=(char *)getenv("PREICONVCFG"))==0 ) {
        preiconvloaded=1; 
      }else if( (fpcfg=fopen(fcfg,"r"))==0  ) {
        printf("WARNNING: open PREICONVCFG file [%s] fail.\n", fcfg);
        preiconvloaded=1; 
      }else{
        memset(line,0,sizeof(line));
        while ( fgets(line, sizeof(line), fpcfg) ) {
          i++;
          line[strlen(line)-1]=0;
          memset(s1, 0, sizeof(s1));
          memset(s2, 0, sizeof(s2));
          memset(s3, 0, sizeof(s3));
          memset(s4, 0, sizeof(s4));
          sscanf( line, "%s%s%s%s", s1,s2,s3,s4);
          if( strlen(s1)>20
             || strlen(s2)>16 || strlen(s2)%2!=0
             || ( strcmp(s3,"null")!=0 && ( strlen(s3)>16 || s3[0]==0 || strlen(s3)%2!=0 ) )
             || s4[0]!=0 ){
            printf("WARNNING: PREICONVCFG file [%s] line [%d] [%s] format error, skipped.\n", fcfg, i, line);
            continue;
          }
          for(pch=s2;pch<s2+strlen(s2);pch++){
            if( !(  ( *pch>='0' && *pch<='9' )
                  ||( *pch>='a' && *pch<='f' )
                  ||( *pch>='A' && *pch<='F' ) ) )
            printf("WARNNING: PREICONVCFG file [%s] line [%d] [%s] format error, skipped.\n", fcfg, i, line);
            continue;
          }
          if( strcmp(s3,"null")!=0 )
          for(pch=s3;pch<s3+strlen(s3);pch++){
            if( !(  ( *pch>='0' && *pch<='9' )
                  ||( *pch>='a' && *pch<='f' )
                  ||( *pch>='A' && *pch<='F' ) ) )
            printf("WARNNING: PREICONVCFG file [%s] line [%d] [%s] format error, skipped.\n", fcfg, i, line);
            continue;
          }
          rightline[rightlinecnt++]=i;
          if( rightlinecnt>=MAXLINEOFCFGFILE ) {
            printf("ERROR: too many line in PREICONVCFG file [%s] \n", fcfg);
            return(1);
          }
        }
  
        pledtot=rightlinecnt;
    
        if(pledtot>0){
          pled=(tppled*)malloc(sizeof(tppled)*pledtot);
          memset(pled, 0, sizeof(tppled)*pledtot);
          rewind(fpcfg);
          i=0; rightlinecnt=0;  
          while ( fgets(line, sizeof(line), fpcfg) ) {
            if( ++i != rightline[rightlinecnt] ) continue;
            line[strlen(line)-1]=0;
    
            memset(s2, 0, sizeof(s2));
            memset(s3, 0, sizeof(s3));
    
            sscanf( line, "%s%s%s%s", pled[rightlinecnt].code,s2,s3);
    
            sscanf(s2, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                  &(pled[rightlinecnt].tranfrom[0]), &(pled[rightlinecnt].tranfrom[1]),
                  &(pled[rightlinecnt].tranfrom[2]), &(pled[rightlinecnt].tranfrom[3]),
                  &(pled[rightlinecnt].tranfrom[4]), &(pled[rightlinecnt].tranfrom[5]),
                  &(pled[rightlinecnt].tranfrom[6]), &(pled[rightlinecnt].tranfrom[7]));
    
            if( strcmp(s3,"null")!=0 )
              sscanf(s3, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
                  &(pled[rightlinecnt].tranto[0]), &(pled[rightlinecnt].tranto[1]),
                  &(pled[rightlinecnt].tranto[2]), &(pled[rightlinecnt].tranto[3]),
                  &(pled[rightlinecnt].tranto[4]), &(pled[rightlinecnt].tranto[5]),
                  &(pled[rightlinecnt].tranto[6]), &(pled[rightlinecnt].tranto[7]));
    
             if( ++rightlinecnt == pledtot ) break;
          }
        }
        fclose(fpcfg);
      }
      preiconvloaded=1;
    }

    if( pledtot==0 ) {
      memcpy(output, cpInput, strlen(cpInput));
      *ioutput=strlen(cpInput);
      return(0);
    }

    *ioutput=0;
    pchfrom=cpInput;
    pchend=pchfrom+strlen(cpInput);
    for(pchto=output;pchfrom<pchend;){
      for(k=0;k<pledtot;k++){
        if( strcmp(pled[k].code, cpEncodeFrom)!=0 ) continue;
        if( pchend-pchfrom<strlen(pled[k].tranfrom) ) continue;
        if( memcmp(pchfrom, pled[k].tranfrom, strlen(pled[k].tranfrom))!=0 ) continue;
        memcpy(pchto, pled[k].tranto, strlen(pled[k].tranto)); 
        pchto+=strlen(pled[k].tranto); *ioutput+=strlen(pled[k].tranto);
        pchfrom+=strlen(pled[k].tranfrom);
        break;
      }
      if( k==pledtot ) {
        *pchto=*pchfrom; pchto+=1;
        *ioutput+=1;
        pchfrom++;
      }
    }

    return(0);
}
//R20170324000008 -���ÿ�����ϵͳ�����޸����� add by yuyang 20170405 end
