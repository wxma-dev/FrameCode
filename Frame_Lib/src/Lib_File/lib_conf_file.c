
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    AddMem_For_AddString                      *
* Description:    ��ȡ�����ļ�ĳһ�У�����ڴ棬�洢�ַ���  *
*                 ����ǵ�һ�ζ�ȡĳһ����malloc�����һ��  *
*                 ���϶�ȡĳһ����realloc�������µ�ַ���   *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             iCount_ReadLine ��ȡĳһ�еĴ���          *
*                 pPoint_AddMem  ���ڴ�ĵ�ַ               *
* Out:            NULL ʧ��                                 *
*                 ��NULL �ɹ�,�µ�ַ                        *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * AddMem_For_AddString( const int iCount_ReadLine, const char * sSrc_String, char ** pPoint_AddMem )
{
	char   * pPoint_Tmp;             /* ��ʱָ�� */
	
	/* ������ʼ�� */
	pPoint_Tmp = NULL;

	/* ��һ�ζ�ȡ */
	if ( 1 == iCount_ReadLine )
	{
		if ( NULL == AddStr_Malloc( sSrc_String, pPoint_AddMem ) )
		{
			return NULL;
		}
	}
	else
	{
		/* ��һ�����϶�ȡ����Ҫ��ԭ��̬�ڴ��β������ַ��� */
		if ( NULL == AddStr_Realloc( *pPoint_AddMem , sSrc_String, &pPoint_Tmp ) )
		{
			return NULL;
		}
		/* �µ�ַ */
		*pPoint_AddMem = pPoint_Tmp;
	}
	
	return *pPoint_AddMem;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    FreeMem_For_AddString                     *
* Description:    �ͷ�AddMem_For_AddString��ӵ��ڴ�        *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_NeedFree ��Ҫ�ͷŵ��ڴ�            *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_ValuseString_ByKey_InConfFile         *
* Description:    �������ļ��л�ȡĿ��Key������             *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sFile_Name �ļ���                         *
*                 sKey Ŀ��Key������                        *
*                 sDst_String Key��Ӧ��value������          *  
* Out:            -1 ʧ��                                   *
*                 -2 ��Ϊ�ڴ������ʧ��                     *
*                 -3 û��ƥ�䵽Ŀ����                       *
*                 0 �ɹ�                                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Get_ValuseString_ByKey_InConfFile( const char * sFile_Name, const char * sKey, char * sDst_String )
{
	FILE * fp;                        /* �ļ�ָ�� */
	char   sString_ReadLine[4096];    /* ��ȡ�ļ���buf */
	char   *pPoint_AddMem;            /* ����ڴ��ָ�� */
	int    iCount_ReadLine;           /* ѭ����ĳһ�еĴ��� */
	char   *pRet;                     /* �����ķ���ָ������ */
	char   sSting_Key[4096];          /* ��ʱ���Key */
	char   sSting_Value[4096 * 10];   /* Valueֵ */
	
	/* ������ʼ�� */
	fp = NULL;
	memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
	pPoint_AddMem = NULL;
	iCount_ReadLine = 0;
	pRet = NULL;
	memset( sSting_Key, 0x00, sizeof( sSting_Key ) );
	memset( sSting_Value, 0x00, sizeof( sSting_Value ) );
	
	/* ���ļ� */
	fp = fopen( sFile_Name, "r" );
	if ( NULL == fp )
	{
		return -1;
	}
	
	while ( 1 )
	{
		/* �ͷ��ڴ� */
		FreeMem_For_AddString( &pPoint_AddMem );
		
		/* ��ĳһ�д������� */
		iCount_ReadLine++;
		
		/* ��ȡ */
		memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
		pRet = fgets( sString_ReadLine, sizeof( sString_ReadLine ) , fp );

		/* ���ַ����ƶ���pPoint_AddMem���ָ��ָ��ĵ�ַ�ռ� */
		if ( NULL == AddMem_For_AddString( iCount_ReadLine, sString_ReadLine, &pPoint_AddMem ) )
		{
			/* �ͷ��ڴ� */
			FreeMem_For_AddString( &pPoint_AddMem );
			fclose( fp );
			return -2;
		}
		
		memset( sSting_Key, 0x00, sizeof( sSting_Key ) );
		
		/* 
			������û�еȺţ��ļ�ĩβ�˳� ���� ��������ѭ��;
			�������еȺţ���ƥ��=��ǰ����Ƿ����Key,������ȡ�Ⱥź����ֵ�ٳɹ����������򱨴��˳� 
		*/
		if ( NULL == Get_String_Before_Dh_QdKg( pPoint_AddMem, sSting_Key ) )
		{
			/* ��һ��û�еȺţ����߻�ȡʧ�ܣ������÷�֧ */
			
			if ( NULL == pRet )
				{
				/* �ͷ��ڴ� */
				FreeMem_For_AddString( &pPoint_AddMem );

				fclose( fp );
				/* �ļ�ĩβ����û���ҵ���Ӧ��Key���򱨴�ʧ�� */
				return -1;
			}
			else if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
			{
				/* ��ĳһ�н�β����ȡ������ʼ�� */
				iCount_ReadLine = 0;

				continue;  /* continue������ȡ��һ�� */
			}
		}
		else
		{
			/* ��һ���еȺţ������÷�֧ */
			
			/* һ�ж��õ��ˣ����ȥ�жϻ�ȡ��Key �Ƿ��� Ŀ��Key ��ͬ */
			if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
			{
				/* ƥ���Ƿ����Key */
				if ( 0 == strcmp( sSting_Key, sKey ) )
				{
					memset( sSting_Value, 0x00, sizeof( sSting_Value ) );
					if ( NULL == Get_String_Behind_Dh_QdKg( pPoint_AddMem, sSting_Value ) )
					{
						/* �ͷ��ڴ� */
						FreeMem_For_AddString( &pPoint_AddMem );

						fclose( fp );
						/* ȡ����ֵ���� */
						return -1;
					}
					else
					{
						/* �ͷ��ڴ� */
						FreeMem_For_AddString( &pPoint_AddMem );
						
						strcpy( sDst_String, sSting_Value );

						fclose( fp );
						return 0;
					}
				}
				else
				{
					/* ��ĳһ�н�β����ȡ������ʼ�� */
					iCount_ReadLine = 0;

					continue; /* continue������ȡ��һ�� */
				}
			}
			/* һ�ж�û�еõ��򣬼�����ȡ���� */
			{
				continue;  /* continue������ȡ���� */
			}
		}
	}
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Get_ValuseString_ByFieldKey_InConfFile    *
* Description:    ����Field��Key���������ļ��л�ȡ��Ӧ��ֵ  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sFile_Name �ļ���                         *
*                 sField  Ŀ���������
*                 sKey Ŀ��Key������                        *
*                 sDst_String Key��Ӧ��value������          *  
* Out:            -1 ʧ��                                   *
*                 -2 ��Ϊ�ڴ������ʧ��                     *
*                 -3 û��ƥ�䵽Ŀ�����Key                  *
*                 0 �ɹ�                                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Get_ValuseString_ByFieldKey_InConfFile( const char * sFile_Name, const char * sField, const char * sKey, char * sDst_String )
{
	FILE * fp;                        /* �ļ�ָ�� */
	char   sString_ReadLine[4096];    /* ��ȡ�ļ���buf */
	char   *pPoint_AddMem;            /* ����ڴ��ָ�� */
	int    iCount_ReadLine;           /* ѭ����ĳһ�еĴ��� */
	char   *pRet;                     /* �����ķ���ָ������ */
	char   sSting_Field[4096];        /* ��ʱ���Key*/
	char   sSting_Key[4096];          /* ��ʱ���Field */
	char   sSting_Value[4096 * 10];   /* Valueֵ */
	
	/* ������ʼ�� */
	fp = NULL;
	memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
	pPoint_AddMem = NULL;
	iCount_ReadLine = 0;
	pRet = NULL;
	memset( sSting_Field, 0x00, sizeof( sSting_Field ) );
	memset( sSting_Value, 0x00, sizeof( sSting_Value ) );
	
	/* ���ļ� */
	fp = fopen( sFile_Name, "r" );
	if ( NULL == fp )
	{
		return -1;
	}
	
	while ( 1 )
	{
		/* �ͷ��ڴ� */
		FreeMem_For_AddString( &pPoint_AddMem );
		
		/* ��ĳһ�д������� */
		iCount_ReadLine++;
		
		/* ��ȡ */
		memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
		pRet = fgets( sString_ReadLine, sizeof( sString_ReadLine ) , fp );

		/* ���ַ����ƶ���pPoint_AddMem���ָ��ָ��ĵ�ַ�ռ� */
		if ( NULL == AddMem_For_AddString( iCount_ReadLine, sString_ReadLine, &pPoint_AddMem ) )
		{
			/* �ͷ��ڴ� */
			FreeMem_For_AddString( &pPoint_AddMem );

			fclose( fp );
			return -2;
		}
		
		memset( sSting_Field, 0x00, sizeof( sSting_Field ) );

		/* 
			������û�������ţ��ļ�ĩβ�˳� ���� ��������ѭ��;
			�������������ţ���ƥ��������ǰ����Ƿ����Field,������ȡ�����ź����ֵ�ٳɹ����������򱨴��˳� 
		*/
		if ( NULL == Get_String_Between_Zkh( pPoint_AddMem, sSting_Field ) )
		{
			/* ��һ��û�еȺţ����߻�ȡʧ�ܣ������÷�֧ */

			if ( NULL == pRet )
			{
				/* �ͷ��ڴ� */
				FreeMem_For_AddString( &pPoint_AddMem );

				fclose( fp );
				/* �ļ�ĩβ����û���ҵ���Ӧ��Field���򱨴�ʧ�� */
				return -1;
			}
			else if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
			{
				/* ��ĳһ�н�β����ȡ������ʼ�� */
				iCount_ReadLine = 0;

				continue;  /* continue������ȡ��һ�� */
			}
		}
		else
		{
			/* ��һ���������ţ������÷�֧ */
			
			/* һ�ж��õ��ˣ����ȥ�жϻ�ȡ��Field �Ƿ��� Ŀ��Field ��ͬ */
			if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
			{
				/* ƥ���Ƿ����Filed */
				if ( 0 == strcmp( sSting_Field, sField ) )
				{
					iCount_ReadLine = 0;

					/* �����ҵ�Key�Ľ׶� */
					while( 1 )
					{
						/* �ͷ��ڴ� */
						FreeMem_For_AddString( &pPoint_AddMem );
						
						/* ��ĳһ�д������� */
						iCount_ReadLine++;
						
						/* ��ȡ */
						memset( sString_ReadLine, 0x00, sizeof( sString_ReadLine ) );
						pRet = fgets( sString_ReadLine, sizeof( sString_ReadLine ) , fp );

						/* ��ȡ�������ţ�����Ϊ������һ�����ˣ���Ȼû���ҵ���ӦKey */
						memset( sSting_Field, 0x00, sizeof( sSting_Field ) );
						if ( NULL != Get_String_Between_Zkh( sString_ReadLine, sSting_Field ) )
						{
							fclose( fp );
							return -3;
						}

						/* ���ַ����ƶ���pPoint_AddMem���ָ��ָ��ĵ�ַ�ռ� */
						if ( NULL == AddMem_For_AddString( iCount_ReadLine, sString_ReadLine, &pPoint_AddMem ) )
						{
							/* �ͷ��ڴ� */
							FreeMem_For_AddString( &pPoint_AddMem );

							fclose( fp );
							return -2;
						}
						
						memset( sSting_Key, 0x00, sizeof( sSting_Key ) );
						
						/* 
							������û�еȺţ��ļ�ĩβ�˳� ���� ��������ѭ��;
							�������еȺţ���ƥ��=��ǰ����Ƿ����Key,������ȡ�Ⱥź����ֵ�ٳɹ����������򱨴��˳� 
						*/
						if ( NULL == Get_String_Before_Dh_QdKg( pPoint_AddMem, sSting_Key ) )
						{
							/* ��һ��û�еȺţ����߻�ȡʧ�ܣ������÷�֧ */
							
							if ( NULL == pRet )
							{
								/* �ͷ��ڴ� */
								FreeMem_For_AddString( &pPoint_AddMem );

								fclose( fp );
								/* �ļ�ĩβ����û���ҵ���Ӧ��Key���򱨴�ʧ�� */
								return -1;
							}
							else if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
							{
								/* ��ĳһ�н�β����ȡ������ʼ�� */
								iCount_ReadLine = 0;

								continue;  /* continue������ȡ��һ�� */
							}
						}
						else
						{
							/* ��һ���еȺţ������÷�֧ */
							
							/* һ�ж��õ��ˣ����ȥ�жϻ�ȡ��Key �Ƿ��� Ŀ��Key ��ͬ */
							if ( '\n' == sString_ReadLine[ strlen(sString_ReadLine) - 1 ] )
							{
								/* ƥ���Ƿ����Key */
								if ( 0 == strcmp( sSting_Key, sKey ) )
								{
									memset( sSting_Value, 0x00, sizeof( sSting_Value ) );
									if ( NULL == Get_String_Behind_Dh_QdKgHh( pPoint_AddMem, sSting_Value ) )
									{
										/* �ͷ��ڴ� */
										FreeMem_For_AddString( &pPoint_AddMem );

										fclose( fp );
										/* ȡ����ֵ���� */
										return -1;
									}
									else
									{
										/* �ͷ��ڴ� */
										FreeMem_For_AddString( &pPoint_AddMem );
										
										strcpy( sDst_String, sSting_Value );

										fclose( fp );
										return 0;
									}
								}
								else
								{
									/* ��ĳһ�н�β����ȡ������ʼ�� */
									iCount_ReadLine = 0;
									
									continue; /* continue������ȡ��һ�� */
								}
							}
							/* һ�ж�û�еõ��򣬼�����ȡ���� */
							{
								continue;  /* continue������ȡ���� */
							}
						}
						
					}
				}
				else
				{
					/* ��ĳһ�н�β����ȡ������ʼ�� */
					iCount_ReadLine = 0;
					
					continue; /* continue������ȡ��һ�� */
				}
			}
			/* һ�ж�û�еõ��򣬼�����ȡ���� */
			{
				continue;  /* continue������ȡ���� */
			}
		}
	}
}

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Wile_ReadLine_FromConfFile                *
* Description:    ѭ����ȡ�����ļ�����Ч��                  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             ST_ReadLine �ṹ��Ķ���ָ��              *
* Out:            pPoint_StReadLine->iFlagFirst             *
*                 -2-�쳣������-1-�ļ���ȡ���              *
*                 0-��һ��ʹ��δ���ļ���1-��һ�ζ�ȡ�����* 
*                 2-�ڶ��δ��ļ���ɣ��Դ�����            *
*                 NULL ��ϻ����쳣����                     *
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



/* �޸������ļ���ĳField/Key��ֵ */



/* �޸������ļ���ĳKey��ֵ(���ļ��е�һ�γ��ֵ�KeyΪ׼)*/




/*
	��;�в����ϸ�ʽ�ģ����˳����򣬱���
	������"|"��Ϊ�ָ�����Ҫ���ַָ���"|" �� ����ֵ"|" ����������ֵ"|"ʹ��"\|"�ĸ�ʽ
		FILE * const pFile �ɶ���ģʽ �򿪵��ļ���ָ��
		char * sIn      ������ַ��������ݣ���Ϊ�ָ���
		char * sOut     ��ȡ���������ļ�������

	return  2��һ�е����ö���ȡ���ˣ�׼��������һ��
			0�����ö��Ѿ���ȡ����
			-1�����õĸ�ʽ���ͣ����������Ĵ���
			1����һ�л���������Ҫȡ
			
	ʹ��ǰ��Ҫ �ɶ���ģʽ �򿪸��ļ� fopen( filename, "r" )
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
		//��Ӧ���ָ��
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
*    FileFunc : ����ʵ���ļ�                                 *
*    Version: V0.1                                           *
*    Author : Sunrier                                        *
*    Date : 2012-05-09                                       *
*    Descp : Linux�»�ȡ�����ļ���Ϣ                         *
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
			if( !iRetCode )/*������*/
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

/*���ִ�Сд*/
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
		if( !iRetCode )/*������*/
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
    char szFileName[100];/*�����ļ���*/  
    char szSectionName[100];/*����*/  
    char szKeyName[100];/*����*/    
    char szKeyValue[100];/*��ֵ*/   
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

config�ļ����ݣ�
[Config1]  
PORT=8080  
#HOSTIP=192.168.1.1  
HOSTIP=192.168.1.2  
SENDIP=192.168.1.3  

#endif


