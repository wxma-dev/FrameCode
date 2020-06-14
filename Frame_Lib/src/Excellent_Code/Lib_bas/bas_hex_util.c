#include <string.h>
#include <ctype.h>

#include "bas/bas_global.h"

#define BIN2HEX(b)  \
    (((b) >= 10) ? (b) - 10 + 'A' : (b) + '0')

#define HEX2BIN(h)  \
    (isupper(h) ? (h) - 'A' + 10 :  \
                  islower(h) ? (h) - 'a' + 10 : \
                               isdigit(h) ? (h) - '0' : 0)

BAS_EXPORT unsigned char* basBin2Hex(unsigned char* pcDst,
                                     const unsigned char* pcSrc, const int iLen)
{
    int i;
    for (i=0; i<iLen/2; ++i,++pcSrc)
    {
        *pcDst++ = BIN2HEX((*pcSrc & 0xf0) >> 4);
        *pcDst++ = BIN2HEX(*pcSrc & 0x0f);
    }
    return pcSrc;
}

BAS_EXPORT unsigned char* basHex2Bin(unsigned char* pcDst,
                                     const unsigned char* pcSrc, const int iLen)
{
    int i;
    /* NOTE:长度必须是偶数，由调用程序保证 */
    for (i=0; i<iLen/2; ++i,pcSrc+=2)
    {
        *pcDst++ = ((HEX2BIN(*pcSrc) & 0x0f) << 4) | (HEX2BIN(*(pcSrc+1)) & 0x0f);
    }
    return pcDst;
}

BAS_EXPORT unsigned char* basBcd2Asc(unsigned char* pcDst,
                                     const unsigned char* pcSrc, const int iLen)
{
    int i;
    int iTruncatedLen = iLen / 2;
    for (i=0; i<iTruncatedLen; ++i,++pcSrc)
    {
        *pcDst++ = '0' + ((*pcSrc & 0xf0) >> 4);
        *pcDst++ = '0' + (*pcSrc & 0x0f);
    }
    if (iLen % 2)
    {
        *pcDst++ = '0' + ((*pcSrc & 0xf0) >> 4);
    }
    return pcSrc;
}

BAS_EXPORT unsigned char* basAsc2Bcd(unsigned char* pcDst,
                                     const unsigned char* pcSrc, const int iLen)
{
    int i;
    for (i=0; i<iLen/2; ++i,pcSrc+=2)
    {
        *pcDst++ = ((*pcSrc & 0x0f) << 4) | (*(pcSrc+1) & 0x0f);
    }
    if ((iLen % 2) != 0)
    {   /* 奇数长度位，在最后补'F' */
        *pcDst++ = ((*pcSrc & 0x0f) << 4) | 0x0f;
    }
    return pcDst;
}

BAS_EXPORT unsigned char* basBc02Asc(unsigned char* pcDst,
                                     const unsigned char* pcSrc, const int iLen)
{
    int i;
    int iTruncatedLen = iLen / 2;
    if (iLen % 2)
    {   /* 奇数长度，去掉最前面补的'0' */
        *pcDst++ = '0' + (*pcSrc++ & 0x0f);
    }
    for (i=0; i<iTruncatedLen; ++i,++pcSrc)
    {
        *pcDst++ = '0' + ((*pcSrc & 0xf0) >> 4);
        *pcDst++ = '0' + (*pcSrc & 0x0f);
    }
    return pcSrc;
}

BAS_EXPORT unsigned char* basAsc2Bc0(unsigned char* pcDst,
                                     const unsigned char* pcSrc, const int iLen)
{
    int i;
    if ((iLen % 2) != 0)
    {   /* 奇数长度位，在最前面补'0' */
        *pcDst++ = *pcSrc++ & 0x0f;
    }
    for (i=0; i<iLen/2; ++i,pcSrc+=2)
    {
        *pcDst++ = ((*pcSrc & 0x0f) << 4) | (*(pcSrc+1) & 0x0f);
    }
    return pcDst;
}
