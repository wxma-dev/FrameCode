/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    lib_string.c                              *
* Description:    关于内存、字符串的lib                     *
* Author:         mawx                                      *
* Create Date:    2015/05/14                                *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>

#include "lib_pipe.h"

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    DoShell_ByPipe                            *
* Description:    将shell执行的结果输出                     *
* Author:         mawx                                      *
* Create Date:    2020/05/19                                *
* In:             aczCommand   shell命令                    *
*                                                           *
* Out:            sOut_Buf  shell执行的结果                 *
*                                                           *
* Return:         NULL失败                                  *
*                 非NULL 执行结果放入                       *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * DoShell_ByPipe( char *aczCommand, char * sOut_Buf )
{
	char    aczBuf[10240];
	FILE    *fp = NULL;

	memset( aczBuf, 0x00, sizeof( aczBuf) );

	if( (fp = popen( aczCommand,"r" ) ) != NULL )
    {
        if( fgets( aczBuf, 10240, fp) != NULL )
        {
            memcpy( sOut_Buf, aczBuf, strlen( aczBuf ) );
        }
    }
    else
    {
        pclose( fp );
        return NULL;
    }
 
	return sOut_Buf;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    AddStr_Realloc                            *
* Description:    在新分配的内存中，在尾部添加字符串        *
*                 原内存也必须是malloc类型函数分配的动态内存*
*                 新分配的内存长度 原内存字符串长度+添加字符串的长度+1 *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pSrc_Mem    原动态内存地址                *
*                 sAdd_String 需要添加的字符串              *
*                 pDst_Mem 新的动态内存的地址               *
* Out:            char * NULL失败                           *
*                 非NULL重新分配的内存地址                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * AddStr_Realloc( char * pSrc_Mem, const char * sAdd_String, char ** pDst_Mem )
{
	char * pRet;                    /* char *类型函数返回值 */
	int    iLength_AddStr;          /* 需要添加的字符串的长度 */
	int    iSize_NewMem;            /* 新内存的大小 */
	
	/* 变量初始化 */
	pRet = NULL;
	iLength_AddStr = 0;
	iSize_NewMem = 0;
	
	/* 计算添加的字符串的长度 */
	iLength_AddStr = strlen( sAdd_String );
	
	/* 计算新的内存大小 = 原字符串的长度 + 添加的字符串的大小 */
	iSize_NewMem = strlen( pSrc_Mem ) + iLength_AddStr + 1;
	
	/* 重新分配内存 */
	pRet = realloc( pSrc_Mem, iSize_NewMem );
	if ( NULL == pRet )
	{
		return NULL;
	}
	else
	{
		/* 添加字符串 */
		strcat( pSrc_Mem, sAdd_String );
		
		*pDst_Mem = pRet;
		return *pDst_Mem;
	}
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    AddStr_Free                               *
* Description:    释放AddStr_Realloc、AddStr_Malloc添加内存 *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_Mem    原动态内存地址              *
* Out:            void                                      *
*                                                           *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
void AddStr_Free( char ** pPoint_Mem )
{
	free( *pPoint_Mem );
	*pPoint_Mem = NULL;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Count_ChildStr_Num                        *
* Description:    判断源字符串中的子串的个数                *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String    源字符串                   *
*                 sChild_String 子字符串                    *
* Out:            int 个数                                  *
*                 -1 报错                                   *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Count_ChildStr_Num( char * sSrc_String, char * sChild_String )
{
	int  iCount;              /* 个数的统计 */
	char  * pPoint_Addr;      /* 查找的地址 */
	char  * pPoint_Ret;       /* 返回的指针 */

	/* 变量初始化 */
	iCount = 0;
	pPoint_Addr = NULL;
	pPoint_Ret = NULL;

	while ( 1 )
	{
		/* 如果是第一次，不是第一次，搜索的地方分别作处理 */
		if ( 0 == iCount )
		{
			pPoint_Addr = sSrc_String;
		}
		else
		{
			pPoint_Addr = pPoint_Ret + strlen( sChild_String ) ;
		}

		/* 根据地址，搜索子串 */
		pPoint_Ret = strstr( pPoint_Addr, sChild_String );
		if ( NULL != pPoint_Ret )
		{
			iCount++;
		}
		else
		{
			return -1;
		}
	}
	
	return iCount;
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Count_Char_Num                            *
* Description:    判断源字符串中的某个字符的个数            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String    源字符串                   *
*                 sChild_Char    字符                       *
* Out:            int 个数                                  *
*                 -1 错误                               *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Count_Char_Num( char * sSrc_String, char sChild_Char )
{
	char   s_point[ 1 + 1];
	char * p_Ret;

	memset( s_point, 0x00, sizeof( s_point ) );
	p_Ret   = NULL;

	p_Ret = Char_To_Str( sChild_Char, s_point );
	if ( NULL == p_Ret )
	{
		return -1;
	}

	return Count_ChildStr_Num(sSrc_String, s_point );
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Char_To_Str                               *
* Description:    字符转换成字符串                          *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             cSrc_Char    源字符                       *
*                 sDst_String  目标字符串，如果目标字符串的空间少于2个，则结果未可知*
* Out:            char * NULL失败，非NULL 目标字符串的地址  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Char_To_Str( const char cSrc_Char, char * sDst_String )
{
	
	/* 参数检查 */
	if ( NULL == sDst_String )
	{
		return NULL;
	}
	
	/* 进行字符串的赋值 */
	*sDst_String       = cSrc_Char;
	*(sDst_String + 1) = '\0';
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Str_To_Char                               *
* Description:    字符串转换成字符（字符串第一位为准）      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String 源字符串，若长度大于1，则以第一位的字符为准*
*                 sDst_Char   目标字符                      *                      
* Out:            char * NULL失败，源字符串的值             *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Str_To_Char( const char * sSrc_String, char sDst_Char )
{
	/* 参数检查 */
	if ( NULL == sSrc_String || 0 == strlen( sSrc_String ) )
	{
		return NULL;
	}

	/* 字符赋值 */
	sDst_Char = *sSrc_String;

	return sSrc_String;
}




/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    GetString_YYYYMMDD                        *
* Description:    获取当前时间的YYYYMMDD                    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_TimeMem  获取的YYYYMMDD放在该字符串中*
*                 pPoint_TimeMem 若是内存不够，结果未可知   *
* Out:            void                                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
void GetString_YYYYMMDD( char * pPoint_TimeMem )
{
	time_t timep;
	struct tm *p;

	time( &timep );
	p=gmtime( &timep );

	sprintf( pPoint_TimeMem, "%d%d%d%c", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday, '\0' ); 
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    GetString_HHMMSS                          *
* Description:    获取当前时间的HHMMSS                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_TimeMem  获取的HHMMSS放在该字符串中*
*                 pPoint_TimeMem 若是内存不够，结果未可知   *
* Out:            void                                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
void GetString_HHMMSS( char * pPoint_TimeMem )
{
	time_t timep;
	struct tm *p;
	
	time(&timep);
	p=gmtime(&timep);

	sprintf( pPoint_TimeMem, "%d%d%d%c", p->tm_hour, p->tm_min, p->tm_sec, '\0' );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    GetString_HHMMSS                          *
* Description:    获取当前时间的计算机的时间（秒数）        *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_TimeMem  获取的HHMMSS放在该字符串中*
*                 pPoint_TimeMem 若是内存不够，结果未可知   *
* Out:            void                                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
void GetString_ComputerSec( char * pPoint_TimeMem )
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday (&tv , &tz);

	sprintf( pPoint_TimeMem, "%d%c", tv.tv_sec, '\0' );
}

#if 0

typedef struct str_strtok{
	char st_A1[35+1];	
	......
	char st_A13[200+1];	
} STR_STRTOK;


/********************************************************************
* Function：														*
*	struct str_strtok * Get_Strtok_String( char * string, struct str_strtok * st_p , char * split )*
* Description：														*
* 	根据分隔字符串，获取分隔内容，放入结构体						*
*	字符串格式 "a1|a2|a3|...|an"									*
* In Params:														*
*	string: 源字符串												*
*	st_p: 存放的结构体指针											*
*	split: 用来分隔的字符串											*
* Out Params:														*
*																	*
* Return:															*
* 　NULL: 失败														*
* 　st_p: 成功，存放的结构体指针									*
********************************************************************/
struct str_strtok * Get_Strtok_String( char * string, struct str_strtok * st_p , char * split )
{
	char * p_point;
	int count;
	char * p_str;
	char * p;
	char str_tmp[1024];
	
	p_point = NULL;
	count = 0;
	p_str = NULL;
	p = NULL;
	memset( str_tmp, 0x00, sizeof( str_tmp ) );
	
	p_str = string;
	while( 1 )
	{
		count++;

		memset( str_tmp, 0x00, sizeof( str_tmp ) );
		p = strstr( p_str, split );
		if ( NULL == p  && 13 != count )
		{
			break;
		}
		else if ( NULL == p && 13 == count )
		{
			strcpy( str_tmp, p_str );
		} 
		else
		{
			strncpy( str_tmp, p_str, abs( p - p_str ) );
		}
		
		switch ( count )
		{
			case 1:
					strcpy( st_p->st_A1, str_tmp );break;
			....
			case 13:
					/*最后一个return*/
					strcpy( st_p->st_A13, str_tmp );
					return st_p;
			default:
					return NULL;
		}

		p_str = p + 1;

	}

	return st_p;
}

#endif


int Get_Strtok_String_Malloc( char * sIn, char * sOut , char * split )
{
	
	
}

int Get_Strtok_String_Free( char * sPoint_Malloc )
{
	free( sPoint_Malloc );
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Dx_String                                 *
* Description:    字符串倒序                                *
*                 如果目的字符串长度小于源字符串的长度，结果未可知*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 原字符串                      *
*                 sDst_string 目的字符串                    *
* Out:            NULL 失败                                 *
*                 非NULL 目的字符串地址                     *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char *Dx_String( char * sSrc_string, char * sDst_string )
{
	char * pP_mem;         /* 分配内存的地址 */
	int iLength;           /* 字符串的长度 */
	int iCount;          
	int iFor_Num;          /* for循环使用 */
	
	pP_mem = NULL;
	iLength = 0;
	iCount = 0;
	iFor_Num = 0;
	
	/* 检查必要的参数 */
	if ( NULL == sSrc_string || NULL == sDst_string )
	{
		return NULL;
	}
	
	/* 统计源字符串的长度 */
	iLength = strlen( sSrc_string );
	
	/* 分配能够放下倒叙字符串的空间 */
	pP_mem = ( char * )malloc( iLength + 1 );
	
	/* 空间初始化 */
	memset( pP_mem , 0x00, iLength + 1 );
	
	for ( iFor_Num = 0, iCount = 0; iFor_Num < iLength; iFor_Num++ )
	{
		/* 源字符串倒叙放入分配的空间 */
		*( pP_mem + iCount ) = *( sSrc_string + iLength - iCount -1 );
		
		/* 变量自加 */
		iCount++;
	}
	
	/* 直接将倒序好的字符串复制到目标空间 */
	strcpy( sDst_string, pP_mem );
	
	/* 释放分配的空间 */
	free( pP_mem );
	pP_mem = NULL;
	
	/* 返回目的地址 */
	return sDst_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Dx_String_Yzfc                            *
* Description:    字符串倒序(直接在源字符串的空间上倒叙)    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 原字符串,改成到倒序的字符串   *
* Out:            NULL 失败                                 *
*                 非NULL  成功，源字符串的地址              *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char *Dx_String_Yzfc( char * sSrc_string )
{
	char * pPoint_Mem;             /* 分配的地址空间 */
	int iLength_Str;               /* 字符串长度 */
	char * pRet;
	
	/* 变量初始化 */
	pPoint_Mem = NULL;
	iLength_Str = 0;
	pRet = NULL;
	
	/* 检查参数 */
	if ( NULL == sSrc_string )
	{
		return NULL;
	}
	
	/* 统计源字符串的长度 */
	iLength_Str = strlen( sSrc_string );
	
	/* 根据源字符串分配对应的空间 */
	pPoint_Mem = ( char * )malloc( iLength_Str + 1 );
	
	/* 分配的空间初始化 */
	memset( pPoint_Mem, 0x00, iLength_Str + 1 );
	
	/* 将源字符串倒叙至目标空间 */
	pRet = Dx_String( sSrc_string, pPoint_Mem );
	
	/* 如果倒叙失败，则return NULL */
	if ( NULL == pRet )
	{
		return NULL;
	}
	
	/* 初始化源字符串 */
	memset( sSrc_string, 0x00, strlen( sSrc_string ) );
	
	/* 将倒叙的字符串到拷贝至 源字符串的地址空间 */
	strcpy( sSrc_string, pPoint_Mem );
	
	/* 释放分配的空间 */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
	
	return sSrc_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Del_Char                                  *
* Description:    去掉字符串中的某一字符(去掉某字符后，源字符串合并)*
*                 如果这个字符在字符串中不存在，则认为删除成功 *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String 原字符串                      *
*                 cChar 去掉字符                            *
* Out:            NULL 失败                                 *
*                 非NULL  成功，源字符串的地址              *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Del_Char( char * sSrc_String, char cChar )
{
	int iCount;                   /* 统计使用 */
	int iLength_Str;              /* 字符串长度 */
	int iFor_Num;                 /* for循环使用 */
	char * pPoint_Mem;            /* 分配内存的地址 */
	
	/* 变量初始化 */
	iCount = 0;
	iLength_Str = 0;
	iFor_Num = 0;
	pPoint_Mem = NULL;
	
	/* 检查参数 */
	if ( NULL == sSrc_String )
	{
		return NULL;
	}
	
	/* 统计源字符串的长度 */
	iLength_Str = strlen( sSrc_String );
	
	/* 分配能够放下倒叙字符串的空间 */
	pPoint_Mem = ( char * )malloc( iLength_Str + 1 );
	
	/* 分配的空间初始化 */
	memset( pPoint_Mem , 0x00, iLength_Str + 1 );

	for ( iFor_Num = 0, iCount = 0; iFor_Num < iLength_Str; iFor_Num++ )
	{
		/* 如果查找到符合的字符则continue 不进行复制到目标空间 */
		if ( cChar == *( sSrc_String + iFor_Num ) )
		{
			continue;
		}
		else
		{
			/* 符合的字符串则进行复制至目标空间 */
			*( pPoint_Mem + iCount ) = *( sSrc_String + iFor_Num );
			iCount++;
		}
	}
	
	/* 初始化源字符串 */
	memset( sSrc_String, 0x00, iLength_Str );
	
	/* 将处理好的字符串复制至源字符的空间 */
	strcpy( sSrc_String, pPoint_Mem );
	
	/* 释放分配的空间 */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
	
	return sSrc_String;
}



/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Del_Char_CopyToDst                        *
* Description:    去掉字符串中的某一字符(去掉某字符后，目标字符串合并)*
*                 如果这个字符在字符串中不存在，则认为删除成功*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 原字符串                      *
*                 sDst_string 目的字符串，若长度不够，结果未可知*
*                 sCh 去掉字符                              *
* Out:            NULL 失败                                 *
*                 非NULL 成功，目标空间地址                 *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Del_Char_CopyToDst( char * sSrc_string, char * sDst_string, char sCh )
{
	char * pPoint_Mem;
	int iLength;
	char  *pRet;
	
	/* 变量初始化 */
	pPoint_Mem = NULL;
	iLength = 0;
	pRet = NULL;
	
	/* 检查变量 */
	if ( NULL == sSrc_string || NULL == sDst_string )
	{
		return NULL;
	}

	/* 源字符串长度 */ 
	iLength = strlen( sSrc_string );
	
	/* 分配能够放下源字符串的空间 */
	pPoint_Mem = ( char * )malloc( iLength + 1 );
	
	/* 空间初始化 */
	memset( pPoint_Mem , 0x00, iLength + 1 );
	
	/* 拷贝源字符串 */
	strcpy( pPoint_Mem, sSrc_string );

	/* 删除目标字符 */
	pRet = Del_Char( pPoint_Mem, sCh );
	if ( NULL == pRet )
	{
		return NULL;
	}
	
	/* 拷贝处理好的字符串至目标字符串的空间 */
	strcpy( sDst_string, pPoint_Mem );
	
	/* 释放空间 */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
	
	return sDst_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Del_Char_Hh                               *
* Description:    去掉字符串中的换行符                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 原字符串                       *
*                                                           *
* Out:            NULL 失败                                 *
*                 非NULL  源字符串地址                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Del_Char_Hh( char * sSrc_string)
{
	return Del_Char( sSrc_string, '\n' );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Replace_Str_To_Str_OrderByCount           *
* Description:    在源字符串中 替换 掉匹配上的第几个字符串  *
*                 如果没有找到需要替换的字符串，则返回NULL  *
*                 sDst_string 目标字符串长度不够的话，结果未可知*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 源字符串                      *
*                 sDst_string 目标字符串                    *
*                 sTh_Src_String 替换源字符串               *
*                 sTh_Dst_String 替换目标字符串             *
*                 iOrderCount    匹配的第几个字符串         *
* Out:            NULL 失败                                 *
*                 非NULL  目标字符串                        *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Replace_Str_To_Str_OrderByCount( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String, int iOrderCount )
{
	char * pRet;                             /* 指针的返回值 */
	char * pPoint_FindString;                /* 查找地址的指针 */
	char * pPoint_Mem;                       /* 分配内存的指针 */
	int    iLength_Dst_String;               /* 替换后需要的字符串长度 */
	int    iLengthString_Tb;                 /* 字符串的头部长度 */
	int    iLengthString_Zj;                 /* 字符串的中间长度 */
	int    iLengthString_Wb;                 /* 字符串的尾部长度 */
	char * pPoint_Addr;                      /* 地址指针 */
	int    iCount_Pp;                        /* 匹配到的字符串的个数 */

	/* 变量初始化 */
	pRet = NULL;
	pPoint_FindString = NULL;
	pPoint_Mem = NULL;
	iLength_Dst_String = 0;
	iLengthString_Tb = 0;
	iLengthString_Zj = 0;
	iLengthString_Wb = 0;
	pPoint_Addr = NULL;
	iCount_Pp = -1;

	/* 参数检查 */
	if ( NULL == sSrc_string || NULL == sDst_string || NULL == sTh_Src_String || NULL == sTh_Dst_String )
	{
		return NULL;
	}

	/* 匹配到的字符串的个数 */
	iCount_Pp = Count_ChildStr_Num( sSrc_string, sTh_Src_String );
	if ( iCount_Pp < iOrderCount )
	{
		return NULL;
	}

	int iCount_While = 0;
	while ( 1 )
	{
		/* 找到地址1 */
		pPoint_FindString = strstr( sSrc_string, sTh_Src_String );
		if ( NULL == pPoint_FindString )
		{
			return NULL;
		}
		else
		{
			/* 匹配到一次，自加1 */
			iCount_While++;

			/* 已经找到要匹配的地方 */
			if ( iCount_While == iOrderCount )
			{
				break;
			}
			else
			{
				/* 没有匹配到的话，则continue */
				continue;
			}
		}
	}

	/*	举例：
		"012345678901234567890123456789" 需要将"56"替换成"abc"
		
		地址为0x005,
		pPoint_FindString  = 0x005;
		iLengthString_Tb   = 5;
		iLengthString_Zj   = 3;
		iLengthString_Wb   = 30 - 2 - 5 = 23;
		iLength_Dst_String = 5 + 3 + 23 = 31;
		pPoint_Addr        = 0x005 + 2 = 0x007;
	*/

	/* 字符串的头部长度 */
	iLengthString_Tb = abs( sSrc_string - pPoint_FindString );
	
	/* 字符串的中间长度 */
	/* iLengthString_Zj = strlen( sTh_Dst_String ) - strlen( sTh_Src_String ); del by mawx@20180712 */
	iLengthString_Zj = strlen( sTh_Dst_String );
	
	/* 字符串的尾部长度 */
	iLengthString_Wb = strlen( sSrc_string ) - strlen( sTh_Src_String ) - iLengthString_Tb;
	
	/* 计算替换后需要的字符串长度 */
	iLength_Dst_String = iLengthString_Tb + iLengthString_Zj + iLengthString_Wb;
	
	/* 分配空间 */
	pPoint_Mem = ( char * )malloc( iLength_Dst_String );

	/* 计算地址2 */
	pPoint_Addr = pPoint_FindString + strlen( sTh_Src_String );

	/* 替换字符串 */
	memcpy( pPoint_Mem, sSrc_string, iLengthString_Tb );
	/* del by mawx@20180712 
	memcpy( pPoint_FindString, sTh_Dst_String, strlen( sTh_Dst_String ) );
	memcpy( pPoint_FindString + strlen( sTh_Dst_String ), pPoint_Addr, iLengthString_Wb ); */
	memcpy( pPoint_Mem + iLengthString_Tb, sTh_Dst_String, strlen( sTh_Dst_String ) );
	memcpy( pPoint_Mem + iLengthString_Tb + strlen( sTh_Dst_String ), pPoint_Addr, iLengthString_Wb );

	/* 拷贝至目标字符串 */
	memcpy( sDst_string, pPoint_Mem, iLength_Dst_String );

	/* 释放地址 */
	free ( pPoint_Mem );
	pPoint_Mem = NULL;

	return sDst_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Replace_Str_To_Str_First                  *
* Description:    在源字符串中替换掉第一个匹配对应的字符串  *
*                 如果没有找到需要替换的字符串，则返回NULL  *
*                 sDst_string 目标字符串长度不够的话，结果未可知*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 源字符串                      *
*                 sDst_string 目标字符串                    *
*                 sTh_Src_String 替换源字符串               *
*                 sTh_Dst_String 替换目标字符串             *
* Out:            NULL 失败                                 *
*                 非NULL  目标字符串                        *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Replace_Str_To_Str_First( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String )
{
	return Replace_Str_To_Str_OrderByCount( sSrc_string, sDst_string, \
											sTh_Src_String, sTh_Dst_String, 1 );
}

#if 0

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Replace_Str_To_Str_All                    *
* Description:    在源字符串中替换掉所有对应的字符串        *
*                 如果没有找到需要替换的字符串，则返回NULL  *
*                 sDst_string 目标字符串长度不够的话，结果未可知*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 源字符串                      *
*                 sDst_string 目标字符串                    *
*                 sTh_Src_String 替换源字符串               *
*                 sTh_Dst_String 替换目标字符串             *
* Out:            NULL 失败                                 *
*                 非NULL  目标字符串                        *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/ /* ??? 该函数没有完成*/
char * Replace_Str_To_Str_All( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String )
{
	int    iCount_Pp;                        /* 匹配到的字符串的个数 */
	int    iLength_Str_Tmp;                  /* 临时字符串的长度 */
	char  *pPoint;                           /* 临时指针 */
	char  *pPoint_Tmp;                       /* 临时指针 */
	int    iLength_pPoint;                   /* 分配的内存的长度 */
	char  *pPoint_Src;                       /* 临时指针 */

	iCount_Pp = -1;
	iLength_Str_Tmp = 0;
	pPoint = NULL;
	pPoint_Tmp = NULL;
	iLength_pPoint = 0;
	pPoint_Src = NULL;

	/* 匹配到的字符串的个数 */
	iCount_Pp = Count_ChildStr_Num( sSrc_string, sTh_Src_String );
	if ( iCount_Pp < iOrderCount )
	{
		return NULL;
	}

	/* 分配临时内存 */
	iLength_pPoint = strlen( sSrc_string ) + iCount_Pp*(strlen(sTh_Dst_String)) - iCount_Pp*(strlen(sTh_Src_String)) + 1;
	pPoint = ( char * )malloc( iLength_pPoint );
	memset( pPoint, 0x00, iLength_pPoint );
	
	pPoint_Src = ( char * )malloc( iLength_pPoint );
	memset( pPoint_Src, 0x00, iLength_pPoint );

	memcpy( pPoint_Src, sSrc_string, strlen( sSrc_string ) );

	int i = 0;
	for ( i = 0 ; i < iCount_Pp; i++ )
	{
		if ( NULL == Replace_Str_To_Str_OrderByCount( pPoint_Src, pPoint, \
										sTh_Src_String, sTh_Dst_String, i ) )
		{
			free( pPoint );
			pPoint = NULL;
			return NULL;
		}

		pPoint_Tmp = pPoint;
		memset( pPoint, 0x00, sizeof( iLength_pPoint ) );

		iLength_Str_Tmp = strlen( sDst_string );
	}

	free( pPoint );
	pPoint = NULL;
	
	return NULL;
}
#endif

#if 0
/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Replace_Str_To_Str_All                    *
* Description:    删除字符串中的所有匹配到的子串            *
*                 如果没有找到需要替换的字符串，则返回NULL  *
*                 sDst_string 目标字符串长度不够的话，结果未可知*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 源字符串                      *
*                 sDst_string 目标字符串                    *
*                 sTh_Src_String 替换源字符串               *
*                 sTh_Dst_String 替换目标字符串             *
* Out:            NULL 失败                                 *
*                 非NULL  目标字符串                        *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Del_Str_To_Str_All( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String )
{
	???
}
#endif

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Del_Char_File                             *
* Description:    批量删除某文件中的某一字符                *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sFilename 文件名(绝对路径，或者相对路径)   *
*                 cCh 需要删除掉的字符                      *
* Out:            -1 失败                                   *
*                 0  成功                                   *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Del_Char_File( char * sFilename, char cCh )
{
	FILE * fp;                  /* 文件指针 */
	char sTmp_Buf[10240];       /* 临时存放字符串buf */
	int iLen_Seek;              /* 文件偏移量 */
	int iLength_i;              /* 长度i */
	int iLength_j;              /* 长度j */
	int iSize_File;             /* 文件的大小 */
	int iLen_Add_Seek;          /* 文件尾部添加的文件偏移量 */
	int iLen_Read_Seek;         /* 读文件的文件偏移量 */
	
	/* 变量初始化 */
	fp = NULL;
	memset( sTmp_Buf, 0x00, sizeof( sTmp_Buf ) );
	iLen_Seek = 0;
	iLength_i = 0;
	iLength_j = 0;
	iSize_File = 0;
	iLen_Add_Seek = 0;
	iLen_Read_Seek = 0;

	/* 写方式打开文件 */
	fp = fopen( sFilename, "w+" );
	if ( NULL == fp )
	{
		return -1;
	}

	/* 获取该文件的文件大小，由此可以得到文件的总的偏移量 */

	/* 文件大小即文件尾部的文件偏移量 */
	iLen_Add_Seek = iSize_File;

	/* 循环读取文件 */
	while( NULL != fgets( sTmp_Buf, sizeof( sTmp_Buf ), fp ) )
	{
		/* 计算读取的长度 */
		iLength_i = strlen( sTmp_Buf );

		/* 删除某字符 */
		if ( NULL == Del_Char( sTmp_Buf, cCh ) )
		{
			return -1;
		}

		/* 计算处理后字符串的长度 */
		iLength_j = strlen( sTmp_Buf );
		
		/* 重置写文件的偏移量 */
		fseek( fp , iLen_Add_Seek, SEEK_SET );

		/* 处理好后的字符串，写入文件尾部 */
		fputs( sTmp_Buf, fp ); 

		/* 重新计算追加文件尾部操作的偏移量 */
		iLen_Add_Seek = iLen_Add_Seek + iLength_j;

		/* 重新计算读文件的偏移量 */
		iLen_Read_Seek = iLen_Read_Seek + iLength_i;

		/* 重置读文件的偏移量 */
		fseek( fp , iLen_Read_Seek, SEEK_SET );

		/* 临时存放字符串buf初始化 */
		memset( sTmp_Buf, 0x00, sizeof( sTmp_Buf ) );
	}

	/* 文件偏移量置为0 */
	fseek( fp , 0, SEEK_SET );

	/* 将文件的原来部分，全部置为空 */

	fclose( fp );

	return 0;
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Trim_Before                               *
* Description:    去掉字符串前面的空格                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sString 源字符串                          *
* Out:                                                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
void Trim_Before( char * sString ) 
{
	char *pPoint_Mem;             /* 分配内存的地址 */
	char *pPoint_tmp;             /* 临时指针 */
	int iCount;                   /* 统计数目 */
	int iLength;                  /* 字符串的长度 */
	int  iFlag_Yd;                /* 字符串前面是否有空格的标识，0-没有空格，1-有空格 */
	
	/* 变量初始化 */
	pPoint_Mem = NULL;
	pPoint_tmp = NULL;
	iCount = 0;
	iLength = 0;
	iFlag_Yd = 0;

	/* 源字符串的长度 */
	iLength = strlen( sString );
	
	/* 分配空间 */
	pPoint_Mem = ( char *)malloc( iLength + 1 );
	
	/* 空间初始化 */
	memset( pPoint_Mem, 0x00, iLength + 1 );
	
	
	while( 1 )
	{
		if ( ' ' != *( sString + iCount ) )
		{
			/* 源字符串前面没有空格，则跳出循环 */
			break;
		}
		else
		{
			/* 字符串前面是否有空格的标识，0-没有空格，1-有空格 */
			iFlag_Yd = 1;
			
			/* 空间初始化 */
			memset( pPoint_Mem, 0x00, iLength + 1 );
			
			/* 源字符串前面有空，则删除空格 */
			strcpy( pPoint_Mem, sString + iCount + 1 );
		}
		
		iCount++;
	}
	
	/* 有空格，存在字符串拷贝的问题 */
	if ( 1 == iFlag_Yd ) 
	{
		/* 源字符串初始化 */
		memset( sString, 0x00, iLength );
		
		/* 将处理好的字符串复制至源字符的空间 */
		strcpy( sString, pPoint_Mem );
	}
	else
	{
		/* 没有空格的化，就没有字符串拷贝的问题，就不做任何动作 */
		/* do nothing */
	}
	
	/* 释放分配的内存 */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
}



/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Trim_Behind                               *
* Description:    去掉字符串后面的空格                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sString 源字符串                          *
* Out:            NULL 失败                                 *
*                 非NULL 成功，源字符串地址                 *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char *Trim_Behind( char * sString ) 
{
	char * pRet;              /* 函数的返回值 */
	
	/* 变量初始化 */
	pRet = NULL;

	/* 源字符串倒叙 */
	pRet = Dx_String_Yzfc( sString );
	if ( NULL == pRet )
	{
		return NULL;
	}
	
	/* 删除倒叙字符串前面的空格 */
	Trim_Before( sString );
	
	/* 在重新倒叙字符串 */
	pRet = Dx_String_Yzfc( sString );
	if ( NULL == pRet )
	{
		return NULL;
	}

	return sString;
}

 
/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Trim                                      *
* Description:    删除字符串前后的空格                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sString 源字符串                          *
* Out:            void                                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
void Trim( char * sString ) 
{
	/* 去掉字符串前面的空格 */
	Trim_Before( sString );
	
	/* 去掉字符串后面的空格 */
	Trim_Behind( sString );
}



/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Trim_And_Del_Hh                           *
* Description:    去掉字符串中的换行符、空格                *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sString 源字符串                          *
* Out:            char * NULL 失败，其他城                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Trim_Kg_Hh( char * sString ) 
{
	/* 参数检查 */
	if ( NULL == sString )
	{
		return NULL;
	}
	
	/* 去掉字符中的换行 */
	if ( NULL == Del_Char_Hh( sString ) )
	{
		return NULL;
	}

	/* 去掉字符串前后空格 */
	Trim( sString );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_String_Behind_Char                    *
* Description:    取得字符cChar后的字符串(若在源字符串中没有对应字符，则认为失败)*
*                 目标字符串空间不够，结果未可知            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 源字符串                      *
*                 cChar 字符                                *
*                 sDst_string 目标字符串                    *
* Out:            NULL 失败                                 *
*                 sDst_string 成功,取得的字符串存放在dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Behind_Char( const char * sSrc_string , char cChar, char * sDst_string )
{
	char * pPoint_Str;
	
	/* 变量初始化 */
	pPoint_Str = NULL;
	
	/* 参数检查 */
	if ( NULL == sSrc_string )
	{
		return NULL;
	}
	
	/* 获取某字符的地址 */
	pPoint_Str = strchr( sSrc_string, cChar );
	if ( NULL == pPoint_Str )
	{
		return NULL;
	}

	/* 拷贝字符串至目标区域 */
	strcpy( sDst_string, pPoint_Str + 1 );
	
	return sDst_string;
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_String_Before_Char                    *
* Description:    取得字符cChar前的字符串(若在源字符串中没有对应字符，则认为失败)*
*                 目标字符串空间不够，结果未可知            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string 源字符串                      *
*                 cChar 字符                                *
*                 sDst_string 目标字符串                    *
* Out:            NULL 失败                                 *
*                 sDst_string 成功,取得的字符串存放在dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Before_Char( const char * sSrc_string , char cChar, char * sDst_string )
{
	char * pPoint_Str;                    /* 某字符串的地址 */
	char * pPoint_Copy_Start;             /* 拷贝开始的地址 */
	int       iLength_Copy;               /* 拷贝结束的地址 */
	
	/* 变量初始化 */
	pPoint_Str = NULL;
	pPoint_Copy_Start = NULL;
	iLength_Copy = 0;
	
	/* 参数检查 */
	if ( NULL == sSrc_string )
	{
		return NULL;
	}
	
	/* 获取某字符的地址 */
	pPoint_Str = strchr( sSrc_string, cChar );
	if ( NULL == pPoint_Str )
	{
		return NULL;
	}

	/* 计算开始的地址 */
	pPoint_Copy_Start = sSrc_string;
	
	/* 计算拷贝的长度 */
	iLength_Copy = abs( pPoint_Copy_Start - pPoint_Str );
	
	/* 拷贝字符串内存至目标区域 */
	memcpy( sDst_string, pPoint_Copy_Start, iLength_Copy );
	
	/* 拷贝'\0' */
	memset( sDst_string + iLength_Copy, 0x00, 1 );
	
	return sDst_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_String_Behind_Dh                      *
* Description:    取得字符'='后的字符串(原生的，没有去掉空格、换行符)*
*                 (若在源字符串中没有对应字符，则认为失败)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_string 源字符串                       *
*                 ch 字符                                   *
* Out:            NULL 失败                                 *
*                 dst_string 成功,取得的字符串存放在dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Behind_Dh( char * src_string , char * dst_string )
{
	return Get_String_Behind_Char( src_string, '=', dst_string );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_String_Before_Dh                      *
* Description:    取得字符'='前的字符串(原生的，没有去掉空格、换行符)*
*                 (若在源字符串中没有对应字符，则认为失败)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_string 源字符串                       *
*                 ch 字符                                   *
* Out:            NULL 失败                                 *
*                 dst_string 成功,取得的字符串存放在dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Before_Dh( char * src_string , char * dst_string )
{
	return Get_String_Before_Char( src_string, '=', dst_string );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_String_Behind_Dh_QdKgHh               *
* Description:    取得字符'='后的字符串(去掉空格、换行符)   *
*                 (若在源字符串中没有对应字符，则认为失败)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String 源字符串                      *
*                 ch 字符                                   *
* Out:            char *                                    *
*                 NULL 失败                                 *
*                 sDst_String 成功,取得的字符串存放在dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Behind_Dh_QdKgHh( char * sSrc_String , char * sDst_String )
{
	char * p;

	/* 变量初始化 */
	p = NULL;

	/* 获取=号后面的字符串 */
	p = Get_String_Behind_Char( sSrc_String, '=', sDst_String );
	if ( NULL == p )
	{
		return NULL;
	}
	
	/* 删除换行 */
	if ( NULL == Del_Char_Hh( sDst_String ) )
	{
		return NULL;
	}
	
	/* 删除空格 */
	Trim( sDst_String );
	
	return sDst_String;
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_String_Before_Dh_QdKgHh               *
* Description:    取得字符'='前的字符串(去掉空格、换行符)   *
*                 (若在源字符串中没有对应字符，则认为失败)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String 源字符串                      *
*                 ch 字符                                   *
* Out:            char *                                    *
*                 NULL 失败                                 *
*                 sDst_String 成功,取得的字符串存放在dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Before_Dh_QdKgHh( char * sSrc_String , char * sDst_String )
{
	char * p;

	/* 变量初始化 */
	p = NULL;

	/* 获取=号前面的字符串 */
	p = Get_String_Before_Char( sSrc_String, '=', sDst_String );
	if ( NULL == p )
	{
		return NULL;
	}
	
	/* 删除换行 */
	if ( NULL == Del_Char_Hh( sDst_String ) )
	{
		return NULL;
	}
	
	/* 删除空格 */
	Trim( sDst_String );
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_String_Behind_Dh_QdKg                 *
* Description:    取得字符'='后的字符串(去掉空格)           *
*                 (若在源字符串中没有对应字符，则认为失败)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String 源字符串                      *
* Out:            NULL 失败                                 *
*                 sDst_String 成功,取得的字符串存放在dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Behind_Dh_QdKg( char * sSrc_String , char * sDst_String )
{
	char * p;

	/* 变量初始化 */
	p = NULL;

	/* 获取=号后面的字符串 */
	p = Get_String_Behind_Char( sSrc_String, '=', sDst_String );
	if ( NULL == p )
	{
		return NULL;
	}

	/* 删除空格 */
	Trim( sDst_String );
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_String_Before_Dh_QdKg                 *
* Description:    取得字符'='前的字符串(去掉空格)           *
*                 (若在源字符串中没有对应字符，则认为失败)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String 源字符串                      *
* Out:            NULL 失败                                 *
*                 sDst_String 成功,取得的字符串存放在dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Before_Dh_QdKg( char * sSrc_String , char * sDst_String )
{
	char * p;

	/* 变量初始化 */
	p = NULL;

	/* 获取=号后面的字符串 */
	p = Get_String_Before_Char( sSrc_String, '=', sDst_String );
	if ( NULL == p )
	{
		return NULL;
	}
	
	/* 删除空格 */
	Trim( sDst_String );
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_KeyValue_Dh_QdKg                      *
* Description:    取得字符'='两边的字符串(去掉空格)，即获取key-value的值*
*                 (若在源字符串中没有对应字符，则认为失败)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String 源字符串                      *
* Out:            NULL 失败                                 *
*                 sDst_KeyString   Key的字符串值，成功返回  *
*                 sDst_ValueString  Value的字符串值         *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_KeyValue_Dh_QdKg( char * sSrc_String , char * sDst_KeyString, char * sDst_ValueString )
{
	/* 检查参数 */
	if ( NULL == sSrc_String || NULL == sDst_KeyString || NULL == sDst_ValueString )
	{
		return NULL;
	}

	/* 获取=后面的字符串 */
	if ( NULL == Get_String_Behind_Dh_QdKg( sSrc_String, sDst_ValueString ) )
	{
		return NULL;
	}
	
	/* 获取=前面的字符串 */
	if ( NULL == Get_String_Before_Dh_QdKg( sSrc_String, sDst_KeyString ) )
	{
		return NULL;
	}

	return sDst_KeyString;
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_String_Between_Zkh                    *
* Description:    取得中括号内的的字符串                    *
*                 (若在源字符串中没有对应字符，则认为失败)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String 源字符串                      *
* Out:            NULL 失败                                 *
*                 sDst_String 成功,取得的字符串存放在dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Between_Zkh( const char * sSrc_String , char * sDst_String )
{
	int  iLength_Src_String;                /* 源字符串的长度 */
	char  *pPoint_Mem;                      /* 临时存放字符串buf */
	
	/* 变量初始化 */
	iLength_Src_String = 0;
	pPoint_Mem = NULL;
	
	/* 检查参数 */
	if ( NULL == sSrc_String || NULL == sDst_String )
	{
		return NULL;
	}
	
	/* 计算源字符串的长度 */
	iLength_Src_String = strlen( sSrc_String );
	
	/* 分配内存 */
	pPoint_Mem = ( char * )malloc( iLength_Src_String + 1 );
	
	/* 分配内存初始化 */
	memset( pPoint_Mem, 0x00, iLength_Src_String + 1 );
	
	/* 获取[后面的字符串 */
	if ( NULL == Get_String_Behind_Char( sSrc_String, '[', pPoint_Mem ) )
	{
		return NULL;
	}
	
	/* 获取]前面的字符串 */
	if ( NULL == Get_String_Before_Char( pPoint_Mem, ']', sDst_String ) )
	{
		return NULL;
	}
	
	/* 释放分配的空间 */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Is_Digit_ByString                         *
* Description:    字符串内容是否都是符合数字规则(可带小数点)*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sDigit_Str 需要判断的字符串               *
* Out:            0 非数字                                  *
*                 1 数字                                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Is_Digit_ByString( const char * sDigit_Str )
{
	int iCount;
	int iNum_For;
	int iLength_Str;

	iNum_For = 0;
	iCount = 0;
	iLength_Str = 0;
	
	iLength_Str = strlen( sDigit_Str );
	for( iNum_For = 0; iNum_For < iLength_Str; iNum_For++ )
	{
		if (  48 <= ( *( sDigit_Str + iNum_For ) )  && 57 >= ( *( sDigit_Str + iNum_For ) ) )
		{
			continue;
		}
		else if ( 46 == ( *( sDigit_Str + iNum_For ) ) )
		{
			iCount++;
			if ( 2 <= iCount || 0 == iNum_For || iLength_Str == ( iNum_For + 1 ) ) 
			{
				return 0;
			}
			continue;
		}
		else
		{
			return 0;
		}
	}

	return 1;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Is_Digit_ByChar                           *
* Description:    字符内容是否都是符合数字规则(可带小数点)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sDigit_Str 需要判断的字符串               *
* Out:            0 非数字                                  *
*                 1 数字                                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Is_Digit_ByChar( const char sDigit_Str )
{
	char pPoint_String[2];         /* 临时字符串 */
	
	/* 变量初始化 */
	memset( pPoint_String, 0x00, sizeof( pPoint_String ) );
	
	/* 转换成字符串 */
	Char_To_Str( sDigit_Str, pPoint_String );
	
	return Is_Digit_ByString( pPoint_String );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Is_EnglishChar_ByString                   *
* Description:    字符串内容是否都是英文字母的规则          *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sEnglishChar_Str  需要判断的字符串        *
* Out:            0 有非字母                                *
*                 1 只有小写字母                            *
*                 2 只有大写字母                            *
*                 3 大写字母、小写字母都有                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Is_EnglishChar_ByString( const char * sEnglishChar_Str )
{
	int iNum_For;               /* for循环使用 */
	int iLength_Str;            /* 字符串的长度 */
	int iIs_Xx;                 /* 小写字母的个数统计 */
	int iIs_Dx;                 /* 大写字母的个数统计 */

	/* 变量初始化 */
	iNum_For = 0;
	iLength_Str = 0;
	
	/* 字符串的长度 */
	iLength_Str = strlen( sEnglishChar_Str );
	
	/* 循环检查 */
	for( iNum_For = 0; iNum_For < iLength_Str; iNum_For++ )
	{
		/* 65即A，90即Z；为大写字母 */
		if (  65 <= ( *( sEnglishChar_Str + iNum_For ) )  && 90 >= ( *( sEnglishChar_Str + iNum_For ) ) )
		{
			iIs_Dx++;
			continue;
		}
		/* 97即a，122即z；为小写字母 */
		else if (  97 <= ( *( sEnglishChar_Str + iNum_For ) )  && 122 >= ( *( sEnglishChar_Str + iNum_For ) ) )
		{
			iIs_Xx++;
			continue;
		}
		else
		{
			return 0;
		}
	}

	if ( 0 != iIs_Xx && 0 == iIs_Dx )
	{
		return 1;
	}
	else if ( 0 == iIs_Xx && 0 != iIs_Dx )
	{
		return 2;
	}
	else if ( 0 != iIs_Xx && 0 != iIs_Dx )
	{
		return 3;
	}
	else
	{
		return 0;
	}
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Is_EnglishChar_ByChar                     *
* Description:    字符内容是否都是英文字母的规则            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sEnglishChar_Str  需要判断的字符串        *
* Out:            0 有非字母                                *
*                 1 只有小写字母                            *
*                 2 只有大写字母                            *
*                 3 大写字母、小写字母都有                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Is_EnglishChar_ByChar( const char sEnglishChar_Str )
{
	char pPoint_String[2];         /* 临时字符串 */
	
	/* 变量初始化 */
	memset( pPoint_String, 0x00, sizeof( pPoint_String ) );
	
	/* 转换成字符串 */
	Char_To_Str( sEnglishChar_Str, pPoint_String );
	
	return Is_EnglishChar_ByString( pPoint_String );
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Is_EnglishChar_Digit_ByString             *
* Description:    字符串内容是否都是英文字母、数字（带小数点）的规则 *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sEnglishChar_Str  需要判断的字符串        *
* Out:            0 非字母，也非数字或小数点                *
*                 1 纯 只有小写字母                         *
*                 2 纯 只有大写字母                         *
*                 3 纯 大写字母、小写字母都有               *
*                 4 纯 数字(包括小数点)                     *
*                 5 数字(包括小数点)、英文字母混合          *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Is_EnglishChar_Digit_ByString( const char * sSrc_Str )
{
	int iNum_For;                 /* for循环使用 */
	int iLength_Str;              /* 字符串的长度 */
	char  cChar_Tmp;              /* 临时字符 */
	char  sStr_Tmp[2];            /* 临时字符串 */
	int  iCount_Digit;            /* 数字个数 */
	int  iCount_EnglishChar1;     /* 英文字母个数 */
	int  iCount_EnglishChar2;     /* 英文字母个数 */
	int  iCount_EnglishChar3;     /* 英文字母个数 */
	int  iRet;                    /* 函数的返回值 */
	
	/* 变量初始化 */
	iNum_For = 0;
	iLength_Str = 0;
	memset( sStr_Tmp, 0x00, sizeof( sStr_Tmp ) );
	iCount_Digit = 0;
	iCount_EnglishChar1 = 0;
	iCount_EnglishChar2 = 0;
	iCount_EnglishChar3 = 0;
	iRet = 0;
	
	/* 获取字符串的长度 */
	iLength_Str =  strlen( sSrc_Str );
	
	/* 循环判断 */
	for ( iNum_For = 0; iNum_For < iLength_Str; iNum_For++ )
	{
		/* 将字符抽取出来 */
		cChar_Tmp = *(sSrc_Str + iNum_For);
		
		/* 是英文字母或者是数字（包括小数点），则认为正确 */
		if ( Is_Digit_ByChar( cChar_Tmp ) )
		{
			iCount_Digit++;
			continue;
		}
		if ( iRet == Is_EnglishChar_ByChar( cChar_Tmp ) )
		{
			if ( 1 == iRet )
			{
				iCount_EnglishChar1++;
				continue;
			}
			else if ( 2 == iRet )
			{
				iCount_EnglishChar2++;
				continue;
			}
			else if ( 3 == iRet )
			{
				iCount_EnglishChar3++;
				continue;
			}
		}
		else
		{
			return 0;
		}
	}
	
	/*
	1 纯 只有小写字母               
	2 纯 只有大写字母               
	3 纯 大写字母、小写字母都有     
	4 纯 数字(包括小数点)           
	5 数字(包括小数点)、英文字母混合
	*/
	if ( iLength_Str == iCount_Digit )
	{
		return 1;
	}
	else if ( iLength_Str == iCount_EnglishChar1 )
	{
		return 2;
	}
	else if ( iLength_Str == iCount_EnglishChar2 )
	{
		return 3;
	}
	else if ( iLength_Str == iCount_EnglishChar3)
	{
		return 4;
	}
	else
	{
		return 5;
	}
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    GetStr_Dkh_Between                        *
* Description:    获取大括号之间的字符串                    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sIn_Str  需要处理的字符串                 *
* Out:            sOut_Str  处理好的字符串                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int GetStr_Dkh_Between( const char * sIn_Str, char * sOut_Str )
{
    
    
    
}























