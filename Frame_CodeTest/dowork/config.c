#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h> 

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
/*
将字符串换成倒序的形式
*/
char * str_hx( char * str )
{
	char str_buf;
	int i;
	int j;
	int k;

	memset( &str_buf, 0x00, sizeof( str_buf ) );
	i = 0;
	j = 0;
	k = 0;

	if ( NULL == str )
	{
		return NULL;
	}

	i = strlen( str );
	k = i/2;

	while ( 1 )
	{
		memcpy( &(str_buf), &(str[j]), 1 );
		memcpy( &(str[j]), &(str[i - 1]), 1 );
		memcpy( &(str[i - 1]), &(str_buf), 1 );
		j++;
		i--;

		if ( k == j )
		{
			break;
		}
	}
	
	return str;
}

/*
字符串从某个位置向前或者向后移动
len: 正数表示向前移动
     负数表示向后移动
str: 字符串的某个位置的地址
要保证地址向前移动的地址，是能够覆盖的。
*/
char * str_xqyd( char * str, int len )
{
	int i;
	int j;

	if ( NULL == str )
	{
		return NULL;
	}

	i = 0;                    /* 现在已经移动了多少个字符 */
	j = strlen( str ) + 1;    /* 一共需要移动了多少个字符 */

	while( 1 )
	{
		memcpy( str + i - len , str + i, 1 );
		i++;

		/* 已经移动的个数 等于 需要移动的个数，则break */
		if ( i == j )
		{
			break;
		}
	}
	
	return str;
}

/*
除去字符串首部的空格
*/
char * trim_qb( char * str )
{
	int i;            /* 字符串的长度 */
	int j;
	
	i = 0;
	j = 0;
	
	i = strlen( str );

	for( j = 0; j < i; j++ )
	{
		/* 如果该位置字符为空格 */
		if ( ' ' == str[j] )
		{
			/* 空格后的一位作为地址，开始向前移动 */
			str_xqyd( (str + j) + 1, 1 );
		}
		/* 如果该位置字符不为空格，则跳出 */
		else
		{
			break;
		}
	}
}

/*
除去字符串尾部的空格
*/
char * trim_wb( char * str )
{
	str_hx( str );
	
	trim_qb( str );
	
	str_hx( str );
}

/*
除去字符串收尾的空格
*/
char * trim( char * str )
{
	trim_qb( str );
	trim_wb( str );
}

char * del_noneed_string()
{
	
}