/**
 *  @brief �����ձ�
 *  �����ձ��ĵ�ַ
 *  �������Ѽ�ϵͳ��
 *  ��ʵ��ip:
 *  http://paper.people.com.cn/rmrb/html/2018-07/31/nw.D110000renmrb_20180731_1-01.htm
 *
 *  ������ȡ��
 *  http://paper.people.com.cn/rmrb/html/2018-07/31/nw.D110000renmrb_20180731_${ĳ�汾�ĵڼ�������}-${�������}.htm
 *
 *  ʹ��boost�⣬�����п������������ĸ�ƽ̨��û������ġ�
 *
 *  ��������
 */
class CReadNewspaper_RMRB: public CReadNewspaper
{
    public:
        CNewspaper oNewsPaper;

        CReadNewspaper_RMRB()
        {
            CNewspaper a( sPaper_Name );
            oNewsPaper = a;

            /* �˴�����include��صĹ�����Ϣ������б䶯����Ҫ�����±���-------------start------------- */
            #include "Gl_rmrb.cfg"
            /* �˴�����include��صĹ�����Ϣ������б䶯����Ҫ�����±���-------------end------------- */

            iFlag_Data = -1;
            iFlag_Title = -1;
        }

    public:
        static char * sPaper_Name;

    public:
        /*
            ��ȡ��һ����ĵ�ַ
            ����string���͵� ��һ����ĵ�ַ
        */
        string Get_NextReading_NewspaperAdreess()
        {
            string sAddress_Newspaper_Tmp;
            char   sBuf_Tmp[4096];

            memset( sBuf_Tmp, 0x00, sizeof( sBuf_Tmp ) );

            sprintf( sBuf_Tmp, 
                   "http://paper.people.com.cn/rmrb/html/%4.4d-%02.2d/%02.2d/nw.D110000renmrb_%4.4d%02.2d%02.2d_%d-%02.2d.htm", 
                   oNewsPaper.iYear_Newspaper,  oNewsPaper.iMon_Newspaper, oNewsPaper.iDate_Newspaper, 
                   oNewsPaper.iYear_Newspaper,  oNewsPaper.iMon_Newspaper, oNewsPaper.iDate_Newspaper,
                   this->iXh_Bm_Newspaper, this->iBm_Newspaper );

            sAddress_Newspaper_Tmp = sBuf_Tmp;
            memset( this->sNextRead_Address, 0x00, sizeof( this->sNextRead_Address ) );
            memcpy( this->sNextRead_Address, sBuf_Tmp, strlen( sBuf_Tmp ) );
            
            DEBUG_FRAME( "Get_NextReading_NewspaperAdreess=[%s].", sBuf_Tmp );
            
            return sAddress_Newspaper_Tmp;
        }

        string Get_NextReading_File()
        {
            string sTmp;
            char   sBuf_Tmp[4096];

            memset( sBuf_Tmp, 0x00, sizeof( sBuf_Tmp ) );

            sprintf( sBuf_Tmp, 
                   "D110000renmrb_%4.4d%02.2d%02.2d_%d-%02.2d.htm", 
                   oNewsPaper.iYear_Newspaper,  oNewsPaper.iMon_Newspaper, oNewsPaper.iDate_Newspaper,
                   this->iXh_Bm_Newspaper, this->iBm_Newspaper );

            sTmp = sBuf_Tmp;
            memset( this->sNextRead_File, 0x00, sizeof( this->sNextRead_File ) );
            memcpy( this->sNextRead_File, sBuf_Tmp, strlen( sBuf_Tmp ) );
            
            DEBUG_FRAME( "Get_NextReading_File=[%s].", sBuf_Tmp );
            
            return sTmp;
        }

        /*
            ��ȡ���ŵı��⡢�������ݣ��ж��������ŵ�����
        */
        int ReadNewsPaper_Date( char * sFileName )
        {
            string  data;
            char  sBuf_Tile[4096];         /* ���ű��� */
            int   iPos;

            memset( sBuf_Tile, 0x00, sizeof( sBuf_Tile ) );
            iPos = 0;

            this->sTile_Newspaper = "";
            this->sData_Newspaper = "";

            iFlag_Data = -1;
            iFlag_Title = -1;

            ifstream infile;
            infile.open( sFileName );
            char ch;
            infile.get(ch);
            if (infile.eof())
            {
                printf( "file[%s] is empty.", sFileName );
                ERROR_FRAME( "file[%s] is empty.", sFileName );

                iFlag_NotFound = 0;

                infile.close();

                char sTmp_Cmd[4096];
                memset( sTmp_Cmd, 0x00, sizeof( sTmp_Cmd ) );
                sprintf( sTmp_Cmd, "rm -f %s", sFileName );
                int iRet = system( sTmp_Cmd );
                if( iRet >> 8 != 0 )
                {
                    printf( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                    ERROR_FRAME( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                }

                return -1;
                
            }
            infile.close();

            // �Զ�ģʽ���ļ�
            infile.open( sFileName );

            int iRet = 0;
            int iCount = 0;

            //while (!infile.eof())
            
            DEBUG_FRAME( "��ʼ�Ķ���ƪ����" );
            while (getline( infile, data))
            {
                iCount++;
                data="";
                infile >> data;

                iRet = Check_ThereIsPaperData( data );
                if ( 1 == iRet )
                {
                    iFlag_NotFound = 1;

                    if ( -1 == iFlag_Data )
                    {
                        this->iXh_Bm_Newspaper++;
                        iFlag_Data = 1;
                    }

                    /* �������� */
                    sData_Newspaper = data;
                    DEBUG_FRAME( "����������" );

                    if ( -1 == iFlag_Title )
                    {
                        /* û�л�ȡ������Ļ�������� */
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    DEBUG_FRAME( "����������" );
                }

                iRet = Check_ThereIsPaperTitle( data );
                if ( 1 ==  iRet )
                {
                    iFlag_NotFound = 1;
                    sTile_Newspaper = data;
                    
                    if( -1 == iFlag_Title ) iFlag_Title = 1;

                    DEBUG_FRAME( "�����ű���" );
                    
                    if ( -1 == iFlag_Data )
                    {
                        /* û�л�ȡ���������ݵĻ�������� */
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    DEBUG_FRAME( "�����ű���" );
                }
                
                if ( iCount > 2000 )
                {
                    DEBUG_FRAME( "iCount > 2000 break" );
                    break;
                }

                data="";
            }
            infile.close();
            DEBUG_FRAME( "iFlag_Title=[%d], iFlag_Data=[%d].", iFlag_Title, iFlag_Data );
            
            if ( -1 == iFlag_Title )
            {
                /* û�л�ȡ���⣬û�������쳣 */
                return -1;
            }
            
            DEBUG_FRAME( "iCount=[%d].", iCount );


            /* �������� */
            DEBUG_FRAME( "��ʼ�������˱���" );
            Filter_Tile_Newspaper();
            DEBUG_FRAME( "��ʼ����������������" );
            Filter_Data_Newspaper();

            char sTmp_Cmd[4096];
            memset( sTmp_Cmd, 0x00, sizeof( sTmp_Cmd ) );
            sprintf( sTmp_Cmd, "rm -f %s", sFileName );
            DEBUG_FRAME( "sTmp_Cmd=[%s]", sTmp_Cmd );
            iRet = system( sTmp_Cmd  );
            if( iRet >> 8 != 0 )
            {
                printf( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                ERROR_FRAME( "system error, sTmp_Cmd=[%s].", sTmp_Cmd );
            }

            return 0;
        }

        int ReadNewsPaper_Date()
        {
            return ReadNewsPaper_Date(this->sNextRead_File );
        }

        /* �Ķ������ݱ�ֽ */
        void ReadNewsPaper()
        {
            CodeConverter cc = CodeConverter("utf-8","gbk"); 
            CodeConverter sc = CodeConverter("utf-8","gbk"); 
            char sBuf[100];

            char sTmp_Cmd[4096];
            memset( sTmp_Cmd, 0x00, sizeof( sTmp_Cmd ) );
            sprintf( sTmp_Cmd, "mkdir -p %s", 
                     oNewsPaper.sDirName_LocaleComputer );
            DEBUG_FRAME( "sTmp_Cmd=[%s]", sTmp_Cmd );
            int iRet = system( sTmp_Cmd );
            if( iRet >> 8 != 0 )
            {
                printf( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                ERROR_FRAME( "system error, sTmp_Cmd=[%s].", sTmp_Cmd );
                return ;
            }

            string sLastRead_Address;
            while ( 1 )
            {
                this->Get_NextReading_NewspaperAdreess();
                
                DEBUG_FRAME( "this->sNextRead_Address=[%s]", this->sNextRead_Address );
                if ( sLastRead_Address == this->sNextRead_Address )
                {
                    printf( "ǰ����ε����ӵ�ַ���󣬹���û���������������ı�ǩ\n" );
                    ERROR_FRAME( "ǰ����ε����ӵ�ַ���󣬹���û���������������ı�ǩ" );
                    return ;
                }
                sLastRead_Address = this->sNextRead_Address;

                this->Get_NextReading_File();

                if( 0 == this->Get_Something_NewspaperAdreess_ToFile() )
                {
                    printf( "news into file succ\n" );
                    DEBUG_FRAME( "news into file succ" );
                    
                    char sTmp_Cmd[4096];
                    memset( sTmp_Cmd, 0x00, sizeof( sTmp_Cmd ) );
                    sprintf( sTmp_Cmd, "sed \":a;N;s/<\\/P>\r\\n<P>/<\\/P><P>/g;$!ba\" %s > %s.tmp ; cp %s.tmp %s; rm -rf %s.tmp;", 
                             this->sNextRead_File, 
                             this->sNextRead_File,
                             this->sNextRead_File,
                             this->sNextRead_File,
                             this->sNextRead_File );
                    DEBUG_FRAME( "sTmp_Cmd=[%s]", sTmp_Cmd );
                    int iRet = system( sTmp_Cmd );
                    if( iRet >> 8 != 0 )
                    {
                        printf( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                        ERROR_FRAME( "system error, sTmp_Cmd=[%s].", sTmp_Cmd );
                    }
                }
                else
                {
                    DEBUG_FRAME( "news into file fail, ����ԭ�򣺸ð汾�ĸ���ţ�û�б��������ݣ��ʲ����κδ���" );
                }

                iRet = this->ReadNewsPaper_Date();
                if ( -1 == iRet )
                {
                    /* ĳһ�������һ����ž���Not Found */
                    if ( -1 == iFlag_Data && -1 == iFlag_Title && 1 == this->iXh_Bm_Newspaper )
                    {
                        printf( "[%d]�ð汾�����������\n", this->iXh_Bm_Newspaper );
                        DEBUG_FRAME( "[%d]�ð汾�����������", this->iXh_Bm_Newspaper );
                        return ;
                    }
                    else
                    {
                        this->iBm_Newspaper++;
                        this->iXh_Bm_Newspaper = 1;
                        
                        /* ������һ���汾 */
                        printf( "�����ð���[%d]\n", this->iXh_Bm_Newspaper );
                        DEBUG_FRAME( "�����ð���[%d]", this->iXh_Bm_Newspaper );
                    }
                    continue;
                }

                char   pTmp[40960];
                char   pTmp2[40960];
                size_t iSize_New = 0;

                iSize_New =  4096;

                memset( pTmp , 0x00, sizeof( pTmp ) );
                memset( pTmp2, 0x00, sizeof( pTmp2) );

                for( int i = 0; i < (this->sTile_Newspaper).length(); i++ )
                {
                    pTmp[i] = (this->sTile_Newspaper)[i];
                }
                cc.convert(pTmp , strlen(pTmp) , pTmp2 , sizeof( pTmp2 ) ); 
                cout << "utf-8-->gb2312 in=" << pTmp << ",out=" << pTmp2 << endl;
                DEBUG_FRAME( "pTmp  = [%s].", pTmp  );
                DEBUG_FRAME( "pTmp2 = [%s].", pTmp2 );

                // ��дģʽ���ļ�
                ofstream outfile;

                memset( sBuf, 0x00, sizeof( sBuf ) );
                strcpy( sBuf, pTmp2 );
                sprintf( pTmp2, "%s/%s", oNewsPaper.sDirName_LocaleComputer , sBuf );
                DEBUG_FRAME( "pTmp2 = [%s].", pTmp2 );

                outfile.open( pTmp2 );

                char   pTmp3[30 + 1];
                char   pTmp4[_NEWSPAPER_DATA_LENGTH_];
                size_t iSize_New3 = 0;

                iSize_New3 =  _NEWSPAPER_DATA_LENGTH_;

                memset( pTmp3, 0x00, sizeof( pTmp3 ) );
                memset( pTmp4, 0x00, sizeof( pTmp4 ) );

                outfile << this->sData_Newspaper << endl;

                // �رմ򿪵��ļ�
                outfile.close();

                char sTmp_Cmd[4096];
                memset( sTmp_Cmd, 0x00, sizeof( sTmp_Cmd ) );
                sprintf( sTmp_Cmd, "mkdir -p %s;iconv -f utf-8 -t gbk %s -o %s.tmp;cp %s.tmp %s; rm -f %s.tmp", 
                         oNewsPaper.sDirName_LocaleComputer,
                         pTmp2, pTmp2, pTmp2, pTmp2, pTmp2 );
                DEBUG_FRAME( "sTmp_Cmd=[%s]", sTmp_Cmd );
                int iRet = system( sTmp_Cmd );
                if( iRet >> 8 != 0 )
                {
                    printf( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                    ERROR_FRAME( "system error, sTmp_Cmd=[%s]", sTmp_Cmd );
                }
            }
        }
};
char * CReadNewspaper_RMRB::sPaper_Name = "�����ձ�" ;
