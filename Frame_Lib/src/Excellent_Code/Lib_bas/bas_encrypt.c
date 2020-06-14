/* multi-bank version */
/* Logging program */
/* Created 11 Sep 1997 soh */

#include "bas/bas_global.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/time.h>

#include <sys/shm.h>

#include <unistd.h>
#include <sys/errno.h>
#include <string.h>
/* ENC is the basic 1 character encoding function to make a char printing.  */
#define	ENC(c) ((c) ? ((c) & 077) + ' ': '`')
#define	XC(c) ((c)^mask)

static const char  stcnEncodeHead[]  =  "DZYA";

#define MAXLEN 51200



static int _lencodelog (char *dest, int maxlen, char *src, int iLen )
{
    register int ch, n, l, mask;
    register char *p;
    long t; 
    char caBufLen[10] = "";

    time(&t);
    mask = t & 0x0ffff;
    memcpy(dest,stcnEncodeHead,4);

    for(l=13; iLen > 0; iLen-=51)
    {
        n = iLen>51? 51:iLen;
            ch = ENC (n);
        dest[l++] = ch;

        for (; n > 0; n -= 3, src += 3)
        {
            ch = XC(*src) >> 2;
            ch = ENC (ch);
	    dest[l++] = ch;

            ch = ((XC(*src) << 4) & 060) | ((XC(src[1]) >> 4) & 017);
            ch = ENC (ch);
	    dest[l++] = ch;

            ch = ((XC(src[1]) << 2) & 074) | ((XC(src[2]) >> 6) & 03);
            ch = ENC (ch);
	    dest[l++] = ch;

            ch = XC(src[2]) & 077;
            ch = ENC (ch);
	    dest[l++] = ch;
        }
    }
    ch = ENC ('\0');
    dest[l++] = ch; 

    ch = mask >> 2;
    ch = ENC (ch);
    dest[4] = ch;

    ch = ((mask << 4) & 060) | (((mask>>8) >> 4) & 017);
    ch = ENC (ch);
    dest[5] = ch;
    dest[l] = 0; 

    sprintf(caBufLen,"%07d",l);

    memcpy(dest +6, caBufLen,7);

    return l;
}


BAS_EXPORT int lencodelog (char *dest, int maxlen, char *src, int iLen )
{
    char * pENCLOG = getenv("ENCLOG");

    if (pENCLOG == NULL ||  strcmp(pENCLOG,"Y") != 0 )
    {
        memcpy(dest,src,iLen);
        return iLen;
    }
    else
    {
        return _lencodelog (dest, maxlen, src,iLen);
    }
}

/* Single character decode.  */
#define	DEC(c)	(((c) - ' ') & 077)

BAS_EXPORT int ldecodelog (char *dest, int maxlen, char *src, int iLen )
{
    register int n, l, on, mask;
    register char ch, *p;

    if (iLen < 14)
    {
        goto codeErr;
    }

    if(memcmp(src,stcnEncodeHead,4) != 0)
    {
	    goto codeErr;
    }

    p = src+4;
    mask = DEC (p[0]) << 2 | DEC (p[1]) >> 4;

    src += 13;
    for(l=0; iLen > 0; iLen-=on)
    {
        /* N is used to avoid writing out all the characters at the end of
             the file.  */

        on = n = DEC (*src);
        if (n <= 0)
            break;
        for (++src; n > 0; src += 4, n -= 3)
        {
            if (n >= 3)
            {
                ch = DEC (src[0]) << 2 | DEC (src[1]) >> 4;
		dest[l++] = ch;
                ch = DEC (src[1]) << 4 | DEC (src[2]) >> 2;
		dest[l++] = ch;
                ch = DEC (src[2]) << 6 | DEC (src[3]);
		dest[l++] = ch;
            }
            else
            {
                if (n >= 1)
                {
                    ch = DEC (src[0]) << 2 | DEC (src[1]) >> 4;
		    dest[l++] = ch;
                }
                if (n >= 2)
                {
                    ch = DEC (src[1]) << 4 | DEC (src[2]) >> 2;
		    dest[l++] = ch;
                }
            }
        }
    }
    for( n=0, p=dest; n < l; p++, n++ )
		*p ^= mask;
    return l;
codeErr:
    return 0;
}


/*
return 0     �ɹ�
return -1    ʧ��
*/

BAS_EXPORT int  ldecodefd2fd(FILE* fdR,FILE* fdW)
{
    //��BUF
    char caReadbuf[MAXLEN*2] = "";  
    
    //����BUF
    char caDecodeBuf[MAXLEN*2] = "";  
    int  iDecodeLen = 0;
    
	
    //��ǰ����ָ��
    int iPos = 0;
    //��ǰƥ���ͷ��������
    int iMatchCnt = 0;    
    
    //�������ݿ鿪ʼ�����ָ��
    int iEncryptPos = 0;

    //���ܺ�ĳ���
    int iEncryLen = 0;

    
    int iRet = 0;

   
    while(1)
    {
        //ÿ�ζ�ȡһ��
        if ((iRet = fread(caReadbuf+ iPos++ ,1,1,fdR)) == 0 )
        {      
           //�����ļ�����
           fprintf(fdW,"%*.*s",iPos,iPos,caReadbuf);
           return 0;    
        }
        
        if (caReadbuf[iPos -1 ] == stcnEncodeHead[iMatchCnt])
        {
            //������ƥ��ͷ��ƥ�������
            iMatchCnt += 1;

            //��ȫƥ�����ҵ����ܲ���
            if(iMatchCnt == 4)
            {
                //ͷ��ȫƥ��
                //��¼�¼��ܲ��ֵĿ�ʼλ��
                iEncryptPos = iPos - 4;

                //��ȡʣ���ͷ����
                if ((iRet = fread(caReadbuf+ iEncryptPos + 4 , 9 , 1, fdR)) == 0 )
                {
                    printf("��ȡ����ͷ���ȳ���\n");
                    return -1;            
                }               
                
                //ȡ���������ĵĳ���
                iEncryLen = atoi(caReadbuf+ iEncryptPos + 6 ); 
                
                //printf("iEncryLen:%d\n",iEncryLen);
                
                //��ȡ�������ܱ���
                if ( (iRet = fread(caReadbuf+ iEncryptPos + 13,iEncryLen - 13, 1, fdR)) == 0 )
                {
                    printf("��ȡ�����峤�ȳ���\n");
                    return -1;   
                } 
                
                //�Լ��ܱ��Ľ��н��� 
                memset(caDecodeBuf,0, sizeof(caDecodeBuf));
                iDecodeLen = ldecodelog(caDecodeBuf, sizeof(caDecodeBuf), caReadbuf+ iEncryptPos, iEncryLen);
                if (iDecodeLen == 0)
                {
                    printf("���ܳ���\n");
                    return -1;   
                }
                
                //��д��ʼδ���ܲ���
                fprintf(fdW,"%*.*s",iEncryptPos,iEncryptPos,caReadbuf);
                //��д����ܵĲ���
                fprintf(fdW,"%*.*s",iDecodeLen,iDecodeLen,caDecodeBuf);
                
                //��������¿�ʼ
                iPos  = 0;
                memset(caReadbuf,0,sizeof(caReadbuf));  
                iMatchCnt = 0;
                continue;
            }
        }
        else
        {            
            //�����ǰ�гɹ�ƥ��ģ��������ƥ�������֮�������´�ͷƥ�䡣
            if (iMatchCnt != 0)
            {
                //��δ����ƥ��������ƥ�������֮��������ƥ�䡣
                iMatchCnt = 0;
                if (caReadbuf[iPos -1] == stcnEncodeHead[iMatchCnt])
                {                
                    iMatchCnt += 1;
                }
            }                        
         }

        //��BUF�Ѿ�������ʱ�򣬽��������ݴ浽�ļ�
        if ( iPos == MAXLEN - 1 )
        {
           //д������
           fprintf(fdW,"%*.*s",iPos - iMatchCnt, iPos - iMatchCnt, caReadbuf); 
           
           //��ͷ�����ı��Ļ��������
           memcpy(caReadbuf, caReadbuf + iPos - iMatchCnt, iMatchCnt);
           memset(caReadbuf + iMatchCnt, 0, sizeof(caReadbuf) - iMatchCnt);
           iPos  = iMatchCnt;           
        }       
    }

    return 0;
}
