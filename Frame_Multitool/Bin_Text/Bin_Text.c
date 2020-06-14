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
char    sBuf_Error[4096];                     /* 保存错误信息的Buf */
int     iRet_Process;
char    sTmp[4096];
char    sTmp_format[4096];

#define  _LINE_MAX_ 40960


/************************************************************
* Function Name: main                                       *
* Description:                                              *
* In Params:     int argc                                   *
*                char** argv                                *
*                argv[1]  日期 YYYY/MM/DD                   *
*                argv[2]  日志文件                          *
* Out Params:                                               *
*                                                           *
* Return:        0: 成功                                    *
*               -1: 其他失败                                *
*               -2: 失败，执行System失败                    *
*               -3：输入参数错误                            *
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
    char    sBz[5+1];                    /* 币种 */

    int i;
    char   sString_Bz[100];
    char   sNewFile[256];
    char   sBuf_File[256 + 1];
    int    iCount_Sql;
    int  chflg;
    char aczTranDate[9];
    int    cFlag;                        /* 生成文件的标志 */
    int    iFlag;                        /* 生成文件的标志 */

    /* 全局变量初始化 */
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

    /* 局部变量初始化 */

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
    sprintf( sTmp_format, "%s%s%s", "与", sTmp_format1, "选输一个" );

    /* 使用手册 */
    if ( 6 != argc )
    {
        printf( "----------------------------------------------------------------------------------------------------------------\n" );
        printf( "Usage: %s [-l] logpath [-p] infilename %s [-o]outfilename \n", argv[0], sTmp_format1 ); 
        printf( "l:日志路径，必输 \n" );
        printf( "p:输入文件（可带路径），必输 \n" ); 
        printf( "o:输出文件（可带路径），可输，后跟参数不能有空格 \n" ); 
        printf( "----------------------------------------------------------------------------------------------------------------\n" ); 

        printf( "t:一行中只有tab字符      ，则去掉Tab字符（\\n字符也是默认写到新文件中），%s\n", sTmp_format );
        printf( "s:一行中只有空格字符     ，则去掉空格   （\\n字符也是默认写到新文件中），%s\n", sTmp_format );
        printf( "a:一行中只有空格和TAB字符，则都去掉     （\\n字符也是默认写到新文件中），%s\n", sTmp_format );
        
        printf( "b:一行中开头的所有tab换成4个空格                                   ，%s\n", sTmp_format );
        printf( "c:一行中开头的所有4个空格换成tab                                   ，%s\n", sTmp_format );
        printf( "d:unix的多行换成2行换行符                                          ，%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "u:windows换行符替换成unix换行符号                                  ，%s\n", sTmp_format );
        printf( "w:unix换行符替换成windows换行符号                                  ，%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "e:一个文件中tab作为对其补齐                                        ，%s\n", sTmp_format );
        printf( "f:一个文件中4个空格作为对其补齐                                    ，%s\n", sTmp_format );
        printf( "z:没有任何缩进的文件，以4个空格对其补齐                            ，%s\n", sTmp_format );
        printf( "                                                                     %s\n" );
        printf( "g:去掉文件有内容的一行的左右空格                                   ，%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "h:去掉文件中所有的双斜杆的注释                                     ，%s\n", sTmp_format );
        printf( "i:去掉文件中所有的斜线与双星号的注释                               ，%s\n", sTmp_format );
        printf( "j:去掉文件中所有的斜线与双星号、双斜杆的注释                       ，%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "k:删除文件中所有的tab空行                                          ，%s\n", sTmp_format );
        printf( "m:删除文件中所有的空格空行                                         ，%s\n", sTmp_format );
        printf( "n:删除文件中所有的只有换行符号的空行                               ，%s\n", sTmp_format );
        printf( "q:删除文件中所有的空行（包括tab空行、空格空行、只有换行符号的空行），%s\n", sTmp_format );
        printf( "                                                                   \n" );
        printf( "r:输出源文件中未使用的变量至指定文件                               ，%s\n", sTmp_format );
        printf( "v:删除源文件中未使用的变量                                         ，%s\n", sTmp_format );
        printf( "x:输出源文件中的所有的函数至指定文件                               ，%s\n", sTmp_format );
        printf( "y:输出头文件中的所有的函数至指定文件                               ，%s\n", sTmp_format );
        printf( "----------------------------------------------------------------------------------------------------------------\n" );

        exit(-1);
    }


    /* 校验参数个数 */
    while( (chflg = getopt(argc,argv,"l:p:o::tsabcdefguwefzghijkmnqrvxy")) != -1 )
    switch(chflg)
    {
        case 'l':
            strcpy( aczLogFile, optarg );
            printf( "日志路径:%s \n",aczLogFile);
            break;

        case 'p':
            strcpy( sInFileName, optarg );
            printf( "输入文件（可带路径）:%s \n", sInFileName);
            break;

        case 'o':
            strcpy( sOutFileName, optarg );
            printf( "输出文件（可带路径）:%s \n", sOutFileName);
            break;

        case 't':
            printf( "一行中只有tab字符      ，则去掉Tab字符 \n" );
            cFlag = _IF_LINE_ONLY_TAB_DO_DELETE_ALL_TAB_;
            break;
        case 's':
            printf( "一行中只有空格字符     ，则去掉空格    \n" );
            cFlag = _IF_LINE_ONLY_KG_DO_DELETE_ALL_KG_;
            break;
        case 'a':
            printf( "一行中只有空格和TAB字符，则都去掉     \n" );
            cFlag = _IF_LINE_ONLY_KG_AND_TABL_DO_DELETE_ALL_;
            break;

        case 'b':
            printf( "一行中开头的所有tab换成4个空格\n" );
            cFlag = _LINE_HEAD_ALL_TAB_DO_REPLACE_FOUR_KG_;
            break;
        case 'c':
            printf( "一行中开头的所有4个空格换成tab\n" );
            cFlag = _LINE_HEAD_ALL_FOUR_KG_DO_REPLACE_TAB_;
            break;
        case 'd':
            printf( "unix的多行换成2行换行符\n" );
            cFlag = _MULTIPLE_HH_DO_REPLACE_TWO_HH_;
            break;

        case 'u':
            printf( "windows换行符替换成unix换行符号\n" );
            cFlag = _WINDOWS_HH_DO_REPLACE_UNIX_HH_;
            break;
        case 'w':
            printf( "unix换行符替换成windows换行符号\n" );
            cFlag = _UNIX_HH_DO_REPLACE_WINDOWS_HH_;
            break;

        case 'e':
            printf( "一个文件中tab作为对其补齐\n" );
            cFlag = _TAB_DQBQ_;
            break;
        case 'f':
            printf( "一个文件中4个空格作为对其补齐\n" );
            cFlag = _FOUR_KG_DQBQ_;
            break;
        case 'z':
            printf( "没有任何缩进的文件，以4个空格对其补齐\n" );
            cFlag = _NO_HAVA_ANY_SJ_DO_ADD_FOUR_KG_DQBQ_;
            break;

        case 'g':
            printf( "去掉文件有内容的一行的左右空格\n" );
            cFlag = _DELETE_LINE_LEFT_RIGHT_KG_;
            break;

        case 'h':
            printf( "去掉文件中所有的双斜杆的注释\n" );
            cFlag = _DELETE_ALL_SXG_ZS_;
            break;
        case 'i':
            printf( "去掉文件中所有的斜线与双星号的注释\n" );
            cFlag = _DELETE_ALL_SX_XH_ZS_;
            break;
        case 'j':
            printf( "去掉文件中所有的斜线与双星号、双斜杆的注释\n" );
            cFlag = _DELETE_ALL_SX_XH_SXG_ZS_;
            break;


        case 'k':
            printf( "删除文件中所有的tab空行\n" );
            cFlag = _DELETE_ALL_TAB_EMPTY_LINE_;
            break;
        case 'm':
            printf( "删除文件中所有的空格空行\n" );
            cFlag = _DELETE_ALL_KG_EMPTY_LINE_;
            break;
        case 'n':
            printf( "去掉文件中所有的斜线与双星号、双斜杆的注释\n" );
            cFlag = _DELETE_ALL_HHFH_EMPTY_LINE_;
            break;
        case 'q':
            printf( "删除文件中所有的空行（包括tab空行、空格空行、只有换行符号的空行）\n" );
            cFlag = _DELETE_ALL_TAB_KG_EMPTY_LINE_;
            break;

        case 'r':
            printf( "输出源文件中未使用的变量至指定文件\n" );
            cFlag = _PUT_FILE_MORE_NOT_USE_VARIABLE_;
            break;
        case 'v':
            printf( "删除源文件中未使用的变量\n" );
            cFlag = _DELETE_MORE_NOT_USE_VARIABLE_;
            break;
        case 'x':
            printf( "输出源文件中的所有的函数至指定文件\n" );
            cFlag = _PUT_FILE_ALL_FUN_FROM_SRC_;
            break;
        case 'y':
            printf( "输出头文件中的所有的函数至指定文件\n" );
            cFlag = _PUT_FILE_SHOW_ALL_FUN_FROM_HEADFILE_;
            break;

    }

    if ( 0 == cFlag )
    {
        printf( "%s\n", sTmp_format );
        exit( -1 );
    }

    /* 日志初始化 */
    DEBUG_OPEN_FRAME( aczLogFile );

    DEBUG_FRAME( "参数校验通过" );

    DEBUG_FRAME( "------处理文件开始------" );

    Do_File( sInFileName, sOutFileName , cFlag );

    printf( "输入文件名为[%s]\n", sInFileName );
    printf( "输出文件名为[%s]\n", sOutFileName );


    DEBUG_FRAME( "------文件处理结束------" );

    DEBUG_CLOSE_FRAME();

    return 0;
}




/*
** end of file
*/