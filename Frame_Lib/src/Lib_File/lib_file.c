#include "lib_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>

/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxxxxxx                 *
* Source File:    Get_FileSize                              *
* Description:    ��ȡ�ļ���С(�ֽ�)                        *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             filename �ļ���(����·��)                 *
*                 valuename_str �����ֶ�����                *
                  value_str �����ֶ�����                    *
* Out:            0 ������                                  *
*                 1 ����                                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
off_t Get_FileSize( char * filename )
{
    struct stat file_stat;
    off_t file_size;
    
    memset( &file_stat, 0x00, sizeof( file_stat ) );
    file_size = 0;
    
    stat( filename, &file_stat );
    
    return file_stat.st_size;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    GetFileRowCount                           *
* Description:    ��ȡ�ļ�����                              *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sFilename �ļ�����·��                    *
*                 pNum �ļ���������ָ��                     *
* Out:            -1 ʧ��                                   *
*                 ���� �ļ�������                           *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int GetFileRowCount( char *sFilename, int * pNum )
{
	char sString[1024];    /* buf */
	int  iNum;
	char *pRet;
	int  iLeng_Str;
	
	memset( sString, 0x00, sizeof( sString ) );
	iNum = 0;
	pRet = NULL;
	iLeng_Str = 0;
	
	/* ���ļ� */
	FILE * fp = fopen( sFilename, "r" );
	
	/* ������ļ�ΪNULL����return -1 */
	if ( NULL == fp )
	{
		return -1;
	}
	
	/* ѭ����ȡ�ļ���ͳ������ */
	while( 1 ) 
	{
		/* ��ȡ */
		pRet = fgets( sString, sizeof( sString ) , fp );
		
		/* ΪNULL˵���Ѿ������ļ�β����������ѭ�� */
		if ( NULL == pRet )  
		{
			break;
		}
		
		/* ��ȡ��ȡ�����ַ����ĳ��� */
		iLeng_Str = strlen( sString );
		
		/* ��Ϊ���з�������Ϊ��һ�У�����continue */
		if ( '\n' == sString[ iLeng_Str ] )
		{
			iNum++;
		}
		else
		{
			continue;
		}
	}
	(*pNum) = iNum;

	return *pNum;
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    While_ReadLine_FromFile                   *
* Description:    ѭ����ȡ�ļ���ÿһ��                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_StReadLine ST_ReadLine�ṹ��Ķ���ָ��*
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
ST_ReadLine * While_ReadLine_FromFile( ST_ReadLine ** pPoint_StReadLine )
{
	char  * pPoint_Tmp;

	/* 0-��һ��ʹ��δ���ļ� */
	if ( 0 == (*pPoint_StReadLine)->iFlagFirst )
	{
		(*pPoint_StReadLine)->pPoint_fp = fopen( (*pPoint_StReadLine)->sStr_FilePathName, "r" );
		if ( NULL == (*pPoint_StReadLine)->pPoint_fp )
		{
			(*pPoint_StReadLine)->iFlagFirst = -2;
			return NULL;
		}
	}

	/* ��������ȡ�ļ� */
	if ( -1 != (*pPoint_StReadLine)->iFlagFirst && -2 != (*pPoint_StReadLine)->iFlagFirst )
	{
		memset( (*pPoint_StReadLine)->sStr_Line, 0x00, sizeof( (*pPoint_StReadLine)->sStr_Line ) );
		pPoint_Tmp = fgets( (*pPoint_StReadLine)->sStr_Line, sizeof( (*pPoint_StReadLine)->sStr_Line ), (*pPoint_StReadLine)->pPoint_fp );
		if ( NULL == pPoint_Tmp )
		{
			if ( 0 != errno )
			{
				(*pPoint_StReadLine)->iFlagFirst = -1;
				return NULL;
			}
			else
			{
				(*pPoint_StReadLine)->iFlagFirst = -2;
				return NULL;
			}
		}
		else
		{
			((*pPoint_StReadLine)->iFlagFirst)++;
			return *pPoint_StReadLine;
		}
	}
}


/*
	���������Ҫ�ƶ�����Ӧ���ļ��Ĳ���������ȥ
	���ܣ���ȡ�ļ���һ�У����������ļ�һ�� �� ��̬�ڴ��ָ�� ���أ�ע������ڴ�ʹ�ú�ǵ��ͷ�
	���Σ�   pOut  malloc�ĵ�ַ�������ļ�һ�е�����
	return  �Ǹ��������еĳ���
			0 ����
			-1 �Ѿ�˳�������ļ�ĩβ  ��aEOF", ��ȡa ��return -1
			-2 �����ļ�ĩβ      "a\nEOF" ���������2�ζ�����һ�ζ�a���ڶ��ζ�ȡEOF return -2
			-3 ����

*/
int GetFileLine_Malloc( FILE * fp , char ** pOut )
{
    char * pTmp = NULL;
    char * pRet = NULL;
    int    iCount_Xh = 0;
    int    iLength = 0;
    int    iChar = 0;
    int    iCount_Line = 0;
    
    if ( EOF == fgetc( fp ) )
    {
        return -2;
    }
    fseek( fp, -1, SEEK_CUR );
    
    while( 1 )
    {
        iChar = fgetc( fp );
        
        if ( '\n' == iChar )
        {
            iCount_Line++;
            break;
        }
        else if ( EOF == iChar )
        {
            iCount_Line++;
            break;
        }
        else
        {
            iCount_Line++;
        }
    }
    
    if ( '\n' == iChar )
    {
    	fseek( fp, -iCount_Line, SEEK_CUR );
    }
    else if ( EOF == iChar )
    {
    	fseek( fp, -( iCount_Line -1 ), SEEK_CUR );
    }

    pTmp = (char *) malloc( iCount_Line );
    memset( pTmp, 0x00, iCount_Line );

    fgets( pTmp, iCount_Line , fp );
    *pOut = pTmp;

	if ( iChar == EOF )
	{
		return -1;
	}
	else if ( iChar == '\n' )
	{
    		fseek( fp, 1, SEEK_CUR );
    		return ( iCount_Line - 1 );
	}

	return -3;
}


/*
�ͷ� GetFileLine_Malloc �õ��ĵ�ַ
*/
void GetFileLine_Free( char * pIn )
{
    free( pIn );
}




/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    Do_File                                   *
* Description:    �ļ�����ش���                            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sInfile    ��Ҫ������ļ�                 *
*                 iFlag == 'y' ������£����� SWITCH CASE �����Ĵ��� *
*                 �����Ĳ�����bin�ļ��Ĳ���Ϊ׼             *
* Out:            sOutFile   ����õ��ļ�                   *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
/*
    �ú����е���غ�Ķ��壬�����ͷ�ļ��еĶ���
*/
int Do_File( char * sInfile, char * sOutFile, char iFlag )
{

    char * sFileName = sInfile;
    char * sNewFile  = sOutFile;
    int  cFlag = iFlag;
    FILE * fpFile = NULL;
    FILE * fpFile1 = NULL;
    char * sBig_Buf = NULL;

    DEBUG_FRAME( "sInfile=[%s],sOutFile=[%s]", sFileName,  sNewFile);

    if ( _FOUR_KG_DQBQ_ == cFlag || _TAB_DQBQ_ == cFlag )
    {
        printf( "�����ļ���Ϊ[%s]\n", sFileName );

        /* ����ͷ��tab����4���ո� */
        Do_File( sFileName, sNewFile , _LINE_HEAD_ALL_TAB_DO_REPLACE_FOUR_KG_ );

        char   sNewFile1[256];
        memset( sNewFile1    , 0x00, sizeof( sNewFile1     ) );

        /* �ļ�������βȥ���ո� */
        Do_File( sNewFile, sNewFile1 , _DELETE_LINE_LEFT_RIGHT_KG_ );

        char   sNewFile2[256];
        memset( sNewFile2    , 0x00, sizeof( sNewFile2     ) );

        /* windows���з��滻��unix���з��� */
        Do_File( sNewFile1, sNewFile2 , _WINDOWS_HH_DO_REPLACE_UNIX_HH_ );

        char   sNewFile3[256];
        memset( sNewFile3    , 0x00, sizeof( sNewFile3     ) );

        /* һ���ļ���4���ո���Ϊ���䲹�룬 */
        Do_File( sNewFile1, sNewFile3 , _NO_HAVA_ANY_SJ_DO_ADD_FOUR_KG_DQBQ_ );

        if ( _TAB_DQBQ_ == cFlag )
        {

            char   sNewFile4[256];
            memset( sNewFile4    , 0x00, sizeof( sNewFile4     ) );

            /* ��4���ո񻻳�tab */
            Do_File( sNewFile2, sNewFile4 , _LINE_HEAD_ALL_FOUR_KG_DO_REPLACE_TAB_ );

            printf( "����ļ���Ϊ[%s]\n", sNewFile4 );

            if ( 0 != remove( sNewFile1 ) ) return -1;
            if ( 0 != remove( sNewFile2 ) ) return -1;
            if ( 0 != remove( sNewFile3 ) ) return -1;

            /* ����õ��ļ� �����޸ĳ� Ҫ������ļ��� */
            if ( 0 != rename( sNewFile4, sNewFile ) ) return -1;

        }
        else if ( _FOUR_KG_DQBQ_ == cFlag )
        {

            printf( "����ļ���Ϊ[%s]\n", sNewFile2 );
            if ( 0 != remove( sNewFile1 ) ) return -1;
            if ( 0 != remove( sNewFile2 ) ) return -1;

            /* ����õ��ļ� �����޸ĳ� Ҫ������ļ��� */
            if ( 0 != rename( sNewFile3, sNewFile ) ) return -1;
        }
    }
    else if ( _PUT_FILE_ALL_FUN_FROM_SRC_ == cFlag || cFlag == _PUT_FILE_SHOW_ALL_FUN_FROM_HEADFILE_ )
    {
        long iSizeFile = Get_FileSize( sFileName );
        sBig_Buf = (char *)malloc( iSizeFile );
    }

    if ( 0 == strlen( sNewFile ) )
    {
        sprintf( sNewFile, "%s%s%s", sFileName, "_", "outtmp" );
        DEBUG_FRAME( "��sOutFileΪ�գ�Ĭ��sOutFile=[%s]", sNewFile);
    }

    if( ( fpFile = fopen( sFileName, "r" ) ) == NULL )
    {
        ERROR_FRAME( "fopen [%s] fail",  sFileName );
        fclose( fpFile );
        return -1;
    }
    DEBUG_FRAME( "fopen [%s] succ",  sFileName );

    if( ( fpFile1 = fopen( sNewFile, "w+" ) ) == NULL )
    {
        ERROR_FRAME( "fopen [%s] fail",  sNewFile );
        fclose( fpFile1 );
        return -1;
    }
    DEBUG_FRAME( "fopen [%s] succ",  sNewFile );

    char sTmp[_LINE_MAX_];
    memset( sTmp, 0x00, sizeof( sTmp ) );

    //unix�Ķ��л���2�л��з�
    if ( cFlag == _MULTIPLE_HH_DO_REPLACE_TWO_HH_ )
    {
        int iChar = 0;
        int iChar1 = 0;
        int iChar_Flag = 0;

        while ( EOF != ( iChar = fgetc( fpFile ) ) )
        {
            if ( iChar == '\n' )
            {
                //ͳ��'\n'����
                iChar_Flag++;
            }
            else
            {
                if ( 1 == iChar_Flag )
                {
                    //һ��������ֱ�Ӵ�ӡ
                    fputc( '\n', fpFile1 );
                    fputc( iChar, fpFile1 );
                }
                else if ( iChar_Flag >= 2 )
                {
                    //��������У�����2�����д���
                    fputc( '\n', fpFile1 );
                    fputc( '\n', fpFile1 );
                    fputc( iChar, fpFile1 );
                }

                else if ( 0 == iChar_Flag )
                {
                    //û�л����ַ���ֱ��д����ַ�
                    fputc( iChar, fpFile1 );
                }

                //�ǻ��е��ַ���iChar_Flag��Ϊ0
                iChar_Flag = 0;
            }
        }
    }
    //���������
    else 
    {
        int iFlag_Dkh    = 0;            /* �����ŵĶ��� */
        int iFlag_Switch = 0;            /* SWITCH ��־  */
        int iFlag_Case   = 0;            /* CASE   ��־  */
        int iFlag_Tmp    = -1;           /* ��ʱ��־     */
        int iCount_Hs    = 0;
        int iSize_Kh     = 0;
        
        while ( NULL != fgets( sTmp, sizeof( sTmp ), fpFile ) )
        {
            iCount_Hs++;
            if ( cFlag == _IF_LINE_ONLY_KG_AND_TABL_DO_DELETE_ALL_ || 
                 cFlag == _IF_LINE_ONLY_KG_DO_DELETE_ALL_KG_       || 
                 cFlag == _IF_LINE_ONLY_TAB_DO_DELETE_ALL_TAB_        
                )
            {
                int i = 0;
                int iLength = strlen( sTmp );
                int iFlag = 0;
                for( i = 0; i < iLength; i++ )
                {
                    if ( cFlag == _IF_LINE_ONLY_KG_AND_TABL_DO_DELETE_ALL_ && ( sTmp[i] != '\t' && sTmp[i] != ' ' ) )
                    {
                        iFlag = 1;
                        break;
                    }

                    if ( cFlag == _IF_LINE_ONLY_TAB_DO_DELETE_ALL_TAB_ && sTmp[i] != '\t' )
                    {
                        iFlag = 1;
                        break;
                    }

                    if ( cFlag == _IF_LINE_ONLY_KG_DO_DELETE_ALL_KG_ && sTmp[i] != ' ' )
                    {
                        iFlag = 1;
                        break;
                    }
                }

                if ( i == iLength && 0 == iFlag )
                {
                    fprintf( fpFile1, "\n" );
                }
                else if ( 1 == iFlag )
                {
                    fprintf( fpFile1, "%s", sTmp );
                }

            }
            else if ( cFlag == _WINDOWS_HH_DO_REPLACE_UNIX_HH_ )
            {
                int iLength_sTmp = strlen( sTmp );
                if ( sTmp[iLength_sTmp - 1] == '\n' && sTmp[iLength_sTmp - 2] == '\r' )
                {
                    char sTmp1[_LINE_MAX_];
                    memset( sTmp1, 0x00, sizeof( sTmp1 ) );
                    memcpy( sTmp1, sTmp, strlen( sTmp ) -2 );

                    fprintf( fpFile1, "%s%s", sTmp1, "\n" );
                }
                else
                {
                    fprintf( fpFile1, "%s", sTmp );
                }
            }
            else if ( cFlag == _UNIX_HH_DO_REPLACE_WINDOWS_HH_ )
            {
                int iLength_sTmp = strlen( sTmp );
                if ( sTmp[iLength_sTmp - 1] == '\n' && sTmp[iLength_sTmp - 2] != '\r' )
                {
                    char sTmp1[_LINE_MAX_];
                    memset( sTmp1, 0x00, sizeof( sTmp1 ) );
                    memcpy( sTmp1, sTmp, strlen( sTmp ) -1 );

                    fprintf( fpFile1, "%s%s", sTmp1, "\r\n" );
                }
                else
                {
                    fprintf( fpFile1, "%s", sTmp );
                }
            }
            else if ( cFlag == _LINE_HEAD_ALL_TAB_DO_REPLACE_FOUR_KG_ )
            {
                int iLength_sTmp = strlen( sTmp );
                int i = 0;

                for( i = 0; i < iLength_sTmp; i++ )
                {
                    if ( '\t' != sTmp[i] && ' ' != sTmp[i] )
                    {
                        fprintf( fpFile1, "%s", sTmp + i );
                        break;
                    }

                    if ( '\t' == sTmp[i] )
                    {
                        fprintf( fpFile1, "%s", "    " );        /* 4���ո� */
                    }
                    else if ( ' ' == sTmp[i] )
                    {
                        fprintf( fpFile1, "%s", " " );           /* 1���ո� */
                    }
                }
            }
            else if ( cFlag == _LINE_HEAD_ALL_FOUR_KG_DO_REPLACE_TAB_ )
            {
                int iLength_sTmp = strlen( sTmp );
                int i = 0;

                for( i = 0; i < iLength_sTmp; )
                {
                    if ( iLength_sTmp < i+4 )
                    {
                        fprintf( fpFile1, "%s", sTmp + i );
                        break;
                    }

                    if ( '\t' != sTmp[i] && 0 !=  strncmp( "    ", sTmp + i, 4 ) )
                    {
                        fprintf( fpFile1, "%s", sTmp + i );
                        break;
                    }

                    if ( 0 ==  strncmp( "    ", sTmp + i, 4 ) )
                    {
                        fprintf( fpFile1, "%s", "\t" );        /* TAB */
                        i = i + 4;
                    }
                    else 
                    {
                        if ( '\t' == sTmp[i] )
                        {
                            fprintf( fpFile1, "%s", "\t" );
                        }

                        i++;
                    }
                }
            }
            else if ( cFlag == _NO_HAVA_ANY_SJ_DO_ADD_FOUR_KG_DQBQ_ ) 
            {
                /*
                switch(chflg)
                    {
                    case 'l':
                        {
                            strcpy( aczLogFile, optarg );
                            printf( "��־·��:%s \n",aczLogFile);
                        }
                        break;

                    case 'p':
                        strcpy( sFileName, optarg );
                        printf( "��Ҫ������ļ����ɴ�·����:%s \n",sFileName);
                        break;
                }
                */

                #define _SWITCH_CASE_  1

                if ( NULL != strstr( sTmp, "}" ) && 0 ==  strncmp( sTmp, "}\n", 2 ) ) 
                {
                    iFlag_Dkh--;

                    #if _SWITCH_CASE_
                    if ( iFlag_Tmp == iFlag_Dkh )
                    {
                        iFlag_Switch = 0;
                        iFlag_Tmp = -1;
                    }
                    #endif
                }

                char sTmp1[4096];
                memset( sTmp1, 0x00, sizeof( sTmp1 ) );

                int i = 0;
                for( i = 0; i < iFlag_Dkh; i++ )
                {
                    strcat( sTmp1, "    " );
                }

                #if _SWITCH_CASE_
                if ( iFlag_Switch > 0 && NULL == strstr( sTmp, "case" ))
                {
                    strcat( sTmp1, "    " );
                }
                #endif

                if ( 1 == strlen( sTmp ) && 0 == strncmp( sTmp, "\n", 1 ) )
                {
                    fprintf( fpFile1, "%s", sTmp );
                }
                else
                {
                    fprintf( fpFile1, "%s%s", sTmp1, sTmp );
                }

                if ( NULL != strstr( sTmp, "{" ) && 0 ==  strncmp( sTmp, "{\n", 2 ) ) iFlag_Dkh++;

                #if _SWITCH_CASE_
                if ( NULL != strstr( sTmp, "switch" ) )  
                    {
                        iFlag_Switch++;
                        if ( iFlag_Switch == 1 )
                        iFlag_Tmp = iFlag_Dkh;
                }
                #endif

            }
            else if ( cFlag == _DELETE_LINE_LEFT_RIGHT_KG_ ) 
            {
                char sTmp1[_LINE_MAX_];
                memset( sTmp1, 0x00, sizeof( sTmp1 ) );
                memcpy( sTmp1, sTmp, strlen( sTmp ) );

                //Trim( sTmp1 );

                fprintf( fpFile1, "%s", sTmp1 );
            }
            else if ( _DELETE_ALL_TAB_EMPTY_LINE_    == cFlag ||
                      _DELETE_ALL_KG_EMPTY_LINE_     == cFlag ||
                      _DELETE_ALL_HHFH_EMPTY_LINE_   == cFlag ||
                      _DELETE_ALL_TAB_KG_EMPTY_LINE_ == cFlag 
                    )
            {
                int i = 0;
                int iLength = strlen( sTmp );
                int iFlag = 0;
                for( i = 0; i < iLength; i++ )
                {
                    if ( sTmp[i] == '\n' )
                    {
                        continue;
                    }

                    if ( cFlag == _DELETE_ALL_TAB_KG_EMPTY_LINE_ && ( sTmp[i] != '\t' && sTmp[i] != ' ' && sTmp[i] != ' ' ) )
                    {
                        iFlag = 1;
                        break;
                    }

                    if ( cFlag == _DELETE_ALL_TAB_EMPTY_LINE_ && sTmp[i] != '\t' )
                    {
                        iFlag = 1;
                        break;
                    }

                    if ( cFlag == _DELETE_ALL_KG_EMPTY_LINE_ && sTmp[i] != ' ' )
                    {
                        iFlag = 1;
                        break;
                    }

                    if ( cFlag == _DELETE_ALL_HHFH_EMPTY_LINE_ && sTmp[i] != '\n' )
                    {
                        iFlag = 1;
                        break;
                    }
                }

                if ( i == iLength && 0 == iFlag )
                {
                    //�������κε�fprintf��������ɾ������
                }
                else if ( 1 == iFlag )
                {
                    fprintf( fpFile1, "%s", sTmp );
                }
            }
            else if ( cFlag == _PUT_FILE_ALL_FUN_FROM_SRC_ || cFlag == _PUT_FILE_SHOW_ALL_FUN_FROM_HEADFILE_ )
            {
                memcpy( sBig_Buf + iSize_Kh, sTmp, strlen( sTmp ) );
                iSize_Kh += strlen( sTmp );
            }

            memset( sTmp, 0x00, sizeof( sTmp ) );
        }

        if ( cFlag == _PUT_FILE_ALL_FUN_FROM_SRC_ || cFlag == _PUT_FILE_SHOW_ALL_FUN_FROM_HEADFILE_ )
        {
            char * pTmp  = sBig_Buf;
            char * pTmp1 = NULL;
            char * pTmp2 = NULL;

            while ( 1 )
            {
                pTmp1 = Get_Point_Dkh_Between( pTmp, '{', &pTmp1, &pTmp2 );
                if ( NULL == pTmp1 )
                {
                    break;
                }
                else
                {
                    char * p = NULL;
                    p = (char *)malloc( pTmp1 - pTmp );
                    memcpy( p, pTmp, pTmp1 - pTmp );
                    fprintf( fpFile1, "%s", pTmp );
                    free( p );
                    pTmp = pTmp2;
                }
            }
        }
    }
    
    if ( cFlag == _PUT_FILE_ALL_FUN_FROM_SRC_ || cFlag == _PUT_FILE_SHOW_ALL_FUN_FROM_HEADFILE_ )
    {
        free( sBig_Buf );
    }

    fclose( fpFile  );
    fclose( fpFile1 );
    DEBUG_FRAME( "fclose [%s] succ",  sFileName );
    DEBUG_FRAME( "fclose [%s] succ",  sNewFile );

}






#define _TEST_LIE_FILE_ 0
#if _TEST_LIE_FILE_
int main()
{
	char * sFileName = "1.txt";
	char  * pTmp;


	FILE * fp = fopen( sFileName, "r" );
	int i = 0;
	for( i = 0 ; i < 20 ; i++ )
	{
		int iRet = GetFileLine_Malloc( fp, &pTmp );
		if ( -2 == iRet ) break;
	
		printf( "[iRet=%d],%s\n", iRet, pTmp );
		GetFileLine_Free( pTmp );
		pTmp = NULL;

		if ( -1 == iRet )
		{
			break;
		}
	}

}
#endif






