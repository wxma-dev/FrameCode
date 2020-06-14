/*************************************************
**调用核心27601交易，判断所查账号是否为一本通主账号
**输入：DICTTAG InDict
**输出：DICTTAG OutDict
**返回值：0  是一本通主账户
          1  不是
          -1 失败
***************************************************/
int iCheckYBTZZH_27601(DICTTAG InDict,  DICTTAG OutDict)
{
        int iRet;
        char    sXym[ 4+1 ];
        char    sXyxx[ 60+1 ];
		char    sDqbs[ 1+1];

        memset( sXym, 0x00, sizeof( sXym ) );
        memset( sXyxx, 0x00, sizeof( sXyxx ) );
        memset( sDqbs, 0x00, sizeof( sDqbs ) );

        /* 生成BAFE外围流水号 */
        if( iGetBafeckh( pTransInfo->qdbm, pTransInfo->wwxtgzh ) )
        {
                ERROR( "生成bafe外围流水号失败!" );
		B3SetCode(OutDict,"XYM", "XYXX","BF0003");
                return -1;
        }
        /* 查询账户信息 */
        SetDictTagString( InDict, "JYDM", "027601" );     /* 核心交易码 */
        SetDictTagString( InDict, "MKBS", "1" );
        SetDictTagString( InDict, "JBS", "01" );
        SetDictTagString( InDict, "OFFSET", "029" );
        SetDictTagString( InDict, "JZHBZ", "1" );

        iRet = BeaiTransRecvMsg( "BANCS", InDict, OutDict, NULL );
        INFO( "Send to BANCS 027601 end. iRet=[%d]", iRet );
        if( iRet == -999 )
        {
                ERROR("027601查询账号信息超时");
		B3SetCode(OutDict,"XYM", "XYXX","BF0002");
                return -1;
        }
        else if( iRet != 0 )
        {
                ERROR( "027601查询账号信息失败" );
		B3SetCode(OutDict,"XYM", "XYXX","BF0001");
                return -1;
        }
        else
        {
                DEBUG( "027601查询帐号信息返回成功" );
                GetDictTagString( OutDict, "XYM", sXym, sizeof( sXym ) );
                GetDictTagString( OutDict, "XYXX", sXyxx, sizeof( sXyxx ) );

                trim( sXym );
                trim( sXyxx );

                if( strcmp( sXym, "0000" ) )
                {
                        ERROR( "27601核心返回失败 sXym[%s], sXyxx[%s]", sXym, sXyxx );
                        return -1;
                }
		GetDictTagString( OutDict, "DQBS", sDqbs, sizeof(sDqbs));
		trim(sDqbs);
		if(sDqbs[0] == 'M')
		{
			DEBUG("027601查询帐号为一本通主账号");
			return 0;
		}else
		{
			DEBUG("027601查询帐号不为一本通主账号");
			return 1;
		}	

        }
}

/************************************************************************
*	函数名称: GetBafeWjXh                                               *
*	函数功能: 产生文件序号                                              *
*	输入参数:															*
*	输出参数: char *outWjxh                                            *
*	返回值：成功 0、失败 -1 											*
*	编 写 人: 王威                                                     *
*************************************************************************/
int GetBafeWjXh(char *outWjxh)
{
	int iRet;
	char sWjxh[20];
	
	if(outWjxh==NULL)
	{
		PERROR("出参地址为null");
		return -1;
	}

	memset(sWjxh,0x00,sizeof(sWjxh));
	
	iRet=GetSerialNo("0002","00000",sWjxh);
	if( iRet!=0 )
	{
		PERROR("GetBafeWjXh err");
		return -1;
	}	
	else 
	{
		sprintf(outWjxh,"%04s",sWjxh);
		return 0;
	}
}



int get_keys(key_t *sem_key, key_t *shm_key);
int SequenceCompare(void *seq1, void *seq2);

static int get_ipc(key_t sem_key, key_t shm_key, int *sem_id, int *shm_id)
{
	if((*sem_id = semget(sem_key, 0, 0660)) == -1) {
		ERROR( "get_ipc semget: %s", strerror(errno));
		return -1;
	}

	if((*shm_id = shmget(shm_key, 0, 0660)) == -1) {
		ERROR( "get_ipc shmget: %s", strerror(errno));
		return -1;
	}

	return 0;
}

int GetSerialNo(char *xhbs, char *fhjgm, char *seqno)
{
	key_t sem_key, shm_key;
	int sem_id, shm_id;
	void *shmbase;
	entry *ent;
	sequence_info *seqinf;
	sequence *seq;
	queue *que;
	sequence_index ind;
	sequence_info *inf;
	long empty_flag = 0, reset_flag;
	long no;

    if(get_keys(&sem_key, &shm_key) < 0)
	{
		ERROR( "GetSerialNo get_keys error");
		return -1;
	}

    if(get_ipc(sem_key, shm_key, &sem_id, &shm_id))
    {
        ERROR( "GetSequence get_ipc error.");
        return -1;
    }

	if(!(shmbase = shmat(shm_id, 0, 0)))
	{
		ERROR( "GetSequence shmat error:%s", strerror(errno));
		return -1;
	}

	ent = shmbase;
	seqinf = shmbase + ent->sequence_info;
	seq = shmbase + ent->sequence;
	que = shmbase + ent->queue;

	strncpy(ind.xhbs, xhbs, sizeof(ind.xhbs));
	ind.xhbs[sizeof(ind.xhbs) - 1] = '\0';
	strncpy(ind.fhjgm, fhjgm, sizeof(ind.fhjgm));
	ind.fhjgm[sizeof(ind.fhjgm) - 1] = '\0';
	inf = bsearch(&ind, seqinf, ent->count, sizeof(sequence_info),
		SequenceCompare);

	if(!inf)
	{	
		ERROR("can not find sequence info, xhbs = %s jgm = %s", xhbs, fhjgm);
		return -1;
	}
	
	reset_flag = seq[inf->number].reset_flag;
	SEM_WAIT_TIME(sem_id, SEM_X(inf->number, SEQUENCE_COUNT));
	SEM_WAIT_UNDO(sem_id, SEM_X(inf->number, SEQUENCE_MUTEX));

	if(reset_flag != seq[inf->number].reset_flag)
		return -1;

	/* 取走序号 */
	no = seq[inf->number].dqxh;
	seq[inf->number].dqxh++;

	/* 如果序号超过最大值，置序号为1 */
	if(seq[inf->number].dqxh > inf->zdxh)
		seq[inf->number].dqxh = 1;

	/* 判断序号是否已经读空 */
	if(seq[inf->number].dqxh == seq[inf->number].db_dqxh)
		empty_flag = 1;

	SEM_SIGNAL_UNDO(sem_id, SEM_X(inf->number, SEQUENCE_MUTEX));

	/* 如果序号已读空，通知产生新序号 */
	if(empty_flag)
	{
		SEM_WAIT_UNDO(sem_id, NOTIFICATION_MUTEX);
		que->queue_data[que->count] = ind;
		que->count ++;
		SEM_SIGNAL_UNDO(sem_id, NOTIFICATION_MUTEX);
		SEM_SIGNAL(sem_id, NOTIFICATION_COUNT);
	}

	shmdt(shmbase);

	sprintf(seqno, "%ld", no);

	return 0;
}

/************************************************************************
*	函数名称: GetBeaiJylsh                                              *
*	函数功能: 产生外围渠道上送前置的流水顺序号                          *
*	输入参数:															*
*	输出参数: char* outJylsh                                            *
*	返回值：成功 0、失败 -1 											*
*	编 写 人: zouql                                                     *
*************************************************************************/
extern TRANSINFO *pTransInfo;

int GetBeaiJylsh(char* outJylsh)
{
	int iRet;
	char sJylsh[20];
	
	if(outJylsh==NULL)
	{
		PERROR("出参地址为null");
		return -1;
	}

	memset(sJylsh,0x00,sizeof(sJylsh));
	
	iRet=GetSerialNo("0001","00000",sJylsh);
	if( iRet!=0 )
	{
		PERROR("GetBeaiJylsh err");
		return -1;
	}	
	else 
	{
		strcpy(outJylsh,sJylsh);
		return 0;
	}
}



/*据bafe流水号生等成外围系统参考号
参数wwxtckh 为储存数据的指针,应用调用时需保证指针大小足够
入参：
     qdbm  外围系统的渠道编码 不少于2位,多于两位的时候取前两位

程序会根据bafe的系统编码,bafe的业务日期和序号(0001,00000)生成wwxtgzh */
int iGetBafeckh(char *qdbm,char *wwxtckh)
{
	int iRet;
	long lTmpxh;
	char sTmplsh[20+1]; 
	char sTmpxh[20+1];
	char sTmprq[8+1];
	char sTmpxtbm[3+1];
	
	memset( sTmplsh,    0x00,sizeof( sTmplsh   )); 
	memset( sTmpxh,    0x00,sizeof( sTmpxh)); 
	memset( sTmprq,    0x00,sizeof( sTmprq)); 
	memset( sTmpxtbm,    0x00,sizeof( sTmpxtbm)); 
	
	PDEBUG("进入iGetBafeckh");
	DEBUG("进入iGetBafeckh");

	if(wwxtckh==NULL||qdbm == NULL)
        {
                PERROR("参数地址为null");
                return -1;
        }
	/*获取流水号*/
	if( GetBeaiJylsh( sTmpxh) )
	{
		PERROR("获取BAFE流水号失败!"); 
		return -1;
	}
	PDEBUG("获取BAFE交易序号成功[%s]",sTmpxh);
	lTmpxh=atol(sTmpxh);
	sprintf( sTmplsh, "%019ld", lTmpxh); 
	PDEBUG("获取bafe流水号成功[%s]",sTmplsh);
	
	/*获取bafe业务日期*/
	
	memcpy(sTmprq,pTransInfo->jyrq,8);
#if 0
	if( GET_STRING_PARA( sTmprq,KJRQ ))
	{
		PERROR("取BAFE业务日期错误");     
		return -1;
	}
#endif
	PDEBUG("BAFE业务日期为[%s]",sTmprq);
	/*获取环境变量BAFEXTBM */
	snprintf(sTmpxtbm,sizeof(sTmpxtbm),"%s",getenv("BAFEXTBM"));
	PDEBUG("BAFE 系统编码为[%s]",sTmpxtbm);
	if(strlen(sTmpxtbm) < 3)
	{
		PERROR("xtbm 取值不正确");
		return -1;	
	}

	memcpy(wwxtckh, qdbm, 2);
	memcpy(wwxtckh+2, sTmpxtbm, 3);
	memcpy(wwxtckh+5, pTransInfo->ywlx, 2);
	memcpy(wwxtckh+7, sTmprq+2, 6);
	memcpy(wwxtckh+13, sTmplsh, 19); 
	
	PDEBUG("离开iGetBafeckh");
	DEBUG("离开iGetBafeckh");
	return 0;
}

/*判断传入账号类型*/

/********************************************************************
*	函数名称: Bafe_Zhlxzh()
*	函数功能: 根据输入账号判断账号类型
*	输入参数: 输入帐号 sZh (最大20账号)
*	输出参数: 
*		  iFlag	 账号类型标志
*	返回参数说明: iFlag==-1 类型未知
*		      iFlag==1  输入内部账
*		      iFlag==2  新核心卡号
*                     iFlag==3  新核心账号
*                     iFlag==4  老核心账号
*                     iFlag==5  老核心卡号
*	 返回值说明:  0 成功  -1 失败
*	编 写 人: 王威
*	日    期: 2011/11/16
*   修改总次数:  共计第0次修改
*   第0次修改:
********************************************************************/
/* 根据行里文档提供的转换规则 
	0. 20 位为老核心帐号（需要转换为新帐号)
	1. 19 位为新系统借记卡
	2. 18 位为老核心帐号（需要转换为新帐号)
	3. 17 位且首位为4,5,7,9为新帐号,9为内部账
	4. 17 位且不在上述范围内的帐号为老核心帐号 (需要转换为新帐号)
	5. 16 为老卡号，需检查卡bin配置 (老核心卡的卡bin为622478  940013,信用卡卡bin未知) 不在卡bin范围内时则判首位是否为'9',若为9则为内部账号，若否则报非法
	6. 小于16 位的，为旧账号，需要查找新老帐号表
	7. 大于20 位，为错误

*/
int Bafe_Zhlxzh(char *sZh,int *iFlag)
{
	int iZhLen =0 ;
	EXEC SQL BEGIN DECLARE SECTION;
	char zh[20+1];
	EXEC SQL END DECLARE SECTION;
	
	
	if(!sZh)
	{
		ERROR("参数检查失败");
		return -1;
	}
	
	if(strlen(sZh) > 20)
	{
		ERROR("输入账号的长度大于20位,参数不合法");
		*iFlag = -1;
		return 0;
	}
		
	memset(zh,0x00,sizeof(zh));
	memcpy(zh,sZh,strlen(sZh)<20?strlen(sZh):20);
	iZhLen=strlen(zh);
	
	switch ( iZhLen ) 
	{
		case 16 :
			zh[6]= 0;	
			EXEC SQL SELECT khd FROM t_pub_khdjgmdzb WHERE khd= :zh;
			if(SQLCODE) {
				/*DEBUG("查找数据库结果sqlcode为%d",SQLCODE);*/
				if( zh[0] == '9' )
					*iFlag=1;
				else 
					*iFlag =-1;		
			}
			else 
				*iFlag = 5;	
			break;		
		case 17 :		
			if( zh[0] == '9' )
				*iFlag=1;		
			else if( zh[0] == '4' || zh[0] == '5' || zh[0] == '7')
				*iFlag = 3;
			else
				*iFlag = 4;
			break;		
		case 18 :
			*iFlag = 4;
			break;			
		case 19 :
			*iFlag = 2;
			break;
		case 20 :
			*iFlag = 4;
			break;			
		default:
			*iFlag = 4;
			break;
	}
	
	return 0;

}

static int zhjudge(char *zh)
{
        if (zh[0] == '4')
                return 0;
        else if (zh[0] == '5')
                return 1;
        else if (zh[0] == '7')
                return 2;
        else if (zh[0] == '9')
                return 3;
        else
                return 4;
}

/********************************************************************
*	函数名称: Bafe_Zhlxpd()
*	函数功能: 把输入账号转换成新帐号并判断类型
*	输入参数: 输入帐号 sinZh 
*	输出参数: 
*		  return 账号类型标志
*	返回参数说明: soutXzh 新帐号
*	 返回值说明:
*		      return -1  错误
*		      return  0  同业
*		      return  1  对公
*         return  2  对私
*         return  3  内部
*         return  4  未知
*	编 写 人: 肖瑞
*	日    期: 2013/01/15
*   修改总次数:  共计第0次修改
*   第0次修改:
********************************************************************/
int Bafe_Zhlxpd(char *sinZh, char *soutXzh)
{
	int iZhLen = 0 ;
	int iRet = 0;
	char zh[18+1];
	char xzh[18+1];
	
	if(!sinZh)
	{
		ERROR("入参为空，参数检查失败");
		return -1;
	}
	
	if(strlen(sinZh) > 18)
	{
		INFO("输入账号的长度大于18位,");
		return 4;
	}
		
	memset(zh,0x00,sizeof(zh));
	memset(xzh,0x00,sizeof(xzh));
	memcpy(zh,sinZh,strlen(sinZh)<18?strlen(sinZh):18);
	iZhLen = strlen(zh);
	
	if (iZhLen < 16)
	{
		INFO("帐号小于16位");
		iRet = DB_XJZHDZB_SELECT_XZH_BY_JZH(zh, xzh);
		if ( iRet == -1)
		{
			ERROR("查找新旧帐号对照表失败");
			memcpy(soutXzh, sinZh, strlen(sinZh));
			return -1;
		}
		else if ( iRet == 1 )
		{
			ERROR("查找新旧帐号对照表未找到记录");
			memcpy(soutXzh, sinZh, strlen(sinZh));
			return 4;
		}
		else /*iRet == 0*/
		{
			memcpy(soutXzh, xzh, strlen(xzh));
			return zhjudge(xzh);
		}
	}
	else if (iZhLen == 16)
	{
		/*16位为老卡号*/
		memcpy(soutXzh, sinZh, strlen(sinZh));
		if (zh[0] == '9')
			return 3;
		else
			return 4;
	}
	else if (iZhLen == 17)
	{
		/*如果是新帐号直接返回结果*/
		if (zhjudge(zh) != 4)
		{
			memcpy(soutXzh, sinZh, strlen(sinZh));
			return zhjudge(zh);
		}
		/*如果不是新帐号查找新旧帐号对照表*/
		else
		{
			/*查找新旧帐号对照表*/
			iRet = DB_XJZHDZB_SELECT_XZH_BY_JZH(zh, xzh);
			if ( iRet == -1)
			{
				ERROR("查找新旧帐号对照表失败");
				memcpy(soutXzh, sinZh, strlen(sinZh));
				return -1;
			}
			else if ( iRet == 1 )
			{
				ERROR("查找新旧帐号对照表未找到记录");
				memcpy(soutXzh, sinZh, strlen(sinZh));
				return 4;
			}
			else /*iRet == 0*/
			{
				memcpy(soutXzh, xzh, strlen(xzh));
				return zhjudge(xzh);
			}
		}
	}
	else/*iZhLen == 18*/
	{
		/*18位为旧帐号，直接查表再判断*/
		iRet = DB_XJZHDZB_SELECT_XZH_BY_JZH(zh, xzh);
		if ( iRet== -1)
		{
			ERROR("查找新旧帐号对照表失败");
			memcpy(soutXzh, sinZh, strlen(sinZh));
			return -1;
		}
		else if ( iRet == 1 )
		{
			ERROR("查找新旧帐号对照表未找到记录");
			memcpy(soutXzh, sinZh, strlen(sinZh));
			return 4;
		}
		else /*iRet == 0*/
		{
			memcpy(soutXzh, xzh, strlen(xzh));
			return zhjudge(xzh);
		}
	}
}


/***********************************************************************
* 　函数名:DB_XJZHDZB_SELECT_XZH_BY_JZH
* 	函数功能:根据旧账号获得新账号
*   输入参数: char *sInzh         旧账号
*   输出参数: char *sOutzh　      旧账号
*   返回值:  0     成功
			 1     查无记录 
*  			-1     失败
*   编写日期:20111202
************************************************************************/
int DB_XJZHDZB_SELECT_XZH_BY_JZH (char *sInzh, char *sOutzh)
{
	EXEC SQL BEGIN DECLARE SECTION;
		char sJzh[22+1];				
		char sDbxzh[17+1];				
	EXEC SQL END DECLARE SECTION;

	memset( sJzh   ,0x00,sizeof( sJzh       ));
	memset( sDbxzh ,0x00,sizeof( sDbxzh 	));

	strpack(sInzh) ;
	if (strlen( sInzh )== 0 )  
	{
		PERROR("非法帐号,帐号为空!") ;
		return -1 ;
	}
		
	strcpy( sJzh, sInzh );
	strpack(sJzh);
	/*先查询对公新旧账号对照表*/
	EXEC SQL SELECT xzh 
		INTO :sDbxzh
		FROM t_pub_xjzhdzb_dg
		WHERE jzh = :sJzh;
	if( SQLCODE ==0 )
        {
                strpack(sDbxzh);
                strcpy( sOutzh, sDbxzh );
                return 0;
        }
	else if( SQLCODE !=SQLNOTFOUND )
        {

                PERROR("根据帐号[%s]查询t_pub_xjzhdzb错误[%d]",sJzh,SQLCODE );
                ERROR("根据帐号[%s]查询t_pub_xjzhdzb错误[%d]",sJzh,SQLCODE );
                return -1;
        }
	/*若对公新旧账号对照表中未查到，再从对私新旧账号对照表中查询*/
	EXEC SQL SELECT xzh 
		INTO :sDbxzh
		FROM t_pub_xjzhdzb
		WHERE jzh = :sJzh;
	if( SQLCODE ==0 )
	{
		strpack(sDbxzh);
		strcpy( sOutzh, sDbxzh );
		return 0;
	}
	else if( SQLCODE ==SQLNOTFOUND )
	{
		strcpy( sOutzh, sJzh );
		return 1;
	}else
	{
		PERROR("根据帐号[%s]查询t_pub_xjzhdzb错误[%d]",sJzh,SQLCODE );
		ERROR("根据帐号[%s]查询t_pub_xjzhdzb错误[%d]",sJzh,SQLCODE );
		return -1;
	}

}


/********************************************************************
*       函数名称: iBafeCheckDate()
*       函数功能: 校验文件名中日期是否与业务日期一致
*       输入参数: 
*           filename 文件名（不含路径）
*           offset   日期位偏移量
*           wwxtbm   外围系统编码
*       输出参数:
*       返回参数说明:
*       返回值说明: 
*           return -1 失败
*           return  0 日期校验一致或者不用校验
*           return  1 日期校验不一致
*       编写人: 肖瑞
*       日  期: 2013/01/21
*   修改总次数:  共计第0次修改
*   第0次修改:
********************************************************************/
int iBafeCheckDate(char *filename, int offset, char *wwxtbm)
{
	char CheckBz[12];
	int  iRet = -1;
	char sSection[3+1]; /*段标签*/
	char sRq[8+1];      /*日期*/

	memset(sSection, 0x00, sizeof(sSection));
	memset(sRq, 0x00, sizeof(sRq));
	memset(CheckBz, 0x00, sizeof(CheckBz));

	if ( strlen(wwxtbm) != 3 )
	{
		ERROR("警告：外围系统编码不为3位");
	}
	memcpy(sSection, wwxtbm, 3);
	iRet = Get_paramvalue("FILEDATE_CHK", sSection, CheckBz);
	if ( iRet < 0 )
	{
		ERROR("读取配置文件ParamValues.conf失败");
		return -1;
	}
	if ( CheckBz[0] != 'N' )
	{
		strncpy(sRq, filename+offset, 8);
		if (!strncmp(pTransInfo->jyrq, sRq, 8))
		{
			INFO("日期校验一致，文件名：[%s]，外围系统编码：[%s]", filename, sSection);
			return 0;
		}
		else
		{
			INFO("日期校验不一致，文件名：[%s]，外围系统编码：[%s]", filename, sSection);
			return 1;
		}
	}
	else
	{
		INFO("不用校验，文件名：[%s]，外围系统编码：[%s]", filename, sSection);
		return 0;
	}
}


/****************************************
*inbuf:输入金额
*infmt:输出的bancs金额类型：S9(14)V999,9(14)V999S 等
*outfmt:输入金额类型，例：
      S14.2(带符号有小数点型式，'.'左右分别表示整数和小数位长度)
      S14+2(带符号无小数点型式，'+'左右分别表示整数和小数位长度)
      14.2(S14.2的无符号型式),14+2(S14+2的无符号型式)
*outbuf:转换后的金额
*成功返回0 ,失败返回-1
****************************************/
int BancsStringToString(char *inbuf, char *infmt, char *outfmt, char *outbuf)
{
	char tmpbuf[32];
	char intmp[32];
	char fmttmp[16];
	char sXS[12],sZS[20];
	char *p=NULL,*q=NULL, *pi=NULL, *pt=NULL;
	int iXS=0,iZS=0,i;
	char flag='+';
	int outOffSet = 0;

	memset(tmpbuf, 0x00, sizeof(tmpbuf));
	memset(intmp, 0x00, sizeof(intmp));
	memset(fmttmp, 0x00, sizeof(fmttmp));
	memset(sZS, 0x00, sizeof(sZS));
	memset(sXS, 0x00, sizeof(sXS));

	strcpy(intmp, inbuf);
	strcpy(fmttmp, infmt);	
	p =intmp;
	q =fmttmp;
	
	if(*q=='S')
	{
		if(*p=='-'|| *p=='+')
		{
			flag = *p; 
			p++;
			q++;
		}else
		{
			ERROR("bancs 金额符号位非法[%s][%s]", intmp, fmttmp);
			return -1;
		}
		
	}else if(fmttmp[strlen(fmttmp)-1]=='S')
	{
		if(intmp[strlen(intmp)-1]=='-'||intmp[strlen(intmp)-1]=='+')
		{
			flag = intmp[strlen(intmp)-1];
			intmp[strlen(intmp)-1] = 0;
			fmttmp[strlen(fmttmp)-1] = 0;
		}else
		{
			ERROR("bancs 金额符号位非法[%s][%s]", intmp, fmttmp);
			return -1;
		}
	}else
	{
		if(*p == '-' || *p == '+' 
	          || intmp[strlen(intmp)-1]=='-' || intmp[strlen(intmp)-1]=='+')
		{
			ERROR("bancs 金额符号位非法[%s][%s]", intmp, fmttmp);
			return -1;
		}
	}
	pt = strchr(q,'V');
	if(!pt)
	{
		ERROR("金额格式非法[%s]", fmttmp);	
		return -1;
	}
	pt++;
	iXS = strlen(pt);
	memcpy(sXS, p+(strlen(p)-iXS), iXS);
	memcpy(sZS, p, strlen(p)-iXS);	
	/*outfmt**/	
	strcpy(fmttmp, outfmt);
	p = fmttmp;
	if(*p == 'S')
	{
		tmpbuf[0]=flag;
		outOffSet ++;
		p++;
	}
	pi = NULL;
	pi = strchr(p, '.');
	if(pi)
	{
		iXS = atoi(pi+1);
		sprintf(tmpbuf+outOffSet,"%s.%-*s", DelLeftZero(sZS), iXS, DelRightZero(sXS));
		strcpy(outbuf, tmpbuf);
		return 0; 
	}
	pi = NULL;
	pi = strchr(p,'+');
	if(pi)
	{	
		
		iXS = atoi(pi+1);
		if(strlen(DelRightZero(sXS))> iXS)
		{
			ERROR("超长小数位不可省略[%s][%s]", sXS, outfmt);
			return -1;
		}
		sprintf(tmpbuf+outOffSet,"%s%0*s", DelLeftZero(sZS), iXS, DelRightZero(sXS));
		strcpy(outbuf, tmpbuf);
		return 0; 
	}
	if(atoi(sXS)>0)
	{
		ERROR("[%s]小数位不能舍弃", inbuf);
		return -1;
	}
	sprintf(tmpbuf+outOffSet, "%s", DelLeftZero(sZS));	
	strcpy(outbuf, tmpbuf);
	return 0;
}

void freeToNull(void **point)
{
	free(*point);
	*point = NULL;
}	

/***金额非法字符判断
    inbuf 待检查金额
    point 小数点标志，0-不带小数点 1-带小数点
    flag  正负号标志，0-不带符号，1-首位带'+'或'-'
除了0-9的数字字符和piont为1时的小数点(最多一个)、flag为1时的首位正负号，其他都为非法字符
返回值：0--通过 其他--非法
***/
int iAmountCheck(char *inbuf, int point, int flag)
{
	int i=0;
	int iCount=0;
	
	if(flag == 1) 
	{
		if(inbuf[0] == '+' || inbuf[0] == '-')
			i++;
	}
	for(; i<strlen(inbuf); i++)
	{
		if(inbuf[i] < 0x30 || inbuf[i] > 0x39)
		{
			if(inbuf[i] == '.' && iCount == 0)
			{
				if(point == 1)
					iCount++;
				else
					return -1;
			}else
				return -1;
		}
	}
	return 0;
}



/***********************************************************************
*	函数名称: get_xnjgczy()   			                               *
*	函数功能: 虚拟机构操作员对照表操作                                       *
*	输入参数: sInFhdm  --分行代码	 	                               *
*			  sInXtbm -- 系统编码                                      *
*	输出参数: 				                                           *
*             sOutXnjg  -- 虚拟机构                                    *
*			  sOutXnczy --虚拟操作员                                   *
*	返回值：0--成功，-1 --失败										   *	
*	编 写 人: ufc                                                      *
*	日    期: 2008/11/10                                               *
*   修改总次数:  共计1次修改	                                       *
*   第1次修改:   2009-1-13                                             *
***********************************************************************/
int get_xnjgczy( char *sInFhdm, char *sInXtbm, char *sOutXnjg, char *sOutXnczy )
{
	EXEC SQL BEGIN DECLARE SECTION;
		char sXnjg[12];
		char sXngy[12];
		char sFhdm[8+1];
		char sXtbm[3+1];
	EXEC SQL END DECLARE SECTION;
	memset(sXnjg, 0x00, sizeof(sXnjg));
	memset(sXngy, 0x00, sizeof(sXngy));
	memset(sFhdm, 0x00, sizeof(sFhdm));
	memset(sXtbm, 0x00, sizeof(sXtbm));
        
	PINFO("进入get_xnjgczy函数");
	if( sInXtbm == NULL )
	{
		ERROR("参数标识不能为空!");
		return -1;
	}
	
	trim(sInFhdm);
	trim(sInXtbm);
	strcpy(sXtbm, sInXtbm);
	strcpy(sFhdm, sInFhdm);
	if(strlen(sInFhdm) == 0)
	{
		strcpy(sFhdm, "00000");
	}
	
	EXEC SQL SELECT XNJG,XNCZY INTO :sXnjg, :sXngy FROM T_PUB_XNJGCZYCSB
		 WHERE FHDM=:sFhdm AND XTBM=:sXtbm;
	if(SQLCODE)
	{
		ERROR("获取虚拟机构及柜员失败 SQLCODE[%d] fhdm[%s] xtbm[%s]",SQLCODE, sFhdm, sXtbm);		

		return -1;
	}	

	strcpy( sOutXnjg, sXnjg );
	strcpy( sOutXnczy, sXngy );
	
	PINFO("离开get_xnjgczy函数");
	return 0;
}


/***********************************************************
* 函数名称:Get_paramvalue
* 函数说明:读取配置文件ParamValues.conf中指定变量的值
* 输入参数:
*       char *sInSection  分段标签说明
*       char *sInTag      变量标签说明
* 输出参数:
*       Char *sOutValue  变量输出值  
* 建立时间:20090403
************************************************************/
int Get_paramvalue( char *sInSection,char* sInTag, char *sOutValue )
{
	char sConFileName[50+1];/*配置文件名*/
	char sConTag[25+1];/*配置标签  */
	char sSection[16+1];/*段标签    */
	char sTags[20+1];/*区域标签  */
	char sValue[100+1];

	memset( sConFileName,0x00,sizeof( sConFileName ));
	memset( sConTag     ,0x00,sizeof( sConTag));
	memset( sSection    ,0x00,sizeof( sSection     ));
	memset( sTags       ,0x00,sizeof( sTags        ));
	memset( sValue      ,0x00,sizeof( sValue    ));

	DEBUG("进入Get_paramvalue函数");
	
	strncpy( sSection,sInSection, 16);
	strcpy( sTags,sInTag        );
	strcpy( sConFileName,"ParamValues.conf" );
	g_hdlCfg = LoadConfigure( sConFileName );
	if( !g_hdlCfg )
	{
		ERROR("获取配置文件[%s]错误",sConFileName );
		return -1;
	}
	if( GetValue(g_hdlCfg,sSection,sTags, sValue,sizeof( sValue )) < 0 )
	{
		ERROR("获取配置文件中配置参数错误[%s][%s]",sSection,sTags );
		FreeConfigure(g_hdlCfg);
		return -1;
	}
	strcpy( sOutValue,sValue );
	FreeConfigure(g_hdlCfg);

	DEBUG("退出Get_paramvalue函数");

	return 0;
}




int processErrFile(FILE* fpFile,char *sInPlWjm,char *firstLine, char *sHxxym,char *sHxxyxx,int flag,char *sBuffer)
{

	EXEC SQL BEGIN DECLARE SECTION;
		char    sWjm[ 200+1 ];             /* 文件名 */
	EXEC SQL END DECLARE SECTION;

	int 	  iRet;
	char    sNewPath[ 512+1 ];            /* 路径 */
	char    sRetFileName[ 512+1 ];        /* 返回中间业务文件名 */
	char    sRetFullFileName[ 512+1 ];    /* 返回中间业务文件全名 */
	char    sRetInbuFileName[ 512+1 ];    /* 返回中间业务文件名 */
	char    sWwXym[ 4+1 ];                /* 外围响应码 */
	char    sWxXyxx[ 60+1 ];              /* 外围响应信息 */
	char    sTmpMx1[ 500+1 ];
	char    sTmpMx2[ 500+1 ];
	char    sCdTagName[ 50+1 ];           /* FtsCom.conf标签名 */
	FILE*   fpRetFile;
	FILE*   fp;
	char    sTrLine[ 1024+1 ];
	char    sLineStr[ 1024+1 ];
	char    sLineStr1[ 1024+1 ];

	iRet = 0;
	fpRetFile = NULL;
	memset( sTrLine, 0x00, sizeof( sTrLine ) );
	memset( sTmpMx1, 0x00, sizeof( sTmpMx1 ) );
	memset( sTmpMx2, 0x00, sizeof( sTmpMx2 ) );
	memset( sWwXym, 0x00, sizeof( sWwXym ) );
	memset( sWxXyxx, 0x00, sizeof( sWxXyxx ) );
	memset( sRetFileName, 0x00, sizeof( sRetFileName ) );
	memset( sNewPath, 0x00, sizeof( sNewPath ) );
	memset( sRetFullFileName, 0x00, sizeof( sRetFullFileName ) );
	memset( sRetInbuFileName, 0x00, sizeof( sRetInbuFileName ) );
	memset( sCdTagName, 0x00, sizeof( sCdTagName ) );
	memset( sLineStr, 0x00, sizeof( sLineStr ) );
	memset( sLineStr1, 0x00, sizeof( sLineStr1 ) );

	trim( sHxxyxx );

	INFO( "批量验户整包失败，写返回文件 sHxxym[%s], sHxxyxx[%s]", sHxxym, sHxxyxx );
	strcpy( sWjm, sInPlWjm );

	/* 国库集中支付电子化系统 */
	if( strncmp( "341", sInPlWjm, 3 ) == 0 )
	{
		strcpy( sCdTagName, BAFE_FFCS_SEND );
	}
	else
	{
		strcpy( sCdTagName, BAFE_INBU_SEND );
	}
	DEBUG( " sCdTagName=[%s] ", sCdTagName );

	strncpy( sRetFileName, sInPlWjm, strlen( sInPlWjm ) - 4 );

	trim(sRetFileName);
	sprintf( sRetInbuFileName, "%s.res", sRetFileName );
	iRet = GetFtsFilepath( sCdTagName, sNewPath );
	if( iRet )
	{
		ERROR( "取发送文件错误" );
		return -1;
	}
	sprintf( sRetFullFileName, "%s/%s", sNewPath, sRetInbuFileName );

	DEBUG( "sRetFileName[%s],sRetInbuFileName[%s], sNewPath[%s]", sRetFileName, sRetInbuFileName, sNewPath );

	if( ( fpRetFile = fopen( sRetFullFileName, "w+" ) ) == NULL)
	{
		ERROR( "打开中间业务返回文件失败[%s]", sRetFullFileName );
		return -1;
	}

	DEBUG( "firstLine=[%s]", firstLine );

	/* 从文件头中截取出成功笔数与成功金额的前后位置 */
	strncpy( sLineStr, firstLine, 57 );
	strncpy( sLineStr1, firstLine+80, strlen(firstLine)-80 );
	DEBUG( "sLineStr=[%s]", sLineStr );
	DEBUG( "sLineStr1=[%s]", sLineStr1 );

	if( flag==4 )
	{
		DEBUG("代发代扣文件直接失败也需要返回成功笔数与成功金额，否则中台无法处理 flag=[%d]", flag);
		fprintf( fpRetFile,"%s00000000%15.0f%s\n", sLineStr, 0, sLineStr1 );
	}
	else
	{
		fprintf( fpRetFile,"%s\n", firstLine);
	}

	iRet = iTranHxToWw( pTransInfo->wwxtbm, "99", "BANCSPL", sHxxym, sWwXym, sWxXyxx );
	if( iRet )
	{
		strcpy( sWwXym, "9999" );
		strcpy( sWxXyxx, "其他错误" );
	}
	DEBUG( "返回中间业务错误信息[%s][%s]", sWwXym, sHxxyxx );

	if( strlen(sHxxyxx) == 0 )
	{
		strcpy( sHxxyxx, sWxXyxx );
	}

	memset( sHxxym, 0x00, sizeof( sHxxym ) );
	strcpy( sHxxym, sWwXym );

	/* 开始写文件体 */
	while(1)
	{
		memset( sTrLine, 0x00, sizeof( sTrLine ) );
		memset( sTmpMx1, 0x00, sizeof( sTmpMx1 ) );
		memset( sTmpMx2, 0x00, sizeof( sTmpMx2 ) );

		if( fgets( sTrLine, sizeof( sTrLine ), fpFile ) == NULL )
		{
			break;
		}

		if( sTrLine[ strlen( sTrLine ) -1 ] == '\n' )
		{
			sTrLine[ strlen( sTrLine ) -1 ] = '\0';
		}

		if( ( strlen( sTrLine ) == 0 ) )
		{
			break;
		}

		switch(flag)
		{
			case 1: /*批量验户*/
				strncpy( sTmpMx1, sTrLine, 118 );
				strncpy( sTmpMx2, sTrLine+strlen(sTrLine)-132, 132 );
				fprintf( fpRetFile, "%-118.118s%-4.4s%-60.60s%-100.100s\n",sTmpMx1, sHxxym, sHxxyxx, sTmpMx2 );
				break;
			case 2:/*开户 */
				strncpy( sTmpMx1, sTrLine, 296 );
				strncpy( sTmpMx2, sTrLine+strlen( sTrLine )-275, 275 );
				fprintf( fpRetFile, "%-296.296sFFF00000%-4.4s%-60.60s%-275.275s\n",sTmpMx1, sHxxym, sHxxyxx, sTmpMx2 );
				break;
			case 3:/* 批量转账 */
				strncpy( sTmpMx1, sTrLine, 282 );
				strncpy( sTmpMx2, sTrLine+strlen( sTrLine )-100, 100 );
				fprintf( fpRetFile, "%-282.282s%-4.4s%-60.60s%015s%8.8s%09.9s%-100.100s\n",sTmpMx1, sHxxym, sHxxyxx, "0", pTransInfo->jyrq, "0", sTmpMx2 );
				break;
			case 4:/*代收代付 */
				strncpy( sTmpMx1, sTrLine, 178 );
				strncpy( sTmpMx2, sTrLine+strlen( sTrLine )-100, 100 );
				fprintf( fpRetFile, "%-178.178s%-4.4s%-60.60s%015s%8.8s%09.9s%-100.100s\n",sTmpMx1, sHxxym, sHxxyxx, "0", pTransInfo->jyrq, "0", sTmpMx2 );
				break;
			case 5:/* 验户开户 */
				strncpy( sTmpMx1, sTrLine, 346 );
				strncpy( sTmpMx2, sTrLine+strlen( sTrLine )-208, 208 );
				fprintf( fpRetFile, "%-346.346sF0000000%-4.4s%-60.60s%-176.176s\n",sTmpMx1, sHxxym, sHxxyxx, sTmpMx2 );
				break;
			default:
				ERROR("Invalid input flag");
				break;
		}
	}

	fclose( fpRetFile );

	SetDictTagString( "InbuCDSendTag", "_FTS_JOB", sCdTagName );
	SetDictTagString( "InbuCDSendTag", "_FTS_FILENAME", sRetInbuFileName );

	/* 调用CD实时文件传输接口 */
	iRet = iBafeSendMsgToFts( "InbuCDSendTag", 1 );
	DEBUG( "调用cd发送实时文件返回值[%d]", iRet );

	if( iRet == -1 )
	{
		ERROR( "文件传输失败!" );
		return -1;
	}
	else if( iRet == -2 )
	{
		ERROR( "标志文件传输失败" );
		return -1;
	}

	EXEC SQL COMMIT WORK;
	INFO( "整包处理失败，返回结果文件 sRetInbuFileName[%s]sHxxym[%s], sHxxyxx[%s]",
		sRetInbuFileName, sHxxym, sHxxyxx );
	return 0;
}



/**********************************************************************************
*  函数名:   iBafeSendMsgToFts
*  程序描述: 发送CD平台实时任务开始报文
*  建立日期: 20111214
*  创建人  : 王威
*  参数说明: DICTTAG sIntag 包含必要要素的字典标签
*            int flag        是否需要发送标志文件 0 不 1  需要 其他 非法
* 返回值说明: 0 成功 -1 失败  -2 发送标志文件失败
***********************************************************************************/
int iBafeSendMsgToFts(DICTTAG sIntag,int flag)
{
	
	int i;
	int j;
	int iLen;
	int iRet;
/*	int port = 32792;*/ /*默认端口*/
	int port;
	HANDLE conf;
	
	char *p;
	char sTaskId[64+1];
	char sFtsJob[256+1];
	char sIp[15+1];
	char sPort[15+1];
	char sEndflag[10+1];
	char sInDir[MAXDIRLEN+1];
	char sOutDir[MAXDIRLEN+1];
	char tmpendfilename[MAXFILELEN+1];
	char endfilename[MAXFILELEN+1];
	char filename_str[MAXFILESTRLEN + 1 ];
	char filename_dest[MAXFILESTRLEN + 1 ];
	char cmd_str[1024+1];
	char tmp_rm_str[1024+1];
	char rm_str[1024*10+1];
	
	FILE_ENTRY *entries;
	FILE_ENTRY *tmp_entries;
	FILE_LIST list;
	
	FILE_ENTRY *flag_entries;
	FILE_ENTRY *tmp_flag_entries;
	FILE_LIST flaglist;

	DictHandle *in  = NULL;

	memset(sInDir,0x00,sizeof(sInDir));
	memset(sOutDir,0x00,sizeof(sOutDir));
	memset(sIp,0x00,sizeof(sIp));
	memset(sPort,0x00,sizeof(sPort));
	memset(sEndflag,0x00,sizeof(sEndflag));
	memset(sFtsJob,0x00,sizeof(sFtsJob));
	memset(sTaskId,0x00,sizeof(sTaskId));
	memset(endfilename,0x00,sizeof(endfilename));
	memset(tmpendfilename,0x00,sizeof(tmpendfilename));
	memset(filename_str,0x00,sizeof(filename_str));
	memset(filename_dest,0x00,sizeof(filename_str));
	memset(&list,0x00,sizeof(FILE_LIST));
	memset(cmd_str,0x00,sizeof(cmd_str));
	memset(rm_str,0x00,sizeof(rm_str));
	memset(tmp_rm_str,0x00,sizeof(tmp_rm_str));

	if(!sIntag)
	{
		PERROR("参数检查失败");
		return -1;
	}
	/*获得字典句柄*/
	if((in = GetDictFromTable(GetGlobalTable(), sIntag)) < 0)
	{
		PERROR("convert  [%s] -> in   Error \n",sIntag);
		return -1;
	}

	/*读取配置文件*/
	conf=LoadConfigure("FtsCom.conf");
	if(conf  == 0)
	{
		PERROR("load FtsCom.conf failed.");
		return -1;
	}
	/*获取bafe定义任务别称*/	
	if(GetDictString(*in,"_FTS_JOB",sFtsJob,sizeof(sFtsJob)) == -1)
	{
		PERROR("获取传入字典中的任务别名失败");
		FreeConfigure(conf);
		return -1;		
	}	
    
	if(GetValue(conf,sFtsJob,"IN",sInDir,sizeof(sInDir))<0)
	{
		PERROR("获取配置文件中任务[%s]的源目录抱败",sFtsJob);
		FreeConfigure(conf);
		return -1;
	}

	PDEBUG("发起方文件存放目录[%s]",sInDir);
	
	if(GetValue(conf,sFtsJob,"OUT",sOutDir,sizeof(sOutDir))<0)
	{
		PERROR("获取配置文件中任务[%s]的目标目录失败",sFtsJob);
		FreeConfigure(conf);
		return -1;
	}
	
	PDEBUG("接收方文件存放目录[%s]",sOutDir);
	
	if(GetValue(conf,sFtsJob,"TASKID",sTaskId,sizeof(sTaskId))<0)
	{
		PERROR("获取配置文件中任务[%s]的TASKID失败",sFtsJob);
		FreeConfigure(conf);
		return -1;
	}
	
	PDEBUG("TASKID ->  [%s] ",sTaskId);
	
	if(GetValue(conf,"COMMONDATA","IP",sIp,sizeof(sIp))<0)
	{
		PERROR("获取配置文件中IP地址失败");
		FreeConfigure(conf);
		return -1;
	}
	 PDEBUG("IP ->  [%s] ",sIp);
    
	if(GetValue(conf,"COMMONDATA","PORT",sPort,sizeof(sPort))<0)
	{
		PERROR("获取配置文件中PORT端口失败");
		FreeConfigure(conf);
		return -1;
	}
	 port=atoi(sPort);
	 PDEBUG("PORT ->  [%d] ",port);

	if(GetValue(conf,sFtsJob,"ENDFLAG",sEndflag,sizeof(sEndflag))<0)
	{
		PERROR("获取配置文件中ENDFLAG失败");
		FreeConfigure(conf);
		return -1;
	}
	 PDEBUG("sFtsJob ->  [%s] ",sFtsJob);

	if(GetDictString(*in,"_FTS_FILENAME",filename_str,sizeof(filename_str))==-1)
	{
		PERROR("获取目标文件列表失败");
		FreeConfigure(conf);
		return -1;
	}
	filename_str[MAXFILESTRLEN]='\0';
	PDEBUG("_FTS_FILENAME ->  [%s] ",filename_str);
	
	i = 0;
	iLen = ParseFileList(filename_str,filename_dest,&i);
	/*设置结构体*/
	entries = (FILE_ENTRY *)malloc((i+1)*sizeof(FILE_ENTRY) );
	if(entries == NULL)
	{
		PERROR("malloc error [%s]",strerror(errno));
		entries = NULL;
		FreeConfigure(conf);
		return -1;	
	}
	tmp_entries = entries;	
	p = filename_dest;
	for(j=0;j<iLen;)
	{
		strncpy(tmp_entries->sname,p,MAXFILELEN);	
		strncpy(tmp_entries->dname,p,MAXFILELEN);	
		strncpy(tmp_entries->spath,sInDir,MAXDIRLEN);	
		strncpy(tmp_entries->dpath,sOutDir,MAXDIRLEN);
		j+=strlen(p)+1;	
		p+=strlen(p)+1;
		tmp_entries++;

	}
	list.count = i+1;
	list.entries = entries;
	iRet = submit( sTaskId, &list , sIp, port);
	if(!iRet )
	{
		/*发送标志文件*/
		/*判断flag是否需要标志文件*/
		if(flag ==1 )
		{
			PDEBUG("发送文件成功,开始发送标志文件");
			
			if(GetValue(conf,sFtsJob,"OUT_BZ",sOutDir,sizeof(sOutDir))<0)
        	{
                	PERROR("获取配置文件中任务[%s]的标志文件目标目录失败",sFtsJob);
                	system(rm_str); 	
					FreeConfigure(conf);
					free(entries);
					entries = NULL;
           		    return -2;
        	}
        	PDEBUG("接收方标志文件文件存放目录[%s]",sOutDir);
			
			flag_entries = (FILE_ENTRY *)malloc(list.count*sizeof(FILE_ENTRY) );
			if(flag_entries == NULL)
			{
				PERROR("malloc error [%s]",strerror(errno));
				flag_entries = NULL;
				FreeConfigure(conf);
				free(entries);
				entries = NULL;
				return -1;	
			}
			tmp_flag_entries = flag_entries;
			tmp_entries = entries;	 /*重置目标文件的结构体指针至首部*/
			strcat(rm_str,"rm -f");
			/*根据目标文件列表中的数据生成标志文件，并赋值给标志文件结构体*/
			for(j=0;j<list.count;j++)
			{
				memset(tmp_rm_str,0x00,sizeof(tmp_rm_str));
				memset(endfilename,0x00,sizeof(endfilename));
				memset(tmpendfilename,0x00,sizeof(tmpendfilename));
				
				strncpy(tmpendfilename,tmp_entries->sname,MAXFILELEN-4);
                		Dellastpoint(tmpendfilename);
				sprintf(endfilename,"%s.%s",tmpendfilename,sEndflag);
				sprintf(cmd_str,"touch %s/%s",sInDir,endfilename);
				sprintf(tmp_rm_str," %s/%s",sInDir,endfilename);
				strcat(	rm_str,tmp_rm_str);
				system(cmd_str);	
				
				strncpy(tmp_flag_entries->sname,endfilename,MAXFILELEN);	
				strncpy(tmp_flag_entries->dname,endfilename,MAXFILELEN);	
				strncpy(tmp_flag_entries->spath,sInDir,MAXDIRLEN);	
				strncpy(tmp_flag_entries->dpath,sOutDir,MAXDIRLEN);
				PDEBUG("标志文件名[%s]",tmp_flag_entries->sname);
				tmp_flag_entries++;
				tmp_entries++;
				
			}
			
			free(entries);
			entries = NULL;
    		list.entries = flag_entries;	
			iRet = submit( sTaskId, &list , sIp, port);
			if(!iRet)
			{
				PDEBUG("发送标志文件成功");
				free(flag_entries);
				flag_entries = NULL;
                system(rm_str); 	
			}
			else
			{
				free(flag_entries);
        		flag_entries = NULL;
        		PERROR("发送标志文件失败");
				PrintFtsErrorMsg(iRet);	
        		FreeConfigure(conf);
                system(rm_str); 	
        		return -2;	
			}
		}
		else if(flag == 0)	
		{
			PDEBUG("不需要发送标志文件");
			free(entries);
			entries = NULL;
		}
		else 
		{
			PERROR("标志文件选项不和法");
			free(entries);
			entries = NULL;
			FreeConfigure(conf);
			return -2;	
		}
		
		
	}
	else 
	{
		free(entries);
		entries = NULL;
		PERROR("发送目标文件失败");
		PrintFtsErrorMsg(iRet);	
		FreeConfigure(conf);
		return -1;
	}	
	FreeConfigure(conf);
	return 0;

}





/**********************************************************************
* 函数作用 : 获取发送实时任务时源文件存放目录 
* 函数名称 : int GetFtsFilepath( char *taskname, char *sOutFilePath )
* 参数说明 : 
*		char *taskname  对于每个发送任务，bafe使用的别名
*		char *sOutFilePath 通过配置文件找到的目录,根据CD系统提供的头文件 ,目录的最大大小为256
*       使用时，大小应该足够大
* 返回值说明 : 
*				0  成功
*				-1 失败
**********************************************************************/
int GetFtsFilepath( char *taskname, char *sOutFilePath )
{
	HANDLE config;
	char sSection[64+1];/*段标签    */
	char sFilePath[256+1];

	memset( sFilePath   ,0x00,sizeof( sFilePath    ));

	DEBUG("进入GetFtsFilepath函数");
	
	strncpy( sSection,taskname ,sizeof(sSection));
	sSection[64] ='\0';	
	config= LoadConfigure( "FtsCom.conf");
	if( !config)
	{
		ERROR("获取配置文件FtsCom.conf错误");
		return -1;
	}
	if( GetValue(config,sSection,"IN", sFilePath,sizeof( sFilePath )) < 0 )
	{
		ERROR("获取配置文件中配置参数错误[%s] : IN",sSection);
		FreeConfigure(config);
		return -1;
	}
	
	strcpy(sOutFilePath,sFilePath);
	FreeConfigure(config);

	DEBUG("退出GetFtsFilepath函数");

	return 0;
}



/******************************************************************
* Function Name: iRm_SendBancsFile                                *
* Description:   删除生成的上送bancs文件（无用的）                *
* In Params:     char    *sFilePath                               *
* Out Params:    char    *sValue                                  *
* Return:        0: 成功                                          *
*                -1: 失败                                         *
* explain:       删除生成的上送bancs文件（无用的）                *
******************************************************************/
int iRm_SendBancsFile( char *sFilePath )
{
        char    sStrCode[ 1024+1 ];           /* shell语句 */

        memset( sStrCode, 0x00, sizeof( sStrCode ) );

        sprintf( sStrCode, "rm %s", sFilePath );

        DEBUG( "删除生成的上送bancs文件的shell命令为：[%s]", sStrCode );

        if( system( sStrCode ) )
        {
                ERROR( "[%s]执行失败！", sStrCode );

                return -1;
        }

        return 0;
}





















