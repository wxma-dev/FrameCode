/**
 *  @brief 经济日报
 *  经济日报的地址
 *  【新闻搜集系统】
 *  真实的ip:
 *  主页面 http://paper.ce.cn/jjrb/html/2018-08/25/node_2.htm
 *         http://paper.ce.cn/jjrb/html/2018-08/25/node_3.htm
 *         http://paper.ce.cn/jjrb/html/2018-08/25/node_4.htm
 *         http://paper.ce.cn/jjrb/html/2018-08/25/node_5.htm
 *         ......
 *         从node_2开始
 *         
 *         比如 http://paper.ce.cn/jjrb/html/2018-08/25/node_2.htm 里面，可以拿到
 *              http://paper.ce.cn/jjrb/html/2018-08/25/node_3.htm 
 *              http://paper.ce.cn/jjrb/html/2018-08/25/node_4.htm 
 *              http://paper.ce.cn/jjrb/html/2018-08/25/node_5.htm 
 *              ......
 *              在http://paper.ce.cn/jjrb/html/2018-08/25/node_2.htm里面，可以拿到
 *                http://paper.ce.cn/jjrb/html/2018-08/25/content_370891.htm 
 *                http://paper.ce.cn/jjrb/html/2018-08/25/content_370892.htm 
 *                ......具体报道的地址，再从这些具体的报道中拿到 新闻标题、新闻内容
 *
 *  新闻搜索
 */
class CReadNewspaper_JJRB: public CReadNewspaper
{
    public:
        CNewspaper oNewsPaper;

        CReadNewspaper_JJRB()
        {
            CNewspaper a( sPaper_Name );
            oNewsPaper = a;

            /* 此处增加include相关的过滤信息，如果有变动，需要则重新编译-------------start------------- */
            #include "Gl_xmwb.cfg"
            /* 此处增加include相关的过滤信息，如果有变动，需要则重新编译-------------end------------- */

            iFlag_Data = -1;
            iFlag_Title = -1;
        }

    public:
        static char * sPaper_Name;

    public:
        /*
            获取下一版面的地址
            返回string类型的 下一版面的地址
        */
        string Get_NextReading_NewspaperAdreess()
        {
            /*
            string sAddress_Newspaper_Tmp;
            char   sBuf_Tmp[4096];

            memset( sBuf_Tmp, 0x00, sizeof( sBuf_Tmp ) );

            sprintf( sBuf_Tmp, 
                   "http://xmwb.xinmin.cn/html/%4.4d-%02.2d/%02.2d/content_%d_%d.htm", 
                   oNewsPaper.iYear_Newspaper,  oNewsPaper.iMon_Newspaper, oNewsPaper.iDate_Newspaper, 

                   this->iBm_Newspaper, this->iXh_Bm_Newspaper );

            sAddress_Newspaper_Tmp = sBuf_Tmp;
            memset( this->sNextRead_Address, 0x00, sizeof( this->sNextRead_Address ) );
            memcpy( this->sNextRead_Address, sBuf_Tmp, strlen( sBuf_Tmp ) );
            
            DEBUG_FRAME( "Get_NextReading_NewspaperAdreess=[%s].", sBuf_Tmp );
            
            return sAddress_Newspaper_Tmp;
            */
            string sAddress_Newspaper_Tmp;
            char   sBuf_Tmp[4096];
            this->iBmAll_Count = 0;
            string sRet ="";

            memset( sBuf_Tmp, 0x00, sizeof( sBuf_Tmp ) );

            sprintf( sBuf_Tmp, 
                   "http://paper.ce.cn/jjrb/html/%4.4d-%02.2d/%02.2d/node_%d.htm", 
                   oNewsPaper.iYear_Newspaper,  oNewsPaper.iMon_Newspaper, oNewsPaper.iDate_Newspaper, 
                   this->iBmAll_Count + 2 );

            char sTmp_Cmd[4096];
            char sFile_Wget[ 256 ];
            memset( sTmp_Cmd, 0x00, sizeof( sTmp_Cmd ) );
            memset( sFile_Wget, 0x00, sizeof( sFile_Wget ) );
            strcpy( sFile_Wget, "node.htm" );
            /*
            sprintf( sTmp_Cmd, "wget %s -O %s; grep "<table width=\\"200px\\" border=\\"0\\" cellpadding=\\"2\\" cellspacing=\\"0\\" bgcolor=\\"#FFFFFF\\">" %s > %s.tmp; cp %s.tmp %s; rm -rf %s.tmp;", 
                     sBuf_Tmp, 
                     sFile_Wget, 
                     sFile_Wget, 
                     sFile_Wget, 
                     sFile_Wget, 
                     sFile_Wget, 
                     sFile_Wget );
            */
            sprintf( sTmp_Cmd, "wget %s -O %s;", 
                     sBuf_Tmp, sFile_Wget );
            int iRet = system( sTmp_Cmd );
            DEBUG_FRAME( "sTmp_Cmd=[%s].", sTmp_Cmd );
            if( iRet >> 8 != 0 )
            {
                printf( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                ERROR_FRAME( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                return sRet;
            }

            this->iBmAll_Count = this->GetStr_FromFile_between( sFile_Wget, "class=\"black\">&nbsp;<a href=\"", "\" class=\"black\" id=\"pageLink\">", this->sBmAll_Buf_Tmp );
            if ( this->iBmAll_Count < 0 )
            {
                ERROR_FRAME( "GetStr_FromFile_between error, this->iBmAll_Count=[%d]\n", this->iBmAll_Count );
                return sRet;
            }
            else
            {
                for( int i = 0; i < this->iBmAll_Count; i++ )
                {
                    char * pPoint_1 = NULL;
                    char * pPoint_2 = NULL;

                    pPoint_1 = &(sBmAll_Buf[i][0]);
                    pPoint_2 = &(sBmAll_Buf_Tmp[i][0]);

                    memcpy( pPoint_1, pPoint_2, strlen(pPoint_2) );
                }

                for( int i = 0; i < this->iBmAll_Count; i++ )
                {
                    char * pPoint_Tmp = NULL;

                    pPoint_Tmp = &(sBmAll_Buf[i][0]);

                    sprintf( sTmp_Cmd, "wget %s -O %s;", 
                             pPoint_Tmp, sFile_Wget );
                    int iRet = system( sTmp_Cmd );
                    if( iRet >> 8 != 0 )
                    {
                        printf( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                        ERROR_FRAME( "system error, sTmp_Cmd=[%s]\n", sTmp_Cmd );
                        return sRet;
                    }
                    
                    int j = GetStr_FromFile_between( sFile_Wget, "class=\"black\">&nbsp;<a href=\"", "\" class=\"black\" id=\"pageLink\">", sAddressAll_Newspaper_Tmp );
                    if ( j < 0 )
                    {
                        ERROR_FRAME( "GetStr_FromFile_between error, this->iBmAll_Count=[%d]\n", this->iBmAll_Count );
                        return sRet;
                    }
                    else
                    {
                        int i_Tmp = iAddressAll_Count;
                        iAddressAll_Count = iAddressAll_Count + j;
                        for( int i = i_Tmp; i < iAddressAll_Count; i++ )
                        {
                            char * pPoint_1 = NULL;
                            char * pPoint_2 = NULL;
                        
                            pPoint_1 = &(sAddressAll_Newspaper[i][0]);
                            pPoint_2 = &(sAddressAll_Newspaper_Tmp[i][0]);
                        
                            memcpy( pPoint_1, pPoint_2, strlen(pPoint_2) );
                        }
                    }
                }
            }
            /* 返回第一个的地址 */
            sRet = sAddressAll_Newspaper[0][0];
            return sRet;
        }

        string Get_NextReading_File()
        {
            string sTmp;
            char   sBuf_Tmp[4096];

            memset( sBuf_Tmp, 0x00, sizeof( sBuf_Tmp ) );

            sprintf( sBuf_Tmp, 
                   "D110000xmwb_%4.4d%02.2d%02.2d_%d-%02.2d.htm", 
                   oNewsPaper.iYear_Newspaper,  oNewsPaper.iMon_Newspaper, oNewsPaper.iDate_Newspaper,
                   this->iXh_Bm_Newspaper, this->iBm_Newspaper );

            sTmp = sBuf_Tmp;
            memset( this->sNextRead_File, 0x00, sizeof( this->sNextRead_File ) );
            memcpy( this->sNextRead_File, sBuf_Tmp, strlen( sBuf_Tmp ) );
            
            DEBUG_FRAME( "Get_NextReading_File=[%s].", sBuf_Tmp );
            
            return sTmp;
        }


        string Get_NextReading_File( char * sURL )
        {
            /* http://paper.ce.cn/jjrb/html/2018-08/27/content_371012.htm */
        
            string sTmp       = "";
            string sTmp_URL   = "";
            string sTmp_File  = "";
            char   sBuf_Tmp[4096];
            int    iPos_Tmp;
            int    iPos_Tmp_Tmp;

            memset( sBuf_Tmp, 0x00, sizeof( sBuf_Tmp ) );
            
            sTmp_URL = sURL;
            while ( 1 )
            {
                iPos_Tmp   = sTmp_URL.find( "/" );
                if ( iPos_Tmp != std::string::npos )
                {
                    
                }
                else
                {
                    break;
                }
                iPos_Tmp_Tmp = iPos_Tmp;
            }

            sTmp = sTmp_URL.substr( iPos_Tmp_Tmp + 1 );
            memset( this->sNextRead_File, 0x00, sizeof( this->sNextRead_File ) );

            for( int i = 0; i < sTmp.length(); i++ )
            {
                this->sNextRead_File[i] = sTmp[ i ];
            }

            DEBUG_FRAME( "Get_NextReading_File=[%s].", this->sNextRead_File );

            return sTmp;
        }

        /*
            获取新闻的标题、新闻内容，判断有无新闻的内容
        */
        int ReadNewsPaper_Date( char * sFileName )
        {
            string  data;
            char  sBuf_Tile[4096];         /* 新闻标题 */
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

            // 以读模式打开文件
            infile.open( sFileName );

            int iRet = 0;
            int iCount = 0;

            //while (!infile.eof())
            
            DEBUG_FRAME( "开始阅读该篇报道" );
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

                    /* 新闻内容 */
                    sData_Newspaper = data;
                    DEBUG_FRAME( "有新闻内容" );

                    if ( -1 == iFlag_Title )
                    {
                        /* 没有获取到标题的话，则继续 */
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    DEBUG_FRAME( "无新闻内容" );
                }

                iRet = Check_ThereIsPaperTitle( data );
                if ( 1 ==  iRet )
                {
                    iFlag_NotFound = 1;
                    sTile_Newspaper = data;
                    
                    if( -1 == iFlag_Title ) iFlag_Title = 1;

                    DEBUG_FRAME( "有新闻标题" );
                    
                    if ( -1 == iFlag_Data )
                    {
                        /* 没有获取到报道内容的话，则继续 */
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    DEBUG_FRAME( "无新闻标题" );
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
                /* 没有获取标题，没法处理，异常 */
                return -1;
            }
            
            DEBUG_FRAME( "iCount=[%d].", iCount );


            /* 过滤数据 */
            DEBUG_FRAME( "开始过来过滤标题" );
            Filter_Tile_Newspaper();
            DEBUG_FRAME( "开始过来过滤新闻内容" );
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

        /* 阅读完整份报纸 */
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
            
            if ( 0 == ((this->Get_NextReading_NewspaperAdreess()).length()) )
            {
                ERROR_FRAME( "Get_NextReading_NewspaperAdreess error!" );
                return ;
            }
            

            string sLastRead_Address;
/*          while ( 1 ) */  for( int i = 0; i < this->iAddressAll_Count; i++ )
            {
/*              this->Get_NextReading_NewspaperAdreess();  del by mawx@20180827  */
                memset( this->sNextRead_Address , 0x00, sizeof( this->sNextRead_Address ) );
                strcpy( this->sNextRead_Address , &(this->sAddressAll_Newspaper[i][0]) );

                DEBUG_FRAME( "this->sNextRead_Address=[%s]", this->sNextRead_Address );
                if ( sLastRead_Address == this->sNextRead_Address )
                {
                    printf( "前后二次的链接地址有误，估计没有配置搜索报道的标签\n" );
                    ERROR_FRAME( "前后二次的链接地址有误，估计没有配置搜索报道的标签" );
                    return ;
                }
                sLastRead_Address = this->sNextRead_Address;

/*              this->Get_NextReading_File(); */
                Get_NextReading_File( this->sNextRead_Address );
                DEBUG_FRAME( "this->sNextRead_File=[%s]", this->sNextRead_File );

                if( 0 == this->Get_Something_NewspaperAdreess_ToFile() )
                {
                    printf( "news into file succ\n" );
                    DEBUG_FRAME( "news into file succ" );
                    
                    char sTmp_Cmd[4096];
                    memset( sTmp_Cmd, 0x00, sizeof( sTmp_Cmd ) );
                    sprintf( sTmp_Cmd, "sed \":a;N;s/<\\/td> <\\/tr> <tr valign=\"top\">/\\n/g;$!ba\" %s > %s.tmp ; cp %s.tmp %s; rm -rf %s.tmp;", 
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
                    DEBUG_FRAME( "news into file fail, 可能原因：该版本的该序号，没有报道的内容，故不走任何处理。" );
                }

                iRet = this->ReadNewsPaper_Date();
                if ( -1 == iRet )
                { /* 该分支代码不适用于 新民晚报，基本上不会走进来，虽然不会用，代码还是基本保留吧 */
                    /* 某一个版面第一个序号就是Not Found */
                    if ( -1 == this->iFlag_Data && -1 == this->iFlag_Title && 1 == this->iXh_Bm_Newspaper )
                    {
                        printf( "[%d]该版本整版下载完成\n", this->iXh_Bm_Newspaper );
                        DEBUG_FRAME( "[%d]该版本整版下载完成", this->iXh_Bm_Newspaper );
                        return ;
                    }
                    else
                    {
                        this->iBm_Newspaper++;
                        this->iXh_Bm_Newspaper = 1;
                        
                        /* 继续下一个版本 */
                        printf( "继续该版面[%d]\n", this->iXh_Bm_Newspaper );
                        DEBUG_FRAME( "继续该版面[%d]", this->iXh_Bm_Newspaper );
                    }
                    continue;
                }
                else
                {
                    if ( -1 == this->iFlag_Data && 1 == this->iFlag_Title && 1 == this->iXh_Bm_Newspaper )
                    {
                        printf( "[%d]该版本整版下载完成\n", this->iXh_Bm_Newspaper );
                        DEBUG_FRAME( "[%d]该版本整版下载完成", this->iXh_Bm_Newspaper );
                        return ;
                    }
                    else if ( -1 == this->iFlag_Data && 1 == this->iFlag_Title )
                    {
                        this->iBm_Newspaper++;
                        this->iXh_Bm_Newspaper = 1;
                        
                        /* 继续下一个版本 */
                        printf( "继续该版面[%d]\n", this->iXh_Bm_Newspaper );
                        DEBUG_FRAME( "继续该版面[%d]", this->iXh_Bm_Newspaper );
                    }
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

                // 以写模式打开文件
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

                // 关闭打开的文件
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
char * CReadNewspaper_JJRB::sPaper_Name = "经济日报" ;
