#include <iostream>
#include <ctime>
#include <cstring>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iconv.h>
#include <unistd.h>

#include "lib_string.h"

#include "Log_Frame.h"

using namespace std;

#define _NEWSPAPER_DATA_LENGTH_  409600


class CodeConverter {
    private:
        iconv_t cd;

    public:

        CodeConverter(const char *from_charset,const char *to_charset) {
            cd = iconv_open(to_charset,from_charset);
        }

        ~CodeConverter() {
            cout << "delete iconv" << endl;
            iconv_close(this->cd);
        }

        int convert(char *inbuf,size_t inlen,char *outbuf,size_t outlen) {
            char **pin = &inbuf;
            char **pout = &outbuf;

            memset(outbuf, 0x00, outlen );
            return iconv(cd, pin, (size_t *)&inlen, pout, (size_t *)&outlen);
        }
};


/**
 *  @brief ��ֽ�ĸ���
 *  ��ֽ�Ļ���
 */
class CNewspaper
{
    public:
        string sAddress_Internet;   /* ������ַ */
        int    iYear_Newspaper;     /* ��ֽ��� */
        int    iMon_Newspaper;      /* ��ֽ�·� */
        int    iDate_Newspaper;     /* ��ֽ�·��е��� */
        char sDirName_LocaleComputer[4096];/* ��ֽ�������Ŀ¼�� */
        char sPaper_Name_CNewspaper[4096];     /* ��ֽ���� */

    public:
        CNewspaper();
        CNewspaper( char * sPaper_NM );
        CNewspaper( int iYear, int iMon, int iDate, char * sPaper_NM );
        CNewspaper( int iYear, int iMon, int iDate );

        void setCNewspaperInfo( int iYear, int iMon, int iDate, char * sPaper_NM );
};

CNewspaper::CNewspaper()
{
    /* ��ȡ���յ����� */

    // ���ڵ�ǰϵͳ�ĵ�ǰ����/ʱ��
    time_t now = time(0);
    tm *ltm = localtime(&now);
    // ��� tm �ṹ�ĸ�����ɲ���
    /*
    cout << "��: "<< 1900 + ltm->tm_year << endl;
    cout << "��: "<< 1 + ltm->tm_mon<< endl;
    cout << "��: "<<  ltm->tm_mday << endl;
    */
    iYear_Newspaper   = 1900 + ltm->tm_year;
    iMon_Newspaper    = 1 + ltm->tm_mon;
    iDate_Newspaper   = ltm->tm_mday;

    /*
        this->CNewspaper::CNewspaper(
            1900 + ltm->tm_year,
            1 + ltm->tm_mon,
            ltm->tm_mday );
    */
    
    memset( sPaper_Name_CNewspaper, 0x00, sizeof( sPaper_Name_CNewspaper ) );

    memset( sDirName_LocaleComputer, 0x00, sizeof( sDirName_LocaleComputer ) );
    sprintf( sDirName_LocaleComputer, "%4.4d%-02.2d%-02.2d", iYear_Newspaper, iMon_Newspaper, iDate_Newspaper );
}

CNewspaper::CNewspaper( char * sPaper_NM )
{
    /* ��ȡ���յ����� */

    // ���ڵ�ǰϵͳ�ĵ�ǰ����/ʱ��
    time_t now = time(0);
    tm *ltm = localtime(&now);
    // ��� tm �ṹ�ĸ�����ɲ���
    /*
    cout << "��: "<< 1900 + ltm->tm_year << endl;
    cout << "��: "<< 1 + ltm->tm_mon<< endl;
    cout << "��: "<<  ltm->tm_mday << endl;
    */

    iYear_Newspaper   = 1900 + ltm->tm_year;
    iMon_Newspaper    = 1 + ltm->tm_mon;
    iDate_Newspaper   = ltm->tm_mday;
    /*
        this->CNewspaper::CNewspaper(
            1900 + ltm->tm_year,
            1 + ltm->tm_mon,
            ltm->tm_mday );
    */
    memset( sPaper_Name_CNewspaper, 0x00, sizeof( sPaper_Name_CNewspaper ) );
    strcpy( sPaper_Name_CNewspaper, sPaper_NM );

    memset( sDirName_LocaleComputer, 0x00, sizeof( sDirName_LocaleComputer ) );
    sprintf( sDirName_LocaleComputer, "%4.4d%-02.2d%-02.2d%s", iYear_Newspaper, iMon_Newspaper, iDate_Newspaper, sPaper_NM );
    
}

CNewspaper::CNewspaper( int iYear, int iMon, int iDate, char * sPaper_NM )
{
    iYear_Newspaper   = iYear ;
    iMon_Newspaper    = iMon ;
    iDate_Newspaper   = iDate;

    memset( sPaper_Name_CNewspaper, 0x00, sizeof( sPaper_Name_CNewspaper ) );
    strcpy( sPaper_Name_CNewspaper, sPaper_NM );

    memset( sDirName_LocaleComputer, 0x00, sizeof( sDirName_LocaleComputer ) );
    sprintf( sDirName_LocaleComputer, "%4.4d%-02.2d%-02.2d%s", iYear_Newspaper, iMon_Newspaper, iDate_Newspaper, sPaper_NM );
}

void CNewspaper::setCNewspaperInfo( int iYear, int iMon, int iDate, char * sPaper_NM )
{
    iYear_Newspaper   = iYear ;
    iMon_Newspaper    = iMon ;
    iDate_Newspaper   = iDate;

    memset( sPaper_Name_CNewspaper, 0x00, sizeof( sPaper_Name_CNewspaper ) );
    strcpy( sPaper_Name_CNewspaper, sPaper_NM );

    memset( sDirName_LocaleComputer, 0x00, sizeof( sDirName_LocaleComputer ) );
    sprintf( sDirName_LocaleComputer, "%4.4d%-02.2d%-02.2d%s", iYear_Newspaper, iMon_Newspaper, iDate_Newspaper, sPaper_NM );
}

#define  _BUF_SIZE_100    100
#define  _BUF_SIZE_200    200
#define  _BUF_SIZE_500    500
class CReadNewspaper
{
    public:
        char   sNextRead_Address[4096];     /* ��һ��׼���Ķ��ĵ�ַ */
        char   sNextRead_File[4096];        /* ��һ��׼���Ķ����������ݷ��ڸ��ļ��� */
        int    iXh_Bm_Newspaper = 1 ;       /* �Ķ��� ĳ�汾�ĵڼ������� */
        int    iBm_Newspaper    = 1 ;       /* �Ķ��� ��ֽ�ĸ����� */
        int    iFlag_NotFound   = 0;        /* ��ҳ�Ƿ������ݵı�־��0-�����ݣ�1-������ */
        string sTile_Newspaper;             /* �Ķ����� ���ű��� */
        string sData_Newspaper;             /* �Ķ����� ���ŵ����� */
        int    iFlag_Data = -1;             /* ���ű����Ƿ��б�־ -1-û�У�1-�� */
        int    iFlag_Title = -1;            /* ���ű����Ƿ��б�־ -1-û�У�1-�� */
        char   sBmAll_Buf[_BUF_SIZE_100][_BUF_SIZE_500];        /* �ڼ���������ڴ������� */
        char   sBmAll_Buf_Tmp[_BUF_SIZE_100][_BUF_SIZE_500];    /* �ڼ���������ڴ�������-��ʱ */
        int    iBmAll_Count;                /* ������ */
        char   sAddressAll_Newspaper[_BUF_SIZE_200][_BUF_SIZE_500];/* ���е����ŵ�ַ�����ڴ������� */
        char   sAddressAll_Newspaper_Tmp[_BUF_SIZE_200][_BUF_SIZE_500];/* ���е����ŵ�ַ�����ڴ�������-��ʱ */
        int    iAddressAll_Count;           /* ���ŵ�ַ�ĸ��� */

        /* ���������еĹ���������Ϣ */
        char sGl_Buf_Data[_BUF_SIZE_100][_BUF_SIZE_500];
        int  iCount_Gl_Buf_Data;

        /* ���������е�ɾ����Ҫ�ַ��� */
        char sGl_Buf_Data_Del[_BUF_SIZE_100][_BUF_SIZE_500];
        int  iCount_Gl_Buf_Data_Del;

        /* ��������Ĺ���������Ϣ */
        char sGl_Buf_Title[_BUF_SIZE_100][_BUF_SIZE_500];
        int  iCount_Gl_Buf_Title;

        /* ���������ɾ����Ҫ�ַ��� */
        char sGl_Buf_Title_Del[_BUF_SIZE_100][_BUF_SIZE_500];
        int  iCount_Gl_Buf_Title_Del;

    public:
        CReadNewspaper();

        /*
            @brief ��ȡĳһ����ַ��������ݣ������ļ���
            @in:    sAddress  �汾��ַ 
                    sFileName �汾���ݷ����ĸ��ļ��ļ�
            @return -1 ʧ��
                    0  �ɹ�
        */
        int Get_Something_NewspaperAdreess_ToFile( char * sAddress, char * sFileName );

#if 0
        int Get_Something_NewspaperAdreess_ToFile( char * sAddress )
        {
            return Get_Something_NewspaperAdreess_ToFile( sAddress, this->sNextRead_File );
        }

        int Get_Something_NewspaperAdreess_ToFile( char * sFileName )
        {
            return Get_Something_NewspaperAdreess_ToFile( this->sNextRead_Address, sFileName );
        }
#endif

        int Get_Something_NewspaperAdreess_ToFile( );


        /*
            @brief ����ܷ����б���������
            @in:    sData    Data����
                    
            @return 1 �б���������
                    0 �ޱ���������
        */
        int Check_ThereIsPaperData( string sData );


        /*
            @brief ����ܷ����б����ı���
            @in:    sData    Data����
                    
            @return 1 �б����ı���
                    0 �ޱ����ı���
        */
        int Check_ThereIsPaperTitle( string sData );

        /*
            @brief �������ű���
            @in:    sData    Data����
                    
            @return 1 �б����ı���
                    0 �ޱ����ı���
        */
        void Filter_Tile_Newspaper();

        /*
            @brief ������������
            @in:    sData    Data����
                    
            @return 1 �б����ı���
                    0 �ޱ����ı���
        */
        void Filter_Data_Newspaper();

        /*
            @brief �滻�ַ���
            @in:    sData    Ҫ�滻���ַ���
                    str_replace  
                    dst_replace  
                    
            @return 1 �滻�ַ�����ֵ
                    0 �ޱ����ı���
        */
        void Replace_Str_FromStr( string& sData, char * src_replace, char * dst_replace );
        
        /*
            @brief �滻�ַ���
            @in:    sData    Ҫ�滻���ַ���
                    str_replace  
                    dst_replace  
                    
            @out:   this->sAddressAll_Newspaper_Tmp  ��ַ���ڸ�������
            @return >0 ���صĸ���
                    0  û���滻��
                    <0 ʧ��
        */
        int GetStr_FromFile_between( char * sFile, char * start_str, char * end_str, char out_Buf[][_BUF_SIZE_500] );
        
        
        
        virtual string Get_NextReading_NewspaperAdreess() = 0;
        virtual string Get_NextReading_File() = 0;
        //virtual string Get_NextReading_File( char * sURL ) = 0;
        virtual int ReadNewsPaper_Date( char * sFileName ) = 0;
        virtual int ReadNewsPaper_Date() = 0;
        virtual void ReadNewsPaper() = 0;
        
        
};

CReadNewspaper::CReadNewspaper()
{
    memset( sNextRead_Address, 0x00, sizeof( sNextRead_Address ) );
    memset( sNextRead_File   , 0x00, sizeof( sNextRead_File    ) );
    iXh_Bm_Newspaper = 1 ;
    iBm_Newspaper    = 1 ;
    iFlag_NotFound   = 0 ;

    memset( sGl_Buf_Data     , 0x00, sizeof( sGl_Buf_Data ) );
    iCount_Gl_Buf_Data = 0;

    memset( sGl_Buf_Data_Del , 0x00, sizeof( sGl_Buf_Data_Del ) );
    iCount_Gl_Buf_Data_Del = 0;

    memset( sGl_Buf_Title    , 0x00, sizeof( sGl_Buf_Title ) );
    iCount_Gl_Buf_Title = 0;

    memset( sGl_Buf_Title_Del, 0x00, sizeof( sGl_Buf_Title_Del ) );
    iCount_Gl_Buf_Title_Del = 0;
    
    memset( sBmAll_Buf, 0x00, sizeof( sBmAll_Buf ) );
    memset( sBmAll_Buf_Tmp, 0x00, sizeof( sBmAll_Buf_Tmp ) );
    iBmAll_Count = 0;
    
    memset( sAddressAll_Newspaper, 0x00, sizeof( sAddressAll_Newspaper ) );
    memset( sAddressAll_Newspaper_Tmp, 0x00, sizeof( sAddressAll_Newspaper_Tmp ) );
    iAddressAll_Count = 0;
}

int CReadNewspaper::Get_Something_NewspaperAdreess_ToFile( char * sAddress, char * sFileName )
{
    char  aczShellCmd[4096];
    memset( aczShellCmd, 0x00, sizeof(aczShellCmd));

    if ( NULL == sAddress || NULL == sFileName )
    {
        DEBUG_FRAME( "NULL == sAddress || NULL == sFileName" );
        return  -1;
    }

    /* unix���� */
    memset( aczShellCmd, 0x00, sizeof(aczShellCmd) );
    sprintf( aczShellCmd , "wget %s -O %s", sAddress, sFileName );

    cout << aczShellCmd << endl;
    DEBUG_FRAME( "aczShellCmd=[%s]", aczShellCmd );

    int iRet = system( aczShellCmd );
    if( iRet >> 8 != 0 )
    {
        printf( "system error, aczShellCmd=[%s]\n", aczShellCmd );
        ERROR_FRAME( "system error, aczShellCmd=[%s].", aczShellCmd );
        return -1;
    }
    else
    {
        ERROR_FRAME( "system succ, aczShellCmd=[%s].", aczShellCmd );
        return 0;
    }
}


int CReadNewspaper::Get_Something_NewspaperAdreess_ToFile( )
{
    return Get_Something_NewspaperAdreess_ToFile( this->sNextRead_Address, this->sNextRead_File );
}


int CReadNewspaper::Check_ThereIsPaperData( string sData )
{
    int iPos = 0;
    for( int i = 0; i < iCount_Gl_Buf_Data; i++ )
    {
        iPos = sData.find( &(sGl_Buf_Data[i][0]) ); /* ƥ�䵽��ҳ������ */
        if ( iPos != std::string::npos )
        {
            /* ƥ�䵽�б��������� */
            DEBUG_FRAME( "ƥ�䵽��ҳ������" );
            return 1;
        }
    }
    return 0;
}

int CReadNewspaper::Check_ThereIsPaperTitle( string sData )
{
    int iPos = 0;
    for( int i = 0; i < iCount_Gl_Buf_Title; i++ )
    {
        iPos = sData.find( &(sGl_Buf_Title[i][0]) ); /* ƥ�䵽����ͷ */
        if ( iPos != std::string::npos )
        {
            iPos = sData.find( "Not Found" ); /* ƥ�䵽"404 Not Found",����Ϊ��ҳ��Ч */
            if ( iPos != std::string::npos )
            {
                DEBUG_FRAME( "ƥ�䵽\"404 Not Found\",����Ϊ��ҳ��Ч" );
                return 0;
            }
            else  /* ƥ�䵽�����ı��� */
            {
                DEBUG_FRAME( "ƥ�䵽�����ı���" );
                return 1;
            }
        }
    }
    return 0;
}


void CReadNewspaper::Filter_Tile_Newspaper()
{
    if ( "" == this->sTile_Newspaper )
    {
        return ;
    }

    int iPos_Tmp = 0;
    for( int i = 0; i < iCount_Gl_Buf_Title_Del; i++ )
    {
        iPos_Tmp = (this->sTile_Newspaper).find( &(sGl_Buf_Title_Del[i][0]) ); 
        if ( iPos_Tmp != std::string::npos )
        {
            this->sTile_Newspaper = (this->sTile_Newspaper).replace( (this->sTile_Newspaper).find( &(sGl_Buf_Title_Del[i][0]) ), strlen( &(sGl_Buf_Title_Del[i][0]) ), "" );
        }
        else
        {
            cout << "���ű������쳣:[" << this->sTile_Newspaper << "]" << endl;
            ERROR_FRAME( "���ű������쳣" );
        }
    }
}


void CReadNewspaper::Filter_Data_Newspaper()
{
    char sGl_Buf3 [ 4096 ];
    char sGl_Buf4 [ 4096 ];
    char sGl_Buf5 [ 4096 ];

    memset( sGl_Buf3 , 0x00, sizeof( sGl_Buf3 ) );
    memset( sGl_Buf4 , 0x00, sizeof( sGl_Buf4 ) );
    memset( sGl_Buf5 , 0x00, sizeof( sGl_Buf5 ) );

    if ( "" == this->sData_Newspaper )
    {
        return ;
    }

    strcpy( sGl_Buf3 , "</p><p>" );
    strcpy( sGl_Buf4 , "</P><P>" );
    strcpy( sGl_Buf5 , "&nbsp;" );

    int iPos_Tmp = 0;

    for( int i = 0; i < iCount_Gl_Buf_Data_Del; i++ )
    {
        iPos_Tmp =  (this->sData_Newspaper).find( &(sGl_Buf_Data_Del[i][0]) );
        if ( iPos_Tmp != std::string::npos )
        {
            this->sData_Newspaper = (this->sData_Newspaper).replace( (this->sData_Newspaper).find( &(sGl_Buf_Data_Del[i][0]) ), strlen( &(sGl_Buf_Data_Del[i][0]) ), "" );
        }
    }

    while( 1 )
    {
        iPos_Tmp = (this->sData_Newspaper).find( sGl_Buf3 ); 
        if ( iPos_Tmp != std::string::npos )
        {
            this->sData_Newspaper = (this->sData_Newspaper).replace( (this->sData_Newspaper).find( sGl_Buf3 ), strlen( sGl_Buf3 ), "\n" );
        }
        else
        {
            iPos_Tmp = (this->sData_Newspaper).find( sGl_Buf4 ); 
            if ( iPos_Tmp != std::string::npos )
            {
                this->sData_Newspaper = (this->sData_Newspaper).replace( (this->sData_Newspaper).find( sGl_Buf4 ), strlen( sGl_Buf4 ), "\n" );
            }
            else
            {
                iPos_Tmp = (this->sData_Newspaper).find( sGl_Buf5 ); 
                if ( iPos_Tmp != std::string::npos )
                {
                    this->sData_Newspaper = (this->sData_Newspaper).replace( (this->sData_Newspaper).find( sGl_Buf5 ), strlen( sGl_Buf5 ), " " );
                }
                else
                {
                    break;
                }
            }
        }
    }
}


void CReadNewspaper::Replace_Str_FromStr( string& sData, char * src_replace, char * dst_replace )
{
    int iPos_Tmp = -1;

    while ( 1 )
    {
        iPos_Tmp = sData.find( src_replace );
        if ( iPos_Tmp != std::string::npos )
        {
            sData = sData.replace( sData.find( src_replace ), strlen( src_replace ), dst_replace );
        }
        else
        {
            break;
        }
    }
}


int CReadNewspaper::GetStr_FromFile_between( char * sFile, char * start_str, char * end_str, char out_Buf[][_BUF_SIZE_500] )
{
    ifstream infile;
    int  iPos_Tmp  = -1;
    int  iPos_Tmp_end = -1;
    int  iPos_Tmp_start = -1;
    string sTmp;
    string data;
    int  iRet_Count = 0;
    
    memset( this->sAddressAll_Newspaper_Tmp, 0x00, sizeof( this->sAddressAll_Newspaper_Tmp ) );
    
    // �Զ�ģʽ���ļ�
    infile.open( sFile );
    while (getline( infile, data))
    {
        data="";
        sTmp="";

        infile >> data;

        iPos_Tmp = data.find( start_str );
        if ( iPos_Tmp != std::string::npos )
        {
            string data_Tmp = data;
            while ( 1 )
            {
                iPos_Tmp_start = data_Tmp.find( start_str );
                if ( iPos_Tmp_start != std::string::npos )
                {
                    
                }
                else
                {
                    /* û�в�ѯ��������һ�����ж������ƥ��ģ�ͻȻƥ�䲻����break���ú���������ȡ��һ����ƥ�� */
                    break;
                }

                iPos_Tmp_end   = data_Tmp.find( end_str );
                if ( iPos_Tmp_end != std::string::npos )
                {
                    sTmp = data_Tmp.substr( iPos_Tmp_start, iPos_Tmp_start - iPos_Tmp_end -1 );
                    for( int i = 0; i < sTmp.length(); i++ )
                    {
                        out_Buf[iRet_Count][i] = sTmp[i];
                    }
                    iRet_Count++;
                    data_Tmp = data_Tmp.substr( iPos_Tmp_end + strlen( end_str ) );
                }
                else
                {
                    /* û�в�ѯ��������һ�����ж������ƥ��ģ�ͻȻƥ�䲻����break���ú���������ȡ��һ����ƥ�� */
                    break;
                }
            }
        }
        else
        {
            /* δƥ�䵽 */
            continue;
        }
    }
    infile.close();
    
    return iRet_Count;
}

#include "News_Collect_rmrb.h"
#include "News_Collect_nmrb.h"
#include "News_Collect_gmrb.h"
#include "News_Collect_zgqnb.h"
#include "News_Collect_xmwb.h"
#include "News_Collect_jjrb.h"


#if 1
// main() �ǳ���ʼִ�еĵط�
int main(int argc, char * argv[])
{
    char  aczTranDate[ 8 + 1 ];
    char  aczLogFile[ 256 + 1 ];
    char  aczNewspaperName[ 500 + 1 ];
    char  chflg;
    char  sYear[10 + 1];
    char  sMon[10 + 1];
    char  sData[ 10 + 1 ];
    
    memset( aczTranDate, 0x00, sizeof( aczTranDate ) );
    memset( aczLogFile , 0x00, sizeof( aczLogFile  ) );
    memset( aczNewspaperName , 0x00, sizeof( aczNewspaperName  ) );
    chflg = 0;
    memset( sYear,  0x00, sizeof(  sYear ) );
    memset( sMon ,  0x00, sizeof(  sMon  ) );
    memset( sData , 0x00, sizeof(  sData ) );
    
    /* У��������� */
    if ( !(argc > 0) )
    {
        printf( "Usage: %s -d ${YYYYMMDD} -l ${logpath} -t ${��ֽ����} \n", argv[0] ); 
        printf( "d:���ڣ���ʽYYYYMMDD���Ǳ��䣬Ĭ�ϵ���\n" ); 
        printf( "l:��־·�����ط���\n" ); 
        printf( "t:�����ֱ�ֽ��д���£��Ǳ��䣬Ĭ�ϳ����µ�ȫ����ֽ��\n \
                   �����ձ�           rmrb��    \n\
                   ũ���ձ�           nmrb��    \n\
                   �й����걨         zgqnb��   \n\
                   ������           xmwb      \n" );

        exit(-1);
    }

    while( (chflg = getopt(argc,argv,"d::l::t::")) != -1 )
    switch(chflg)
    {
        case 'd':
            strcpy(aczTranDate,optarg);
            printf( "��������:%s \n",aczTranDate);
            break;
        case 'l':
            strcpy( aczLogFile, optarg );
            printf( "��־·��:%s \n",aczLogFile);
            break;
        case 't':
            strcpy( aczNewspaperName, optarg );
            printf( "����[%s] \n", aczNewspaperName );
            break;
    }

    /* ��־��ʼ�� */
    if ( 0 != strlen( aczLogFile ) )
    {
        DEBUG_OPEN_FRAME( aczLogFile );
    }
    else
    {
        sprintf( aczLogFile, "%s%s.log", argv[0], aczTranDate );
        DEBUG_OPEN_FRAME( aczLogFile );
    }
    DEBUG_FRAME( "��������:%s ",aczTranDate);
    DEBUG_FRAME( "��־·��:%s ",aczLogFile);
    DEBUG_FRAME( "����[%s] ", aczNewspaperName );

    if ( 0 != strlen( aczTranDate ) )
    {
        /* У�����ڳ��� */
        if ( 8 != strlen( aczTranDate ) )
        {
            ERROR_FRAME( "aczTranDateΪ����YYYYMMDD�����Ȳ�����" );
            DEBUG_CLOSE_FRAME();
            exit( -1 );
        }

        memcpy( sYear,  aczTranDate,     4 );
        memcpy( sMon ,  aczTranDate+4,   2 );
        memcpy( sData , aczTranDate+4+2, 2 );

        DEBUG_FRAME( "sYear = [%s].", sYear );
        DEBUG_FRAME( "sMon  = [%s].", sMon  );
        DEBUG_FRAME( "sData = [%s].", sData );
    }
    
    if ( 0 == strncmp( aczNewspaperName, "rmrb"  , 4 ) || 0 == strlen( aczNewspaperName ) )      /*  �����ձ�    */
    {
        DEBUG_FRAME( "�����ձ�" );
        CReadNewspaper_RMRB   MyRead_rmrb;
        if ( 0 != strlen( aczTranDate ) )
        {
            DEBUG_FRAME( "���ñ�ֽ������[%s]", aczTranDate );
            MyRead_rmrb.oNewsPaper.setCNewspaperInfo( atoi(sYear), atoi(sMon), atoi(sData), MyRead_rmrb.sPaper_Name );
        }
        DEBUG_FRAME( "��ʼ�Ķ���ֽ" );
        MyRead_rmrb.ReadNewsPaper();
    }
    
    if ( 0 == strncmp( aczNewspaperName, "nmrb"  , 4 ) || 0 == strlen( aczNewspaperName ) )      /*  ũ���ձ�    */
    {
        DEBUG_FRAME( "ũ���ձ�" );
        CReadNewspaper_NMRB   MyRead_nmrb;
        if ( 0 != strlen( aczTranDate ) )
        {
            DEBUG_FRAME( "���ñ�ֽ������[%s]", aczTranDate );
            MyRead_nmrb.oNewsPaper.setCNewspaperInfo( atoi(sYear), atoi(sMon), atoi(sData), MyRead_nmrb.sPaper_Name );
        }
        DEBUG_FRAME( "��ʼ�Ķ���ֽ" );
        MyRead_nmrb.ReadNewsPaper();
    }
    
    if ( 0 == strncmp( aczNewspaperName, "gmrb"  , 4 ) || 0 == strlen( aczNewspaperName ) )      /*  �����ձ�    */
    {
        DEBUG_FRAME( "�����ձ�" );
        CReadNewspaper_GMRB   MyRead_gmrb;
        if ( 0 != strlen( aczTranDate ) )
        {
            DEBUG_FRAME( "���ñ�ֽ������[%s]", aczTranDate );
            MyRead_gmrb.oNewsPaper.setCNewspaperInfo( atoi(sYear), atoi(sMon), atoi(sData), MyRead_gmrb.sPaper_Name );
        }
        DEBUG_FRAME( "��ʼ�Ķ���ֽ" );
        MyRead_gmrb.ReadNewsPaper();
    }
    
    if ( 0 == strncmp( aczNewspaperName, "zgqnb" , 5 ) || 0 == strlen( aczNewspaperName ) )      /*  �й����걨  */
    {
        DEBUG_FRAME( "�й����걨" );
        CReadNewspaper_ZGQNB   MyRead_zgqnb;
        if ( 0 != strlen( aczTranDate ) )
        {
            DEBUG_FRAME( "���ñ�ֽ������[%s]", aczTranDate );
            MyRead_zgqnb.oNewsPaper.setCNewspaperInfo( atoi(sYear), atoi(sMon), atoi(sData), MyRead_zgqnb.sPaper_Name );
        }
        DEBUG_FRAME( "��ʼ�Ķ���ֽ" );
        MyRead_zgqnb.ReadNewsPaper();
    }
    
    if ( 0 == strncmp( aczNewspaperName, "xmwb"  , 4 ) || 0 == strlen( aczNewspaperName ) )      /*  ������    */
    {
        DEBUG_FRAME( "������" );
        CReadNewspaper_XMWB   MyRead_xmwb;
        if ( 0 != strlen( aczTranDate ) )
        {
            DEBUG_FRAME( "���ñ�ֽ������[%s]", aczTranDate );
            MyRead_xmwb.oNewsPaper.setCNewspaperInfo( atoi(sYear), atoi(sMon), atoi(sData), MyRead_xmwb.sPaper_Name );
        }
        DEBUG_FRAME( "��ʼ�Ķ���ֽ" );
        MyRead_xmwb.ReadNewsPaper();
    }
    
    if ( 0 == strncmp( aczNewspaperName, "jjrb"  , 4 ) || 0 == strlen( aczNewspaperName ) )      /*  �����ձ�    */
    {
        DEBUG_FRAME( "�����ձ�" );
        CReadNewspaper_JJRB   MyRead_jjrb;
        if ( 0 != strlen( aczTranDate ) )
        {
            DEBUG_FRAME( "���ñ�ֽ������[%s]", aczTranDate );
            MyRead_jjrb.oNewsPaper.setCNewspaperInfo( atoi(sYear), atoi(sMon), atoi(sData), MyRead_jjrb.sPaper_Name );
        }
        DEBUG_FRAME( "��ʼ�Ķ���ֽ" );
        MyRead_jjrb.ReadNewsPaper();
    }
    
    return 0;
}

#else

#define OUTLEN 4096

int main(int argc, char **argv)
 {
   //char *in_utf8 = "正�?��??�?";
   //char *in_gb2312 = "���ڰ�װ";
   char  in_utf8[40960];
   char  in_gb2312[32];            //<------ ����test����ֵ���ֻ������Ϊ31,�����core
   char  out[40960];

   memset( in_utf8,   0x00, sizeof( in_utf8 ) );
   memset( in_gb2312, 0x00, sizeof( in_gb2312 ) );
   memset( out,       0x00, sizeof( out ) );

   strcpy( in_utf8 , "正�?��??�?" );
   strcpy( in_gb2312 , "��" );

   cout << "length:[" << strlen( in_utf8 ) << "]" << endl;
   cout << "length:[" << strlen( in_gb2312 ) << "]" << endl;

   // utf-8-->gb2312 
   CodeConverter cc = CodeConverter("utf-8","gb2312");
   //cc.convert(in_utf8,strlen(in_utf8),out,OUTLEN);
   cc.convert( in_utf8, strlen(in_utf8), out, 40960 );
   cout << "utf-8-->gb2312 in=" << in_utf8 << ",out=" << out << endl;

   //memset( out, 0x00, sizeof( out ) );
   
   // gb2312-->utf-8 
   CodeConverter cc2 = CodeConverter("gb2312","utf-8");
   cc2.convert(in_gb2312,strlen(in_gb2312),out,400);
   cout << "gb2312-->utf-8 in=" << in_gb2312 << ",out=" << out << endl;

}

#endif
