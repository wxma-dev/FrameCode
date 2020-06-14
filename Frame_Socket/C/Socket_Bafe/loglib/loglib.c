/**************************************************************** 
* Copyright (c) 2008 ��������ͨ�Ƽ����޹�˾						*
* Source File:		loglib.c 									*
* Description:		��־����ĺ�����			  				*
* Author:			��ٻ��										*
* Create Date:		2008-6-25									*
*---------------------------------------------------------------*
*Modify Date:													*
*Modifier:														*
*Modify Note:													*
****************************************************************/
#include "Plat_Pub.h"
#include <sys/shm.h>
#include <stdio.h>

ST_HASHTABLE *pstHashTable;

long g_lSessionID = 0;

/*���ϵͳ����yyyymmdd��ʱ��hhmmss*/
int   GetSysdateAndtime6L (char*  outDate,char*  outTime)  
{   
	time_t tnum;
	struct tm *ts;
	
     memset(outDate,0x00,sizeof(outDate));
     memset(outTime,0x00,sizeof(outTime));
     
	time( &tnum );
	ts=localtime( &tnum );
    
    sprintf( outDate,"%04d%02d%02d",ts->tm_year+1900,
                                    ts->tm_mon+1,
                                    ts->tm_mday );
                                    
	sprintf( outTime,"%02d%02d%02d",ts->tm_hour,
                                      ts->tm_min,
                                      ts->tm_sec );                               
	return  0;
}

/*���ϵͳ����yyyymmdd��ʱ��hh:mm:ss*/
int   GetSysdateAndtime (char*  outDate,char*  outTime)  
{   
	time_t tnum;
	struct tm *ts;
	
     memset(outDate,0x00,sizeof(outDate));
     memset(outTime,0x00,sizeof(outTime));
     
    time( &tnum );
    ts=localtime( &tnum );
    
    sprintf( outDate,"%04d%02d%02d",ts->tm_year+1900,
                                    ts->tm_mon+1,
                                    ts->tm_mday );
                                    
	sprintf( outTime,"%02d:%02d:%02d",ts->tm_hour,
                                      ts->tm_min,
                                      ts->tm_sec );                               
	return  0;
}
/**��ȡ���� ʱ��  ��΢���ʱ��**/
static int  iGetSysDateTimeMsTime(char *outDate,char *outTime,char *outMsTime)
{
        struct  tm      *ts;
        struct timeval   tmst;
 
        gettimeofday( &tmst,NULL );
        ts = localtime( &tmst.tv_sec );
        sprintf( outDate, "%04d%02d%02d",ts->tm_year+1900,ts->tm_mon+1,ts->tm_mday);
        sprintf( outTime, "%02d:%02d:%02d",ts->tm_hour, ts->tm_min, ts->tm_sec);
        sprintf( outMsTime, "%02d:%02d:%02d:%03d",ts->tm_hour, ts->tm_min, ts->tm_sec,tmst.tv_usec / 1000);
 
        return 0;
 
}

/********************************************************************
* Function��														*
*	char  *GetSystime () 											*
* Description��														*
* 	���ϵͳʱ��													*
* In Params:														*
*	��																*
* Out Params:														*
*	��																*
* Return:															*
* ��ϵͳʱ��														*
********************************************************************/
char  *GetSystime()  
{   
	time_t tnum;
	struct tm *ts;
	static char caTime[8+1];
	
	memset(caTime,0x00,sizeof(caTime));
    time( &tnum );
    ts=localtime( &tnum );
    sprintf( caTime,"%02d:%02d:%02d",ts->tm_hour,
                                      ts->tm_min,
                                      ts->tm_sec );
    
	return  caTime;
}

char  *GetSystime6L()  /*6λ��*/
{   
	time_t tnum;
	struct tm *ts;
	static char caTime[6+1];
	
	memset(caTime,0x00,sizeof(caTime));
    time( &tnum );
    ts=localtime( &tnum );
    sprintf( caTime,"%02d%02d%02d",ts->tm_hour,
                                      ts->tm_min,
                                      ts->tm_sec );
    
	return  caTime;
}

/************************************************************************  
* Function��															* 
*	char  *GetSysdate () 	  											* 
* Description��															* 
* 	���ϵͳ����														* 
* In Params:															* 
*	 ��																	*	
* Out Params:															* 
*	��																	* 
* Return:																* 
* 	ϵͳ����									 						* 
*************************************************************************/

char  *GetSysdate ()  /*���ϵͳ����:yyyymmdd*/
{   
	time_t tnum;
	struct tm *ts;
	static char  caDate[8+1];  
	
    memset(caDate,0x00,sizeof(caDate));
    
    time( &tnum );
    ts=localtime( &tnum );    
    
    sprintf( caDate,"%04d%02d%02d",ts->tm_year+1900,
                                    ts->tm_mon+1,
                                    ts->tm_mday );
	return  caDate;
}
/********************************************************************
* Function��														*
*	void WriteFiles (FileForOut,ifHead,sFile , iLine,fmt, va_alist )*
* Description��														*
* 	ֱ��д�ļ��ķ�ʽ������־,Ҳ�����ڼ�¼��־��������е���Ϣ		*
* In Params:														*
*	FileForOut,ifHead,sFile , iLine,fmt, va_alist					*
* Out Params:														*
*	��																*
* Return:															*
* ����																*
********************************************************************/
void WriteFiles (FileForOut,ifHead,sFile , iLine,fmt, va_alist ) 
char* FileForOut;
int		ifHead;
char* sFile ;
int   iLine;
const char *fmt;
va_dcl 
{
	FILE *pFile;
	char caFilename[100];
	va_list args;
	char caTime[20+1];
	char  caStr[LOGCONTSIZ];

	memset(caStr,0x00,sizeof( caStr));
	memset(caFilename,0x00,sizeof( caFilename));	 
	
	memset(caTime,0x00,sizeof(caTime));
	strcpy(caTime,GetSystime());

	va_start(args);
	vsnprintf(caStr,sizeof(caStr),fmt,args);
	va_end(args);	
	
	sprintf(caFilename,"%s/%s",getenv("LOGDIR"),FileForOut);
	
	pFile =fopen(caFilename ,"a+" );
	
	if(ifHead==0)				/*������־ͷ,���ӻ��еĴ���*/
	{
		fprintf(pFile,"%s",caStr );
	}
	else if(ifHead==1)	/*����־ͷ,�ӻ���*/
	{	
		fprintf(pFile,"[%s] [%s] [line:%d] [pid:%d] %s\n\n",
			caTime,sFile , iLine,getpid(),caStr );
	}
	
	fclose(pFile );	
}	

/********************************************************************
* Function��														*
*	void WriteLogs (sFile,iLine,fmt, va_alist) 						*
* Description��														*
* 	��¼��־��������е���Ϣ										*
* In Params:														*
*	sFile,iLine,fmt�� va_alist���ɱ����							*
* Out Params:														*
*	��																*
* Return:															*
* ����																*
********************************************************************/
void WriteLogs (sFile , iLine,fmt, va_alist ) 
char* sFile ;
int  iLine;
const char *fmt;
va_dcl 
{
	char  caStr[1000];
	FILE *pFile;
	char caFilename[100];
	va_list args;
	char caTime[20+1];


	memset(caStr,0x00,sizeof( caStr));
	memset(caFilename,0x00,sizeof( caFilename));	 

	memset(caTime,0x00,sizeof(caTime));
	strcpy(caTime,GetSystime());

	va_start(args);
	vsnprintf(caStr,sizeof(caStr),fmt,args);
	va_end(args);	
	
	sprintf(caFilename,"%s/log.err",getenv("LOGDIR"));
	
	pFile =fopen(caFilename ,"a+" );
	fprintf(pFile,"[%s] [%s] [line:%d] [pid:%d] %s\n\n",
		caTime,sFile , iLine,getpid(),caStr );
	fclose(pFile );
	
}	

/*******************************************************************
* Function��														*
*	void DestroyLogmsg(int iMsgid)									*
* Description��														*
* 	������Ϣ����													*
* In Params:														*
*	int iMsgid														*
* Out Params:														*
*	��																*
* Return:															*
* ����																*
*********************************************************************/
void DestroyLogmsg(int iMsgid)
{
	if(msgctl(iMsgid ,IPC_RMID , &g_stMsgBuff))
		LOGERR("������Ϣ���г���");
	else LOGERR("[suc]������Ϣ����");
}

/********************************************************************
* Function��														*
*	int  CreatInitLogmsg(key_t  ipckey)								*
* Description��														*
* 	������������Ϣ����												*
* In Params:														*
*	key_t  ipckey													*
* Out Params:														*
*	��																*
* Return:															*
* ��iMsgid :��Ϣ���е�id��		��									*	
*	-1    ��ʧ��													*
********************************************************************/
int  CreatInitLogmsg(key_t  ipckey)
{
	static int iMsgid;
	int iFlag;   
	/*struct msqid_ds g_stMsgBuff;*/

	/*������Ϣ����*/
	iFlag= 0664|IPC_CREAT|IPC_EXCL;  
	if(-1==( iMsgid =msgget(ipckey,iFlag)))
	{
		LOGERR("������Ϣ����ʧ��");
		return  -1;	
	}	

	/*������Ϣ����*/		
	if(msgctl(iMsgid,IPC_STAT,&g_stMsgBuff))
	{
		LOGERR("������Ϣ����ʧ�ܡ�IPC_STAT��");
		DestroyLogmsg(iMsgid);
		return -1;
	}
	
	g_stMsgBuff.msg_qbytes= LOGMSGSIZE; /***/
	
	if(msgctl(iMsgid,IPC_SET,&g_stMsgBuff))
	{
		LOGERR("������Ϣ����ʧ�ܡ�IPC_SET����%s",strerror(errno));
		DestroyLogmsg(iMsgid);
		return -1;
	}	
	return  iMsgid;	
}

/************************************************************************
* Description��										                    *
* 	��־ָ��꣩�Ĵ���							                    *
* In Params:										                    *
*	pFile , iLine, iLxFlag ,  iLoglevel,  pFlag, fmt, va_alist          * 
* Out Params:												            *
*	��														            *
* Return:													            *
* 	0	  ���ɹ�											            *	
*	-1    ��ʧ��											            *
*************************************************************************/
int  PltTrcAppLog (pFile , iLine, cFlag ,  iLoglevel,  pFlag, fmt, va_alist)
char *pFile ;   
int  iLine;     
char  cFlag;
int  iLoglevel; 
char *pFlag;    
char *fmt;
va_dcl
{	
	if(iLoglevel <= g_iLogLevel )
	{    

		va_list args;
		char caStrHead[200];	
		int iTmpMsgid;
		int  iTmpLen=0;
/*  �޸�Ϊchar��������
		int gMsTime[12+1];
*/
		char gMsTime[12+1];
/****����ע���˴�**********
		memset(gcaContent,0x00,sizeof(gcaContent));
		memset(gcaStrbody,0x00,sizeof(gcaStrbody));
****����ע���˴�***********/	
	
		iGetSysDateTimeMsTime(gsDate,gsTime,gMsTime); /**��������ʱ��**/
		memset(caStrHead,' ',80);
		caStrHead[80]='\0';		
	

		/*��־��*/
		va_start( args );
		vsnprintf(gcaStrbody,sizeof(gcaStrbody),fmt,args);
		va_end( args);	   

		
		/*��־����="��־ͷ���ȣ�3λ��"+"��־ͷ"+"��־��"*/
		switch(cFlag)  
		{
			case  PLT_FLAG:     /*pltlog */
				sprintf( caStrHead,"[%6s] %s %s:%d PID:%d SID:%ld",
 					g_caSysJym,gMsTime,  pFile,  iLine,  getpid(), g_lSessionID); 
 				iTmpMsgid=	g_iMsgid_plt;
 				break;		
			case  TRC_FLAG:         /* trclog */		        
				sprintf(caStrHead,"[%6s] [%6s] %s pid:%d SID:%ld", 
					g_caJyczy, g_caSysJym, gMsTime, getpid(), g_lSessionID);
				iTmpMsgid=	g_iMsgid_trc;
				break;
			/*case  1003: */     /*comlog*/   /*���ӽ�������Ϣ*/
			/*	sprintf(caStrHead,"[TID:%d] [%s]", g_iTransTID,g_caSysJym  );
				 break;*/
			case APP_FLAG:     /*applog (����"��־��ʶ"��Ч�����)*/
				sprintf(caStrHead,"[%6s] %s %s:%d PID:%d SID:%ld",
					 g_caSysJym,gMsTime,pFile, iLine,  getpid() , g_lSessionID); 
				iTmpMsgid=	g_iMsgid_app;
				break;
			default :
			    LOGERR("cFlag[%c] is wrong",cFlag);
			    return -1;			
		}
		
		caStrHead[80]='\0';/*��֤��־ͷ���Ȳ�����80��������ضϣ�*/
		
		/*��־��Ҫ������־ͷ��80���ȣ��ÿո��룩*/
		if(strlen(caStrHead)<80)
 			*(caStrHead+strlen(caStrHead))=' ';	 			 	


		iTmpLen=strlen(gcaStrbody);
		if( iTmpLen > LOGMAXSIZE_1_USED  )		/*��־����,added by zouql 20100119*/
		{
				
				gcaStrbody[LOGMAXSIZE_1_USED-1]=0;
				LOGERR("��־�ߴ�[%d]����,��־��ʽΪ[%s],����Ϊ\n[%s%s%s]",
						iTmpLen,fmt,caStrHead,pFlag,gcaStrbody);
				
				/*��ERROR,��־�ߴ糬��*/
				sprintf( gcaContent,"%s%s��־�ߴ糬��[%d],���log.err",
						caStrHead,"[ERROR]", iTmpLen); 					
				iTmpLen=strlen(gcaContent);
				
		}
		else
		{					
				
				/*�����־����*/
				sprintf( gcaContent,"%s%s%s",caStrHead,pFlag, gcaStrbody); 	
				iTmpLen=iTmpLen+80+strlen(pFlag);
				
		}
			
	   /*����־����д����Ϣ����*/
	   if(WriteLogmsg(iTmpMsgid ,gcaContent,iTmpLen))
	   {	
			LOGERR("cFlag[%c]�ļ�¼�޷��ɹ�д�뵽��Ϣ����, \
						WriteLogmsg����gcaContent=%s]",cFlag,gcaContent);                          
			return -1;
		}

	}

	return 0;
}
/********************************************************************
* Function��														*
*	int  ComLog_(char *pcStr ,int  iLen,int  iFormattype)			*
* Description��														*
* 	������־�Ĵ���													*
* In Params:														*
*	char *pcStr ,int  iLen,int  iFormattype		  					*		
* Out Params:														*
*	��																*
* Return:															*
* 	0	  ���ɹ�													*
*	-1    ��ʧ��													*
*********************************************************************/
int  ComLog_(char *pcStr ,int  iLen,int  iFormattype,char* sFlag)
{
	char *pcAcsStr, caNod[5],*pcHexStr;
	int i;	
	int iTmpLen=0;

	memset(gcaContent,0x00,sizeof(gcaContent));
	memset(caNod,0x00,sizeof(caNod));	

	GetSysdateAndtime(gsDate,gsTime);		/*�������ں�ʱ��*/	
	
		
	memset(gcaContent,' ',80);	 
	/*[TID:10186][jym] 18:13:16 [pid:1450236][src][rev][optcode]*/
	sprintf(gcaContent,"[TID:%d][%s] %8s [pid:%d][s:%s][r:%s][%s]",
	  	g_iTransTID,g_caSysJym,gsTime, getpid(),g_caSrc,g_caRcv,g_caCode);	 
	gcaContent[strlen(gcaContent)]=' ';  			
	
	/*��־����COMLEVEL*/	
	if( COMLEVEL <= g_iLogLevel )
	{ 
		if(  iFormattype==HEX   &&  iLen*3<LOGMAXSIZE_1_USED )  /* HEX ��ʽʱ*/
		{	   
			if ((	pcHexStr = malloc(iLen*3+1)) == NULL) 
			{    	
				free(pcHexStr);	
				/*pcHexStr=NULL;*/
				LOGERR("�޷��ɹ���¼��־����[ComLog_(){malloc}ʧ��]");				
				return -1;
			}
			memset(pcHexStr,0x00,iLen*3+1);

			for( i=0;i<iLen;i++ )
		    {	   
				memset(caNod,0x00,sizeof(caNod));
				sprintf( caNod,"%02x ", *(pcStr +i));
				strncat(pcHexStr,caNod,3 );
			}	
			/*  *( pcHexStr+	iLen*3)='\0';		*/		
			
			/*��־����   "80λ��־ͷ[sFlag][pcHexStr]"*/
		  	sprintf(gcaContent+80,"[%s][", sFlag);
		  	iTmpLen=strlen(gcaContent);
		  	memcpy(gcaContent+iTmpLen,	pcHexStr  ,iLen*3 );
		  	memcpy(gcaContent+iTmpLen+iLen*3,"]",1);
		  	iTmpLen=iTmpLen+iLen*3+1;
			
			free(pcHexStr);	
			pcHexStr=NULL;
		}
		else if(  iFormattype==ASC  &&    iLen<LOGMAXSIZE_1_USED ) 
		/* iFormattypeΪASC��ʽʱ��������ת��*/
		{
			/*��־��*/
			if ((pcAcsStr = malloc(iLen+1)) == NULL) 
			{    	
				free(pcAcsStr);	
				/*pcAcsStr=NULL;*/
				LOGERR("�޷��ɹ���¼��־����[ComLog_(){malloc}ʧ��]");				
				return -1;
			}
			memset(pcAcsStr,0x00,iLen+1);
			memcpy(pcAcsStr,pcStr,iLen);
			/*  *(pcAcsStr+iLen)= '\0';  */
			
			/*��־����   "80λ��־ͷ[sFlag][pcAcsStr]"*/
		  	sprintf(gcaContent+80,"[%s][", sFlag);
		  	iTmpLen=strlen(gcaContent);
		  	memcpy(gcaContent+iTmpLen,	pcAcsStr  ,iLen );
		  	memcpy(gcaContent+iTmpLen+iLen,"]",1);
		  	iTmpLen=iTmpLen+iLen+1;
		  				
			free(pcAcsStr);	
			pcAcsStr=NULL;
		}
		else
		{	
			LOGERR("ComLog_����γ���[%d]��������ֵ[asc:%d;hex:%d]",
				iLen,LOGMAXSIZE_1_USED,LOGMAXSIZE_1_USED/3);
			return -1;	
		}	


		/*����־����д����Ϣ����*/
        if(WriteLogmsg(g_iMsgid_com, gcaContent ,iTmpLen) )
        {	 
            LOGERR("comlog��־�޷��ɹ�д����Ϣ����, WriteLogmsg()ʧ��.\
            		\n gcaContent=%s,]",    		gcaContent);   
            return -1;		 		
        }		
	}
	return  0;
}
/************************************************************************
* Function��															*
*	int  ComLog_seg(int iNo,char *pcName ,char *pcStr ,                 *
*					int  iLen,int iFormattype)			         		*
* Description��															*
* 	������־�����ݲ��֣��Ĵ���											*
* In Params:															*
*	int  iNo,char *pcName ,char *pcStr ,int  iLen,int  iFormattype 		*		
* Out Params:															*
*	��																	*
* Return:																*
* 	0	  ���ɹ�														*
*	-1    ��ʧ��														*
*************************************************************************/

int  ComLog_seg(int  iNo,char *pcName ,char *pcStr ,int  iLen,int  iFormattype)
{   
	char *pcAcsStr, caNod[5],*pcHexStr;
	int i;
	int iTmpLen=0;

	memset(gcaContent,0x00,sizeof(gcaContent));
	memset(caNod,0x00,sizeof(caNod));
	
	GetSysdateAndtime(gsDate,gsTime);		/*�������ں�ʱ��*/
	
	/*��־����COMLEVEL*/	
	if( COMLEVEL_SEG <= g_iLogLevel )
	{ 		
		if(iFormattype==HEX &&  iLen*3<LOGMAXSIZE_1_USED)  /* HEX ��ʽʱ*/
		{	   
			if ((pcHexStr = malloc(iLen*3+1)) == NULL) 
			{    	
				free(pcHexStr);	
				/*pcHexStr=NULL;*/
				LOGERR("�޷��ɹ���¼��־����[ComLog_seg(){malloc}ʧ��]");				
				return -1;
			}
			memset(pcHexStr,0x00,iLen*3+1);

			for( i=0;i<iLen;i++ )
			{	   
				memset(caNod,0x00,sizeof(caNod));
				sprintf( caNod,"%02x ", *(pcStr +i));
				strncat(pcHexStr,caNod,3 );
		     }	
			/*  *( pcHexStr+	iLen*3)='\0';		 */		
		
			/*��־����  "[TID:%d][%s] ��[%d]%s:[%s]" */
			sprintf(gcaContent,"[TID:%d][%s] ��[%d]%s:[",
					g_iTransTID,g_caSysJym,iNo,pcName);					
			iTmpLen=strlen(gcaContent);
			memcpy(gcaContent+iTmpLen  ,pcHexStr,iLen*3);
			memcpy(gcaContent+iTmpLen+iLen*3,  "]",1);
			iTmpLen=iTmpLen+iLen*3+1;	
								
					
			free(pcHexStr);	
			pcHexStr=NULL;
		}
		else if(iFormattype==ASC &&  iLen<LOGMAXSIZE_1_USED) 
		/* iFormattypeΪASC��ʽʱ��������ת��*/
		{
			/*��־��*/
			if ((pcAcsStr = malloc(iLen+1)) == NULL) 
			{    	
				free(pcAcsStr);	
				/*pcAcsStr=NULL;*/
				LOGERR("�޷��ɹ���¼��־����[ComLog_seg()mallocʧ��]");				
				return -1;
			}
			memset(pcAcsStr,0x00,iLen+1);
			memcpy(pcAcsStr,pcStr,iLen);
			/*  *(pcAcsStr+iLen)= '\0';  */
				
			/*��־����  "[TID:%d][%s] ��[%d]%s:[%s]"*/
			sprintf(gcaContent,"[TID:%d][%s] ��[%d]%s:[",
					g_iTransTID,g_caSysJym,iNo,pcName);					
			iTmpLen=strlen(gcaContent);
			memcpy(gcaContent+iTmpLen  ,pcAcsStr,iLen);
			memcpy(gcaContent+iTmpLen+iLen,  "]",1);
			iTmpLen=iTmpLen+iLen+1;	
				
			free(pcAcsStr);	
			pcAcsStr=NULL;
		}
		else 
		{	
			LOGERR("ComLog_����γ���[%d]��������ֵ[asc:%d;hex:%d]",
				iLen,LOGMAXSIZE_1_USED,LOGMAXSIZE_1_USED/3);
			return -1;	
		}

		/*����־����д����Ϣ����*/
		if(WriteLogmsg(g_iMsgid_com, gcaContent ,iTmpLen) )
		{	 
		 	LOGERR("comlog��־�޷��ɹ�д����Ϣ����,	WriteLogmsg()ʧ��.\
		 		\n  gcaContent=%s",	gcaContent);                       
			return -1;		 		
		 }				 
	}
	return  0;
}
/************************************************************************  
* Function��															* 
*	int  WriteLogmsg(  int iMsgid, char  *pcLogcontent ,int   iBuffLen)   				* 
* Description��															* 
* 	д��־��Ϣ����Ϣ����												* 
* In Params:															* 
*	 int iLogbs, char  *pcLogcontent   	,int   iBuffLen								*	
* Out Params:															* 
*	��																	* 
* Return:																* 
* 	0	  ���ɹ�														* 
*	-1    ��ʧ��														* 
*************************************************************************/
 /*���͵���Ϣ���е����� = ����+��־����+\n\n */
int  WriteLogmsg(  int iMsgid, char  *pcLogcontent ,int   iBuffLen)  
{		
	
	/*���ݴ�����Ϣ����*/
	gstLogmsg.mtype=1;   /*������־ͳһ������1������Ϣ����*/
	
	/*  8λʱ��+��־����+\n\n  */
/****����ע���˴�***********
	memset(gstLogmsg.caLogcontent,0x00,sizeof(gstLogmsg.caLogcontent));
****����ע���˴�***********/
	sprintf(gstLogmsg.caLogcontent,"%8s",gsDate);
	memcpy(gstLogmsg.caLogcontent+8, pcLogcontent,iBuffLen);
	memcpy(gstLogmsg.caLogcontent+8+iBuffLen,"\n\n",2);

	if(msgsnd(iMsgid, &gstLogmsg,  8+ iBuffLen+2,  0)) 
	{
		LOGERR("msgsnd err[%s],iMsgid[%d]\n",strerror(errno),iMsgid);
		return -1;
	}	
	
	return 0;	
}


/************************************************************************  
* Description��															* 
* 	���ķ�ʽ���id�����򲻴���							            	* 
* In Params:															* 
*	 HANDLE hConfig,char* sBasic,char* sSysname							*	
* Out Params:															* 
*	int* outiMsgid														* 
* Return:																* 
* ��0:�ɹ� ��-1��ʧ��								 					* 
*************************************************************************/    
int GetMsgid(HANDLE hConfig,char* sBasic,char* sSysname,int* outiMsgid)
{
    char sDiff[20];
    key_t basic,diff,ipckey;
        
    memset(sDiff,0x00,sizeof(sDiff));
    
    /*���ƫ����*/
    if(GetValue(hConfig ,"logmsg" ,sSysname,sDiff,sizeof(sDiff)))    	
    {	
    	LOGERR("InitLogenv()�е�GetValue()ʧ��,g_hdlLogCfg[%x] ,\
			caSection[logmsg] ,name[%s] ",	hConfig,sSysname );
    	return -1;
    } 

    sscanf(sBasic,"%x",&basic);    /*����ַ*/  
    sscanf(sDiff, "%x",&diff);       /*ƫ����:ת����ʮ��������.
                                        Ϊ�������,9->9��11->11,
                                        �м�ʡȥ��ʮ�������е�a,b,c,d,e,f*/
	ipckey=basic+ diff;           /*  9->0x40007009  ,11 -> 0x40007011*/
	
	*outiMsgid =msgget(ipckey,IPC_R);  /*��ȡmsgid*/	
	if(*outiMsgid==-1 )
	{
		LOGERR("�޷����ipckeyֵΪ[%x]��msgid��,sBasic[%s],sDiff[%s]  \
				\n %s",ipckey,sBasic,sDiff,strerror(errno));
		return -1; 
	}
	return 0;		    
}


/**/
int SetMsgenv(int* outMsgid)
{
    int	    iCount;
    int     iRet;
    int		iMsgid;
    
    /*�����Ϣ����*/
    ipckey=basic+diff;      
    iCount=5;
    iRet=-1;
    while(iCount--)
    {	
		iMsgid =msgget(ipckey,IPC_R);  /*��ȡmsgid*/
		if( iMsgid==-1 )
		{			
			if( errno==ENOENT )		/*��Ϣ���в�����ʱ������*/
			{	
				iMsgid=CreatInitLogmsg( ipckey);
				if( iMsgid==-1 )
				{	
					LOGERR("�޷�����(����)ipckeyֵΪ[%x]��msgid��\
					\n [CreatInitLogmsg����:%s]",ipckey,strerror(errno));
					return -1; 
				}
				iRet=0;
				break;
			}	
			if( errno== EINTR ) 
				continue;	
			else 
			{
				LOGERR("�޷����ipckeyֵΪ[%x]��msgid��\
					\n [msgget����:%s]",ipckey,strerror(errno));
				return -1; 
			}				
		}
		iRet=0;
		break;
	}	
	if( iRet!=0 )
	{
		LOGERR("��γ��Ժ����޷����ipckeyֵΪ[%x]��msgid��\
					\n [msgget����:%s]",ipckey,strerror(errno));
		return -1;	
	}	
	
	*outMsgid=iMsgid;
	return 0;
}

/**/
int SetFilenv(FILE** outfp)
{
	struct  stat stFilestat_tmp;  
	char  sTmpDate[8+1];
	char sTmpTime[6+1];
	
	memset(sTmpDate,0x00,sizeof(sTmpDate));
	memset(sTmpTime,0x00,sizeof(sTmpTime));
	
    /*����ļ���*//*����Ŀ¼���ļ�*/    
    GetSysdateAndtime6L(sTmpDate,sTmpTime);
    strcpy( sRegdate,sTmpDate );  /*����¼���ڡ�*/    
    memset( sFullFilename,0x00,sizeof(sFullFilename) );
    sprintf(sFullFilename,"%s/%s.%s%s",sDir,sFile,sTmpDate,sTmpTime );  
    
    memset( &stFstat,0x00,sizeof(struct  stat) );
    if( stat( sDir,&stFstat )!=0 )  /*���Ŀ¼�Ƿ���ڣ�����������򴴽�*/
	{	
		if( mkdir( sDir, S_IRWXU|S_IRGRP|S_IROTH) )
		{	
			LOGERR("�ļ�Ŀ¼[%s]�򿪣�����������",sDir);
			return -1;
		}		
	} 	                           
    *outfp = fopen(sFullFilename,"a+");
    if( *outfp==NULL )
    {
        LOGERR("fopen err\n");
        return -1;   
    } 
    
     /*����ļ��Ĵ�С,��ΪiLen_total��ʼ��С*/
    memset( &stFilestat_tmp,0x00,sizeof(struct  stat) );
    if( stat( sFullFilename,&stFilestat_tmp ) !=0 )
    {
    		LOGERR("��ȡ�ļ�[%s]��Ϣ��",sFullFilename);
    		return -1;
    	}
    iLen_total=stFilestat_tmp.st_size;
    
	return 0; 
}


/**/
int ReSetFilenv(FILE* oldfp,int iFlag, 
				FILE** newfp)
{   	       
	struct  stat stFilestat_tmp;  
	char  sTmpDate[8+1];
	char sTmpTime[6+1];

	memset(sTmpDate,0x00,sizeof(sTmpDate));
	memset(sTmpTime,0x00,sizeof(sTmpTime));
			       
	fclose(oldfp);
	
	switch(iFlag)
	{
		case FILELOST:
			memset( &stFstat,0x00,sizeof(struct  stat) );
		    if( stat( sDir,&stFstat ) !=0 ) /*���Ŀ¼�Ƿ����,���������򴴽�*/
			{	
				if( mkdir( sDir, S_IRWXU|S_IRGRP|S_IROTH) )
				{	
					LOGERR("�ļ�Ŀ¼[%s]�򿪣�����������",sDir);
					return -1;
				}		
			} 
		    break;				
		case DATECHANGE:			           
            strcpy(sRegdate,GetSysdate());	 /*�޸�"��¼����"*/
            /*����µ��ļ�ȫ��*/
            memset( sFullFilename,0x00,sizeof(sFullFilename) );
            sprintf(sFullFilename,"%s/%s.%s",sDir,sFile,sRegdate );
            break;
		case SIZEBEYOND:			            
			memset( sBakFilename,0x00,sizeof(sBakFilename));
	        sprintf(sBakFilename,"%s%s",sFullFilename, GetSystime6L() );
	        if( rename( sFullFilename, sBakFilename) ) 
    	    {
    	   		LOGERR("rename err:%s",strerror(errno));
    	   		return -1;
    	   	}    
            break;
		case NEWFILE:             	
			break;
		default:
			LOGERR("rename err:%s",strerror(errno));
    	   	return -1;				
	}
	
	/*����ļ���*//*����Ŀ¼���ļ�*/    		
	GetSysdateAndtime6L(sTmpDate,sTmpTime);
	strcpy( sRegdate,sTmpDate );  /*����¼���ڡ�*/		  
	memset( sFullFilename,0x00,sizeof(sFullFilename) );  
	sprintf(sFullFilename,"%s/%s.%s%s",sDir,sFile,sTmpDate,sTmpTime );  			

	/*iLen_total=0;  	*/
   	*newfp = fopen(sFullFilename,"a+");
    if( *newfp==NULL )
    {
        LOGERR("fopen err\n");
        return -1; 
    }  
    
     /*����ļ��Ĵ�С,��ΪiLen_total��ʼ��С*/
    memset( &stFilestat_tmp,0x00,sizeof(struct  stat) );
    if( stat( sFullFilename,&stFilestat_tmp ) !=0 )
    {
    		LOGERR("��ȡ�ļ�[%s]��Ϣ��",sFullFilename);
    		return -1;
    	}
    iLen_total=stFilestat_tmp.st_size;
    

	
	return 0;
}


/***********************************************************************
							Ӧ�ó������
***********************************************************************/
/************************************************************************  
* Function��															* 
*	int InitLogenv(char*  sSysname) 									* 
* Description��															* 
* 	��ʼ����־����Ļ���                                                *
*	��Ӧ���ڵ�����˺����󣬼��ɿ�ʼʹ��PINFO\PDEBUG�����ˣ�			*
*	�˺�����ʼ����־����=4����־Ŀ���ַ��λpltlog.****�x	    		*
*	���Ҫ�ı���־�������־��Ŀ���ַ��������������SetLogenv����		*
*																		* 
* In Params:															* 
*	char*  sSysname			ϵͳ���									*	
* Out Params:															* 
*	��																	* 
* Return:																* 
* ��0���ɹ�																*
*	-1��ʧ��										 					* 
*************************************************************************/
int InitLogenv(char*  sSysname)
{
	static char caConfig[100];
	char sRzjb_Value[10];
	char sBasic_value[15]; 		     /*����ַ*/
	char sTmp_value[10];            /*ȡ��Ϣ���еı��*/
	char sTmpSysname[10];
	
	memset(caConfig,0x00,sizeof(caConfig));
	memset(sRzjb_Value,0x00,sizeof(sRzjb_Value));
	memset(sBasic_value,0x00,sizeof(sBasic_value));
	memset(sTmp_value,0x00,sizeof(sTmp_value));
	memset(gsDate,0x00,sizeof(gsDate));
	memset(gsTime,0x00,sizeof(gsTime));
	memset(sTmpSysname,0x00,sizeof(sTmpSysname));
	
	strcpy(sTmpSysname,sSysname);
	/*�����ں�ʱ�丳ֵ*/
	GetSysdateAndtime(gsDate,gsTime);		/*�������ں�ʱ��*/
	if( strlen(gsDate)!=8 || strlen(gsTime)!=8 )
	{
        LOGERR("����gsDate[%s]ʱ��gsTime[%s]ȡֵ����",gsDate,gsTime);
        return -1;
    } 
    
	 /*װ�������ļ�*/
	 strcpy( caConfig,"logConfigure.conf" );
	 g_hdlLogCfg = LoadConfigure( caConfig );
	 if(!g_hdlLogCfg)
    {
        LOGERR("LoadConfigure failed.");
        return -1;
    }
    
    /*���pltlog��comlog��trclog��app������Ϣ���е�id*/    
    if( GetValue( g_hdlLogCfg ,"logmsg" ,"basickey",
				sBasic_value,sizeof( sBasic_value ) ) )    	
    {	
		LOGERR("InitLogenv()�е�GetValue()ʧ��,g_hdlLogCfg[%x] ,\
			caSection[logmsg] ,name[basickey] ",	g_hdlLogCfg);
		FreeConfigure( g_hdlLogCfg );   		
		return -1;
    }      
    
    if( GetMsgid( g_hdlLogCfg , sBasic_value, "plt",&g_iMsgid_plt) )
    {   
        LOGERR("GetMsgid[plt] failed"); 
        FreeConfigure( g_hdlLogCfg );   
        return -1;    
    }
    if( GetMsgid( g_hdlLogCfg , sBasic_value, "trc",&g_iMsgid_trc) )
    {   
        LOGERR("GetMsgid[trc] failed"); 
        FreeConfigure( g_hdlLogCfg );   
        return -1;    
    }
	if( GetMsgid( g_hdlLogCfg , sBasic_value, "com",&g_iMsgid_com) )
    {   
        LOGERR("GetMsgid[com] failed"); 
        FreeConfigure( g_hdlLogCfg );   
        return -1;    
    }
    if( GetMsgid( g_hdlLogCfg , sBasic_value,sTmpSysname ,&g_iMsgid_app) )
    {   
        LOGERR("GetMsgid[app:%s] failed",sTmpSysname); 
        FreeConfigure( g_hdlLogCfg );   
        return -1;    
    }  
    /* "��־����" ����ʼֵ*/      /*�����ļ��е�initlevel��Ϊ��ʼֵ*/        
    if( GetValue( g_hdlLogCfg ,"loglevel" ,"initlevel",
		sRzjb_Value,sizeof( sRzjb_Value ) ) )    	
    {	
		LOGERR("InitLogenv()�е�GetValue()ʧ��,g_hdlLogCfg[%x] ,\
			caSection[loglevel] ,name[initlevel] ",	g_hdlLogCfg );
		FreeConfigure( g_hdlLogCfg );   	
		return -1;
    }    
    g_iLogLevel=atoi(sRzjb_Value);
    if( g_iLogLevel>=0 && g_iLogLevel<=4 ){}
    else 
    {
		LOGERR("����־�����ļ��ж�ȡ���ĳ�ʼ��־����[%s]Ϊ��Чֵ",
			sRzjb_Value);
		FreeConfigure( g_hdlLogCfg );   		
		return -1;
    }   
    
    FreeConfigure( g_hdlLogCfg );   
    return 0;	  
}
/************************************************************************  
* Function��															* 
*	int SetLogenv(int iLevel )							                * 
* ������־����															* 
* In Params:															* 
*	int iLevel															*	
* Out Params:															* 
*	��																	* 
* Return:																* 
* ��0���ɹ�																*
*	-1��ʧ��										 					* 
*************************************************************************/
int SetLogenv(int iLevel )
{		
	char value[10];
    char name[5];
    char caSection[20];
	memset(value ,0x00,sizeof(value));
	memset(name ,0x00,sizeof(name));
	memset(caSection,0x00,sizeof(caSection));
	
	/*ȷ��g_iLogLevel��ֵ*/
		g_iLogLevel=iLevel;
    if( g_iLogLevel>=0 && g_iLogLevel<=4 ){}
    else 
    {
        LOGERR("level[%d] ��Ч",g_iLogLevel);
        return -1;
    }  	

    return 0;	    
}	    

