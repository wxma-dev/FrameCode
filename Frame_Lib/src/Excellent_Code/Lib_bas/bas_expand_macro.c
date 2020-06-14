/*****************************************************************************/
/*                           TOPCARD                                         */
/*****************************************************************************/
/* PROGRAM NAME: bas_expand_macro.c                                          */
/* DESCRIPTIONS: �궨����չ                                                  */
/* AUTHOR      : Zhaozheqian                                                 */
/* CREATE DATE : 2009-09-20                                                  */
/*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "bas/bas_global.h"

/*****************************************************************************/
/* FUNC:   char* basExpandMacro(const char* pcStr)                           */
/* PARAMS: pcStr - ԭʼ�ַ���                                                */
/* RETURN: ��չ֮����ַ���                                                  */
/* DESC:   ���ַ������к궨����չ                                            */
/*****************************************************************************/
BAS_EXPORT char* basExpandMacro(const char* pcStr)
{
    static char mcaBuffer[4096];
    char caMacro[128];
    const char *s;
    char *p, *q;
    int iState = 0;
    BOOL bPrevIsEscape = BAS_FALSE;

    memset(mcaBuffer, '\0', sizeof(mcaBuffer));
    for (s=pcStr,p=mcaBuffer; *s; ++s)
    {
        if (iState == 0)
        {   /* normal state */
            if ((*s == '$') && (bPrevIsEscape != BAS_TRUE))
            {
                iState = 1;
                if ((*(s+1) == '{') || (*(s+1) == '('))
                {
                    s += 1;
                }
                q = caMacro;
                continue;
            }
            else
            {
                *p++ = *s;
            }
        }
        else
        {   /* extract macro state */
            if (!(isalpha(*s) || (*s == '_')))
            {   /* end of macro, do expand */
                *q = '\0';
                if (getenv(caMacro) == NULL)
                {
                    BAS_LOG(BAS_LOGINFO, 0, 0, "macro[%s] does not exist\n", caMacro);
                }
                else
                {
                    strcpy(p, getenv(caMacro));
                    p += strlen(p);
                }
                if (!((*s == '}') || (*s == ')')))
                {
                    *p++ = *s;
                }
                iState = 0;
            }
            else
            {
                *q++ = *s;
            }
        }
        if (*s == '\\')
        {
            bPrevIsEscape = BAS_TRUE;
        }
        else
        {
            bPrevIsEscape = BAS_FALSE;
        }
    }
    return mcaBuffer;
}

/*****************************************************************************/
/* FUNC:   char* basGetFileFullName(const char* pcFileName,                  */
/*                                  const char* pcDefaultDir)                */
/* PARAMS: pcFileName - �ļ���(I)                                            */
/*         pcDefaultDir - ȱʡ·��(I) - NULL��ʾʹ��APPHOME                  */
/* RETURN: �ļ�ȫ��                                                          */
/* DESC:   �����ļ����õ��ļ���ȫ·������                                    */
/*****************************************************************************/
BAS_EXPORT char* basGetFileFullName(const char* pcFileName,
                                    const char* pcDefaultDir)
{
    static char mcaFullName[2048];
    char        caFileName[2048];

    strcpy(caFileName, basExpandMacro(pcFileName));
    if (caFileName[0] == '/')
    {   // ����·��
        strcpy(mcaFullName, caFileName);
    }
    else
    {   // ���·��
        if (pcDefaultDir == NULL)
        {
            if (getenv("APPHOME") == NULL)
            {
                strcpy(mcaFullName, getenv("HOME"));
            }
            else
            {
                strcpy(mcaFullName, getenv("APPHOME"));
            }
        }
        else
        {
            if (pcDefaultDir[0] == '/')
            {   // ȱʡ·���Ǿ���·��
                strcpy(mcaFullName, basExpandMacro(pcDefaultDir));
            }
            else
            {   // ȱʡ·�������·��
                if (getenv("APPHOME") == NULL)
                {
                    strcpy(mcaFullName, getenv("HOME"));
                }
                else
                {
                    strcpy(mcaFullName, getenv("APPHOME"));
                }
                strcat(mcaFullName, "/");
                strcat(mcaFullName, basExpandMacro(pcDefaultDir));
            }
        }
        if (mcaFullName[strlen(mcaFullName)-1] != '/')
        {   // ȱʡ·��������'/'��β
            strcat(mcaFullName, "/");
        }
        strcat(mcaFullName, basExpandMacro(pcFileName));
    }

    return mcaFullName;
}

