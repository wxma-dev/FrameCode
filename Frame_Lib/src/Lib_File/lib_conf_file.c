
#include "lib_conf_file.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    AddMem_For_AddString                      *
* Description:    读取配置文件某一行，添加内存，存储字符串  *
*                 如果是第一次读取某一行则malloc，如果一次  *
*                 以上读取某一行则realloc，并将新地址输出   *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             iCount_ReadLine 读取某一行的次数          *
*                 pPoint_AddMem  新内存的地址               *
* Out:            NULL 失败                                 *
*                 非NULL 成功,新地址                        *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * AddMem_For_AddString( const int iCount_ReadLine, const char * sSrc_String, char ** pPoint_AddMem )
{
	char   * pPoint_Tmp;             /* 临时指针 */
	
	/* 变量初始化 */
	pPoint_Tmp = NULL;

	/* 第一次读取 */
	if ( 1 == iCount_ReadLine )
	{
		if ( NULL == AddStr_Malloc( sSrc_String, pPoint_AddMem ) )
		{
			return NULL;
		}
	}
	else
	{
		/* 第一次以上读取，需要在原动态内存的尾部添加字符串 */
		if ( NULL == AddStr_Realloc( *pPoint_AddMem , sSrc_String, &pPoint_Tmp ) )
		{
			return NULL;
		}
		/* 新地址 */
		*pPoint_AddMem = pPoint_Tmp;
	}
	
	return *pPoint_AddMem;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    FreeMem_For_AddString                     *
* Description:    释放AddMem_For_AddString添加的内存        *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_NeedFree 需要释放的内存            *
* Out:            void                                      *
*                                                           *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
void FreeMem_For_AddString( char ** pPoint_NeedFree )
{
	AddStr_Free( pPoint_NeedFree );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_ValuseString_ByKey_InConfFile         *
* Description:    从配置文件中获取目标Key的内容             *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sFile_Name 文件名                         *
*                 sKey 目标Key的名字                        *
*                 sDst_String Key对应的value的内容          *  
* Out:            -1 失败                                   *
*                 -2 因为内存的问题失败                     *
*                 -3 没有匹配到目标域                       *
*                 0 成功                                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Get_ValuseString_ByKey_InConfFile( const char * sFile_Name, const char * sKey, char * sDst_String )
{
	FILE * fp;                        /* 文件指针 */
	char   sString_ReadLine[4096];    /* 读取文件的buf */
	char   *pPoint_AddMem;            /* 添加内存的指针 */
	int    iCount_ReadLine;           /* 循环读某一行的次数 */
	char   *pRet;                     /* 函数的返回指针内容 */
	char   sSting_Key[4096];          /* 临时存放Key */
	char   sSting_Value[4096 * 10];   /* Value值 */
	
	/* 变量初始化 */
	fp = NULL;
	memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
	pPoint_AddMem = NULL;
	iCount_ReadLine = 0;
	pRet = NULL;
	memset( sSting_Key, 0x00, sizeof( sSting_Key ) );
	memset( sSting_Value, 0x00, sizeof( sSting_Value ) );
	
	/* 打开文件 */
	fp = fopen( sFile_Name, "r" );
	if ( NULL == fp )
	{
		return -1;
	}
	
	while ( 1 )
	{
		/* 释放内存 */
		FreeMem_For_AddString( &pPoint_AddMem );
		
		/* 读某一行次数自增 */
		iCount_ReadLine++;
		
		/* 读取 */
		memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
		pRet = fgets( sString_ReadLine, sizeof( sString_ReadLine ) , fp );

		/* 将字符串移动到pPoint_AddMem这个指针指向的地址空间 */
		if ( NULL == AddMem_For_AddString( iCount_ReadLine, sString_ReadLine, &pPoint_AddMem ) )
		{
			/* 释放内存 */
			FreeMem_For_AddString( &pPoint_AddMem );
			fclose( fp );
			return -2;
		}
		
		memset( sSting_Key, 0x00, sizeof( sSting_Key ) );
		
		/* 
			若这行没有等号，文件末尾退出 或者 跳出本次循环;
			若这行有等号，则匹配=号前面的是否等于Key,等于则取等号后面的值再成功跳出；否则报错退出 
		*/
		if ( NULL == Get_String_Before_Dh_QdKg( pPoint_AddMem, sSting_Key ) )
		{
			/* 这一行没有等号，或者获取失败，则进入该分支 */
			
			if ( NULL == pRet )
				{
				/* 释放内存 */
				FreeMem_For_AddString( &pPoint_AddMem );

				fclose( fp );
				/* 文件末尾，还没有找到对应的Key，则报错失败 */
				return -1;
			}
			else if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
			{
				/* 到某一行结尾，读取次数初始化 */
				iCount_ReadLine = 0;

				continue;  /* continue继续读取下一行 */
			}
		}
		else
		{
			/* 这一行有等号，则进入该分支 */
			
			/* 一行都得到了，则进去判断获取的Key 是否与 目标Key 相同 */
			if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
			{
				/* 匹配是否等于Key */
				if ( 0 == strcmp( sSting_Key, sKey ) )
				{
					memset( sSting_Value, 0x00, sizeof( sSting_Value ) );
					if ( NULL == Get_String_Behind_Dh_QdKg( pPoint_AddMem, sSting_Value ) )
					{
						/* 释放内存 */
						FreeMem_For_AddString( &pPoint_AddMem );

						fclose( fp );
						/* 取不到值报错 */
						return -1;
					}
					else
					{
						/* 释放内存 */
						FreeMem_For_AddString( &pPoint_AddMem );
						
						strcpy( sDst_String, sSting_Value );

						fclose( fp );
						return 0;
					}
				}
				else
				{
					/* 到某一行结尾，读取次数初始化 */
					iCount_ReadLine = 0;

					continue; /* continue继续读取下一行 */
				}
			}
			/* 一行都没有得到则，继续读取该行 */
			{
				continue;  /* continue继续读取该行 */
			}
		}
	}
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Get_ValuseString_ByFieldKey_InConfFile    *
* Description:    根据Field、Key的在配置文件中获取对应的值  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sFile_Name 文件名                         *
*                 sField  目标域的名字
*                 sKey 目标Key的名字                        *
*                 sDst_String Key对应的value的内容          *  
* Out:            -1 失败                                   *
*                 -2 因为内存的问题失败                     *
*                 -3 没有匹配到目标域或Key                  *
*                 0 成功                                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Get_ValuseString_ByFieldKey_InConfFile( const char * sFile_Name, const char * sField, const char * sKey, char * sDst_String )
{
	FILE * fp;                        /* 文件指针 */
	char   sString_ReadLine[4096];    /* 读取文件的buf */
	char   *pPoint_AddMem;            /* 添加内存的指针 */
	int    iCount_ReadLine;           /* 循环读某一行的次数 */
	char   *pRet;                     /* 函数的返回指针内容 */
	char   sSting_Field[4096];        /* 临时存放Key*/
	char   sSting_Key[4096];          /* 临时存放Field */
	char   sSting_Value[4096 * 10];   /* Value值 */
	
	/* 变量初始化 */
	fp = NULL;
	memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
	pPoint_AddMem = NULL;
	iCount_ReadLine = 0;
	pRet = NULL;
	memset( sSting_Field, 0x00, sizeof( sSting_Field ) );
	memset( sSting_Value, 0x00, sizeof( sSting_Value ) );
	
	/* 打开文件 */
	fp = fopen( sFile_Name, "r" );
	if ( NULL == fp )
	{
		return -1;
	}
	
	while ( 1 )
	{
		/* 释放内存 */
		FreeMem_For_AddString( &pPoint_AddMem );
		
		/* 读某一行次数自增 */
		iCount_ReadLine++;
		
		/* 读取 */
		memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
		pRet = fgets( sString_ReadLine, sizeof( sString_ReadLine ) , fp );

		/* 将字符串移动到pPoint_AddMem这个指针指向的地址空间 */
		if ( NULL == AddMem_For_AddString( iCount_ReadLine, sString_ReadLine, &pPoint_AddMem ) )
		{
			/* 释放内存 */
			FreeMem_For_AddString( &pPoint_AddMem );

			fclose( fp );
			return -2;
		}
		
		memset( sSting_Field, 0x00, sizeof( sSting_Field ) );

		/* 
			若这行没有中扩号，文件末尾退出 或者 跳出本次循环;
			若这行有中扩号，则匹配中扩号前面的是否等于Field,等于则取中扩号后面的值再成功跳出；否则报错退出 
		*/
		if ( NULL == Get_String_Between_Zkh( pPoint_AddMem, sSting_Field ) )
		{
			/* 这一行没有等号，或者获取失败，则进入该分支 */

			if ( NULL == pRet )
			{
				/* 释放内存 */
				FreeMem_For_AddString( &pPoint_AddMem );

				fclose( fp );
				/* 文件末尾，还没有找到对应的Field，则报错失败 */
				return -1;
			}
			else if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
			{
				/* 到某一行结尾，读取次数初始化 */
				iCount_ReadLine = 0;

				continue;  /* continue继续读取下一行 */
			}
		}
		else
		{
			/* 这一行有中扩号，则进入该分支 */
			
			/* 一行都得到了，则进去判断获取的Field 是否与 目标Field 相同 */
			if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
			{
				/* 匹配是否等于Filed */
				if ( 0 == strcmp( sSting_Field, sField ) )
				{
					iCount_ReadLine = 0;

					/* 进入找到Key的阶段 */
					while( 1 )
					{
						/* 释放内存 */
						FreeMem_For_AddString( &pPoint_AddMem );
						
						/* 读某一行次数自增 */
						iCount_ReadLine++;
						
						/* 读取 */
						memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
						pRet = fgets( sString_ReadLine, sizeof( sString_ReadLine ) , fp );

						/* 获取中中括号，则认为到了下一个域了，依然没有找到对应Key */
						memset( sSting_Field, 0x00, sizeof( sSting_Field ) );
						if ( NULL != Get_String_Between_Zkh( sString_ReadLine, sSting_Field ) )
						{
							fclose( fp );
							return -3;
						}

						/* 将字符串移动到pPoint_AddMem这个指针指向的地址空间 */
						if ( NULL == AddMem_For_AddString( iCount_ReadLine, sString_ReadLine, &pPoint_AddMem ) )
						{
							/* 释放内存 */
							FreeMem_For_AddString( &pPoint_AddMem );

							fclose( fp );
							return -2;
						}
						
						memset( sSting_Key, 0x00, sizeof( sSting_Key ) );
						
						/* 
							若这行没有等号，文件末尾退出 或者 跳出本次循环;
							若这行有等号，则匹配=号前面的是否等于Key,等于则取等号后面的值再成功跳出；否则报错退出 
						*/
						if ( NULL == Get_String_Before_Dh_QdKg( pPoint_AddMem, sSting_Key ) )
						{
							/* 这一行没有等号，或者获取失败，则进入该分支 */
							
							if ( NULL == pRet )
							{
								/* 释放内存 */
								FreeMem_For_AddString( &pPoint_AddMem );

								fclose( fp );
								/* 文件末尾，还没有找到对应的Key，则报错失败 */
								return -1;
							}
							else if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
							{
								/* 到某一行结尾，读取次数初始化 */
								iCount_ReadLine = 0;

								continue;  /* continue继续读取下一行 */
							}
						}
						else
						{
							/* 这一行有等号，则进入该分支 */
							
							/* 一行都得到了，则进去判断获取的Key 是否与 目标Key 相同 */
							if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
							{
								/* 匹配是否等于Key */
								if ( 0 == strcmp( sSting_Key, sKey ) )
								{
									memset( sSting_Value, 0x00, sizeof( sSting_Value ) );
									if ( NULL == Get_String_Behind_Dh_QdKgHh( pPoint_AddMem, sSting_Value ) )
									{
										/* 释放内存 */
										FreeMem_For_AddString( &pPoint_AddMem );

										fclose( fp );
										/* 取不到值报错 */
										return -1;
									}
									else
									{
										/* 释放内存 */
										FreeMem_For_AddString( &pPoint_AddMem );
										
										strcpy( sDst_String, sSting_Value );

										fclose( fp );
										return 0;
									}
								}
								else
								{
									/* 到某一行结尾，读取次数初始化 */
									iCount_ReadLine = 0;
									
									continue; /* continue继续读取下一行 */
								}
							}
							/* 一行都没有得到则，继续读取该行 */
							{
								continue;  /* continue继续读取该行 */
							}
						}
						
					}
				}
				else
				{
					/* 到某一行结尾，读取次数初始化 */
					iCount_ReadLine = 0;
					
					continue; /* continue继续读取下一行 */
				}
			}
			/* 一行都没有得到则，继续读取该行 */
			{
				continue;  /* continue继续读取该行 */
			}
		}
	}
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Wile_ReadLine_FromConfFile                *
* Description:    循环读取配置文件的有效行                  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             ST_ReadLine 结构体的二级指针              *
* Out:            pPoint_StReadLine->iFlagFirst             *
*                 -2-异常结束，-1-文件读取完毕              *
*                 0-第一次使用未打开文件，1-第一次读取文完成* 
*                 2-第二次打开文件完成，以此类推            *
*                 NULL 完毕或者异常结束                     *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
ST_ReadLine * Wile_ReadLine_FromConfFile( ST_ReadLine ** pPoint_StReadLine )
{
	while( NULL != While_ReadLine_FromFile( pPoint_StReadLine ) )
	{
		Trim( (*pPoint_StReadLine)->sStr_Line );
		if( 0 == strncmp( (*pPoint_StReadLine)->sStr_Line, "#", 1 ) )
		{
			continue;
		}
		else if ( 0 == strcmp( (*pPoint_StReadLine)->sStr_Line, "\n" ) )
		{
			continue;
		}

		return *pPoint_StReadLine;
	}
}



/* 修改配置文件中某Field/Key的值 */



/* 修改配置文件中某Key的值(以文件中第一次出现的Key为准)*/




/*
	中途有不符合格式的，则退出程序，报错
	比如以"|"作为分隔符，要区分分隔符"|" 域 配置值"|" 的区别，配置值"|"使用"\|"的格式
		FILE * const pFile 可读的模式 打开的文件的指针
		char * sIn      以这个字符串的内容，作为分隔符
		char * sOut     读取到的配置文件的内容

	return  2：一行的配置都读取完了，准备调整下一行
			0：配置都已经读取完了
			-1：配置的格式发送，或者其他的错误
			1：这一行还有配置需要取
			
	使用前需要 可读的模式 打开该文件 fopen( filename, "r" )
*/
int GetValue_ByStr( FILE * const pFile, const char * sIn, char * sOut)
{
	char  * pTmp;

	while ( 1 )
	{
		int iRet = GetFileLine_Malloc( pFile, &pTmp );
		if ( -2 == iRet )
		{
			break;
		}
		
		//printf( "[iRet=%d],%s\n", iRet, pTmp );
		//对应发分割处理
		char * pPoint = strstr( pTmp, sIn );
		if ( NULL == pPoint )
		{
			continue;
		}
		
		
		
		GetFileLine_Free( pTmp );
		pTmp = NULL;

		if ( -1 == iRet )
		{
			break;
		}
	}
}


/***********************************************************************************************/







#if 0

/*************************************************************
*    FileName : config.c                                     *
*    FileFunc : 定义实现文件                                 *
*    Version: V0.1                                           *
*    Author : Sunrier                                        *
*    Date : 2012-05-09                                       *
*    Descp : Linux下获取配置文件信息                         *
*************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"


int GetConfigStringValue(char *pInFileName,char *pInSectionName,char *pInKeyName,char *pOutKeyValue)
{
	FILE *fpConfig;
	char szBuffer[150];
	char *pStr1,*pStr2;
	int iRetCode = 0;
	
	/*test*/
	/* 
	printf("pInFileName: %s !\n",pInFileName); 
	printf("pInSectionName: %s !\n",pInSectionName); 
	printf("pInKeyName: %s !\n",pInKeyName); 
	*/
	
	memset(szBuffer,0,sizeof(szBuffer));
	if( NULL==( fpConfig=fopen(pInFileName,"r") ) )
		return FILENAME_NOTEXIST;
		
	while( !feof(fpConfig) )
	{
		if( NULL==fgets(szBuffer,150,fpConfig) )
			break;
		pStr1 = szBuffer ;	
		while( (' '==*pStr1) || ('\t'==*pStr1) )
			pStr1++;
		if( '['==*pStr1 )
		{
			pStr1++;
			while( (' '==*pStr1) || ('\t'==*pStr1) )
				pStr1++;
			pStr2 = pStr1;
			while( (']'!=*pStr1) && ('\0'!=*pStr1) )
				pStr1++;
			if( '\0'==*pStr1)
				continue;
			while( ' '==*(pStr1-1) )
				pStr1--;
			*pStr1 = '\0';

			iRetCode = CompareString(pStr2,pInSectionName); 
			if( !iRetCode )/*检查节名*/
			{
				iRetCode = GetKeyValue(fpConfig,pInKeyName,pOutKeyValue);
				fclose(fpConfig);
				return iRetCode;
			}
		}
	}
	
	fclose(fpConfig);
	return SECTIONNAME_NOTEXIST;
	
}	 

/*区分大小写*/
int CompareString(char *pInStr1,char *pInStr2)
{
	if( strlen(pInStr1)!=strlen(pInStr2) )
	{
		return STRING_LENNOTEQUAL;
	}	 
		
	/*while( toupper(*pInStr1)==toupper(*pInStr2) )*//*#include <ctype.h>*/
	while( *pInStr1==*pInStr2 )
	{
		if( '\0'==*pInStr1 )
			break;	
		pInStr1++;
		pInStr2++;	
	}
	
	if( '\0'==*pInStr1 )
		return STRING_EQUAL;
		
	return STRING_NOTEQUAL; 
	
}

int GetKeyValue(FILE *fpConfig,char *pInKeyName,char *pOutKeyValue)
{
	char szBuffer[150];
	char *pStr1,*pStr2,*pStr3;
	unsigned int uiLen;
	int iRetCode = 0;
	
	memset(szBuffer,0,sizeof(szBuffer));	
	while( !feof(fpConfig) )
	{	 
		if( NULL==fgets(szBuffer,150,fpConfig) )
			break;
		pStr1 = szBuffer;	 
		while( (' '==*pStr1) || ('\t'==*pStr1) )
			pStr1++;
		if( '#'==*pStr1 )	 
			continue;
		if( ('/'==*pStr1)&&('/'==*(pStr1+1)) )	
			continue;	 
		if( ('\0'==*pStr1)||(0x0d==*pStr1)||(0x0a==*pStr1) )	
			continue;	 
		if( '['==*pStr1 )
		{
			pStr2 = pStr1;
			while( (']'!=*pStr1)&&('\0'!=*pStr1) )
				pStr1++;
			if( ']'==*pStr1 )
				break;
			pStr1 = pStr2;	
		}	 
		pStr2 = pStr1;
		while( ('='!=*pStr1)&&('\0'!=*pStr1) )
			pStr1++;
		if( '\0'==*pStr1 )	
			continue;
		pStr3 = pStr1+1;
		if( pStr2==pStr1 )
			continue;	 
		*pStr1 = '\0';
		pStr1--;
		while( (' '==*pStr1)||('\t'==*pStr1) )
		{
			*pStr1 = '\0';
			pStr1--;
		}
		
		iRetCode = CompareString(pStr2,pInKeyName);
		if( !iRetCode )/*检查键名*/
		{
			pStr1 = pStr3;
			while( (' '==*pStr1)||('\t'==*pStr1) )
				pStr1++;
			pStr3 = pStr1;
			while( ('\0'!=*pStr1)&&(0x0d!=*pStr1)&&(0x0a!=*pStr1) )
			{
				if( ('/'==*pStr1)&&('/'==*(pStr1+1)) )
					break;
				pStr1++;	
			}	 
			*pStr1 = '\0';
			uiLen = strlen(pStr3);
			memcpy(pOutKeyValue,pStr3,uiLen);
			*(pOutKeyValue+uiLen) = '\0';
			return SUCCESS;
		}
	}
	
	return KEYNAME_NOTEXIST;
}

int GetConfigIntValue(char *pInFileName,char *pInSectionName,char *pInKeyName,int *pOutKeyValue)
{
	int iRetCode = 0;
	char szKeyValue[16],*pStr;
	
	memset(szKeyValue,0,sizeof(szKeyValue));
	iRetCode = GetConfigStringValue(pInFileName,pInSectionName,pInKeyName,szKeyValue);
	if( iRetCode )
		return iRetCode;
	pStr	= szKeyValue;
	while( (' '==*pStr)||('\t'==*pStr))
		pStr++;
	if( ('0'==*pStr)&&( ('x'==*(pStr+1))||('X'==*(pStr+1)) ) )	
		sscanf(pStr+2,"%x",pOutKeyValue);
	else
		sscanf(pStr,"%d",pOutKeyValue);
		
	return SUCCESS; 
			
}


#endif

#if 0

#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include "config.h"  
  
int main (int argc,char *argv[])  
{  
    char szFileName[100];/*配置文件名*/  
    char szSectionName[100];/*节名*/  
    char szKeyName[100];/*键名*/    
    char szKeyValue[100];/*键值*/   
    int iRetCode = 0;  
    int iPort = -1;  
    char szHostIp[30];  
      
    memset(szFileName,0,sizeof(szFileName));  
    sprintf(szFileName,"%s/etc/sysconfig",getenv("HOME"));  
    memset(szSectionName,0,sizeof(szSectionName));  
    memcpy(szSectionName,argv[1],sizeof(argv[1]));  
    memset(szKeyName,0,sizeof(szKeyName));  
    memcpy(szKeyName,argv[2],sizeof(argv[2]));  
    memset(szKeyValue,0,sizeof(szKeyValue));  
    memset(szHostIp,0,sizeof(szHostIp));  
      
    iRetCode = GetConfigStringValue(szFileName,argv[1],argv[2],szHostIp);  
    if( iRetCode )  
    {  
        printf("iRetCode : %d !\n",iRetCode );  
    }  
    else  
    {  
        printf("HOSTIP: %s\n",szHostIp);  
    }  
      
    iRetCode = GetConfigIntValue(szFileName,"Config1","PORT",&iPort);  
    if( iRetCode )  
    {  
        printf("iRetCode : %d !\n",iRetCode );  
    }  
    else  
    {  
        printf("PORT: %d\n",iPort);  
    }  
      
    return 0;  
}  

[Sunrier@localhost Sunrier]$ ./Manager Config1 HOSTIP
HOSTIP: 192.168.1.2
PORT: 8080

config文件内容：
[Config1]  
PORT=8080  
#HOSTIP=192.168.1.1  
HOSTIP=192.168.1.2  
SENDIP=192.168.1.3  

#endif


