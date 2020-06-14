/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    lib_string.c                              *
* Description:    �����ڴ桢�ַ�����lib                     *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    DoShell_ByPipe                            *
* Description:    ��shellִ�еĽ�����                     *
* Author:         mawx                                      *
* Create Date:    2020/05/19                                *
* In:             aczCommand   shell����                    *
*                                                           *
* Out:            sOut_Buf  shellִ�еĽ��                 *
*                                                           *
* Return:         NULLʧ��                                  *
*                 ��NULL ִ�н������                       *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    AddStr_Realloc                            *
* Description:    ���·�����ڴ��У���β������ַ���        *
*                 ԭ�ڴ�Ҳ������malloc���ͺ�������Ķ�̬�ڴ�*
*                 �·�����ڴ泤�� ԭ�ڴ��ַ�������+����ַ����ĳ���+1 *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pSrc_Mem    ԭ��̬�ڴ��ַ                *
*                 sAdd_String ��Ҫ��ӵ��ַ���              *
*                 pDst_Mem �µĶ�̬�ڴ�ĵ�ַ               *
* Out:            char * NULLʧ��                           *
*                 ��NULL���·�����ڴ��ַ                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * AddStr_Realloc( char * pSrc_Mem, const char * sAdd_String, char ** pDst_Mem )
{
	char * pRet;                    /* char *���ͺ�������ֵ */
	int    iLength_AddStr;          /* ��Ҫ��ӵ��ַ����ĳ��� */
	int    iSize_NewMem;            /* ���ڴ�Ĵ�С */
	
	/* ������ʼ�� */
	pRet = NULL;
	iLength_AddStr = 0;
	iSize_NewMem = 0;
	
	/* ������ӵ��ַ����ĳ��� */
	iLength_AddStr = strlen( sAdd_String );
	
	/* �����µ��ڴ��С = ԭ�ַ����ĳ��� + ��ӵ��ַ����Ĵ�С */
	iSize_NewMem = strlen( pSrc_Mem ) + iLength_AddStr + 1;
	
	/* ���·����ڴ� */
	pRet = realloc( pSrc_Mem, iSize_NewMem );
	if ( NULL == pRet )
	{
		return NULL;
	}
	else
	{
		/* ����ַ��� */
		strcat( pSrc_Mem, sAdd_String );
		
		*pDst_Mem = pRet;
		return *pDst_Mem;
	}
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    AddStr_Free                               *
* Description:    �ͷ�AddStr_Realloc��AddStr_Malloc����ڴ� *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_Mem    ԭ��̬�ڴ��ַ              *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Count_ChildStr_Num                        *
* Description:    �ж�Դ�ַ����е��Ӵ��ĸ���                *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String    Դ�ַ���                   *
*                 sChild_String ���ַ���                    *
* Out:            int ����                                  *
*                 -1 ����                                   *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Count_ChildStr_Num( char * sSrc_String, char * sChild_String )
{
	int  iCount;              /* ������ͳ�� */
	char  * pPoint_Addr;      /* ���ҵĵ�ַ */
	char  * pPoint_Ret;       /* ���ص�ָ�� */

	/* ������ʼ�� */
	iCount = 0;
	pPoint_Addr = NULL;
	pPoint_Ret = NULL;

	while ( 1 )
	{
		/* ����ǵ�һ�Σ����ǵ�һ�Σ������ĵط��ֱ������� */
		if ( 0 == iCount )
		{
			pPoint_Addr = sSrc_String;
		}
		else
		{
			pPoint_Addr = pPoint_Ret + strlen( sChild_String ) ;
		}

		/* ���ݵ�ַ�������Ӵ� */
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Count_Char_Num                            *
* Description:    �ж�Դ�ַ����е�ĳ���ַ��ĸ���            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String    Դ�ַ���                   *
*                 sChild_Char    �ַ�                       *
* Out:            int ����                                  *
*                 -1 ����                               *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Char_To_Str                               *
* Description:    �ַ�ת�����ַ���                          *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             cSrc_Char    Դ�ַ�                       *
*                 sDst_String  Ŀ���ַ��������Ŀ���ַ����Ŀռ�����2��������δ��֪*
* Out:            char * NULLʧ�ܣ���NULL Ŀ���ַ����ĵ�ַ  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Char_To_Str( const char cSrc_Char, char * sDst_String )
{
	
	/* ������� */
	if ( NULL == sDst_String )
	{
		return NULL;
	}
	
	/* �����ַ����ĸ�ֵ */
	*sDst_String       = cSrc_Char;
	*(sDst_String + 1) = '\0';
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Str_To_Char                               *
* Description:    �ַ���ת�����ַ����ַ�����һλΪ׼��      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String Դ�ַ����������ȴ���1�����Ե�һλ���ַ�Ϊ׼*
*                 sDst_Char   Ŀ���ַ�                      *                      
* Out:            char * NULLʧ�ܣ�Դ�ַ�����ֵ             *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Str_To_Char( const char * sSrc_String, char sDst_Char )
{
	/* ������� */
	if ( NULL == sSrc_String || 0 == strlen( sSrc_String ) )
	{
		return NULL;
	}

	/* �ַ���ֵ */
	sDst_Char = *sSrc_String;

	return sSrc_String;
}




/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    GetString_YYYYMMDD                        *
* Description:    ��ȡ��ǰʱ���YYYYMMDD                    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_TimeMem  ��ȡ��YYYYMMDD���ڸ��ַ�����*
*                 pPoint_TimeMem �����ڴ治�������δ��֪   *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    GetString_HHMMSS                          *
* Description:    ��ȡ��ǰʱ���HHMMSS                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_TimeMem  ��ȡ��HHMMSS���ڸ��ַ�����*
*                 pPoint_TimeMem �����ڴ治�������δ��֪   *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    GetString_HHMMSS                          *
* Description:    ��ȡ��ǰʱ��ļ������ʱ�䣨������        *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_TimeMem  ��ȡ��HHMMSS���ڸ��ַ�����*
*                 pPoint_TimeMem �����ڴ治�������δ��֪   *
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
* Function��														*
*	struct str_strtok * Get_Strtok_String( char * string, struct str_strtok * st_p , char * split )*
* Description��														*
* 	���ݷָ��ַ�������ȡ�ָ����ݣ�����ṹ��						*
*	�ַ�����ʽ "a1|a2|a3|...|an"									*
* In Params:														*
*	string: Դ�ַ���												*
*	st_p: ��ŵĽṹ��ָ��											*
*	split: �����ָ����ַ���											*
* Out Params:														*
*																	*
* Return:															*
* ��NULL: ʧ��														*
* ��st_p: �ɹ�����ŵĽṹ��ָ��									*
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
					/*���һ��return*/
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Dx_String                                 *
* Description:    �ַ�������                                *
*                 ���Ŀ���ַ�������С��Դ�ַ����ĳ��ȣ����δ��֪*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string ԭ�ַ���                      *
*                 sDst_string Ŀ���ַ���                    *
* Out:            NULL ʧ��                                 *
*                 ��NULL Ŀ���ַ�����ַ                     *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char *Dx_String( char * sSrc_string, char * sDst_string )
{
	char * pP_mem;         /* �����ڴ�ĵ�ַ */
	int iLength;           /* �ַ����ĳ��� */
	int iCount;          
	int iFor_Num;          /* forѭ��ʹ�� */
	
	pP_mem = NULL;
	iLength = 0;
	iCount = 0;
	iFor_Num = 0;
	
	/* ����Ҫ�Ĳ��� */
	if ( NULL == sSrc_string || NULL == sDst_string )
	{
		return NULL;
	}
	
	/* ͳ��Դ�ַ����ĳ��� */
	iLength = strlen( sSrc_string );
	
	/* �����ܹ����µ����ַ����Ŀռ� */
	pP_mem = ( char * )malloc( iLength + 1 );
	
	/* �ռ��ʼ�� */
	memset( pP_mem , 0x00, iLength + 1 );
	
	for ( iFor_Num = 0, iCount = 0; iFor_Num < iLength; iFor_Num++ )
	{
		/* Դ�ַ�������������Ŀռ� */
		*( pP_mem + iCount ) = *( sSrc_string + iLength - iCount -1 );
		
		/* �����Լ� */
		iCount++;
	}
	
	/* ֱ�ӽ�����õ��ַ������Ƶ�Ŀ��ռ� */
	strcpy( sDst_string, pP_mem );
	
	/* �ͷŷ���Ŀռ� */
	free( pP_mem );
	pP_mem = NULL;
	
	/* ����Ŀ�ĵ�ַ */
	return sDst_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Dx_String_Yzfc                            *
* Description:    �ַ�������(ֱ����Դ�ַ����Ŀռ��ϵ���)    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string ԭ�ַ���,�ĳɵ�������ַ���   *
* Out:            NULL ʧ��                                 *
*                 ��NULL  �ɹ���Դ�ַ����ĵ�ַ              *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char *Dx_String_Yzfc( char * sSrc_string )
{
	char * pPoint_Mem;             /* ����ĵ�ַ�ռ� */
	int iLength_Str;               /* �ַ������� */
	char * pRet;
	
	/* ������ʼ�� */
	pPoint_Mem = NULL;
	iLength_Str = 0;
	pRet = NULL;
	
	/* ������ */
	if ( NULL == sSrc_string )
	{
		return NULL;
	}
	
	/* ͳ��Դ�ַ����ĳ��� */
	iLength_Str = strlen( sSrc_string );
	
	/* ����Դ�ַ��������Ӧ�Ŀռ� */
	pPoint_Mem = ( char * )malloc( iLength_Str + 1 );
	
	/* ����Ŀռ��ʼ�� */
	memset( pPoint_Mem, 0x00, iLength_Str + 1 );
	
	/* ��Դ�ַ���������Ŀ��ռ� */
	pRet = Dx_String( sSrc_string, pPoint_Mem );
	
	/* �������ʧ�ܣ���return NULL */
	if ( NULL == pRet )
	{
		return NULL;
	}
	
	/* ��ʼ��Դ�ַ��� */
	memset( sSrc_string, 0x00, strlen( sSrc_string ) );
	
	/* ��������ַ����������� Դ�ַ����ĵ�ַ�ռ� */
	strcpy( sSrc_string, pPoint_Mem );
	
	/* �ͷŷ���Ŀռ� */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
	
	return sSrc_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Del_Char                                  *
* Description:    ȥ���ַ����е�ĳһ�ַ�(ȥ��ĳ�ַ���Դ�ַ����ϲ�)*
*                 �������ַ����ַ����в����ڣ�����Ϊɾ���ɹ� *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String ԭ�ַ���                      *
*                 cChar ȥ���ַ�                            *
* Out:            NULL ʧ��                                 *
*                 ��NULL  �ɹ���Դ�ַ����ĵ�ַ              *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Del_Char( char * sSrc_String, char cChar )
{
	int iCount;                   /* ͳ��ʹ�� */
	int iLength_Str;              /* �ַ������� */
	int iFor_Num;                 /* forѭ��ʹ�� */
	char * pPoint_Mem;            /* �����ڴ�ĵ�ַ */
	
	/* ������ʼ�� */
	iCount = 0;
	iLength_Str = 0;
	iFor_Num = 0;
	pPoint_Mem = NULL;
	
	/* ������ */
	if ( NULL == sSrc_String )
	{
		return NULL;
	}
	
	/* ͳ��Դ�ַ����ĳ��� */
	iLength_Str = strlen( sSrc_String );
	
	/* �����ܹ����µ����ַ����Ŀռ� */
	pPoint_Mem = ( char * )malloc( iLength_Str + 1 );
	
	/* ����Ŀռ��ʼ�� */
	memset( pPoint_Mem , 0x00, iLength_Str + 1 );

	for ( iFor_Num = 0, iCount = 0; iFor_Num < iLength_Str; iFor_Num++ )
	{
		/* ������ҵ����ϵ��ַ���continue �����и��Ƶ�Ŀ��ռ� */
		if ( cChar == *( sSrc_String + iFor_Num ) )
		{
			continue;
		}
		else
		{
			/* ���ϵ��ַ�������и�����Ŀ��ռ� */
			*( pPoint_Mem + iCount ) = *( sSrc_String + iFor_Num );
			iCount++;
		}
	}
	
	/* ��ʼ��Դ�ַ��� */
	memset( sSrc_String, 0x00, iLength_Str );
	
	/* ������õ��ַ���������Դ�ַ��Ŀռ� */
	strcpy( sSrc_String, pPoint_Mem );
	
	/* �ͷŷ���Ŀռ� */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
	
	return sSrc_String;
}



/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Del_Char_CopyToDst                        *
* Description:    ȥ���ַ����е�ĳһ�ַ�(ȥ��ĳ�ַ���Ŀ���ַ����ϲ�)*
*                 �������ַ����ַ����в����ڣ�����Ϊɾ���ɹ�*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string ԭ�ַ���                      *
*                 sDst_string Ŀ���ַ����������Ȳ��������δ��֪*
*                 sCh ȥ���ַ�                              *
* Out:            NULL ʧ��                                 *
*                 ��NULL �ɹ���Ŀ��ռ��ַ                 *
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
	
	/* ������ʼ�� */
	pPoint_Mem = NULL;
	iLength = 0;
	pRet = NULL;
	
	/* ������ */
	if ( NULL == sSrc_string || NULL == sDst_string )
	{
		return NULL;
	}

	/* Դ�ַ������� */ 
	iLength = strlen( sSrc_string );
	
	/* �����ܹ�����Դ�ַ����Ŀռ� */
	pPoint_Mem = ( char * )malloc( iLength + 1 );
	
	/* �ռ��ʼ�� */
	memset( pPoint_Mem , 0x00, iLength + 1 );
	
	/* ����Դ�ַ��� */
	strcpy( pPoint_Mem, sSrc_string );

	/* ɾ��Ŀ���ַ� */
	pRet = Del_Char( pPoint_Mem, sCh );
	if ( NULL == pRet )
	{
		return NULL;
	}
	
	/* ��������õ��ַ�����Ŀ���ַ����Ŀռ� */
	strcpy( sDst_string, pPoint_Mem );
	
	/* �ͷſռ� */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
	
	return sDst_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Del_Char_Hh                               *
* Description:    ȥ���ַ����еĻ��з�                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string ԭ�ַ���                       *
*                                                           *
* Out:            NULL ʧ��                                 *
*                 ��NULL  Դ�ַ�����ַ                      *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Replace_Str_To_Str_OrderByCount           *
* Description:    ��Դ�ַ����� �滻 ��ƥ���ϵĵڼ����ַ���  *
*                 ���û���ҵ���Ҫ�滻���ַ������򷵻�NULL  *
*                 sDst_string Ŀ���ַ������Ȳ����Ļ������δ��֪*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string Դ�ַ���                      *
*                 sDst_string Ŀ���ַ���                    *
*                 sTh_Src_String �滻Դ�ַ���               *
*                 sTh_Dst_String �滻Ŀ���ַ���             *
*                 iOrderCount    ƥ��ĵڼ����ַ���         *
* Out:            NULL ʧ��                                 *
*                 ��NULL  Ŀ���ַ���                        *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Replace_Str_To_Str_OrderByCount( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String, int iOrderCount )
{
	char * pRet;                             /* ָ��ķ���ֵ */
	char * pPoint_FindString;                /* ���ҵ�ַ��ָ�� */
	char * pPoint_Mem;                       /* �����ڴ��ָ�� */
	int    iLength_Dst_String;               /* �滻����Ҫ���ַ������� */
	int    iLengthString_Tb;                 /* �ַ�����ͷ������ */
	int    iLengthString_Zj;                 /* �ַ������м䳤�� */
	int    iLengthString_Wb;                 /* �ַ�����β������ */
	char * pPoint_Addr;                      /* ��ַָ�� */
	int    iCount_Pp;                        /* ƥ�䵽���ַ����ĸ��� */

	/* ������ʼ�� */
	pRet = NULL;
	pPoint_FindString = NULL;
	pPoint_Mem = NULL;
	iLength_Dst_String = 0;
	iLengthString_Tb = 0;
	iLengthString_Zj = 0;
	iLengthString_Wb = 0;
	pPoint_Addr = NULL;
	iCount_Pp = -1;

	/* ������� */
	if ( NULL == sSrc_string || NULL == sDst_string || NULL == sTh_Src_String || NULL == sTh_Dst_String )
	{
		return NULL;
	}

	/* ƥ�䵽���ַ����ĸ��� */
	iCount_Pp = Count_ChildStr_Num( sSrc_string, sTh_Src_String );
	if ( iCount_Pp < iOrderCount )
	{
		return NULL;
	}

	int iCount_While = 0;
	while ( 1 )
	{
		/* �ҵ���ַ1 */
		pPoint_FindString = strstr( sSrc_string, sTh_Src_String );
		if ( NULL == pPoint_FindString )
		{
			return NULL;
		}
		else
		{
			/* ƥ�䵽һ�Σ��Լ�1 */
			iCount_While++;

			/* �Ѿ��ҵ�Ҫƥ��ĵط� */
			if ( iCount_While == iOrderCount )
			{
				break;
			}
			else
			{
				/* û��ƥ�䵽�Ļ�����continue */
				continue;
			}
		}
	}

	/*	������
		"012345678901234567890123456789" ��Ҫ��"56"�滻��"abc"
		
		��ַΪ0x005,
		pPoint_FindString  = 0x005;
		iLengthString_Tb   = 5;
		iLengthString_Zj   = 3;
		iLengthString_Wb   = 30 - 2 - 5 = 23;
		iLength_Dst_String = 5 + 3 + 23 = 31;
		pPoint_Addr        = 0x005 + 2 = 0x007;
	*/

	/* �ַ�����ͷ������ */
	iLengthString_Tb = abs( sSrc_string - pPoint_FindString );
	
	/* �ַ������м䳤�� */
	/* iLengthString_Zj = strlen( sTh_Dst_String ) - strlen( sTh_Src_String ); del by mawx@20180712 */
	iLengthString_Zj = strlen( sTh_Dst_String );
	
	/* �ַ�����β������ */
	iLengthString_Wb = strlen( sSrc_string ) - strlen( sTh_Src_String ) - iLengthString_Tb;
	
	/* �����滻����Ҫ���ַ������� */
	iLength_Dst_String = iLengthString_Tb + iLengthString_Zj + iLengthString_Wb;
	
	/* ����ռ� */
	pPoint_Mem = ( char * )malloc( iLength_Dst_String );

	/* �����ַ2 */
	pPoint_Addr = pPoint_FindString + strlen( sTh_Src_String );

	/* �滻�ַ��� */
	memcpy( pPoint_Mem, sSrc_string, iLengthString_Tb );
	/* del by mawx@20180712 
	memcpy( pPoint_FindString, sTh_Dst_String, strlen( sTh_Dst_String ) );
	memcpy( pPoint_FindString + strlen( sTh_Dst_String ), pPoint_Addr, iLengthString_Wb ); */
	memcpy( pPoint_Mem + iLengthString_Tb, sTh_Dst_String, strlen( sTh_Dst_String ) );
	memcpy( pPoint_Mem + iLengthString_Tb + strlen( sTh_Dst_String ), pPoint_Addr, iLengthString_Wb );

	/* ������Ŀ���ַ��� */
	memcpy( sDst_string, pPoint_Mem, iLength_Dst_String );

	/* �ͷŵ�ַ */
	free ( pPoint_Mem );
	pPoint_Mem = NULL;

	return sDst_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Replace_Str_To_Str_First                  *
* Description:    ��Դ�ַ������滻����һ��ƥ���Ӧ���ַ���  *
*                 ���û���ҵ���Ҫ�滻���ַ������򷵻�NULL  *
*                 sDst_string Ŀ���ַ������Ȳ����Ļ������δ��֪*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string Դ�ַ���                      *
*                 sDst_string Ŀ���ַ���                    *
*                 sTh_Src_String �滻Դ�ַ���               *
*                 sTh_Dst_String �滻Ŀ���ַ���             *
* Out:            NULL ʧ��                                 *
*                 ��NULL  Ŀ���ַ���                        *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Replace_Str_To_Str_All                    *
* Description:    ��Դ�ַ������滻�����ж�Ӧ���ַ���        *
*                 ���û���ҵ���Ҫ�滻���ַ������򷵻�NULL  *
*                 sDst_string Ŀ���ַ������Ȳ����Ļ������δ��֪*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string Դ�ַ���                      *
*                 sDst_string Ŀ���ַ���                    *
*                 sTh_Src_String �滻Դ�ַ���               *
*                 sTh_Dst_String �滻Ŀ���ַ���             *
* Out:            NULL ʧ��                                 *
*                 ��NULL  Ŀ���ַ���                        *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/ /* ??? �ú���û�����*/
char * Replace_Str_To_Str_All( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String )
{
	int    iCount_Pp;                        /* ƥ�䵽���ַ����ĸ��� */
	int    iLength_Str_Tmp;                  /* ��ʱ�ַ����ĳ��� */
	char  *pPoint;                           /* ��ʱָ�� */
	char  *pPoint_Tmp;                       /* ��ʱָ�� */
	int    iLength_pPoint;                   /* ������ڴ�ĳ��� */
	char  *pPoint_Src;                       /* ��ʱָ�� */

	iCount_Pp = -1;
	iLength_Str_Tmp = 0;
	pPoint = NULL;
	pPoint_Tmp = NULL;
	iLength_pPoint = 0;
	pPoint_Src = NULL;

	/* ƥ�䵽���ַ����ĸ��� */
	iCount_Pp = Count_ChildStr_Num( sSrc_string, sTh_Src_String );
	if ( iCount_Pp < iOrderCount )
	{
		return NULL;
	}

	/* ������ʱ�ڴ� */
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Replace_Str_To_Str_All                    *
* Description:    ɾ���ַ����е�����ƥ�䵽���Ӵ�            *
*                 ���û���ҵ���Ҫ�滻���ַ������򷵻�NULL  *
*                 sDst_string Ŀ���ַ������Ȳ����Ļ������δ��֪*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string Դ�ַ���                      *
*                 sDst_string Ŀ���ַ���                    *
*                 sTh_Src_String �滻Դ�ַ���               *
*                 sTh_Dst_String �滻Ŀ���ַ���             *
* Out:            NULL ʧ��                                 *
*                 ��NULL  Ŀ���ַ���                        *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Del_Char_File                             *
* Description:    ����ɾ��ĳ�ļ��е�ĳһ�ַ�                *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sFilename �ļ���(����·�����������·��)   *
*                 cCh ��Ҫɾ�������ַ�                      *
* Out:            -1 ʧ��                                   *
*                 0  �ɹ�                                   *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Del_Char_File( char * sFilename, char cCh )
{
	FILE * fp;                  /* �ļ�ָ�� */
	char sTmp_Buf[10240];       /* ��ʱ����ַ���buf */
	int iLen_Seek;              /* �ļ�ƫ���� */
	int iLength_i;              /* ����i */
	int iLength_j;              /* ����j */
	int iSize_File;             /* �ļ��Ĵ�С */
	int iLen_Add_Seek;          /* �ļ�β����ӵ��ļ�ƫ���� */
	int iLen_Read_Seek;         /* ���ļ����ļ�ƫ���� */
	
	/* ������ʼ�� */
	fp = NULL;
	memset( sTmp_Buf, 0x00, sizeof( sTmp_Buf ) );
	iLen_Seek = 0;
	iLength_i = 0;
	iLength_j = 0;
	iSize_File = 0;
	iLen_Add_Seek = 0;
	iLen_Read_Seek = 0;

	/* д��ʽ���ļ� */
	fp = fopen( sFilename, "w+" );
	if ( NULL == fp )
	{
		return -1;
	}

	/* ��ȡ���ļ����ļ���С���ɴ˿��Եõ��ļ����ܵ�ƫ���� */

	/* �ļ���С���ļ�β�����ļ�ƫ���� */
	iLen_Add_Seek = iSize_File;

	/* ѭ����ȡ�ļ� */
	while( NULL != fgets( sTmp_Buf, sizeof( sTmp_Buf ), fp ) )
	{
		/* �����ȡ�ĳ��� */
		iLength_i = strlen( sTmp_Buf );

		/* ɾ��ĳ�ַ� */
		if ( NULL == Del_Char( sTmp_Buf, cCh ) )
		{
			return -1;
		}

		/* ���㴦����ַ����ĳ��� */
		iLength_j = strlen( sTmp_Buf );
		
		/* ����д�ļ���ƫ���� */
		fseek( fp , iLen_Add_Seek, SEEK_SET );

		/* ����ú���ַ�����д���ļ�β�� */
		fputs( sTmp_Buf, fp ); 

		/* ���¼���׷���ļ�β��������ƫ���� */
		iLen_Add_Seek = iLen_Add_Seek + iLength_j;

		/* ���¼�����ļ���ƫ���� */
		iLen_Read_Seek = iLen_Read_Seek + iLength_i;

		/* ���ö��ļ���ƫ���� */
		fseek( fp , iLen_Read_Seek, SEEK_SET );

		/* ��ʱ����ַ���buf��ʼ�� */
		memset( sTmp_Buf, 0x00, sizeof( sTmp_Buf ) );
	}

	/* �ļ�ƫ������Ϊ0 */
	fseek( fp , 0, SEEK_SET );

	/* ���ļ���ԭ�����֣�ȫ����Ϊ�� */

	fclose( fp );

	return 0;
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Trim_Before                               *
* Description:    ȥ���ַ���ǰ��Ŀո�                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sString Դ�ַ���                          *
* Out:                                                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
void Trim_Before( char * sString ) 
{
	char *pPoint_Mem;             /* �����ڴ�ĵ�ַ */
	char *pPoint_tmp;             /* ��ʱָ�� */
	int iCount;                   /* ͳ����Ŀ */
	int iLength;                  /* �ַ����ĳ��� */
	int  iFlag_Yd;                /* �ַ���ǰ���Ƿ��пո�ı�ʶ��0-û�пո�1-�пո� */
	
	/* ������ʼ�� */
	pPoint_Mem = NULL;
	pPoint_tmp = NULL;
	iCount = 0;
	iLength = 0;
	iFlag_Yd = 0;

	/* Դ�ַ����ĳ��� */
	iLength = strlen( sString );
	
	/* ����ռ� */
	pPoint_Mem = ( char *)malloc( iLength + 1 );
	
	/* �ռ��ʼ�� */
	memset( pPoint_Mem, 0x00, iLength + 1 );
	
	
	while( 1 )
	{
		if ( ' ' != *( sString + iCount ) )
		{
			/* Դ�ַ���ǰ��û�пո�������ѭ�� */
			break;
		}
		else
		{
			/* �ַ���ǰ���Ƿ��пո�ı�ʶ��0-û�пո�1-�пո� */
			iFlag_Yd = 1;
			
			/* �ռ��ʼ�� */
			memset( pPoint_Mem, 0x00, iLength + 1 );
			
			/* Դ�ַ���ǰ���пգ���ɾ���ո� */
			strcpy( pPoint_Mem, sString + iCount + 1 );
		}
		
		iCount++;
	}
	
	/* �пո񣬴����ַ������������� */
	if ( 1 == iFlag_Yd ) 
	{
		/* Դ�ַ�����ʼ�� */
		memset( sString, 0x00, iLength );
		
		/* ������õ��ַ���������Դ�ַ��Ŀռ� */
		strcpy( sString, pPoint_Mem );
	}
	else
	{
		/* û�пո�Ļ�����û���ַ������������⣬�Ͳ����κζ��� */
		/* do nothing */
	}
	
	/* �ͷŷ�����ڴ� */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
}



/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Trim_Behind                               *
* Description:    ȥ���ַ�������Ŀո�                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sString Դ�ַ���                          *
* Out:            NULL ʧ��                                 *
*                 ��NULL �ɹ���Դ�ַ�����ַ                 *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char *Trim_Behind( char * sString ) 
{
	char * pRet;              /* �����ķ���ֵ */
	
	/* ������ʼ�� */
	pRet = NULL;

	/* Դ�ַ������� */
	pRet = Dx_String_Yzfc( sString );
	if ( NULL == pRet )
	{
		return NULL;
	}
	
	/* ɾ�������ַ���ǰ��Ŀո� */
	Trim_Before( sString );
	
	/* �����µ����ַ��� */
	pRet = Dx_String_Yzfc( sString );
	if ( NULL == pRet )
	{
		return NULL;
	}

	return sString;
}

 
/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Trim                                      *
* Description:    ɾ���ַ���ǰ��Ŀո�                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sString Դ�ַ���                          *
* Out:            void                                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
void Trim( char * sString ) 
{
	/* ȥ���ַ���ǰ��Ŀո� */
	Trim_Before( sString );
	
	/* ȥ���ַ�������Ŀո� */
	Trim_Behind( sString );
}



/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Trim_And_Del_Hh                           *
* Description:    ȥ���ַ����еĻ��з����ո�                *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sString Դ�ַ���                          *
* Out:            char * NULL ʧ�ܣ�������                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Trim_Kg_Hh( char * sString ) 
{
	/* ������� */
	if ( NULL == sString )
	{
		return NULL;
	}
	
	/* ȥ���ַ��еĻ��� */
	if ( NULL == Del_Char_Hh( sString ) )
	{
		return NULL;
	}

	/* ȥ���ַ���ǰ��ո� */
	Trim( sString );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_String_Behind_Char                    *
* Description:    ȡ���ַ�cChar����ַ���(����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)*
*                 Ŀ���ַ����ռ䲻�������δ��֪            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string Դ�ַ���                      *
*                 cChar �ַ�                                *
*                 sDst_string Ŀ���ַ���                    *
* Out:            NULL ʧ��                                 *
*                 sDst_string �ɹ�,ȡ�õ��ַ��������dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Behind_Char( const char * sSrc_string , char cChar, char * sDst_string )
{
	char * pPoint_Str;
	
	/* ������ʼ�� */
	pPoint_Str = NULL;
	
	/* ������� */
	if ( NULL == sSrc_string )
	{
		return NULL;
	}
	
	/* ��ȡĳ�ַ��ĵ�ַ */
	pPoint_Str = strchr( sSrc_string, cChar );
	if ( NULL == pPoint_Str )
	{
		return NULL;
	}

	/* �����ַ�����Ŀ������ */
	strcpy( sDst_string, pPoint_Str + 1 );
	
	return sDst_string;
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_String_Before_Char                    *
* Description:    ȡ���ַ�cCharǰ���ַ���(����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)*
*                 Ŀ���ַ����ռ䲻�������δ��֪            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_string Դ�ַ���                      *
*                 cChar �ַ�                                *
*                 sDst_string Ŀ���ַ���                    *
* Out:            NULL ʧ��                                 *
*                 sDst_string �ɹ�,ȡ�õ��ַ��������dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Before_Char( const char * sSrc_string , char cChar, char * sDst_string )
{
	char * pPoint_Str;                    /* ĳ�ַ����ĵ�ַ */
	char * pPoint_Copy_Start;             /* ������ʼ�ĵ�ַ */
	int       iLength_Copy;               /* ���������ĵ�ַ */
	
	/* ������ʼ�� */
	pPoint_Str = NULL;
	pPoint_Copy_Start = NULL;
	iLength_Copy = 0;
	
	/* ������� */
	if ( NULL == sSrc_string )
	{
		return NULL;
	}
	
	/* ��ȡĳ�ַ��ĵ�ַ */
	pPoint_Str = strchr( sSrc_string, cChar );
	if ( NULL == pPoint_Str )
	{
		return NULL;
	}

	/* ���㿪ʼ�ĵ�ַ */
	pPoint_Copy_Start = sSrc_string;
	
	/* ���㿽���ĳ��� */
	iLength_Copy = abs( pPoint_Copy_Start - pPoint_Str );
	
	/* �����ַ����ڴ���Ŀ������ */
	memcpy( sDst_string, pPoint_Copy_Start, iLength_Copy );
	
	/* ����'\0' */
	memset( sDst_string + iLength_Copy, 0x00, 1 );
	
	return sDst_string;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_String_Behind_Dh                      *
* Description:    ȡ���ַ�'='����ַ���(ԭ���ģ�û��ȥ���ո񡢻��з�)*
*                 (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_string Դ�ַ���                       *
*                 ch �ַ�                                   *
* Out:            NULL ʧ��                                 *
*                 dst_string �ɹ�,ȡ�õ��ַ��������dst_string*
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_String_Before_Dh                      *
* Description:    ȡ���ַ�'='ǰ���ַ���(ԭ���ģ�û��ȥ���ո񡢻��з�)*
*                 (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_string Դ�ַ���                       *
*                 ch �ַ�                                   *
* Out:            NULL ʧ��                                 *
*                 dst_string �ɹ�,ȡ�õ��ַ��������dst_string*
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_String_Behind_Dh_QdKgHh               *
* Description:    ȡ���ַ�'='����ַ���(ȥ���ո񡢻��з�)   *
*                 (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String Դ�ַ���                      *
*                 ch �ַ�                                   *
* Out:            char *                                    *
*                 NULL ʧ��                                 *
*                 sDst_String �ɹ�,ȡ�õ��ַ��������dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Behind_Dh_QdKgHh( char * sSrc_String , char * sDst_String )
{
	char * p;

	/* ������ʼ�� */
	p = NULL;

	/* ��ȡ=�ź�����ַ��� */
	p = Get_String_Behind_Char( sSrc_String, '=', sDst_String );
	if ( NULL == p )
	{
		return NULL;
	}
	
	/* ɾ������ */
	if ( NULL == Del_Char_Hh( sDst_String ) )
	{
		return NULL;
	}
	
	/* ɾ���ո� */
	Trim( sDst_String );
	
	return sDst_String;
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_String_Before_Dh_QdKgHh               *
* Description:    ȡ���ַ�'='ǰ���ַ���(ȥ���ո񡢻��з�)   *
*                 (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String Դ�ַ���                      *
*                 ch �ַ�                                   *
* Out:            char *                                    *
*                 NULL ʧ��                                 *
*                 sDst_String �ɹ�,ȡ�õ��ַ��������dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Before_Dh_QdKgHh( char * sSrc_String , char * sDst_String )
{
	char * p;

	/* ������ʼ�� */
	p = NULL;

	/* ��ȡ=��ǰ����ַ��� */
	p = Get_String_Before_Char( sSrc_String, '=', sDst_String );
	if ( NULL == p )
	{
		return NULL;
	}
	
	/* ɾ������ */
	if ( NULL == Del_Char_Hh( sDst_String ) )
	{
		return NULL;
	}
	
	/* ɾ���ո� */
	Trim( sDst_String );
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_String_Behind_Dh_QdKg                 *
* Description:    ȡ���ַ�'='����ַ���(ȥ���ո�)           *
*                 (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String Դ�ַ���                      *
* Out:            NULL ʧ��                                 *
*                 sDst_String �ɹ�,ȡ�õ��ַ��������dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Behind_Dh_QdKg( char * sSrc_String , char * sDst_String )
{
	char * p;

	/* ������ʼ�� */
	p = NULL;

	/* ��ȡ=�ź�����ַ��� */
	p = Get_String_Behind_Char( sSrc_String, '=', sDst_String );
	if ( NULL == p )
	{
		return NULL;
	}

	/* ɾ���ո� */
	Trim( sDst_String );
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_String_Before_Dh_QdKg                 *
* Description:    ȡ���ַ�'='ǰ���ַ���(ȥ���ո�)           *
*                 (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String Դ�ַ���                      *
* Out:            NULL ʧ��                                 *
*                 sDst_String �ɹ�,ȡ�õ��ַ��������dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Before_Dh_QdKg( char * sSrc_String , char * sDst_String )
{
	char * p;

	/* ������ʼ�� */
	p = NULL;

	/* ��ȡ=�ź�����ַ��� */
	p = Get_String_Before_Char( sSrc_String, '=', sDst_String );
	if ( NULL == p )
	{
		return NULL;
	}
	
	/* ɾ���ո� */
	Trim( sDst_String );
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_KeyValue_Dh_QdKg                      *
* Description:    ȡ���ַ�'='���ߵ��ַ���(ȥ���ո�)������ȡkey-value��ֵ*
*                 (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String Դ�ַ���                      *
* Out:            NULL ʧ��                                 *
*                 sDst_KeyString   Key���ַ���ֵ���ɹ�����  *
*                 sDst_ValueString  Value���ַ���ֵ         *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_KeyValue_Dh_QdKg( char * sSrc_String , char * sDst_KeyString, char * sDst_ValueString )
{
	/* ������ */
	if ( NULL == sSrc_String || NULL == sDst_KeyString || NULL == sDst_ValueString )
	{
		return NULL;
	}

	/* ��ȡ=������ַ��� */
	if ( NULL == Get_String_Behind_Dh_QdKg( sSrc_String, sDst_ValueString ) )
	{
		return NULL;
	}
	
	/* ��ȡ=ǰ����ַ��� */
	if ( NULL == Get_String_Before_Dh_QdKg( sSrc_String, sDst_KeyString ) )
	{
		return NULL;
	}

	return sDst_KeyString;
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_String_Between_Zkh                    *
* Description:    ȡ���������ڵĵ��ַ���                    *
*                 (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sSrc_String Դ�ַ���                      *
* Out:            NULL ʧ��                                 *
*                 sDst_String �ɹ�,ȡ�õ��ַ��������dst_string*
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_String_Between_Zkh( const char * sSrc_String , char * sDst_String )
{
	int  iLength_Src_String;                /* Դ�ַ����ĳ��� */
	char  *pPoint_Mem;                      /* ��ʱ����ַ���buf */
	
	/* ������ʼ�� */
	iLength_Src_String = 0;
	pPoint_Mem = NULL;
	
	/* ������ */
	if ( NULL == sSrc_String || NULL == sDst_String )
	{
		return NULL;
	}
	
	/* ����Դ�ַ����ĳ��� */
	iLength_Src_String = strlen( sSrc_String );
	
	/* �����ڴ� */
	pPoint_Mem = ( char * )malloc( iLength_Src_String + 1 );
	
	/* �����ڴ��ʼ�� */
	memset( pPoint_Mem, 0x00, iLength_Src_String + 1 );
	
	/* ��ȡ[������ַ��� */
	if ( NULL == Get_String_Behind_Char( sSrc_String, '[', pPoint_Mem ) )
	{
		return NULL;
	}
	
	/* ��ȡ]ǰ����ַ��� */
	if ( NULL == Get_String_Before_Char( pPoint_Mem, ']', sDst_String ) )
	{
		return NULL;
	}
	
	/* �ͷŷ���Ŀռ� */
	free( pPoint_Mem );
	pPoint_Mem = NULL;
	
	return sDst_String;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Is_Digit_ByString                         *
* Description:    �ַ��������Ƿ��Ƿ������ֹ���(�ɴ�С����)*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sDigit_Str ��Ҫ�жϵ��ַ���               *
* Out:            0 ������                                  *
*                 1 ����                                    *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Is_Digit_ByChar                           *
* Description:    �ַ������Ƿ��Ƿ������ֹ���(�ɴ�С����)  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sDigit_Str ��Ҫ�жϵ��ַ���               *
* Out:            0 ������                                  *
*                 1 ����                                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Is_Digit_ByChar( const char sDigit_Str )
{
	char pPoint_String[2];         /* ��ʱ�ַ��� */
	
	/* ������ʼ�� */
	memset( pPoint_String, 0x00, sizeof( pPoint_String ) );
	
	/* ת�����ַ��� */
	Char_To_Str( sDigit_Str, pPoint_String );
	
	return Is_Digit_ByString( pPoint_String );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Is_EnglishChar_ByString                   *
* Description:    �ַ��������Ƿ���Ӣ����ĸ�Ĺ���          *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sEnglishChar_Str  ��Ҫ�жϵ��ַ���        *
* Out:            0 �з���ĸ                                *
*                 1 ֻ��Сд��ĸ                            *
*                 2 ֻ�д�д��ĸ                            *
*                 3 ��д��ĸ��Сд��ĸ����                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Is_EnglishChar_ByString( const char * sEnglishChar_Str )
{
	int iNum_For;               /* forѭ��ʹ�� */
	int iLength_Str;            /* �ַ����ĳ��� */
	int iIs_Xx;                 /* Сд��ĸ�ĸ���ͳ�� */
	int iIs_Dx;                 /* ��д��ĸ�ĸ���ͳ�� */

	/* ������ʼ�� */
	iNum_For = 0;
	iLength_Str = 0;
	
	/* �ַ����ĳ��� */
	iLength_Str = strlen( sEnglishChar_Str );
	
	/* ѭ����� */
	for( iNum_For = 0; iNum_For < iLength_Str; iNum_For++ )
	{
		/* 65��A��90��Z��Ϊ��д��ĸ */
		if (  65 <= ( *( sEnglishChar_Str + iNum_For ) )  && 90 >= ( *( sEnglishChar_Str + iNum_For ) ) )
		{
			iIs_Dx++;
			continue;
		}
		/* 97��a��122��z��ΪСд��ĸ */
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Is_EnglishChar_ByChar                     *
* Description:    �ַ������Ƿ���Ӣ����ĸ�Ĺ���            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sEnglishChar_Str  ��Ҫ�жϵ��ַ���        *
* Out:            0 �з���ĸ                                *
*                 1 ֻ��Сд��ĸ                            *
*                 2 ֻ�д�д��ĸ                            *
*                 3 ��д��ĸ��Сд��ĸ����                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Is_EnglishChar_ByChar( const char sEnglishChar_Str )
{
	char pPoint_String[2];         /* ��ʱ�ַ��� */
	
	/* ������ʼ�� */
	memset( pPoint_String, 0x00, sizeof( pPoint_String ) );
	
	/* ת�����ַ��� */
	Char_To_Str( sEnglishChar_Str, pPoint_String );
	
	return Is_EnglishChar_ByString( pPoint_String );
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Is_EnglishChar_Digit_ByString             *
* Description:    �ַ��������Ƿ���Ӣ����ĸ�����֣���С���㣩�Ĺ��� *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sEnglishChar_Str  ��Ҫ�жϵ��ַ���        *
* Out:            0 ����ĸ��Ҳ�����ֻ�С����                *
*                 1 �� ֻ��Сд��ĸ                         *
*                 2 �� ֻ�д�д��ĸ                         *
*                 3 �� ��д��ĸ��Сд��ĸ����               *
*                 4 �� ����(����С����)                     *
*                 5 ����(����С����)��Ӣ����ĸ���          *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Is_EnglishChar_Digit_ByString( const char * sSrc_Str )
{
	int iNum_For;                 /* forѭ��ʹ�� */
	int iLength_Str;              /* �ַ����ĳ��� */
	char  cChar_Tmp;              /* ��ʱ�ַ� */
	char  sStr_Tmp[2];            /* ��ʱ�ַ��� */
	int  iCount_Digit;            /* ���ָ��� */
	int  iCount_EnglishChar1;     /* Ӣ����ĸ���� */
	int  iCount_EnglishChar2;     /* Ӣ����ĸ���� */
	int  iCount_EnglishChar3;     /* Ӣ����ĸ���� */
	int  iRet;                    /* �����ķ���ֵ */
	
	/* ������ʼ�� */
	iNum_For = 0;
	iLength_Str = 0;
	memset( sStr_Tmp, 0x00, sizeof( sStr_Tmp ) );
	iCount_Digit = 0;
	iCount_EnglishChar1 = 0;
	iCount_EnglishChar2 = 0;
	iCount_EnglishChar3 = 0;
	iRet = 0;
	
	/* ��ȡ�ַ����ĳ��� */
	iLength_Str =  strlen( sSrc_Str );
	
	/* ѭ���ж� */
	for ( iNum_For = 0; iNum_For < iLength_Str; iNum_For++ )
	{
		/* ���ַ���ȡ���� */
		cChar_Tmp = *(sSrc_Str + iNum_For);
		
		/* ��Ӣ����ĸ���������֣�����С���㣩������Ϊ��ȷ */
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
	1 �� ֻ��Сд��ĸ               
	2 �� ֻ�д�д��ĸ               
	3 �� ��д��ĸ��Сд��ĸ����     
	4 �� ����(����С����)           
	5 ����(����С����)��Ӣ����ĸ���
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    GetStr_Dkh_Between                        *
* Description:    ��ȡ������֮����ַ���                    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sIn_Str  ��Ҫ������ַ���                 *
* Out:            sOut_Str  ����õ��ַ���                  *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int GetStr_Dkh_Between( const char * sIn_Str, char * sOut_Str )
{
    
    
    
}























