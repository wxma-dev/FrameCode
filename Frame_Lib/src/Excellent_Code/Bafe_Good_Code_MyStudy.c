/*************************************************
**���ú���27601���ף��ж������˺��Ƿ�Ϊһ��ͨ���˺�
**���룺DICTTAG InDict
**�����DICTTAG OutDict
**����ֵ��0  ��һ��ͨ���˻�
          1  ����
          -1 ʧ��
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

        /* ����BAFE��Χ��ˮ�� */
        if( iGetBafeckh( pTransInfo->qdbm, pTransInfo->wwxtgzh ) )
        {
                ERROR( "����bafe��Χ��ˮ��ʧ��!" );
		B3SetCode(OutDict,"XYM", "XYXX","BF0003");
                return -1;
        }
        /* ��ѯ�˻���Ϣ */
        SetDictTagString( InDict, "JYDM", "027601" );     /* ���Ľ����� */
        SetDictTagString( InDict, "MKBS", "1" );
        SetDictTagString( InDict, "JBS", "01" );
        SetDictTagString( InDict, "OFFSET", "029" );
        SetDictTagString( InDict, "JZHBZ", "1" );

        iRet = BeaiTransRecvMsg( "BANCS", InDict, OutDict, NULL );
        INFO( "Send to BANCS 027601 end. iRet=[%d]", iRet );
        if( iRet == -999 )
        {
                ERROR("027601��ѯ�˺���Ϣ��ʱ");
		B3SetCode(OutDict,"XYM", "XYXX","BF0002");
                return -1;
        }
        else if( iRet != 0 )
        {
                ERROR( "027601��ѯ�˺���Ϣʧ��" );
		B3SetCode(OutDict,"XYM", "XYXX","BF0001");
                return -1;
        }
        else
        {
                DEBUG( "027601��ѯ�ʺ���Ϣ���سɹ�" );
                GetDictTagString( OutDict, "XYM", sXym, sizeof( sXym ) );
                GetDictTagString( OutDict, "XYXX", sXyxx, sizeof( sXyxx ) );

                trim( sXym );
                trim( sXyxx );

                if( strcmp( sXym, "0000" ) )
                {
                        ERROR( "27601���ķ���ʧ�� sXym[%s], sXyxx[%s]", sXym, sXyxx );
                        return -1;
                }
		GetDictTagString( OutDict, "DQBS", sDqbs, sizeof(sDqbs));
		trim(sDqbs);
		if(sDqbs[0] == 'M')
		{
			DEBUG("027601��ѯ�ʺ�Ϊһ��ͨ���˺�");
			return 0;
		}else
		{
			DEBUG("027601��ѯ�ʺŲ�Ϊһ��ͨ���˺�");
			return 1;
		}	

        }
}

/************************************************************************
*	��������: GetBafeWjXh                                               *
*	��������: �����ļ����                                              *
*	�������:															*
*	�������: char *outWjxh                                            *
*	����ֵ���ɹ� 0��ʧ�� -1 											*
*	�� д ��: ����                                                     *
*************************************************************************/
int GetBafeWjXh(char *outWjxh)
{
	int iRet;
	char sWjxh[20];
	
	if(outWjxh==NULL)
	{
		PERROR("���ε�ַΪnull");
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

	/* ȡ����� */
	no = seq[inf->number].dqxh;
	seq[inf->number].dqxh++;

	/* �����ų������ֵ�������Ϊ1 */
	if(seq[inf->number].dqxh > inf->zdxh)
		seq[inf->number].dqxh = 1;

	/* �ж�����Ƿ��Ѿ����� */
	if(seq[inf->number].dqxh == seq[inf->number].db_dqxh)
		empty_flag = 1;

	SEM_SIGNAL_UNDO(sem_id, SEM_X(inf->number, SEQUENCE_MUTEX));

	/* �������Ѷ��գ�֪ͨ��������� */
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
*	��������: GetBeaiJylsh                                              *
*	��������: ������Χ��������ǰ�õ���ˮ˳���                          *
*	�������:															*
*	�������: char* outJylsh                                            *
*	����ֵ���ɹ� 0��ʧ�� -1 											*
*	�� д ��: zouql                                                     *
*************************************************************************/
extern TRANSINFO *pTransInfo;

int GetBeaiJylsh(char* outJylsh)
{
	int iRet;
	char sJylsh[20];
	
	if(outJylsh==NULL)
	{
		PERROR("���ε�ַΪnull");
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



/*��bafe��ˮ�����ȳ���Χϵͳ�ο���
����wwxtckh Ϊ�������ݵ�ָ��,Ӧ�õ���ʱ�豣ָ֤���С�㹻
��Σ�
     qdbm  ��Χϵͳ���������� ������2λ,������λ��ʱ��ȡǰ��λ

��������bafe��ϵͳ����,bafe��ҵ�����ں����(0001,00000)����wwxtgzh */
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
	
	PDEBUG("����iGetBafeckh");
	DEBUG("����iGetBafeckh");

	if(wwxtckh==NULL||qdbm == NULL)
        {
                PERROR("������ַΪnull");
                return -1;
        }
	/*��ȡ��ˮ��*/
	if( GetBeaiJylsh( sTmpxh) )
	{
		PERROR("��ȡBAFE��ˮ��ʧ��!"); 
		return -1;
	}
	PDEBUG("��ȡBAFE������ųɹ�[%s]",sTmpxh);
	lTmpxh=atol(sTmpxh);
	sprintf( sTmplsh, "%019ld", lTmpxh); 
	PDEBUG("��ȡbafe��ˮ�ųɹ�[%s]",sTmplsh);
	
	/*��ȡbafeҵ������*/
	
	memcpy(sTmprq,pTransInfo->jyrq,8);
#if 0
	if( GET_STRING_PARA( sTmprq,KJRQ ))
	{
		PERROR("ȡBAFEҵ�����ڴ���");     
		return -1;
	}
#endif
	PDEBUG("BAFEҵ������Ϊ[%s]",sTmprq);
	/*��ȡ��������BAFEXTBM */
	snprintf(sTmpxtbm,sizeof(sTmpxtbm),"%s",getenv("BAFEXTBM"));
	PDEBUG("BAFE ϵͳ����Ϊ[%s]",sTmpxtbm);
	if(strlen(sTmpxtbm) < 3)
	{
		PERROR("xtbm ȡֵ����ȷ");
		return -1;	
	}

	memcpy(wwxtckh, qdbm, 2);
	memcpy(wwxtckh+2, sTmpxtbm, 3);
	memcpy(wwxtckh+5, pTransInfo->ywlx, 2);
	memcpy(wwxtckh+7, sTmprq+2, 6);
	memcpy(wwxtckh+13, sTmplsh, 19); 
	
	PDEBUG("�뿪iGetBafeckh");
	DEBUG("�뿪iGetBafeckh");
	return 0;
}

/*�жϴ����˺�����*/

/********************************************************************
*	��������: Bafe_Zhlxzh()
*	��������: ���������˺��ж��˺�����
*	�������: �����ʺ� sZh (���20�˺�)
*	�������: 
*		  iFlag	 �˺����ͱ�־
*	���ز���˵��: iFlag==-1 ����δ֪
*		      iFlag==1  �����ڲ���
*		      iFlag==2  �º��Ŀ���
*                     iFlag==3  �º����˺�
*                     iFlag==4  �Ϻ����˺�
*                     iFlag==5  �Ϻ��Ŀ���
*	 ����ֵ˵��:  0 �ɹ�  -1 ʧ��
*	�� д ��: ����
*	��    ��: 2011/11/16
*   �޸��ܴ���:  ���Ƶ�0���޸�
*   ��0���޸�:
********************************************************************/
/* ���������ĵ��ṩ��ת������ 
	0. 20 λΪ�Ϻ����ʺţ���Ҫת��Ϊ���ʺ�)
	1. 19 λΪ��ϵͳ��ǿ�
	2. 18 λΪ�Ϻ����ʺţ���Ҫת��Ϊ���ʺ�)
	3. 17 λ����λΪ4,5,7,9Ϊ���ʺ�,9Ϊ�ڲ���
	4. 17 λ�Ҳ���������Χ�ڵ��ʺ�Ϊ�Ϻ����ʺ� (��Ҫת��Ϊ���ʺ�)
	5. 16 Ϊ�Ͽ��ţ����鿨bin���� (�Ϻ��Ŀ��Ŀ�binΪ622478  940013,���ÿ���binδ֪) ���ڿ�bin��Χ��ʱ������λ�Ƿ�Ϊ'9',��Ϊ9��Ϊ�ڲ��˺ţ������򱨷Ƿ�
	6. С��16 λ�ģ�Ϊ���˺ţ���Ҫ���������ʺű�
	7. ����20 λ��Ϊ����

*/
int Bafe_Zhlxzh(char *sZh,int *iFlag)
{
	int iZhLen =0 ;
	EXEC SQL BEGIN DECLARE SECTION;
	char zh[20+1];
	EXEC SQL END DECLARE SECTION;
	
	
	if(!sZh)
	{
		ERROR("�������ʧ��");
		return -1;
	}
	
	if(strlen(sZh) > 20)
	{
		ERROR("�����˺ŵĳ��ȴ���20λ,�������Ϸ�");
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
				/*DEBUG("�������ݿ���sqlcodeΪ%d",SQLCODE);*/
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
*	��������: Bafe_Zhlxpd()
*	��������: �������˺�ת�������ʺŲ��ж�����
*	�������: �����ʺ� sinZh 
*	�������: 
*		  return �˺����ͱ�־
*	���ز���˵��: soutXzh ���ʺ�
*	 ����ֵ˵��:
*		      return -1  ����
*		      return  0  ͬҵ
*		      return  1  �Թ�
*         return  2  ��˽
*         return  3  �ڲ�
*         return  4  δ֪
*	�� д ��: Ф��
*	��    ��: 2013/01/15
*   �޸��ܴ���:  ���Ƶ�0���޸�
*   ��0���޸�:
********************************************************************/
int Bafe_Zhlxpd(char *sinZh, char *soutXzh)
{
	int iZhLen = 0 ;
	int iRet = 0;
	char zh[18+1];
	char xzh[18+1];
	
	if(!sinZh)
	{
		ERROR("���Ϊ�գ��������ʧ��");
		return -1;
	}
	
	if(strlen(sinZh) > 18)
	{
		INFO("�����˺ŵĳ��ȴ���18λ,");
		return 4;
	}
		
	memset(zh,0x00,sizeof(zh));
	memset(xzh,0x00,sizeof(xzh));
	memcpy(zh,sinZh,strlen(sinZh)<18?strlen(sinZh):18);
	iZhLen = strlen(zh);
	
	if (iZhLen < 16)
	{
		INFO("�ʺ�С��16λ");
		iRet = DB_XJZHDZB_SELECT_XZH_BY_JZH(zh, xzh);
		if ( iRet == -1)
		{
			ERROR("�����¾��ʺŶ��ձ�ʧ��");
			memcpy(soutXzh, sinZh, strlen(sinZh));
			return -1;
		}
		else if ( iRet == 1 )
		{
			ERROR("�����¾��ʺŶ��ձ�δ�ҵ���¼");
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
		/*16λΪ�Ͽ���*/
		memcpy(soutXzh, sinZh, strlen(sinZh));
		if (zh[0] == '9')
			return 3;
		else
			return 4;
	}
	else if (iZhLen == 17)
	{
		/*��������ʺ�ֱ�ӷ��ؽ��*/
		if (zhjudge(zh) != 4)
		{
			memcpy(soutXzh, sinZh, strlen(sinZh));
			return zhjudge(zh);
		}
		/*����������ʺŲ����¾��ʺŶ��ձ�*/
		else
		{
			/*�����¾��ʺŶ��ձ�*/
			iRet = DB_XJZHDZB_SELECT_XZH_BY_JZH(zh, xzh);
			if ( iRet == -1)
			{
				ERROR("�����¾��ʺŶ��ձ�ʧ��");
				memcpy(soutXzh, sinZh, strlen(sinZh));
				return -1;
			}
			else if ( iRet == 1 )
			{
				ERROR("�����¾��ʺŶ��ձ�δ�ҵ���¼");
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
		/*18λΪ���ʺţ�ֱ�Ӳ�����ж�*/
		iRet = DB_XJZHDZB_SELECT_XZH_BY_JZH(zh, xzh);
		if ( iRet== -1)
		{
			ERROR("�����¾��ʺŶ��ձ�ʧ��");
			memcpy(soutXzh, sinZh, strlen(sinZh));
			return -1;
		}
		else if ( iRet == 1 )
		{
			ERROR("�����¾��ʺŶ��ձ�δ�ҵ���¼");
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
* ��������:DB_XJZHDZB_SELECT_XZH_BY_JZH
* 	��������:���ݾ��˺Ż�����˺�
*   �������: char *sInzh         ���˺�
*   �������: char *sOutzh��      ���˺�
*   ����ֵ:  0     �ɹ�
			 1     ���޼�¼ 
*  			-1     ʧ��
*   ��д����:20111202
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
		PERROR("�Ƿ��ʺ�,�ʺ�Ϊ��!") ;
		return -1 ;
	}
		
	strcpy( sJzh, sInzh );
	strpack(sJzh);
	/*�Ȳ�ѯ�Թ��¾��˺Ŷ��ձ�*/
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

                PERROR("�����ʺ�[%s]��ѯt_pub_xjzhdzb����[%d]",sJzh,SQLCODE );
                ERROR("�����ʺ�[%s]��ѯt_pub_xjzhdzb����[%d]",sJzh,SQLCODE );
                return -1;
        }
	/*���Թ��¾��˺Ŷ��ձ���δ�鵽���ٴӶ�˽�¾��˺Ŷ��ձ��в�ѯ*/
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
		PERROR("�����ʺ�[%s]��ѯt_pub_xjzhdzb����[%d]",sJzh,SQLCODE );
		ERROR("�����ʺ�[%s]��ѯt_pub_xjzhdzb����[%d]",sJzh,SQLCODE );
		return -1;
	}

}


/********************************************************************
*       ��������: iBafeCheckDate()
*       ��������: У���ļ����������Ƿ���ҵ������һ��
*       �������: 
*           filename �ļ���������·����
*           offset   ����λƫ����
*           wwxtbm   ��Χϵͳ����
*       �������:
*       ���ز���˵��:
*       ����ֵ˵��: 
*           return -1 ʧ��
*           return  0 ����У��һ�»��߲���У��
*           return  1 ����У�鲻һ��
*       ��д��: Ф��
*       ��  ��: 2013/01/21
*   �޸��ܴ���:  ���Ƶ�0���޸�
*   ��0���޸�:
********************************************************************/
int iBafeCheckDate(char *filename, int offset, char *wwxtbm)
{
	char CheckBz[12];
	int  iRet = -1;
	char sSection[3+1]; /*�α�ǩ*/
	char sRq[8+1];      /*����*/

	memset(sSection, 0x00, sizeof(sSection));
	memset(sRq, 0x00, sizeof(sRq));
	memset(CheckBz, 0x00, sizeof(CheckBz));

	if ( strlen(wwxtbm) != 3 )
	{
		ERROR("���棺��Χϵͳ���벻Ϊ3λ");
	}
	memcpy(sSection, wwxtbm, 3);
	iRet = Get_paramvalue("FILEDATE_CHK", sSection, CheckBz);
	if ( iRet < 0 )
	{
		ERROR("��ȡ�����ļ�ParamValues.confʧ��");
		return -1;
	}
	if ( CheckBz[0] != 'N' )
	{
		strncpy(sRq, filename+offset, 8);
		if (!strncmp(pTransInfo->jyrq, sRq, 8))
		{
			INFO("����У��һ�£��ļ�����[%s]����Χϵͳ���룺[%s]", filename, sSection);
			return 0;
		}
		else
		{
			INFO("����У�鲻һ�£��ļ�����[%s]����Χϵͳ���룺[%s]", filename, sSection);
			return 1;
		}
	}
	else
	{
		INFO("����У�飬�ļ�����[%s]����Χϵͳ���룺[%s]", filename, sSection);
		return 0;
	}
}


/****************************************
*inbuf:������
*infmt:�����bancs������ͣ�S9(14)V999,9(14)V999S ��
*outfmt:���������ͣ�����
      S14.2(��������С������ʽ��'.'���ҷֱ��ʾ������С��λ����)
      S14+2(��������С������ʽ��'+'���ҷֱ��ʾ������С��λ����)
      14.2(S14.2���޷�����ʽ),14+2(S14+2���޷�����ʽ)
*outbuf:ת����Ľ��
*�ɹ�����0 ,ʧ�ܷ���-1
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
			ERROR("bancs ������λ�Ƿ�[%s][%s]", intmp, fmttmp);
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
			ERROR("bancs ������λ�Ƿ�[%s][%s]", intmp, fmttmp);
			return -1;
		}
	}else
	{
		if(*p == '-' || *p == '+' 
	          || intmp[strlen(intmp)-1]=='-' || intmp[strlen(intmp)-1]=='+')
		{
			ERROR("bancs ������λ�Ƿ�[%s][%s]", intmp, fmttmp);
			return -1;
		}
	}
	pt = strchr(q,'V');
	if(!pt)
	{
		ERROR("����ʽ�Ƿ�[%s]", fmttmp);	
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
			ERROR("����С��λ����ʡ��[%s][%s]", sXS, outfmt);
			return -1;
		}
		sprintf(tmpbuf+outOffSet,"%s%0*s", DelLeftZero(sZS), iXS, DelRightZero(sXS));
		strcpy(outbuf, tmpbuf);
		return 0; 
	}
	if(atoi(sXS)>0)
	{
		ERROR("[%s]С��λ��������", inbuf);
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

/***���Ƿ��ַ��ж�
    inbuf �������
    point С�����־��0-����С���� 1-��С����
    flag  �����ű�־��0-�������ţ�1-��λ��'+'��'-'
����0-9�������ַ���piontΪ1ʱ��С����(���һ��)��flagΪ1ʱ����λ�����ţ�������Ϊ�Ƿ��ַ�
����ֵ��0--ͨ�� ����--�Ƿ�
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
*	��������: get_xnjgczy()   			                               *
*	��������: �����������Ա���ձ����                                       *
*	�������: sInFhdm  --���д���	 	                               *
*			  sInXtbm -- ϵͳ����                                      *
*	�������: 				                                           *
*             sOutXnjg  -- �������                                    *
*			  sOutXnczy --�������Ա                                   *
*	����ֵ��0--�ɹ���-1 --ʧ��										   *	
*	�� д ��: ufc                                                      *
*	��    ��: 2008/11/10                                               *
*   �޸��ܴ���:  ����1���޸�	                                       *
*   ��1���޸�:   2009-1-13                                             *
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
        
	PINFO("����get_xnjgczy����");
	if( sInXtbm == NULL )
	{
		ERROR("������ʶ����Ϊ��!");
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
		ERROR("��ȡ�����������Աʧ�� SQLCODE[%d] fhdm[%s] xtbm[%s]",SQLCODE, sFhdm, sXtbm);		

		return -1;
	}	

	strcpy( sOutXnjg, sXnjg );
	strcpy( sOutXnczy, sXngy );
	
	PINFO("�뿪get_xnjgczy����");
	return 0;
}


/***********************************************************
* ��������:Get_paramvalue
* ����˵��:��ȡ�����ļ�ParamValues.conf��ָ��������ֵ
* �������:
*       char *sInSection  �ֶα�ǩ˵��
*       char *sInTag      ������ǩ˵��
* �������:
*       Char *sOutValue  �������ֵ  
* ����ʱ��:20090403
************************************************************/
int Get_paramvalue( char *sInSection,char* sInTag, char *sOutValue )
{
	char sConFileName[50+1];/*�����ļ���*/
	char sConTag[25+1];/*���ñ�ǩ  */
	char sSection[16+1];/*�α�ǩ    */
	char sTags[20+1];/*�����ǩ  */
	char sValue[100+1];

	memset( sConFileName,0x00,sizeof( sConFileName ));
	memset( sConTag     ,0x00,sizeof( sConTag));
	memset( sSection    ,0x00,sizeof( sSection     ));
	memset( sTags       ,0x00,sizeof( sTags        ));
	memset( sValue      ,0x00,sizeof( sValue    ));

	DEBUG("����Get_paramvalue����");
	
	strncpy( sSection,sInSection, 16);
	strcpy( sTags,sInTag        );
	strcpy( sConFileName,"ParamValues.conf" );
	g_hdlCfg = LoadConfigure( sConFileName );
	if( !g_hdlCfg )
	{
		ERROR("��ȡ�����ļ�[%s]����",sConFileName );
		return -1;
	}
	if( GetValue(g_hdlCfg,sSection,sTags, sValue,sizeof( sValue )) < 0 )
	{
		ERROR("��ȡ�����ļ������ò�������[%s][%s]",sSection,sTags );
		FreeConfigure(g_hdlCfg);
		return -1;
	}
	strcpy( sOutValue,sValue );
	FreeConfigure(g_hdlCfg);

	DEBUG("�˳�Get_paramvalue����");

	return 0;
}




int processErrFile(FILE* fpFile,char *sInPlWjm,char *firstLine, char *sHxxym,char *sHxxyxx,int flag,char *sBuffer)
{

	EXEC SQL BEGIN DECLARE SECTION;
		char    sWjm[ 200+1 ];             /* �ļ��� */
	EXEC SQL END DECLARE SECTION;

	int 	  iRet;
	char    sNewPath[ 512+1 ];            /* ·�� */
	char    sRetFileName[ 512+1 ];        /* �����м�ҵ���ļ��� */
	char    sRetFullFileName[ 512+1 ];    /* �����м�ҵ���ļ�ȫ�� */
	char    sRetInbuFileName[ 512+1 ];    /* �����м�ҵ���ļ��� */
	char    sWwXym[ 4+1 ];                /* ��Χ��Ӧ�� */
	char    sWxXyxx[ 60+1 ];              /* ��Χ��Ӧ��Ϣ */
	char    sTmpMx1[ 500+1 ];
	char    sTmpMx2[ 500+1 ];
	char    sCdTagName[ 50+1 ];           /* FtsCom.conf��ǩ�� */
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

	INFO( "�����黧����ʧ�ܣ�д�����ļ� sHxxym[%s], sHxxyxx[%s]", sHxxym, sHxxyxx );
	strcpy( sWjm, sInPlWjm );

	/* ���⼯��֧�����ӻ�ϵͳ */
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
		ERROR( "ȡ�����ļ�����" );
		return -1;
	}
	sprintf( sRetFullFileName, "%s/%s", sNewPath, sRetInbuFileName );

	DEBUG( "sRetFileName[%s],sRetInbuFileName[%s], sNewPath[%s]", sRetFileName, sRetInbuFileName, sNewPath );

	if( ( fpRetFile = fopen( sRetFullFileName, "w+" ) ) == NULL)
	{
		ERROR( "���м�ҵ�񷵻��ļ�ʧ��[%s]", sRetFullFileName );
		return -1;
	}

	DEBUG( "firstLine=[%s]", firstLine );

	/* ���ļ�ͷ�н�ȡ���ɹ�������ɹ�����ǰ��λ�� */
	strncpy( sLineStr, firstLine, 57 );
	strncpy( sLineStr1, firstLine+80, strlen(firstLine)-80 );
	DEBUG( "sLineStr=[%s]", sLineStr );
	DEBUG( "sLineStr1=[%s]", sLineStr1 );

	if( flag==4 )
	{
		DEBUG("���������ļ�ֱ��ʧ��Ҳ��Ҫ���سɹ�������ɹ���������̨�޷����� flag=[%d]", flag);
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
		strcpy( sWxXyxx, "��������" );
	}
	DEBUG( "�����м�ҵ�������Ϣ[%s][%s]", sWwXym, sHxxyxx );

	if( strlen(sHxxyxx) == 0 )
	{
		strcpy( sHxxyxx, sWxXyxx );
	}

	memset( sHxxym, 0x00, sizeof( sHxxym ) );
	strcpy( sHxxym, sWwXym );

	/* ��ʼд�ļ��� */
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
			case 1: /*�����黧*/
				strncpy( sTmpMx1, sTrLine, 118 );
				strncpy( sTmpMx2, sTrLine+strlen(sTrLine)-132, 132 );
				fprintf( fpRetFile, "%-118.118s%-4.4s%-60.60s%-100.100s\n",sTmpMx1, sHxxym, sHxxyxx, sTmpMx2 );
				break;
			case 2:/*���� */
				strncpy( sTmpMx1, sTrLine, 296 );
				strncpy( sTmpMx2, sTrLine+strlen( sTrLine )-275, 275 );
				fprintf( fpRetFile, "%-296.296sFFF00000%-4.4s%-60.60s%-275.275s\n",sTmpMx1, sHxxym, sHxxyxx, sTmpMx2 );
				break;
			case 3:/* ����ת�� */
				strncpy( sTmpMx1, sTrLine, 282 );
				strncpy( sTmpMx2, sTrLine+strlen( sTrLine )-100, 100 );
				fprintf( fpRetFile, "%-282.282s%-4.4s%-60.60s%015s%8.8s%09.9s%-100.100s\n",sTmpMx1, sHxxym, sHxxyxx, "0", pTransInfo->jyrq, "0", sTmpMx2 );
				break;
			case 4:/*���մ��� */
				strncpy( sTmpMx1, sTrLine, 178 );
				strncpy( sTmpMx2, sTrLine+strlen( sTrLine )-100, 100 );
				fprintf( fpRetFile, "%-178.178s%-4.4s%-60.60s%015s%8.8s%09.9s%-100.100s\n",sTmpMx1, sHxxym, sHxxyxx, "0", pTransInfo->jyrq, "0", sTmpMx2 );
				break;
			case 5:/* �黧���� */
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

	/* ����CDʵʱ�ļ�����ӿ� */
	iRet = iBafeSendMsgToFts( "InbuCDSendTag", 1 );
	DEBUG( "����cd����ʵʱ�ļ�����ֵ[%d]", iRet );

	if( iRet == -1 )
	{
		ERROR( "�ļ�����ʧ��!" );
		return -1;
	}
	else if( iRet == -2 )
	{
		ERROR( "��־�ļ�����ʧ��" );
		return -1;
	}

	EXEC SQL COMMIT WORK;
	INFO( "��������ʧ�ܣ����ؽ���ļ� sRetInbuFileName[%s]sHxxym[%s], sHxxyxx[%s]",
		sRetInbuFileName, sHxxym, sHxxyxx );
	return 0;
}



/**********************************************************************************
*  ������:   iBafeSendMsgToFts
*  ��������: ����CDƽ̨ʵʱ����ʼ����
*  ��������: 20111214
*  ������  : ����
*  ����˵��: DICTTAG sIntag ������ҪҪ�ص��ֵ��ǩ
*            int flag        �Ƿ���Ҫ���ͱ�־�ļ� 0 �� 1  ��Ҫ ���� �Ƿ�
* ����ֵ˵��: 0 �ɹ� -1 ʧ��  -2 ���ͱ�־�ļ�ʧ��
***********************************************************************************/
int iBafeSendMsgToFts(DICTTAG sIntag,int flag)
{
	
	int i;
	int j;
	int iLen;
	int iRet;
/*	int port = 32792;*/ /*Ĭ�϶˿�*/
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
		PERROR("�������ʧ��");
		return -1;
	}
	/*����ֵ���*/
	if((in = GetDictFromTable(GetGlobalTable(), sIntag)) < 0)
	{
		PERROR("convert  [%s] -> in   Error \n",sIntag);
		return -1;
	}

	/*��ȡ�����ļ�*/
	conf=LoadConfigure("FtsCom.conf");
	if(conf  == 0)
	{
		PERROR("load FtsCom.conf failed.");
		return -1;
	}
	/*��ȡbafe����������*/	
	if(GetDictString(*in,"_FTS_JOB",sFtsJob,sizeof(sFtsJob)) == -1)
	{
		PERROR("��ȡ�����ֵ��е��������ʧ��");
		FreeConfigure(conf);
		return -1;		
	}	
    
	if(GetValue(conf,sFtsJob,"IN",sInDir,sizeof(sInDir))<0)
	{
		PERROR("��ȡ�����ļ�������[%s]��ԴĿ¼����",sFtsJob);
		FreeConfigure(conf);
		return -1;
	}

	PDEBUG("�����ļ����Ŀ¼[%s]",sInDir);
	
	if(GetValue(conf,sFtsJob,"OUT",sOutDir,sizeof(sOutDir))<0)
	{
		PERROR("��ȡ�����ļ�������[%s]��Ŀ��Ŀ¼ʧ��",sFtsJob);
		FreeConfigure(conf);
		return -1;
	}
	
	PDEBUG("���շ��ļ����Ŀ¼[%s]",sOutDir);
	
	if(GetValue(conf,sFtsJob,"TASKID",sTaskId,sizeof(sTaskId))<0)
	{
		PERROR("��ȡ�����ļ�������[%s]��TASKIDʧ��",sFtsJob);
		FreeConfigure(conf);
		return -1;
	}
	
	PDEBUG("TASKID ->  [%s] ",sTaskId);
	
	if(GetValue(conf,"COMMONDATA","IP",sIp,sizeof(sIp))<0)
	{
		PERROR("��ȡ�����ļ���IP��ַʧ��");
		FreeConfigure(conf);
		return -1;
	}
	 PDEBUG("IP ->  [%s] ",sIp);
    
	if(GetValue(conf,"COMMONDATA","PORT",sPort,sizeof(sPort))<0)
	{
		PERROR("��ȡ�����ļ���PORT�˿�ʧ��");
		FreeConfigure(conf);
		return -1;
	}
	 port=atoi(sPort);
	 PDEBUG("PORT ->  [%d] ",port);

	if(GetValue(conf,sFtsJob,"ENDFLAG",sEndflag,sizeof(sEndflag))<0)
	{
		PERROR("��ȡ�����ļ���ENDFLAGʧ��");
		FreeConfigure(conf);
		return -1;
	}
	 PDEBUG("sFtsJob ->  [%s] ",sFtsJob);

	if(GetDictString(*in,"_FTS_FILENAME",filename_str,sizeof(filename_str))==-1)
	{
		PERROR("��ȡĿ���ļ��б�ʧ��");
		FreeConfigure(conf);
		return -1;
	}
	filename_str[MAXFILESTRLEN]='\0';
	PDEBUG("_FTS_FILENAME ->  [%s] ",filename_str);
	
	i = 0;
	iLen = ParseFileList(filename_str,filename_dest,&i);
	/*���ýṹ��*/
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
		/*���ͱ�־�ļ�*/
		/*�ж�flag�Ƿ���Ҫ��־�ļ�*/
		if(flag ==1 )
		{
			PDEBUG("�����ļ��ɹ�,��ʼ���ͱ�־�ļ�");
			
			if(GetValue(conf,sFtsJob,"OUT_BZ",sOutDir,sizeof(sOutDir))<0)
        	{
                	PERROR("��ȡ�����ļ�������[%s]�ı�־�ļ�Ŀ��Ŀ¼ʧ��",sFtsJob);
                	system(rm_str); 	
					FreeConfigure(conf);
					free(entries);
					entries = NULL;
           		    return -2;
        	}
        	PDEBUG("���շ���־�ļ��ļ����Ŀ¼[%s]",sOutDir);
			
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
			tmp_entries = entries;	 /*����Ŀ���ļ��Ľṹ��ָ�����ײ�*/
			strcat(rm_str,"rm -f");
			/*����Ŀ���ļ��б��е��������ɱ�־�ļ�������ֵ����־�ļ��ṹ��*/
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
				PDEBUG("��־�ļ���[%s]",tmp_flag_entries->sname);
				tmp_flag_entries++;
				tmp_entries++;
				
			}
			
			free(entries);
			entries = NULL;
    		list.entries = flag_entries;	
			iRet = submit( sTaskId, &list , sIp, port);
			if(!iRet)
			{
				PDEBUG("���ͱ�־�ļ��ɹ�");
				free(flag_entries);
				flag_entries = NULL;
                system(rm_str); 	
			}
			else
			{
				free(flag_entries);
        		flag_entries = NULL;
        		PERROR("���ͱ�־�ļ�ʧ��");
				PrintFtsErrorMsg(iRet);	
        		FreeConfigure(conf);
                system(rm_str); 	
        		return -2;	
			}
		}
		else if(flag == 0)	
		{
			PDEBUG("����Ҫ���ͱ�־�ļ�");
			free(entries);
			entries = NULL;
		}
		else 
		{
			PERROR("��־�ļ�ѡ��ͷ�");
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
		PERROR("����Ŀ���ļ�ʧ��");
		PrintFtsErrorMsg(iRet);	
		FreeConfigure(conf);
		return -1;
	}	
	FreeConfigure(conf);
	return 0;

}





/**********************************************************************
* �������� : ��ȡ����ʵʱ����ʱԴ�ļ����Ŀ¼ 
* �������� : int GetFtsFilepath( char *taskname, char *sOutFilePath )
* ����˵�� : 
*		char *taskname  ����ÿ����������bafeʹ�õı���
*		char *sOutFilePath ͨ�������ļ��ҵ���Ŀ¼,����CDϵͳ�ṩ��ͷ�ļ� ,Ŀ¼������СΪ256
*       ʹ��ʱ����СӦ���㹻��
* ����ֵ˵�� : 
*				0  �ɹ�
*				-1 ʧ��
**********************************************************************/
int GetFtsFilepath( char *taskname, char *sOutFilePath )
{
	HANDLE config;
	char sSection[64+1];/*�α�ǩ    */
	char sFilePath[256+1];

	memset( sFilePath   ,0x00,sizeof( sFilePath    ));

	DEBUG("����GetFtsFilepath����");
	
	strncpy( sSection,taskname ,sizeof(sSection));
	sSection[64] ='\0';	
	config= LoadConfigure( "FtsCom.conf");
	if( !config)
	{
		ERROR("��ȡ�����ļ�FtsCom.conf����");
		return -1;
	}
	if( GetValue(config,sSection,"IN", sFilePath,sizeof( sFilePath )) < 0 )
	{
		ERROR("��ȡ�����ļ������ò�������[%s] : IN",sSection);
		FreeConfigure(config);
		return -1;
	}
	
	strcpy(sOutFilePath,sFilePath);
	FreeConfigure(config);

	DEBUG("�˳�GetFtsFilepath����");

	return 0;
}



/******************************************************************
* Function Name: iRm_SendBancsFile                                *
* Description:   ɾ�����ɵ�����bancs�ļ������õģ�                *
* In Params:     char    *sFilePath                               *
* Out Params:    char    *sValue                                  *
* Return:        0: �ɹ�                                          *
*                -1: ʧ��                                         *
* explain:       ɾ�����ɵ�����bancs�ļ������õģ�                *
******************************************************************/
int iRm_SendBancsFile( char *sFilePath )
{
        char    sStrCode[ 1024+1 ];           /* shell��� */

        memset( sStrCode, 0x00, sizeof( sStrCode ) );

        sprintf( sStrCode, "rm %s", sFilePath );

        DEBUG( "ɾ�����ɵ�����bancs�ļ���shell����Ϊ��[%s]", sStrCode );

        if( system( sStrCode ) )
        {
                ERROR( "[%s]ִ��ʧ�ܣ�", sStrCode );

                return -1;
        }

        return 0;
}





















