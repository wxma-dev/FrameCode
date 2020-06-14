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
return 0     成功
return -1    失败
*/

BAS_EXPORT int  ldecodefd2fd(FILE* fdR,FILE* fdW)
{
    //读BUF
    char caReadbuf[MAXLEN*2] = "";  
    
    //解密BUF
    char caDecodeBuf[MAXLEN*2] = "";  
    int  iDecodeLen = 0;
    
	
    //当前读的指针
    int iPos = 0;
    //当前匹配的头特征个数
    int iMatchCnt = 0;    
    
    //加密数据块开始缓存的指针
    int iEncryptPos = 0;

    //解密后的长度
    int iEncryLen = 0;

    
    int iRet = 0;

   
    while(1)
    {
        //每次读取一个
        if ((iRet = fread(caReadbuf+ iPos++ ,1,1,fdR)) == 0 )
        {      
           //读到文件结束
           fprintf(fdW,"%*.*s",iPos,iPos,caReadbuf);
           return 0;    
        }
        
        if (caReadbuf[iPos -1 ] == stcnEncodeHead[iMatchCnt])
        {
            //当继续匹配头后，匹配个数加
            iMatchCnt += 1;

            //完全匹配则找到加密部分
            if(iMatchCnt == 4)
            {
                //头完全匹配
                //记录下加密部分的开始位置
                iEncryptPos = iPos - 4;

                //读取剩余的头部分
                if ((iRet = fread(caReadbuf+ iEncryptPos + 4 , 9 , 1, fdR)) == 0 )
                {
                    printf("读取加密头长度出错\n");
                    return -1;            
                }               
                
                //取出整个报文的长度
                iEncryLen = atoi(caReadbuf+ iEncryptPos + 6 ); 
                
                //printf("iEncryLen:%d\n",iEncryLen);
                
                //读取整个加密报文
                if ( (iRet = fread(caReadbuf+ iEncryptPos + 13,iEncryLen - 13, 1, fdR)) == 0 )
                {
                    printf("读取加密体长度出错\n");
                    return -1;   
                } 
                
                //对加密报文进行解密 
                memset(caDecodeBuf,0, sizeof(caDecodeBuf));
                iDecodeLen = ldecodelog(caDecodeBuf, sizeof(caDecodeBuf), caReadbuf+ iEncryptPos, iEncryLen);
                if (iDecodeLen == 0)
                {
                    printf("解密出错\n");
                    return -1;   
                }
                
                //先写开始未加密部分
                fprintf(fdW,"%*.*s",iEncryptPos,iEncryptPos,caReadbuf);
                //再写入解密的部分
                fprintf(fdW,"%*.*s",iDecodeLen,iDecodeLen,caDecodeBuf);
                
                //清掉，重新开始
                iPos  = 0;
                memset(caReadbuf,0,sizeof(caReadbuf));  
                iMatchCnt = 0;
                continue;
            }
        }
        else
        {            
            //如果先前有成功匹配的，则先清除匹配个数，之后再重新从头匹配。
            if (iMatchCnt != 0)
            {
                //当未继续匹配后，先清除匹配个数，之后再重新匹配。
                iMatchCnt = 0;
                if (caReadbuf[iPos -1] == stcnEncodeHead[iMatchCnt])
                {                
                    iMatchCnt += 1;
                }
            }                        
         }

        //当BUF已经快满的时候，将缓存数据存到文件
        if ( iPos == MAXLEN - 1 )
        {
           //写入数据
           fprintf(fdW,"%*.*s",iPos - iMatchCnt, iPos - iMatchCnt, caReadbuf); 
           
           //将头特征的报文缓存过来。
           memcpy(caReadbuf, caReadbuf + iPos - iMatchCnt, iMatchCnt);
           memset(caReadbuf + iMatchCnt, 0, sizeof(caReadbuf) - iMatchCnt);
           iPos  = iMatchCnt;           
        }       
    }

    return 0;
}
