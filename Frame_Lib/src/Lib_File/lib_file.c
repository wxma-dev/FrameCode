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
* Description:    获取文件大小(字节)                        *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             filename 文件名(绝对路径)                 *
*                 valuename_str 配置字段名称                *
                  value_str 配置字段内容                    *
* Out:            0 非数字                                  *
*                 1 数字                                    *
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    GetFileRowCount                           *
* Description:    获取文件行数                              *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sFilename 文件绝对路径                    *
*                 pNum 文件行数变量指针                     *
* Out:            -1 失败                                   *
*                 其他 文件的行数                           *  
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
	
	/* 打开文件 */
	FILE * fp = fopen( sFilename, "r" );
	
	/* 如果打开文件为NULL，则return -1 */
	if ( NULL == fp )
	{
		return -1;
	}
	
	/* 循环读取文件，统计行数 */
	while( 1 ) 
	{
		/* 读取 */
		pRet = fgets( sString, sizeof( sString ) , fp );
		
		/* 为NULL说明已经到达文件尾部，则跳出循环 */
		if ( NULL == pRet )  
		{
			break;
		}
		
		/* 获取读取到的字符串的长度 */
		iLeng_Str = strlen( sString );
		
		/* 若为换行符，则认为是一行；否则continue */
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
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    While_ReadLine_FromFile                   *
* Description:    循环读取文件的每一行                      *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_StReadLine ST_ReadLine结构体的二级指针*
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
ST_ReadLine * While_ReadLine_FromFile( ST_ReadLine ** pPoint_StReadLine )
{
	char  * pPoint_Tmp;

	/* 0-第一次使用未打开文件 */
	if ( 0 == (*pPoint_StReadLine)->iFlagFirst )
	{
		(*pPoint_StReadLine)->pPoint_fp = fopen( (*pPoint_StReadLine)->sStr_FilePathName, "r" );
		if ( NULL == (*pPoint_StReadLine)->pPoint_fp )
		{
			(*pPoint_StReadLine)->iFlagFirst = -2;
			return NULL;
		}
	}

	/* 接下来读取文件 */
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
	这个函数需要移动到对应的文件的操作函数中去
	功能：读取文件的一行，并将存有文件一行 的 动态内存的指针 返回，注意这个内存使用后记得释放
	出参：   pOut  malloc的地址，存有文件一行的内容
	return  非负整数，行的长度
			0 空行
			-1 已经顺利读到文件末尾  “aEOF", 读取a 并return -1
			-2 到底文件末尾      "a\nEOF" 这种情况分2次读，第一次读a，第二次读取EOF return -2
			-3 出错

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
释放 GetFileLine_Malloc 得到的地址
*/
void GetFileLine_Free( char * pIn )
{
    free( pIn );
}




/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Do_File                                   *
* Description:    文件的相关处理                            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             sInfile    需要处理的文件                 *
*                 iFlag == 'y' 的情况下，处理 SWITCH CASE 缩进的处理 *
*                 其他的参数与bin文件的参数为准             *
* Out:            sOutFile   处理好的文件                   *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
/*
    该函数中的相关宏的定义，具体见头文件中的定义
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
        printf( "输入文件名为[%s]\n", sFileName );

        /* 将开头的tab换成4个空格 */
        Do_File( sFileName, sNewFile , _LINE_HEAD_ALL_TAB_DO_REPLACE_FOUR_KG_ );

        char   sNewFile1[256];
        memset( sNewFile1    , 0x00, sizeof( sNewFile1     ) );

        /* 文件中行首尾去掉空格 */
        Do_File( sNewFile, sNewFile1 , _DELETE_LINE_LEFT_RIGHT_KG_ );

        char   sNewFile2[256];
        memset( sNewFile2    , 0x00, sizeof( sNewFile2     ) );

        /* windows换行符替换成unix换行符号 */
        Do_File( sNewFile1, sNewFile2 , _WINDOWS_HH_DO_REPLACE_UNIX_HH_ );

        char   sNewFile3[256];
        memset( sNewFile3    , 0x00, sizeof( sNewFile3     ) );

        /* 一个文件中4个空格作为对其补齐， */
        Do_File( sNewFile1, sNewFile3 , _NO_HAVA_ANY_SJ_DO_ADD_FOUR_KG_DQBQ_ );

        if ( _TAB_DQBQ_ == cFlag )
        {

            char   sNewFile4[256];
            memset( sNewFile4    , 0x00, sizeof( sNewFile4     ) );

            /* 将4个空格换成tab */
            Do_File( sNewFile2, sNewFile4 , _LINE_HEAD_ALL_FOUR_KG_DO_REPLACE_TAB_ );

            printf( "输出文件名为[%s]\n", sNewFile4 );

            if ( 0 != remove( sNewFile1 ) ) return -1;
            if ( 0 != remove( sNewFile2 ) ) return -1;
            if ( 0 != remove( sNewFile3 ) ) return -1;

            /* 处理好的文件 名字修改成 要输出的文件名 */
            if ( 0 != rename( sNewFile4, sNewFile ) ) return -1;

        }
        else if ( _FOUR_KG_DQBQ_ == cFlag )
        {

            printf( "输出文件名为[%s]\n", sNewFile2 );
            if ( 0 != remove( sNewFile1 ) ) return -1;
            if ( 0 != remove( sNewFile2 ) ) return -1;

            /* 处理好的文件 名字修改成 要输出的文件名 */
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
        DEBUG_FRAME( "因sOutFile为空，默认sOutFile=[%s]", sNewFile);
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

    //unix的多行换成2行换行符
    if ( cFlag == _MULTIPLE_HH_DO_REPLACE_TWO_HH_ )
    {
        int iChar = 0;
        int iChar1 = 0;
        int iChar_Flag = 0;

        while ( EOF != ( iChar = fgetc( fpFile ) ) )
        {
            if ( iChar == '\n' )
            {
                //统计'\n'个数
                iChar_Flag++;
            }
            else
            {
                if ( 1 == iChar_Flag )
                {
                    //一个换行则直接打印
                    fputc( '\n', fpFile1 );
                    fputc( iChar, fpFile1 );
                }
                else if ( iChar_Flag >= 2 )
                {
                    //若多个换行，则以2个换行处理
                    fputc( '\n', fpFile1 );
                    fputc( '\n', fpFile1 );
                    fputc( iChar, fpFile1 );
                }

                else if ( 0 == iChar_Flag )
                {
                    //没有换行字符，直接写入该字符
                    fputc( iChar, fpFile1 );
                }

                //非换行的字符，iChar_Flag置为0
                iChar_Flag = 0;
            }
        }
    }
    //其他的情况
    else 
    {
        int iFlag_Dkh    = 0;            /* 大括号的对数 */
        int iFlag_Switch = 0;            /* SWITCH 标志  */
        int iFlag_Case   = 0;            /* CASE   标志  */
        int iFlag_Tmp    = -1;           /* 临时标志     */
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
                        fprintf( fpFile1, "%s", "    " );        /* 4个空格 */
                    }
                    else if ( ' ' == sTmp[i] )
                    {
                        fprintf( fpFile1, "%s", " " );           /* 1个空格 */
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
                            printf( "日志路径:%s \n",aczLogFile);
                        }
                        break;

                    case 'p':
                        strcpy( sFileName, optarg );
                        printf( "需要处理的文件（可带路径）:%s \n",sFileName);
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
                    //不进行任何的fprintf操作，即删除动作
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






