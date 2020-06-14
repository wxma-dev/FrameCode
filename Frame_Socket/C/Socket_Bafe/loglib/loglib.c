/**************************************************************** 
* Copyright (c) 2008 北京联银通科技有限公司						*
* Source File:		loglib.c 									*
* Description:		日志处理的函数库			  				*
* Author:			邹倩林										*
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

/*获得系统日期yyyymmdd和时间hhmmss*/
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

/*获得系统日期yyyymmdd和时间hh:mm:ss*/
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
/**获取日期 时间  带微秒的时间**/
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
* Function：														*
*	char  *GetSystime () 											*
* Description：														*
* 	获得系统时间													*
* In Params:														*
*	无																*
* Out Params:														*
*	无																*
* Return:															*
* 　系统时间														*
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

char  *GetSystime6L()  /*6位长*/
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
* Function：															* 
*	char  *GetSysdate () 	  											* 
* Description：															* 
* 	获得系统日期														* 
* In Params:															* 
*	 无																	*	
* Out Params:															* 
*	无																	* 
* Return:																* 
* 	系统日期									 						* 
*************************************************************************/

char  *GetSysdate ()  /*获得系统日期:yyyymmdd*/
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
* Function：														*
*	void WriteFiles (FileForOut,ifHead,sFile , iLine,fmt, va_alist )*
* Description：														*
* 	直接写文件的方式处理日志,也可用于记录日志处理过程中的信息		*
* In Params:														*
*	FileForOut,ifHead,sFile , iLine,fmt, va_alist					*
* Out Params:														*
*	无																*
* Return:															*
* 　无																*
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
	
	if(ifHead==0)				/*不带日志头,不加换行的处理*/
	{
		fprintf(pFile,"%s",caStr );
	}
	else if(ifHead==1)	/*带日志头,加换行*/
	{	
		fprintf(pFile,"[%s] [%s] [line:%d] [pid:%d] %s\n\n",
			caTime,sFile , iLine,getpid(),caStr );
	}
	
	fclose(pFile );	
}	

/********************************************************************
* Function：														*
*	void WriteLogs (sFile,iLine,fmt, va_alist) 						*
* Description：														*
* 	记录日志处理过程中的信息										*
* In Params:														*
*	sFile,iLine,fmt、 va_alist，可变参数							*
* Out Params:														*
*	无																*
* Return:															*
* 　无																*
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
* Function：														*
*	void DestroyLogmsg(int iMsgid)									*
* Description：														*
* 	销毁消息队列													*
* In Params:														*
*	int iMsgid														*
* Out Params:														*
*	无																*
* Return:															*
* 　无																*
*********************************************************************/
void DestroyLogmsg(int iMsgid)
{
	if(msgctl(iMsgid ,IPC_RMID , &g_stMsgBuff))
		LOGERR("销毁消息队列出错");
	else LOGERR("[suc]销毁消息队列");
}

/********************************************************************
* Function：														*
*	int  CreatInitLogmsg(key_t  ipckey)								*
* Description：														*
* 	创建并设置消息队列												*
* In Params:														*
*	key_t  ipckey													*
* Out Params:														*
*	无																*
* Return:															*
* 　iMsgid :消息队列的id号		；									*	
*	-1    ：失败													*
********************************************************************/
int  CreatInitLogmsg(key_t  ipckey)
{
	static int iMsgid;
	int iFlag;   
	/*struct msqid_ds g_stMsgBuff;*/

	/*创建消息队列*/
	iFlag= 0664|IPC_CREAT|IPC_EXCL;  
	if(-1==( iMsgid =msgget(ipckey,iFlag)))
	{
		LOGERR("创建消息队列失败");
		return  -1;	
	}	

	/*设置消息队列*/		
	if(msgctl(iMsgid,IPC_STAT,&g_stMsgBuff))
	{
		LOGERR("设置消息队列失败【IPC_STAT】");
		DestroyLogmsg(iMsgid);
		return -1;
	}
	
	g_stMsgBuff.msg_qbytes= LOGMSGSIZE; /***/
	
	if(msgctl(iMsgid,IPC_SET,&g_stMsgBuff))
	{
		LOGERR("设置消息队列失败【IPC_SET】，%s",strerror(errno));
		DestroyLogmsg(iMsgid);
		return -1;
	}	
	return  iMsgid;	
}

/************************************************************************
* Description：										                    *
* 	日志指令（宏）的处理							                    *
* In Params:										                    *
*	pFile , iLine, iLxFlag ,  iLoglevel,  pFlag, fmt, va_alist          * 
* Out Params:												            *
*	无														            *
* Return:													            *
* 	0	  ：成功											            *	
*	-1    ：失败											            *
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
/*  修改为char类型数据
		int gMsTime[12+1];
*/
		char gMsTime[12+1];
/****调优注掉此处**********
		memset(gcaContent,0x00,sizeof(gcaContent));
		memset(gcaStrbody,0x00,sizeof(gcaStrbody));
****调优注掉此处***********/	
	
		iGetSysDateTimeMsTime(gsDate,gsTime,gMsTime); /**更新日期时间**/
		memset(caStrHead,' ',80);
		caStrHead[80]='\0';		
	

		/*日志体*/
		va_start( args );
		vsnprintf(gcaStrbody,sizeof(gcaStrbody),fmt,args);
		va_end( args);	   

		
		/*日志内容="日志头长度（3位）"+"日志头"+"日志体"*/
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
			/*case  1003: */     /*comlog*/   /*增加交易码信息*/
			/*	sprintf(caStrHead,"[TID:%d] [%s]", g_iTransTID,g_caSysJym  );
				 break;*/
			case APP_FLAG:     /*applog (包括"日志标识"无效的情况)*/
				sprintf(caStrHead,"[%6s] %s %s:%d PID:%d SID:%ld",
					 g_caSysJym,gMsTime,pFile, iLine,  getpid() , g_lSessionID); 
				iTmpMsgid=	g_iMsgid_app;
				break;
			default :
			    LOGERR("cFlag[%c] is wrong",cFlag);
			    return -1;			
		}
		
		caStrHead[80]='\0';/*保证日志头长度不超过80（超过则截断）*/
		
		/*日志需要补齐日志头到80长度（用空格补齐）*/
		if(strlen(caStrHead)<80)
 			*(caStrHead+strlen(caStrHead))=' ';	 			 	


		iTmpLen=strlen(gcaStrbody);
		if( iTmpLen > LOGMAXSIZE_1_USED  )		/*日志超长,added by zouql 20100119*/
		{
				
				gcaStrbody[LOGMAXSIZE_1_USED-1]=0;
				LOGERR("日志尺寸[%d]超长,日志格式为[%s],内容为\n[%s%s%s]",
						iTmpLen,fmt,caStrHead,pFlag,gcaStrbody);
				
				/*报ERROR,日志尺寸超长*/
				sprintf( gcaContent,"%s%s日志尺寸超长[%d],详见log.err",
						caStrHead,"[ERROR]", iTmpLen); 					
				iTmpLen=strlen(gcaContent);
				
		}
		else
		{					
				
				/*获得日志内容*/
				sprintf( gcaContent,"%s%s%s",caStrHead,pFlag, gcaStrbody); 	
				iTmpLen=iTmpLen+80+strlen(pFlag);
				
		}
			
	   /*把日志数据写入消息队列*/
	   if(WriteLogmsg(iTmpMsgid ,gcaContent,iTmpLen))
	   {	
			LOGERR("cFlag[%c]的记录无法成功写入到消息队列, \
						WriteLogmsg出错，gcaContent=%s]",cFlag,gcaContent);                          
			return -1;
		}

	}

	return 0;
}
/********************************************************************
* Function：														*
*	int  ComLog_(char *pcStr ,int  iLen,int  iFormattype)			*
* Description：														*
* 	报文日志的处理													*
* In Params:														*
*	char *pcStr ,int  iLen,int  iFormattype		  					*		
* Out Params:														*
*	无																*
* Return:															*
* 	0	  ：成功													*
*	-1    ：失败													*
*********************************************************************/
int  ComLog_(char *pcStr ,int  iLen,int  iFormattype,char* sFlag)
{
	char *pcAcsStr, caNod[5],*pcHexStr;
	int i;	
	int iTmpLen=0;

	memset(gcaContent,0x00,sizeof(gcaContent));
	memset(caNod,0x00,sizeof(caNod));	

	GetSysdateAndtime(gsDate,gsTime);		/*更新日期和时间*/	
	
		
	memset(gcaContent,' ',80);	 
	/*[TID:10186][jym] 18:13:16 [pid:1450236][src][rev][optcode]*/
	sprintf(gcaContent,"[TID:%d][%s] %8s [pid:%d][s:%s][r:%s][%s]",
	  	g_iTransTID,g_caSysJym,gsTime, getpid(),g_caSrc,g_caRcv,g_caCode);	 
	gcaContent[strlen(gcaContent)]=' ';  			
	
	/*日志级别＝COMLEVEL*/	
	if( COMLEVEL <= g_iLogLevel )
	{ 
		if(  iFormattype==HEX   &&  iLen*3<LOGMAXSIZE_1_USED )  /* HEX 格式时*/
		{	   
			if ((	pcHexStr = malloc(iLen*3+1)) == NULL) 
			{    	
				free(pcHexStr);	
				/*pcHexStr=NULL;*/
				LOGERR("无法成功记录日志内容[ComLog_(){malloc}失败]");				
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
			
			/*日志内容   "80位日志头[sFlag][pcHexStr]"*/
		  	sprintf(gcaContent+80,"[%s][", sFlag);
		  	iTmpLen=strlen(gcaContent);
		  	memcpy(gcaContent+iTmpLen,	pcHexStr  ,iLen*3 );
		  	memcpy(gcaContent+iTmpLen+iLen*3,"]",1);
		  	iTmpLen=iTmpLen+iLen*3+1;
			
			free(pcHexStr);	
			pcHexStr=NULL;
		}
		else if(  iFormattype==ASC  &&    iLen<LOGMAXSIZE_1_USED ) 
		/* iFormattype为ASC格式时，不需做转换*/
		{
			/*日志体*/
			if ((pcAcsStr = malloc(iLen+1)) == NULL) 
			{    	
				free(pcAcsStr);	
				/*pcAcsStr=NULL;*/
				LOGERR("无法成功记录日志内容[ComLog_(){malloc}失败]");				
				return -1;
			}
			memset(pcAcsStr,0x00,iLen+1);
			memcpy(pcAcsStr,pcStr,iLen);
			/*  *(pcAcsStr+iLen)= '\0';  */
			
			/*日志内容   "80位日志头[sFlag][pcAcsStr]"*/
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
			LOGERR("ComLog_的入参长度[%d]超过允许值[asc:%d;hex:%d]",
				iLen,LOGMAXSIZE_1_USED,LOGMAXSIZE_1_USED/3);
			return -1;	
		}	


		/*把日志数据写入消息队列*/
        if(WriteLogmsg(g_iMsgid_com, gcaContent ,iTmpLen) )
        {	 
            LOGERR("comlog日志无法成功写入消息队列, WriteLogmsg()失败.\
            		\n gcaContent=%s,]",    		gcaContent);   
            return -1;		 		
        }		
	}
	return  0;
}
/************************************************************************
* Function：															*
*	int  ComLog_seg(int iNo,char *pcName ,char *pcStr ,                 *
*					int  iLen,int iFormattype)			         		*
* Description：															*
* 	报文日志（数据部分）的处理											*
* In Params:															*
*	int  iNo,char *pcName ,char *pcStr ,int  iLen,int  iFormattype 		*		
* Out Params:															*
*	无																	*
* Return:																*
* 	0	  ：成功														*
*	-1    ：失败														*
*************************************************************************/

int  ComLog_seg(int  iNo,char *pcName ,char *pcStr ,int  iLen,int  iFormattype)
{   
	char *pcAcsStr, caNod[5],*pcHexStr;
	int i;
	int iTmpLen=0;

	memset(gcaContent,0x00,sizeof(gcaContent));
	memset(caNod,0x00,sizeof(caNod));
	
	GetSysdateAndtime(gsDate,gsTime);		/*更新日期和时间*/
	
	/*日志级别＝COMLEVEL*/	
	if( COMLEVEL_SEG <= g_iLogLevel )
	{ 		
		if(iFormattype==HEX &&  iLen*3<LOGMAXSIZE_1_USED)  /* HEX 格式时*/
		{	   
			if ((pcHexStr = malloc(iLen*3+1)) == NULL) 
			{    	
				free(pcHexStr);	
				/*pcHexStr=NULL;*/
				LOGERR("无法成功记录日志内容[ComLog_seg(){malloc}失败]");				
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
		
			/*日志内容  "[TID:%d][%s] 域[%d]%s:[%s]" */
			sprintf(gcaContent,"[TID:%d][%s] 域[%d]%s:[",
					g_iTransTID,g_caSysJym,iNo,pcName);					
			iTmpLen=strlen(gcaContent);
			memcpy(gcaContent+iTmpLen  ,pcHexStr,iLen*3);
			memcpy(gcaContent+iTmpLen+iLen*3,  "]",1);
			iTmpLen=iTmpLen+iLen*3+1;	
								
					
			free(pcHexStr);	
			pcHexStr=NULL;
		}
		else if(iFormattype==ASC &&  iLen<LOGMAXSIZE_1_USED) 
		/* iFormattype为ASC格式时，不需做转换*/
		{
			/*日志体*/
			if ((pcAcsStr = malloc(iLen+1)) == NULL) 
			{    	
				free(pcAcsStr);	
				/*pcAcsStr=NULL;*/
				LOGERR("无法成功记录日志内容[ComLog_seg()malloc失败]");				
				return -1;
			}
			memset(pcAcsStr,0x00,iLen+1);
			memcpy(pcAcsStr,pcStr,iLen);
			/*  *(pcAcsStr+iLen)= '\0';  */
				
			/*日志内容  "[TID:%d][%s] 域[%d]%s:[%s]"*/
			sprintf(gcaContent,"[TID:%d][%s] 域[%d]%s:[",
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
			LOGERR("ComLog_的入参长度[%d]超过允许值[asc:%d;hex:%d]",
				iLen,LOGMAXSIZE_1_USED,LOGMAXSIZE_1_USED/3);
			return -1;	
		}

		/*把日志数据写入消息队列*/
		if(WriteLogmsg(g_iMsgid_com, gcaContent ,iTmpLen) )
		{	 
		 	LOGERR("comlog日志无法成功写入消息队列,	WriteLogmsg()失败.\
		 		\n  gcaContent=%s",	gcaContent);                       
			return -1;		 		
		 }				 
	}
	return  0;
}
/************************************************************************  
* Function：															* 
*	int  WriteLogmsg(  int iMsgid, char  *pcLogcontent ,int   iBuffLen)   				* 
* Description：															* 
* 	写日志信息到消息队列												* 
* In Params:															* 
*	 int iLogbs, char  *pcLogcontent   	,int   iBuffLen								*	
* Out Params:															* 
*	无																	* 
* Return:																* 
* 	0	  ：成功														* 
*	-1    ：失败														* 
*************************************************************************/
 /*发送到消息对列的内容 = 日期+日志内容+\n\n */
int  WriteLogmsg(  int iMsgid, char  *pcLogcontent ,int   iBuffLen)  
{		
	
	/*数据存入消息队列*/
	gstLogmsg.mtype=1;   /*所有日志统一用类型1传到消息队列*/
	
	/*  8位时间+日志主体+\n\n  */
/****调优注掉此处***********
	memset(gstLogmsg.caLogcontent,0x00,sizeof(gstLogmsg.caLogcontent));
****调优注掉此处***********/
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
* Description：															* 
* 	读的方式获得id，无则不创建							            	* 
* In Params:															* 
*	 HANDLE hConfig,char* sBasic,char* sSysname							*	
* Out Params:															* 
*	int* outiMsgid														* 
* Return:																* 
* 　0:成功 ；-1：失败								 					* 
*************************************************************************/    
int GetMsgid(HANDLE hConfig,char* sBasic,char* sSysname,int* outiMsgid)
{
    char sDiff[20];
    key_t basic,diff,ipckey;
        
    memset(sDiff,0x00,sizeof(sDiff));
    
    /*获得偏移量*/
    if(GetValue(hConfig ,"logmsg" ,sSysname,sDiff,sizeof(sDiff)))    	
    {	
    	LOGERR("InitLogenv()中的GetValue()失败,g_hdlLogCfg[%x] ,\
			caSection[logmsg] ,name[%s] ",	hConfig,sSysname );
    	return -1;
    } 

    sscanf(sBasic,"%x",&basic);    /*基地址*/  
    sscanf(sDiff, "%x",&diff);       /*偏移量:转换成十六进制数.
                                        为方便起见,9->9，11->11,
                                        中间省去了十六进制中的a,b,c,d,e,f*/
	ipckey=basic+ diff;           /*  9->0x40007009  ,11 -> 0x40007011*/
	
	*outiMsgid =msgget(ipckey,IPC_R);  /*读取msgid*/	
	if(*outiMsgid==-1 )
	{
		LOGERR("无法获得ipckey值为[%x]的msgid号,sBasic[%s],sDiff[%s]  \
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
    
    /*获得消息队列*/
    ipckey=basic+diff;      
    iCount=5;
    iRet=-1;
    while(iCount--)
    {	
		iMsgid =msgget(ipckey,IPC_R);  /*读取msgid*/
		if( iMsgid==-1 )
		{			
			if( errno==ENOENT )		/*消息队列不存在时，创建*/
			{	
				iMsgid=CreatInitLogmsg( ipckey);
				if( iMsgid==-1 )
				{	
					LOGERR("无法创建(设置)ipckey值为[%x]的msgid号\
					\n [CreatInitLogmsg出错:%s]",ipckey,strerror(errno));
					return -1; 
				}
				iRet=0;
				break;
			}	
			if( errno== EINTR ) 
				continue;	
			else 
			{
				LOGERR("无法获得ipckey值为[%x]的msgid号\
					\n [msgget出错:%s]",ipckey,strerror(errno));
				return -1; 
			}				
		}
		iRet=0;
		break;
	}	
	if( iRet!=0 )
	{
		LOGERR("多次尝试后仍无法获得ipckey值为[%x]的msgid号\
					\n [msgget出错:%s]",ipckey,strerror(errno));
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
	
    /*获得文件名*//*创建目录和文件*/    
    GetSysdateAndtime6L(sTmpDate,sTmpTime);
    strcpy( sRegdate,sTmpDate );  /*“记录日期”*/    
    memset( sFullFilename,0x00,sizeof(sFullFilename) );
    sprintf(sFullFilename,"%s/%s.%s%s",sDir,sFile,sTmpDate,sTmpTime );  
    
    memset( &stFstat,0x00,sizeof(struct  stat) );
    if( stat( sDir,&stFstat )!=0 )  /*检查目录是否存在，如果不存在则创建*/
	{	
		if( mkdir( sDir, S_IRWXU|S_IRGRP|S_IROTH) )
		{	
			LOGERR("文件目录[%s]打开（创建）出错",sDir);
			return -1;
		}		
	} 	                           
    *outfp = fopen(sFullFilename,"a+");
    if( *outfp==NULL )
    {
        LOGERR("fopen err\n");
        return -1;   
    } 
    
     /*获得文件的大小,作为iLen_total初始大小*/
    memset( &stFilestat_tmp,0x00,sizeof(struct  stat) );
    if( stat( sFullFilename,&stFilestat_tmp ) !=0 )
    {
    		LOGERR("获取文件[%s]信息错",sFullFilename);
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
		    if( stat( sDir,&stFstat ) !=0 ) /*检查目录是否存在,若不存在则创建*/
			{	
				if( mkdir( sDir, S_IRWXU|S_IRGRP|S_IROTH) )
				{	
					LOGERR("文件目录[%s]打开（创建）出错",sDir);
					return -1;
				}		
			} 
		    break;				
		case DATECHANGE:			           
            strcpy(sRegdate,GetSysdate());	 /*修改"记录日期"*/
            /*获得新的文件全名*/
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
	
	/*获得文件名*//*创建目录和文件*/    		
	GetSysdateAndtime6L(sTmpDate,sTmpTime);
	strcpy( sRegdate,sTmpDate );  /*“记录日期”*/		  
	memset( sFullFilename,0x00,sizeof(sFullFilename) );  
	sprintf(sFullFilename,"%s/%s.%s%s",sDir,sFile,sTmpDate,sTmpTime );  			

	/*iLen_total=0;  	*/
   	*newfp = fopen(sFullFilename,"a+");
    if( *newfp==NULL )
    {
        LOGERR("fopen err\n");
        return -1; 
    }  
    
     /*获得文件的大小,作为iLen_total初始大小*/
    memset( &stFilestat_tmp,0x00,sizeof(struct  stat) );
    if( stat( sFullFilename,&stFilestat_tmp ) !=0 )
    {
    		LOGERR("获取文件[%s]信息错",sFullFilename);
    		return -1;
    	}
    iLen_total=stFilestat_tmp.st_size;
    

	
	return 0;
}


/***********************************************************************
							应用程序调用
***********************************************************************/
/************************************************************************  
* Function：															* 
*	int InitLogenv(char*  sSysname) 									* 
* Description：															* 
* 	初始化日志程序的环境                                                *
*	（应用在调用完此函数后，即可开始使用PINFO\PDEBUG命令了）			*
*	此函数初始了日志级别=4、日志目标地址定位pltlog.****	    		*
*	如果要改变日志级别和日志的目标地址，则需调用下面的SetLogenv函数		*
*																		* 
* In Params:															* 
*	char*  sSysname			系统检查									*	
* Out Params:															* 
*	无																	* 
* Return:																* 
* 　0：成功																*
*	-1：失败										 					* 
*************************************************************************/
int InitLogenv(char*  sSysname)
{
	static char caConfig[100];
	char sRzjb_Value[10];
	char sBasic_value[15]; 		     /*基地址*/
	char sTmp_value[10];            /*取消息队列的编号*/
	char sTmpSysname[10];
	
	memset(caConfig,0x00,sizeof(caConfig));
	memset(sRzjb_Value,0x00,sizeof(sRzjb_Value));
	memset(sBasic_value,0x00,sizeof(sBasic_value));
	memset(sTmp_value,0x00,sizeof(sTmp_value));
	memset(gsDate,0x00,sizeof(gsDate));
	memset(gsTime,0x00,sizeof(gsTime));
	memset(sTmpSysname,0x00,sizeof(sTmpSysname));
	
	strcpy(sTmpSysname,sSysname);
	/*给日期和时间赋值*/
	GetSysdateAndtime(gsDate,gsTime);		/*更新日期和时间*/
	if( strlen(gsDate)!=8 || strlen(gsTime)!=8 )
	{
        LOGERR("日期gsDate[%s]时间gsTime[%s]取值不对",gsDate,gsTime);
        return -1;
    } 
    
	 /*装载配置文件*/
	 strcpy( caConfig,"logConfigure.conf" );
	 g_hdlLogCfg = LoadConfigure( caConfig );
	 if(!g_hdlLogCfg)
    {
        LOGERR("LoadConfigure failed.");
        return -1;
    }
    
    /*获得pltlog、comlog、trclog、app四种消息队列的id*/    
    if( GetValue( g_hdlLogCfg ,"logmsg" ,"basickey",
				sBasic_value,sizeof( sBasic_value ) ) )    	
    {	
		LOGERR("InitLogenv()中的GetValue()失败,g_hdlLogCfg[%x] ,\
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
    /* "日志级别" 赋初始值*/      /*配置文件中的initlevel即为初始值*/        
    if( GetValue( g_hdlLogCfg ,"loglevel" ,"initlevel",
		sRzjb_Value,sizeof( sRzjb_Value ) ) )    	
    {	
		LOGERR("InitLogenv()中的GetValue()失败,g_hdlLogCfg[%x] ,\
			caSection[loglevel] ,name[initlevel] ",	g_hdlLogCfg );
		FreeConfigure( g_hdlLogCfg );   	
		return -1;
    }    
    g_iLogLevel=atoi(sRzjb_Value);
    if( g_iLogLevel>=0 && g_iLogLevel<=4 ){}
    else 
    {
		LOGERR("从日志配置文件中读取出的初始日志级别[%s]为无效值",
			sRzjb_Value);
		FreeConfigure( g_hdlLogCfg );   		
		return -1;
    }   
    
    FreeConfigure( g_hdlLogCfg );   
    return 0;	  
}
/************************************************************************  
* Function：															* 
*	int SetLogenv(int iLevel )							                * 
* 设置日志级别															* 
* In Params:															* 
*	int iLevel															*	
* Out Params:															* 
*	无																	* 
* Return:																* 
* 　0：成功																*
*	-1：失败										 					* 
*************************************************************************/
int SetLogenv(int iLevel )
{		
	char value[10];
    char name[5];
    char caSection[20];
	memset(value ,0x00,sizeof(value));
	memset(name ,0x00,sizeof(name));
	memset(caSection,0x00,sizeof(caSection));
	
	/*确定g_iLogLevel的值*/
		g_iLogLevel=iLevel;
    if( g_iLogLevel>=0 && g_iLogLevel<=4 ){}
    else 
    {
        LOGERR("level[%d] 无效",g_iLogLevel);
        return -1;
    }  	

    return 0;	    
}	    

