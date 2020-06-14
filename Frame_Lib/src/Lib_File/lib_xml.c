

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib_xml.h"

/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Get_FindString                            *
* Description:    寻找字符串中第一个str1与str2中的字符串    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
*                 fnd_str 存放查到到的字符串                *
*                 str1 查找的开始字符串位置                 *
*                 str2 查询的结束字符串位置                 *
* Out:            NULL 查询无                               *
*                 非空 查询到字符串                         *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Get_FindString( char * src_str, char * fnd_str , char * str1, char * str2 )
{
	char *p_tmp_first;
	char *p_tmp_end;
	int length;

	p_tmp_first = NULL;
	p_tmp_end = NULL;
	length = 0;
	
	p_tmp_first = strstr( src_str, str1 );
	if ( NULL == p_tmp_first )
	{
		return NULL;
	}

	p_tmp_end = strstr( p_tmp_first, str2 );
	if ( NULL == p_tmp_end )
	{
		return NULL;
	}
		
	strncpy( fnd_str, p_tmp_first + 1, abs( p_tmp_end - p_tmp_first) - 1 );
	
	return fnd_str;
}



/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Get_FirstBracket                          *
* Description:    寻找字符串中第一个<>中的字符串            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
*                 fnd_str 存放查到到的字符串                *
* Out:            NULL 查询无                               *
*                 非空 查询到字符串                         *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Get_FirstBracket( char * src_str, char * fnd_str )
{
	return Get_FindString( src_str, fnd_str , "<", ">" );
}




/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Is_FirstSpace_String                      *
* Description:    判断字符串首部中是否有空格                *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
* Out:            0 无                                      *
*                 1 有                                      *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Is_FirstSpace_String( char * str )
{
	int i;
	
	i = 0;

	if ( ' ' == *( str + i ) )
	{
		return 1;
	}
	
	return 0;
}



/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Is_EndSpace_String                        *
* Description:    判断字符串尾部是否有空格                  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
* Out:            0 无                                      *
*                 1 有                                      *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Is_EndSpace_String( char * str )
{
	int iRet;
	
	iRet = 0;
	
	/*字符串倒序*/

	
	iRet = Is_FirstSpace_String( str );
	if ( 0 == iRet )
	{
		return 0;
	}
	else if ( 1 == iRet )
	{
		return 1;
	}
}


/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Is_Space_String                           *
* Description:    判断字符串中是否有空格                    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
* Out:            0 无                                      *
*                 1 有                                      *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Is_Space_String( char * str )
{
	int length;
	int i;
	
	length = 0;
	i = 0;
	
	length = strlen( str );
	
	for ( i = 0; i < length; i++ )
	{
		if ( ' ' == *( str + i ) )
		{
			return 1;
		}
	}
	
	return 0;

}




/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Check_BracketString                       *
* Description:    校验<>中的内容是否有空格                  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             str 源字符串                              *
* Out:            -1 有空格                                 *
*                 0 无空格                                  *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Check_BracketString( char * str )
{
	int iRet;
	
	iRet = 0;
	
	iRet = Is_Space_String( str );
	if( 1 == iRet )
	{
		return -1;
	}
	else if ( 0 == iRet )
	{
		return 0;
	}
}



/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Check_XmlString                           *
* Description:    校验字符串中是否有对应字段str的</str>的部分*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
*                 str 检验字符串                            *
* Out:            -1 无</str>部分                           *
*                 0  有</str>部分                           *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Check_XmlString( char * src_str, char * str )
{
	char * p;
	int length;
	
	p = NULL;
	length = 0;
	
	length = strlen( str ) + strlen( "<" ) + strlen( "/" ) + strlen( ">" );

	p = ( char * )malloc( length );
	memset( p, 0x00, length );

	strcat( p, "<" );
	strcat( p, "/" );	
	strcat( p, str );
	strcat( p, ">" );
	
	if ( NULL == strstr( src_str, p ) )
	{
		free( p );
		p = NULL;
		return -1;
	}
	
	free( p );
	p = NULL;
	return 0;	
}



/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Find_ZdXml                                *
* Description:    寻找<fnd_str的完整部分，比如<fnd_str class="listitem">*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             str 源字符串                              *
*                 fnd_str 字段字符串                        *
* Out:            NULL    失败                              *
*                非NULL  对应查询到字符串值                 *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Find_ZdXml( char * str, char * fnd_str )
{
	char * p;
	char * p1;
	char * p_tmp;
	
	p = NULL;
	p1 = NULL;
	p_tmp = NULL;
	
	p1 = ( char * )malloc( strlen( fnd_str ) + strlen( "<" ) + 1 );
	memset( p1, 0x00, strlen( fnd_str ) + strlen( "<" ) + 1 );
	
	strcat( p1, "<" );
	strcat( p1, fnd_str );
	
	p = strstr( str, p1 );
	if ( NULL == p )
	{
		return NULL;
	}
	
	p_tmp = strstr( p + strlen(p1), ">" );
	if ( NULL == p_tmp )
	{
		return NULL;
	}
	
	strncpy( fnd_str, p, abs( p_tmp - p + 1 ) );
	
	return fnd_str;
}




/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Get_ChildString                           *
* Description:    将<fnd_str>与</fnd_str>之间的字符串拿出来 *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             str 源字符串                              *
*                 fnd_str 字段字符串                        *
* Out:            NULL    失败                              *
*                非NULL  对应查询到字符串值                 *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Get_ChildString( char * str , char * fnd_str )
{
	char *p_tmp;
	char *p_tmp2;
	
	p_tmp = NULL;
	p_tmp2 = NULL;

	p_tmp = (char *)malloc( strlen( str ) + 1 );
	memset( p_tmp, 0x00, strlen( str ) + 1 );
	
	p_tmp2 = (char *)malloc( strlen( str ) + 1 );
	memset( p_tmp2, 0x00, strlen( str ) + 1 );

	strcat( p_tmp, fnd_str );

	if( NULL == Find_ZdXml( str, p_tmp ) )
	{
		free( p_tmp );
		free( p_tmp2 );
		return NULL;
	}

	strcat( p_tmp2, "</" );
	strcat( p_tmp2, fnd_str );
	strcat( p_tmp2, ">" );
	
	if ( NULL == Get_FindString( str, fnd_str, p_tmp , p_tmp2) )
	{
		free( p_tmp );
		free( p_tmp2 );
		return NULL;
	}
	
	free( p_tmp );
	free( p_tmp2 );
	
	return fnd_str;
}


/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Get_Check_ChildString                     *
* Description:    将<fnd_str>与</fnd_str>之间的字符串拿出来，并且校验是否合法*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             str 源字符串                              *
*                 fnd_str 字段字符串                        *
* Out:            NULL    失败                              *
*                非NULL  对应查询到字符串值                 *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Get_Check_ChildString( char * str , char * fnd_str )
{
	if ( NULL == Get_ChildString( str, fnd_str ) )
	{
		
		
	}

}




























