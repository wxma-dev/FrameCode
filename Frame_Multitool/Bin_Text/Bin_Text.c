/*
*    Copyright (c) 2000-2005 GIT Ltd.
*    All rights reserved
*
*    This is unpublished proprietary source code of GIT Ltd.
*    The copyright notice above does not evidence any actual
*    or intended publication of such source code.
*
*    NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
*    SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
*/

/*
*    ProgramName : commerce.ec
*    SystemName  : EasyLink
*    Version     : 1.0
*    Description : used by rz_06.sh
*    History     :
*    YYYY/MM/DD        Position        Author         Description
*    -------------------------------------------------------------
*    2015/11/06        JiaXing         zhr            create
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>

#include "Log_Frame.h"

#include "../Frame_Lib/include/lib_string.h"

char      sDate_Sql[8+1];

int     iRet = 0;
char    aczLogFile[4096];

char    Err_Message[4096];
char    aczShellCmd[4096];
char    sBuf_Error[4096];                     /* ���������Ϣ��Buf */
int     iRet_Process;
char    sTmp[4096];
char    sTmp_format[4096];

#define  _LINE_MAX_ 40960


/************************************************************
* Function Name: main                                       *
* Description:                                              *
* In Params:     int argc                                   *
*                char** argv                                *
*                argv[1]  ���� YYYY/MM/DD                   *
*                argv[2]  ��־�ļ�                          *
* Out Params:                                               *
*                                                           *
* Return:        0: �ɹ�                                    *
*               -1: ����ʧ��                                *
*               -2: ʧ�ܣ�ִ��Systemʧ��                    *
*               -3�������������                            *
*                                                           *
* explain:                                                  *
************************************************************/
int main(int argc,char** argv)
{

    char          sBuf_Bz[10];

    char    sInFileName[4096];
    char    sOutFileName[4096];
    char    str8Date[8+1];
    FILE    *fpFile;
    FILE    *fpFile1;
    char    sBz[5+1];                    /* ���� */

    int i;
    char   sString_Bz[100];
    char   sNewFile[256];
    char   sBuf_File[256 + 1];
    int    iCount_Sql;
    int  chflg;
    char aczTranDate[9];
    int    cFlag;                        /* �����ļ��ı�־ */
    int    iFlag;                        /* �����ļ��ı�־ */

    /* ȫ�ֱ�����ʼ�� */
    iRet = 0;
    memset( sDate_Sql   , 0x00, sizeof( sDate_Sql    ) );
    memset( aczLogFile  , 0x00, sizeof( aczLogFile   ) );
    memset( sInFileName   , 0x00, sizeof( sInFileName    ) );
    memset( sOutFileName   , 0x00, sizeof( sOutFileName    ) );
    memset( Err_Message , 0x00, sizeof( Err_Message  ) );
    memset( aczShellCmd , 0x00, sizeof( aczShellCmd  ) );
    memset( sBuf_Error  , 0x00, sizeof( sBuf_Error   ) );
    iRet_Process = 0;
    memset( sTmp        , 0x00, sizeof( sTmp         ) );
    memset( sTmp_format , 0x00, sizeof( sTmp_format  ) );

    /* �ֲ�������ʼ�� */

    memset( sBuf_Bz     , 0x00, sizeof( sBuf_Bz      ) );
    memset( str8Date    , 0x00, sizeof( str8Date     ) );
    fpFile = NULL;                                   
    fpFile1 = NULL;                                   
    memset( sBz         , 0x00, sizeof( sBz          ) );

    i = 0;
    memset( sString_Bz  , 0x00, sizeof( sString_Bz   ) );
    memset( sNewFile    , 0x00, sizeof( sNewFile     ) );
    memset( sBuf_File   , 0x00, sizeof( sBuf_File    ) );
    iCount_Sql = 0;
    chflg = 0;
    memset( aczTranDate , 0x00, sizeof( aczTranDate  ) );
    cFlag = 0;
    iFlag = 0;

    char sTmp_format1[ 300+1 ];
    char sTmp_format [ 300+1 ];
    memset( sTmp_format1, 0x00, sizeof( sTmp_format1 ) );
    memset( sTmp_format , 0x00, sizeof( sTmp_format  ) );
    sprintf( sTmp_format1, "%s", "[-t|-s|-a|-b|-c|-d|-e|-f|-g|-u|-w][-e][-f][-z][-g][-h][-i][-j][-k][-m][-n][-q][-r][-v][-x][-y]" );
    sprintf( sTmp_format, "%s%s%s", "��", sTmp_format1, "ѡ��һ��" );

    /* ʹ���ֲ� */
    if ( 6 != argc )
    {
        printf( "----------------------------------------------------------------------------------------------------------------\n" );
        printf( "Usage: %s [-l] logpath [-p] infilename %s [-o]outfilename \n", argv[0], sTmp_format1 ); 
        printf( "l:��־·�������� \n" );
        printf( "p:�����ļ����ɴ�·���������� \n" ); 
        printf( "o:����ļ����ɴ�·���������䣬������������пո� \n" ); 
        printf( "----------------------------------------------------------------------------------------------------------------\n" ); 

        printf( "t:һ����ֻ��tab�ַ�      ����ȥ��Tab�ַ���\\n�ַ�Ҳ��Ĭ��д�����ļ��У���%s\n", sTmp_format );
        printf( "s:һ����ֻ�пո��ַ�     ����ȥ���ո�   ��\\n�ַ�Ҳ��Ĭ��д�����ļ��У���%s\n", sTmp_format );
        printf( "a:һ����ֻ�пո��TAB�ַ�����ȥ��     ��\\n�ַ�Ҳ��Ĭ��д�����ļ��У���%s\n", sTmp_format );
        
        printf( "b:һ���п�ͷ������tab����4���ո�                                   ��%s\n", sTmp_format );
        printf( "c:һ���п�ͷ������4���ո񻻳�tab                                   ��%s\n", sTmp_format );
        printf( "d:unix�Ķ��л���2�л��з�                                          ��%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "u:windows���з��滻��unix���з���                                  ��%s\n", sTmp_format );
        printf( "w:unix���з��滻��windows���з���                                  ��%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "e:һ���ļ���tab��Ϊ���䲹��                                        ��%s\n", sTmp_format );
        printf( "f:һ���ļ���4���ո���Ϊ���䲹��                                    ��%s\n", sTmp_format );
        printf( "z:û���κ��������ļ�����4���ո���䲹��                            ��%s\n", sTmp_format );
        printf( "                                                                     %s\n" );
        printf( "g:ȥ���ļ������ݵ�һ�е����ҿո�                                   ��%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "h:ȥ���ļ������е�˫б�˵�ע��                                     ��%s\n", sTmp_format );
        printf( "i:ȥ���ļ������е�б����˫�Ǻŵ�ע��                               ��%s\n", sTmp_format );
        printf( "j:ȥ���ļ������е�б����˫�Ǻš�˫б�˵�ע��                       ��%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "k:ɾ���ļ������е�tab����                                          ��%s\n", sTmp_format );
        printf( "m:ɾ���ļ������еĿո����                                         ��%s\n", sTmp_format );
        printf( "n:ɾ���ļ������е�ֻ�л��з��ŵĿ���                               ��%s\n", sTmp_format );
        printf( "q:ɾ���ļ������еĿ��У�����tab���С��ո���С�ֻ�л��з��ŵĿ��У���%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "r:���Դ�ļ���δʹ�õı�����ָ���ļ�                               ��%s\n", sTmp_format );
        printf( "v:ɾ��Դ�ļ���δʹ�õı���                                         ��%s\n", sTmp_format );
        printf( "x:���Դ�ļ��е����еĺ�����ָ���ļ�                               ��%s\n", sTmp_format );
        printf( "y:���ͷ�ļ��е����еĺ�����ָ���ļ�                               ��%s\n", sTmp_format );
        printf( "----------------------------------------------------------------------------------------------------------------\n" );

        exit(-1);
    }


    /* У��������� */
    while( (chflg = getopt(argc,argv,"l:p:o::tsabcdefguwefzghijkmnqrvxy")) != -1 )
    switch(chflg)
    {
        case 'l':
            strcpy( aczLogFile, optarg );
            printf( "��־·��:%s \n",aczLogFile);
            break;

        case 'p':
            strcpy( sInFileName, optarg );
            printf( "�����ļ����ɴ�·����:%s \n", sInFileName);
            break;

        case 'o':
            strcpy( sOutFileName, optarg );
            printf( "����ļ����ɴ�·����:%s \n", sOutFileName);
            break;

        case 't':
            printf( "һ����ֻ��tab�ַ�      ����ȥ��Tab�ַ� \n" );
            cFlag = _IF_LINE_ONLY_TAB_DO_DELETE_ALL_TAB_;
            break;
        case 's':
            printf( "һ����ֻ�пո��ַ�     ����ȥ���ո�    \n" );
            cFlag = _IF_LINE_ONLY_KG_DO_DELETE_ALL_KG_;
            break;
        case 'a':
            printf( "һ����ֻ�пո��TAB�ַ�����ȥ��     \n" );
            cFlag = _IF_LINE_ONLY_KG_AND_TABL_DO_DELETE_ALL_;
            break;

        case 'b':
            printf( "һ���п�ͷ������tab����4���ո�\n" );
            cFlag = _LINE_HEAD_ALL_TAB_DO_REPLACE_FOUR_KG_;
            break;
        case 'c':
            printf( "һ���п�ͷ������4���ո񻻳�tab\n" );
            cFlag = _LINE_HEAD_ALL_FOUR_KG_DO_REPLACE_TAB_;
            break;
        case 'd':
            printf( "unix�Ķ��л���2�л��з�\n" );
            cFlag = _MULTIPLE_HH_DO_REPLACE_TWO_HH_;
            break;

        case 'u':
            printf( "windows���з��滻��unix���з���\n" );
            cFlag = _WINDOWS_HH_DO_REPLACE_UNIX_HH_;
            break;
        case 'w':
            printf( "unix���з��滻��windows���з���\n" );
            cFlag = _UNIX_HH_DO_REPLACE_WINDOWS_HH_;
            break;

        case 'e':
            printf( "һ���ļ���tab��Ϊ���䲹��\n" );
            cFlag = _TAB_DQBQ_;
            break;
        case 'f':
            printf( "һ���ļ���4���ո���Ϊ���䲹��\n" );
            cFlag = _FOUR_KG_DQBQ_;
            break;
        case 'z':
            printf( "û���κ��������ļ�����4���ո���䲹��\n" );
            cFlag = _NO_HAVA_ANY_SJ_DO_ADD_FOUR_KG_DQBQ_;
            break;

        case 'g':
            printf( "ȥ���ļ������ݵ�һ�е����ҿո�\n" );
            cFlag = _DELETE_LINE_LEFT_RIGHT_KG_;
            break;

        case 'h':
            printf( "ȥ���ļ������е�˫б�˵�ע��\n" );
            cFlag = _DELETE_ALL_SXG_ZS_;
            break;
        case 'i':
            printf( "ȥ���ļ������е�б����˫�Ǻŵ�ע��\n" );
            cFlag = _DELETE_ALL_SX_XH_ZS_;
            break;
        case 'j':
            printf( "ȥ���ļ������е�б����˫�Ǻš�˫б�˵�ע��\n" );
            cFlag = _DELETE_ALL_SX_XH_SXG_ZS_;
            break;


        case 'k':
            printf( "ɾ���ļ������е�tab����\n" );
            cFlag = _DELETE_ALL_TAB_EMPTY_LINE_;
            break;
        case 'm':
            printf( "ɾ���ļ������еĿո����\n" );
            cFlag = _DELETE_ALL_KG_EMPTY_LINE_;
            break;
        case 'n':
            printf( "ȥ���ļ������е�б����˫�Ǻš�˫б�˵�ע��\n" );
            cFlag = _DELETE_ALL_HHFH_EMPTY_LINE_;
            break;
        case 'q':
            printf( "ɾ���ļ������еĿ��У�����tab���С��ո���С�ֻ�л��з��ŵĿ��У�\n" );
            cFlag = _DELETE_ALL_TAB_KG_EMPTY_LINE_;
            break;

        case 'r':
            printf( "���Դ�ļ���δʹ�õı�����ָ���ļ�\n" );
            cFlag = _PUT_FILE_MORE_NOT_USE_VARIABLE_;
            break;
        case 'v':
            printf( "ɾ��Դ�ļ���δʹ�õı���\n" );
            cFlag = _DELETE_MORE_NOT_USE_VARIABLE_;
            break;
        case 'x':
            printf( "���Դ�ļ��е����еĺ�����ָ���ļ�\n" );
            cFlag = _PUT_FILE_ALL_FUN_FROM_SRC_;
            break;
        case 'y':
            printf( "���ͷ�ļ��е����еĺ�����ָ���ļ�\n" );
            cFlag = _PUT_FILE_SHOW_ALL_FUN_FROM_HEADFILE_;
            break;

    }

    if ( 0 == cFlag )
    {
        printf( "%s\n", sTmp_format );
        exit( -1 );
    }

    /* ��־��ʼ�� */
    DEBUG_OPEN_FRAME( aczLogFile );

    DEBUG_FRAME( "����У��ͨ��" );

    DEBUG_FRAME( "------�����ļ���ʼ------" );

    Do_File( sInFileName, sOutFileName , cFlag );

    printf( "�����ļ���Ϊ[%s]\n", sInFileName );
    printf( "����ļ���Ϊ[%s]\n", sOutFileName );


    DEBUG_FRAME( "------�ļ��������------" );

    DEBUG_CLOSE_FRAME();

    return 0;
}




/*
** end of file
*/