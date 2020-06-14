/*************************************************
*  Copyright (C) 2011 HICOM
*  All rights reserved
*
*  This is unpublished proprietary source code of HICOM Ltd.
*  The copyright notice above does not evidence any actual
*  or intended publication of such source code.
*
*  NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
*  SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
*
*  File name:      lib_shm.c
*
*  Author:       Version:        Date: 
*  mawx           1.0             2018-05-08
*
*  Description: 客户化处理回调函数程序
*  Others:       
*  Function List: 
*      1. ADPT_AppGenErrBuf()       客户化处理,生成错误报文
*      2. ADPT_AppGenTranCode()     客户化处理,生成交易码
*      3. ADPT_TxMsgInCustomDl()    客户化前处理主控处理函数
*      4. ADPT_TxMsgOutCustomDl()   客户化后处理主控处理函数
*  History:        
*                  
* NO  Date        Author       reason     Modification
* 1   2011-10-19  lzh                     建立
* 2   2016-08-11  zhouyb                  增加函数：ADPT_TranFraud
************************************************/
/* Unix Head File */
#include "sysunix.h"
#include "global.h"
#include "svc_base.h"
#include "atmi.h"
#include "len.h"
#include "dbstruct.h"
#include "ClientAPI.h"

/*流水号起始位置*/
int SerialBegin = 0; 

/*流水号个数*/
int SerialCount = 0;
int SerialIndex = 0;

static int  giInLen;
static char gaczInBuf[1024*20];
char        recvcupshead[46+1];


/*************************************************
*  Function:       ADPT_AppSvrInit
*  Description:    框架程序启动初始化时回调的客户化服务进
*                  程启动初始化函数
*                  此函数由客户化程序具体实现
*  Table Accessed: 
*  Table Updated:  
*  Input:          argc - 命令行参数个数
                   argv - 命令行参数
*  Output:         无
*  Return:         0 - 成功 其他 - 失败
*  Error:          
*  Others:         根据需要做些客户化的初始，比如：打开数据库
*************************************************/
int ADPT_AppSvrInit(int argc, char *argv[])
{
    int iRet;

    EXEC SQL BEGIN DECLARE SECTION;
        char aczDbName[20];
    EXEC SQL END DECLARE SECTION;

    memset(aczDbName, 0, sizeof(aczDbName));
    snprintf(aczDbName, sizeof(aczDbName), "%s", getenv("ELRUNDBNAME"));

    EXEC SQL DATABASE :aczDbName;
    if (SQLCODE) 
    {
       TOOL_Log(LERROR,
                "数据库打开失败[%d], 请检查数据库或环境变量ELRUNDBNAME\n",
               SQLCODE);
       return(FAIL);
    }

    giInLen = 0;
    memset(gaczInBuf, 0x00, sizeof(gaczInBuf));

    return(SUCC);
    //return ADPT_GenCupsSerial(ADPT_ATMCARD_FLAG, &SerialBegin, &SerialCount);
} /* end of ADPT_AppSvrInit() */

/*************************************************
*  Function:       ADPT_AppSvrDone
*  Description:    框架程序终止处理时回调的客户化服务进
*                  程终止处理函数
*                  此函数由客户化程序具体实现
*  Table Accessed: 
*  Table Updated:  
*  Input:          无
*  Output:         无
*  Return:         无
*  Error:          
*  Others:    
*************************************************/
void ADPT_AppSvrDone()
{
    EXEC SQL CLOSE DATABASE;

    return;
} /* end of ADPT_AppSvrDone() */

/*************************************************
*  Function:       ADPT_AppGenErrBuf
*  Description:    客户化处理: 生成错误报文 
*  Table Accessed: 
*  Table Updated:  
*  Input:        
*                2)  epczErrNum 错误码
*                3)  epczErrMsg 错误信息
*  Output:         
                 1)  epstBuffer 产生的错误报文   
*  Return:       错误报文的整体长度 - 成功 其他 - 失败
*  Error:          
*  Others:         
*************************************************/
long ADPT_AppGenErrBuf( char *eopstBuffer, char *epczErrNum, 
                     char * epczErrMsg )
{
    char headbuf[ADPT_CUPS_HEAD_LEN+1];
    char aczTmp39[3];
    char sBufLen[5];
    char sendbuf[1024*20];
    int  isendlen,iRet;
    long iBufLen = 0;

    memset( headbuf, 0x00, sizeof( headbuf ) );
    memset( sendbuf, 0x00, sizeof( sendbuf ) );
    memset( sBufLen, 0x00, sizeof( sBufLen ) );

    return -1;

    /*
    if( memcmp(epczErrNum,"30" , 2  ) == 0 )
        strcpy( epczErrNum , "A0" );
    else
        strcpy( epczErrNum , "06" );
    */
    memset( aczTmp39, 0x00, sizeof( aczTmp39 ) );
    //memcpy( aczTmp39, epczErrNum , 2 );
    memcpy( aczTmp39, "98" , 2 );
    TOOL_Log( LHINT , " 返回给银联的数据发送%d,errcode=[%s]",giInLen,epczErrNum);

    iRet = InsertFldValue( gaczInBuf, giInLen, 39, aczTmp39, 2, sendbuf,&isendlen);
    if( iRet < 0 )
    {
        TOOL_Log( LERROR, "InsertFldValue错，返回=[%d]", iRet );
        return -1;
    }
    TOOL_Log( LHINT , " 返回给银联的数据发送%d",giInLen);

    sendbuf[2] += 1 ;

    pkg_cupshead(headbuf);
    memcpy( eopstBuffer, headbuf, ADPT_CUPS_HEAD_LEN );

    iBufLen += ADPT_CUPS_HEAD_LEN;
    iBufLen += isendlen;

    sprintf( sBufLen, "%04.4d", iBufLen );
    memcpy( eopstBuffer+2, sBufLen, 4 );

    memcpy( eopstBuffer+46, sendbuf, isendlen);

    return -1;

    //return(iBufLen);
}

static int GetChlRqUIDByParseXml_exg(char *epcOrgMsg, int eiOrgMsgLen,
                               char oaczRqUID[ORGELSERIAL_LEN+1])
{
    return(SUCC);
}

int ADPT_AppGenChnSerl(char *epstBuffer, long iMsgLen, char *opczChnSerl)
{
    /*生成流水号*/ 

    return(SUCC);
}

int ADPT_VerMac_BfeAtm(SVC_Content stSvc, char *epczBuffer, int eiLen)
{
    return(SUCC);
}

/*生成MAC*/
int AppGenMac_cups( SVC_Content stSvc, char *pczRecvBuf , int eiLen, PFS eopfReqList)
{
    int iRet,flag=1;
    int iLen;
    char aczKeyBuf[512+1];
    char aczSeCtrl[17];
    char aczFld48[512];
    char aczFld96[128];
    int  iFldLen;
    memset(aczKeyBuf,0x00,sizeof(aczKeyBuf));
    
    if( memcmp( pczRecvBuf+46, "0830", 4 ) == 0 )
    {
        return 0;
    }
    if( memcmp( pczRecvBuf+46, "0630", 4 ) == 0 )
    {
        return 0;
    }
    if( memcmp( pczRecvBuf+46, "08", 2 ) == 0 )
    {
    	iFldLen = 0;
  
      memset( aczSeCtrl, 0 ,sizeof( aczSeCtrl ) );
      iRet = FS_GetField(eopfReqList, 53, 0,aczSeCtrl, &iFldLen);
      if( aczSeCtrl[1] == '6' || aczSeCtrl[1] == '4' )
      {
         memset( aczFld48, 0 ,sizeof( aczFld48 ) );
         iRet = FS_GetField(eopfReqList, 48, 0,aczFld48, &iFldLen);
         
         memset( aczKeyBuf, 0 ,sizeof( aczKeyBuf ) );
         Chg_Hex_To_Asc( aczFld48+2, 16, aczKeyBuf );
      }
      	
      else
      {
         memset( aczFld96, 0 ,sizeof( aczFld96 ) );
         iRet = FS_GetField(eopfReqList, 96, 0,aczFld96, &iFldLen);
         memset( aczKeyBuf, 0 ,sizeof( aczKeyBuf ) );
         Chg_Hex_To_Asc( aczFld96, 8, aczKeyBuf );
      }
    	
  
	TOOL_Log(LDEBUG, "retmsg[%s]", aczKeyBuf);
	iRet = BUS_KEYMAC( pczRecvBuf + 46 , flag, aczKeyBuf );
	if( iRet < 0 )
	{
	    TOOL_Log( LERROR, "校验MAC错,返回=[%d]", iRet );
	    return 1000;
	}
    }
    else if( memcmp( pczRecvBuf+46, "01", 2 ) == 0 ||
             memcmp( pczRecvBuf+46, "02", 2 ) == 0 ||
             memcmp( pczRecvBuf+46, "04", 2 ) == 0 ||
	     memcmp( pczRecvBuf+46, "05", 2 ) == 0 )
    {
        iRet = BUS_MAC( pczRecvBuf+46  , flag );
        if( iRet < 0 )
        {
            TOOL_Log( LERROR, "校验MAC错,返回=[%d]", iRet );
            return 1000;
        }
    }
    else
    {
        TOOL_Log( LERROR, "8583数据包错误" );
        return -1;
    }

    /*
    iRet=Jour_Update_exp( stSvc.iChannelId, stSvc.iSvcId , 0, eopfReqList, stSvc.aczElSerial, 0);
    if( iRet != 0 )
    {
        TOOL_Log( LERROR, "记前置流水失败[%d]", iRet );
    }
    */

    return 0;
}

/*************************************************
*  Function:       ADPT_TxMsgOutCustomDl
*  Description:    客户化处理
*  Table Accessed: 
*  Table Updated:  
*  Input:        
*                1)  epstBuffer   组包后生成的报文
*                2)  epstMsgHead  报文头信息
*                3)  epfRspList   FSS链
*  Output:         
*                4)  eopiMsgLen   消息长度
*  Return:       0 - 成功 其他 - 失败
*  Error:          
*  Others:         
*************************************************/
int ADPT_TxMsgOutCustomDl(char *eopczSndMsgBuf, 
                          ADPT_MsgHd_AdptKnl *epstMsgHead, 
                          PFS epfRspList, long *eopiMsgLen)
{
    long iMsgLen = 0;
    int  iTmp1Len = 0;
    int  iLen = 0;
    int  iNodeId = 0;
    int  iMsgDscrb = 0;
    int  iRet = 0;
    char pczRetCode[24+1];
    char pczRetMsg[256+1];
    SVC_Content  stService;
    char sBufLen[5];
    int  isendlen;
    long iBufLen = 0;

    char head6[3+1];
    char head8[8+1];
    char strTranCode[20];

    memset(head6, 0x00, sizeof(head6) );
    memset(head8, 0x00, sizeof(head8) );

    memset(pczRetCode, 0, sizeof(pczRetCode));
    memset(pczRetMsg, 0, sizeof(pczRetMsg));
    memset(&stService, 0, sizeof(stService));

    memset(recvcupshead, 0, sizeof(recvcupshead));
    iRet = FS_GetField(epfRspList, ADPT_FLD_CUPSHEAD_NO, 0, recvcupshead, &iLen);
    if (SUCC != iRet)
    {
        TOOL_Log(LERROR, "取银联头域号值失败 iRet[%d]", iRet);
        ADPT_AppGenErrBuf(eopczSndMsgBuf, "06", "取银联头域号值失败");
        return(FAIL);
    }

    memcpy( JGDM , recvcupshead+6 , 8 );
    JGDM[8]=0;
    TOOL_Log( LHINT, "JGDM =[%s]",JGDM);

    /*
     * 组包处理从FSS到字符串形式
    */
    TOOL_Log(LDEBUG, "nodeid[%s],colid[%s],msdig[%s],trancode[%s],"
                     "uuid[%s], dsb[%s]", 
                      epstMsgHead->aczNodeID, epstMsgHead->aczCorrelID, 
                      epstMsgHead->aczMsgID, epstMsgHead->aczTranCode, 
                      epstMsgHead->aczUUID, epstMsgHead->aczMsgDscrb);
    iNodeId = atoi(epstMsgHead->aczNodeID);
    iMsgDscrb = atoi(epstMsgHead->aczMsgDscrb);

    iMsgLen = *eopiMsgLen;
    iTmp1Len = iMsgLen;
    iRet = PKG_ToMsg(&stService, eopczSndMsgBuf, &iTmp1Len, 
                     iNodeId, epstMsgHead->aczTranCode,
                      2, iMsgDscrb, epfRspList );
    iMsgLen = iTmp1Len;
    *eopiMsgLen = iMsgLen;
    if (SUCC != iRet)
    {
        TOOL_Log(LERROR, "组报文失败nodeid[%d],trancode[%s],msgDsccb[%d]", 
                 iNodeId, epstMsgHead->aczTranCode, iMsgDscrb);
        if (iRet == PKG_SYSCOMB)
        {
            ADPT_AppGenErrBuf(eopczSndMsgBuf, gstGlobalInfo.stErr.aczChanRetCode, gstGlobalInfo.stErr.aczChanErrMsg );
	      }
        else
        {
            ADPT_AppGenErrBuf(eopczSndMsgBuf, "06", "组报文失败");
        }

        return FAIL;
    }
    TOOL_Log(LDEBUG, "===chl tomsg end");

    /*错误返回渠道(不重组)*/
    TOOL_Log(LDEBUG, "交换返回的重组模式[%c]", epstMsgHead->cPkgAsmMode);
    if (epstMsgHead->cPkgAsmMode == ADPT_PKGASM_ERRNOASM)
    {
        iRet = FS_GetField(epfRspList, ADPT_FLD_RETCODE, 0, pczRetCode, &iLen);
        if (SUCC != iRet)
        {
            TOOL_Log(LERROR, "从链表中取[%d]值失败",ADPT_FLD_RETCODE);
            return FAIL;
        }
        TOOL_Log(LDEBUG, "retcode[%s]", pczRetCode);

        iRet = FS_GetField(epfRspList, ADPT_FLD_RETMSG, 0, pczRetMsg, &iLen);
        if (SUCC != iRet)
        {
            TOOL_Log(LERROR, "从链表中取[%d]值失败",ADPT_FLD_RETMSG);
            return FAIL;
        }
        TOOL_Log(LDEBUG, "retmsg[%s]", pczRetMsg);

        *eopiMsgLen = ADPT_AppGenErrBuf(eopczSndMsgBuf, pczRetCode, pczRetMsg);
        if (*eopiMsgLen <= 0)
        {
            TOOL_Log(LERROR, "GenErrMsg失败");
            return FAIL;
        }

        /*for 监控*/
        //ADPT_TranInfoMonitor(&gstGlobalInfo, 1, epfRspList, gstGlobalInfo.aczElSerialNo);

        //return SUCC;
    }

    /*生成MAC*/
    if (epfRspList)
    {
        //  TOOL_Dump(5,"acct_cups_in_1/macdump_send.log", eopczSndMsgBuf+46, iMsgLen-46);
        iRet = AppGenMac_cups( stService, eopczSndMsgBuf, iMsgLen, epfRspList);
        if( iRet < 0 ) 
        {
            TOOL_Log( LERROR, "生成MAC失败[%d]", iRet );
            ADPT_AppGenErrBuf(eopczSndMsgBuf, "06", "生成MAC失败");
            return(FAIL);
            return (SUCC);
        }
    }

    //memset( stMsgSend.mtext, 0x00 , sizeof( stMsgSend.mtext ) );

    /*银联报文头处理*/
    memcpy(eopczSndMsgBuf, recvcupshead, ADPT_CUPS_HEAD_LEN);
    memcpy(eopczSndMsgBuf+6, recvcupshead+6+11, 11);
    memcpy(eopczSndMsgBuf+6+11, recvcupshead+6, 11);
    memcpy(eopczSndMsgBuf+ADPT_CUPS_HEAD_LEN-5, "00000", 5);
    memcpy(eopczSndMsgBuf+ADPT_CUPS_HEAD_LEN-5-1-8, "00000000", 8);

    iBufLen += ADPT_CUPS_HEAD_LEN;
    iBufLen += iMsgLen;
    sprintf( sBufLen, "%04.4d", iBufLen );
    memcpy( eopczSndMsgBuf+2, sBufLen, 4 );
    //TOOL_Dump(5,"acct_cups_in_1/macdump_send.log", eopczSndMsgBuf, iMsgLen);
    /*****************/

    /*
    memcpy(head6, recvcupshead+28, 3);
    memcpy(head8, recvcupshead+32, 8);

    TOOL_Log( LERROR, "H6[%s],H8[%s]", head6, head8);

    memcpy(eopczSndMsgBuf+28 ,head6 ,3 );
    memcpy(eopczSndMsgBuf+32 ,head8 ,8 );
    */

    /*
    memcpy( stMsgSend.mtext, epstBuffer->pcBuf, epstBuffer->iLen);
    stMsgSend.mtype = 100 ;

    TOOL_Log( LERROR , " 机构号[%s] ",JGDM);
    if( iRet = GetYLMsgId(JGDM,&sendid,&recvid) !=  0 )
    {
        TOOL_Log( LERROR, "取得消息队列ID失败,[%d]", iRet );
        return -2;
    }
    TOOL_Log( LDEBUG , " 取得机消息id[%d] ",sendid);
    */

    /*for 监控*/
    ADPT_TranInfoMonitor(&gstGlobalInfo, 1, epfRspList, gstGlobalInfo.aczElSerialNo);
    
    /* 增加反欺诈接口  by zhouyb 20160822 start */
    /* for 风控 */
    /* 获取交易码 */
    memset(strTranCode,0x00,sizeof(strTranCode));
    strcpy(strTranCode,gstGlobalInfo.aczTranCode);
    
    TOOL_Log( LHINT, "交易码为[%s]",strTranCode);

    if(0 == memcmp(strTranCode,"200000",6) || 0 == memcmp(strTranCode,"202000",6) ||
    	 0 == memcmp(strTranCode,"420000",6) || 0 == memcmp(strTranCode,"422000",6) ||
    	 0 == memcmp(strTranCode,"100306",6) || 0 == memcmp(strTranCode,"102006",6) ||
    	 0 == memcmp(strTranCode,"420306",6) || 0 == memcmp(strTranCode,"422016",6) ||
    	 0 == memcmp(strTranCode,"200006",6) || 0 == memcmp(strTranCode,"202006",6) ||
    	 0 == memcmp(strTranCode,"420006",6) || 0 == memcmp(strTranCode,"422026",6))
    {
        TOOL_Log( LHINT, "调用风控函数开始...");
        ADPT_TranFraud(&gstGlobalInfo,1,epfRspList,gstGlobalInfo.aczElSerialNo);
        TOOL_Log( LHINT, "调用风控函数结束...");
    }
    /* 增加反欺诈接口  by zhouyb 20160822 end */

    return(SUCC);

}


/*************************************************
*  Function:       ADPT_SetTuxSvr
*  Description:    客户化处理: 取服务名
*  Table Accessed:
*  Table Updated:
*  Input:
*                1)  epczSvcName - 服务名
*  Return:       0 - 成功 其他 - 失败
*  Error:
*  Others:
*************************************************/
int ADPT_SetTuxSvr(char *epczSvcName)
{
EXEC SQL BEGIN DECLARE SECTION;
    char aczSvrName[20+1];
    char aczStatus[1+1];
    char aczSvrName_2[20+1];
    char aczStatus_2[1+1];
EXEC SQL END DECLARE SECTION;

    memset(aczSvrName, 0x00, sizeof(aczSvrName));
    memset(aczStatus, 0x00, sizeof(aczStatus));
    memset(aczSvrName_2, 0x00, sizeof(aczSvrName_2));
    memset(aczStatus_2, 0x00, sizeof(aczStatus_2));

    /*设置脏读*/
    EXEC SQL SET ISOLATION TO DIRTY READ;
    if (SQLCODE)
    {
	TOOL_Log(LERROR, "设置脏读失败sqlcode=[%d]", SQLCODE);
	return(FAIL);
    }
    
    EXEC SQL SELECT status,reserve1 INTO :aczStatus,:aczSvrName
				 FROM  ta_hoststatus
				   WHERE host_id='yl1';
    if (SQLCODE)
    {
        TOOL_Log(LERROR, "查询主通讯线路失败sqlcode=[%d]", SQLCODE);
        return(FAIL);
    }
    PUB_StrTrim(aczStatus, ' ');
    PUB_StrTrim(aczSvrName, ' ');

    EXEC SQL SELECT status,reserve1 INTO :aczStatus_2,:aczSvrName_2
				FROM  ta_hoststatus
				  WHERE host_id='yl2';
    if (SQLCODE)
    {
        TOOL_Log(LERROR, "查询备通讯线路失败sqlcode=[%d]", SQLCODE);
        return(FAIL);
    }
    PUB_StrTrim(aczStatus_2, ' ');
    PUB_StrTrim(aczSvrName_2, ' ');

    if (aczStatus[0] == '0' && aczStatus_2[0] == '0')
    {
        return(SUCC);
    }
    else if (aczStatus[0] == '0')
    {
	memcpy(epczSvcName, aczSvrName, strlen(aczSvrName));
	TOOL_Log(LDEBUG, "======svcname1[%s] aczSvrname[%s]", epczSvcName, aczSvrName);
    }
    else if (aczStatus_2[0] == '0')
    {
        memcpy(epczSvcName, aczSvrName_2, strlen(aczSvrName_2));
        TOOL_Log(LDEBUG, "======svcname2[%s] aczSvrName_2[%s]", epczSvcName, aczSvrName_2);
    }
    else
    {
	TOOL_Log(LERROR, "通讯机两条通讯线路都发生异常");
	return(FAIL);
    }
    return(SUCC);
}


/*************************************************
*  Function:       ADPT_TranFraud
*  Description:    将交易信息转换给反欺诈接口
*  Table Accessed:
*  Table Updated:
*  Input:          
*		   1)stGlobal       - 全局信息 
*		   2)pfReqList      - FSS链 
*		   3)epczSerial     - 交易流水 
*  Output:         
*  Return:         0 - 成功 其他 - 失败
*  Error:
*  Others:
*************************************************/
int ADPT_TranFraud(GlobalInfo *stGlobal, int itype, PFS pfReqList, char *epczSerial)
{  
    int iRet=0;
    int currentTLVIndex=0;
    char szSendMessage[1024];
    char strBuf[200];
    void* pSession = 0;
    GlobalInfo sTmpGlobalInfo;
    struct tm tmNow;
    struct timeval tvNow;
    int  iFldLen;
    char Finance_Net_Data[1000];
    char acz63Flag[2 + 1];
    char temp[1024+1];
    int  currentIndex = 0;
    char strValue[1024];
    char strTranCode[10];
    char strCurrency[4];
    char strAmount[20];
    char str_reserved[1000];
    char dishextag[4 + 1];
    char valuelen[9 + 1];
    char dishexvalue[1024 + 1];
    char str_cardtype[30];
    char aczCfgFile[500];
    char szServerIP[50];
    char strPort[10];
    int nPort;
    char szLogConfigFilePath[500];
    int nResult;
    char aczDBName[100];
    char strValues[100];
    Boscardbin stBoscardbin;
    
    typedef struct _TLVEntity
    {
        char Tag[2 + 1];
        char Length[1 + 1];
        char Value[128 +1];
        int TagSize;
        int LengthSize;
        int ValueSize;
    }TLVEntity;
    TLVEntity tlvEntity[30];
    memset( tlvEntity ,0x00, sizeof(TLVEntity) * 30 );
    
    
    memset(&sTmpGlobalInfo, 0x00, sizeof(GlobalInfo));
    memset(szSendMessage,0x00,sizeof(szSendMessage));
    memset(Finance_Net_Data,0x00,sizeof(Finance_Net_Data));
    memset(acz63Flag,0x00,sizeof(acz63Flag));
    memset(temp,0x00,sizeof(temp));
    memset(strValue,0x00,sizeof(strValue));
    memset(strTranCode,0x00,sizeof(strTranCode));
    
    memcpy(&sTmpGlobalInfo, stGlobal, sizeof(GlobalInfo));
    
    //组报文接口
    /* 交易唯一流水号 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,epczSerial);
    TOOL_Log(LDEBUG,"交易流水号=[%s]",strValue);
    sprintf(strBuf,"{\"transJnlno\":\"%s\",",strValue);
    strcpy(szSendMessage,strBuf);
    
    /* 交易时间 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    gettimeofday(&tvNow, NULL);
    memcpy(&tmNow, localtime(&tvNow.tv_sec), sizeof(tmNow));
    sprintf(strValue,"%04d%02d%02d%02d%02d%02d",tmNow.tm_year + 1900, 
               tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
    TOOL_Log(LDEBUG,"交易时间=[%s]",strValue);
    sprintf(strBuf,"\"transDateTime\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    
    /* 渠道类型 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(str_reserved,0x00,sizeof(str_reserved));
    iRet = FS_GetField(pfReqList, 60, 0, str_reserved,  &iFldLen);     // 获取60域值
    if (SUCC != iRet)
    {
        TOOL_Log(LERROR, "取60域值失败 iRet[%d]", iRet);
        return -1;
    }    
	  TOOL_Log( LDEBUG, "60 域 =[%s]",str_reserved);	
	      
    memset(strBuf,0x00,sizeof(strBuf));
    iRet = FS_GetField(pfReqList, 63, 0, Finance_Net_Data,  &iFldLen); // 获取63域值
    if (SUCC != iRet)
    {
        TOOL_Log(LERROR, "取63域值失败 iRet[%d]", iRet);
        return -1;
    }
	  TOOL_Log( LDEBUG, "63 域 =[%s]",Finance_Net_Data);
	  
    if(!strncmp(str_reserved + 11, "4", 1))
    {
        memcpy(acz63Flag, Finance_Net_Data, 2);
        if(!strncmp(acz63Flag, "TK", 2))
        {
            memcpy(temp,Finance_Net_Data + 5,sizeof(temp));
        }
        else
        {
            memcpy(temp,Finance_Net_Data + 26,sizeof(temp));

        }

        while(currentIndex < strlen(temp))
        {
            //T
            if((temp[currentIndex] & 0x1f) == 0x1f)
            {
                tlvEntity[currentTLVIndex].TagSize = 2;
                memcpy(tlvEntity[currentTLVIndex].Tag, temp + currentIndex, tlvEntity[currentTLVIndex].TagSize);
                currentIndex += tlvEntity[currentTLVIndex].TagSize;
            }
            else
            {
                tlvEntity[currentTLVIndex].TagSize = 1;
                memcpy(tlvEntity[currentTLVIndex].Tag, temp + currentIndex, tlvEntity[currentTLVIndex].TagSize);
                currentIndex += tlvEntity[currentTLVIndex].TagSize;
            }
            memset( dishextag, 0x00, sizeof(dishextag) );
            Bin2Hex( (unsigned char *)tlvEntity[currentTLVIndex].Tag, dishextag, tlvEntity[currentTLVIndex].TagSize );
            //TOOL_Log( LERROR, "[%d]TAG   [%s]", currentTLVIndex, dishextag );
            //L
            if((temp[currentIndex] & 0x80) != 0x80)
            {
                tlvEntity[currentTLVIndex].LengthSize = 1;
                memcpy(tlvEntity[currentTLVIndex].Length, temp + currentIndex, 1);
                memset(valuelen, 0x00, sizeof( valuelen ) );
                sprintf(valuelen, "%d", tlvEntity[currentTLVIndex].Length[0]);
                tlvEntity[currentTLVIndex].ValueSize=atoi(valuelen);
                currentIndex += tlvEntity[currentTLVIndex].LengthSize;
            }
            //TOOL_Log( LERROR, "[%d]LENGTH[%02X][%d]", currentTLVIndex, tlvEntity[currentTLVIndex].Length[0],tlvEntity[currentTLVIndex].ValueSize );
            //V
            memcpy(tlvEntity[currentTLVIndex].Value, temp + currentIndex, tlvEntity[currentTLVIndex].ValueSize);
            //TOOL_Log( LERROR, "[%d]VALUE [%s]", currentTLVIndex, tlvEntity[currentTLVIndex].Value );
            memset( dishexvalue, 0x00, sizeof(dishexvalue) );
            Bin2Hex( (unsigned char *)tlvEntity[currentTLVIndex].Value, dishexvalue, tlvEntity[currentTLVIndex].ValueSize );
            //TOOL_Log( LERROR, "[%d]VALUE [%s]", currentTLVIndex, dishexvalue );
            currentIndex += tlvEntity[currentTLVIndex].ValueSize;
            
            if(!strncmp(dishextag, "05", 2) )
            {
                if(!strncmp(tlvEntity[currentTLVIndex].Value, "01", 2) ) //AM
                {
                    strcpy(strValue,"50");                 
                }
                else if(!strncmp(tlvEntity[currentTLVIndex].Value, "02", 2) ) //HCE
                {
                    strcpy(strValue,"60");                  
                }
            }
            if(!strncmp(dishextag,"08",2))
            {
                if(!strncmp(dishexvalue+6, "41", 2) )             //HW
                {
                    strcpy(strValue,"40");
                } else if (!strncmp(dishexvalue+6, "40", 2)){     //AM                         
                    strcpy(strValue,"50");
                } else {
                    strcpy(strValue," ");
                }
            }
            /* and  end */
        }
        TOOL_Log(LERROR, "渠道标志[%s]",strValue);
    } else {
        // 非华为Pay,直接结束
        TOOL_Log(LERROR, "非Pay支付");
        return -1;
    }
    sprintf(strBuf,"\"sourceName\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    
    /* 业务类型 */
    memset(strBuf, 0x00, sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"1003");	
    sprintf(strBuf,"\"transCode\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "业务类型 = [%s]",strValue);
    
    /* 交易类型 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strTranCode,sTmpGlobalInfo.aczTranCode);
    TOOL_Log( LDEBUG, "交易码=[%s]",strTranCode);
    
    if(0 == memcmp(strTranCode,"200000",6))         //消费
    {  
        strcpy(strValue,"401");
    } else if(0 == memcmp(strTranCode,"202000",6)){ //消费撤销
        strcpy(strValue,"402");
    } else if(0 == memcmp(strTranCode,"420000",6)){ //消费冲正
        strcpy(strValue,"403");
    } else if(0 == memcmp(strTranCode,"422000",6)){ //消费撤销冲正
        strcpy(strValue,"404");
    } else if(0 == memcmp(strTranCode,"100306",6)){ //预授权
        strcpy(strValue,"405");
    } else if(0 == memcmp(strTranCode,"102006",6)){ //预授权撤销
        strcpy(strValue,"406");
    } else if(0 == memcmp(strTranCode,"420306",6)){ //预授权冲正
        strcpy(strValue,"407");
    } else if(0 == memcmp(strTranCode,"422016",6)){ //预授权撤销冲正
        strcpy(strValue,"408");
    } else if(0 == memcmp(strTranCode,"200006",6)){ //预授权完成
        strcpy(strValue,"409");
    } else if(0 == memcmp(strTranCode,"202006",6)){ //预授权完成撤销
        strcpy(strValue,"410");
    } else if(0 == memcmp(strTranCode,"420006",6)){ //预授权完成冲正
        strcpy(strValue,"411");
    } else if(0 == memcmp(strTranCode,"422026",6)){ //预授权完成撤销冲正
        strcpy(strValue,"412");
    }
    TOOL_Log( LDEBUG, "交易类型 = [%s]",strValue);
    sprintf(strBuf,"\"transAcType\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    
    /* 交易主体账户名称 ：持卡人姓名 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"");
    sprintf(strBuf,"\"payerAcName\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "持卡人姓名 = [%s]",strValue);
    
    /* 交易主体账户号码 ：卡号 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    iRet = FS_GetField(pfReqList, 2, 0, strValue,  &iFldLen); // 取2域的值
    if (SUCC != iRet)
    {
        TOOL_Log(LERROR, "取2域值失败 iRet[%d]", iRet);
        return -1;
    }
    TOOL_Log(LDEBUG,"2 域 =[%s]",strValue);
    sprintf(strBuf,"\"payerAcNo\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "交易主体账户号码  = [%s]",strValue);
    
    /* 交易主体唯一标识 ：客户号  */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"");
    sprintf(strBuf,"\"userId\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "交易主体唯一标识 = [%s]",strValue);
    
    /* 交易币种 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    memset(strCurrency,0x00,sizeof(strCurrency));
    iRet = FS_GetField(pfReqList, 49, 0, strCurrency,  &iFldLen); //获取49域的值
    if (SUCC != iRet)
    {
        TOOL_Log(LERROR, "取49域值失败 iRet[%d]", iRet);
        return -1;
    }
	  TOOL_Log( LDEBUG, "49 域 =[%s]",strCurrency);
	  if(0==memcmp(strCurrency,"156",3)){ //人民币
        strcpy(strValue,"01");
	  }else{
        strcpy(strValue,"02");        //其他币种
	  }
	  TOOL_Log( LDEBUG, "交易币种 = [%s]",strValue);
    sprintf(strBuf,"\"transCurrency\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    
    /* 交易金额 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    memset(strAmount,0x00,sizeof(strAmount));
    iRet = FS_GetField(pfReqList, 4, 0, strAmount,  &iFldLen); //获取4域值
    if (SUCC != iRet)
    {
        TOOL_Log(LERROR, "取4域值失败 iRet[%d]", iRet);
        //return -1;
    }
    TOOL_Log( LDEBUG, "4域值 =[%s]",strAmount);
    strcpy(strValue,strAmount);
    sprintf(strBuf,"\"transAmount\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "交易金额 = [%s]",strValue);
    
    /* 卡类型 01:借记；02:贷记 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(str_cardtype,0x00,sizeof(str_cardtype));
    memset(strValue,0x00,sizeof(strValue));
    iRet = FS_GetField(pfReqList, 2, 0, strValue,  &iFldLen); // 取2域的值
    if (SUCC != iRet)
    {
        TOOL_Log(LERROR, "取2域值失败 iRet[%d]", iRet);
        //return -1;
    } 
    PUB_StrTrim(strValue, ' ');
    memset(&stBoscardbin, 0x00, sizeof(stBoscardbin));
    memcpy(stBoscardbin.cardbin, strValue, 6);
    TOOL_Log(LERROR, "卡bin = [%s]", stBoscardbin.cardbin);
    
    memset(aczDBName, 0x00, sizeof(aczDBName));
    snprintf(aczDBName, sizeof(aczDBName), "%s", getenv("ELRUNDBNAME"));
    TOOL_Log(LDEBUG,"数据库名称 aczDBNAME = [%s]",aczDBName);
    
    iRet = DB_Open(aczDBName);
    if (0 != iRet)
    {
        TOOL_Log(LERROR, "打开数据库[%s]失败！sqlcode = [%d]", aczDBName, iRet);
        return -1;
    }
    
    iRet = DB_UnqBoscardbin(&stBoscardbin, DBS_FIND);
    if(iRet != 0 && iRet != 100)
    {
        TOOL_Log(LERROR, "查询数据库出错[%d]", iRet);
        return -1;
    }   
    else if(iRet == 0)
    {
        if(!memcmp(stBoscardbin.flag,"2",1)){            // 本行借记卡
            memcpy(str_cardtype,"01",2);                 // 借记卡上送:"01"
        } else if(!memcmp(stBoscardbin.flag,"3",1)){     // 本行贷记卡
            memcpy(str_cardtype,"02",2);                 // 贷记卡上送:"02"
        } else {
            memcpy(str_cardtype,"03",2);                 // 其他类型 
        }
    }
    sprintf(strBuf,"\"cardtype\":\"%s\",",str_cardtype); 
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "卡类型 = [%s]",str_cardtype);
    
    DB_Close();
    
    /* IP地址 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"");
    sprintf(strBuf,"\"ipAddress\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "IP地址 = [%s]",strValue);
    
    /* MAC地址 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"");
    sprintf(strBuf,"\"macAddress\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "MAC地址 = [%s]",strValue);
    
    /* 交易主体名称 -持卡人姓名 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"");
    sprintf(strBuf,"\"nickName\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "交易主体名称 = [%s]",strValue);
    
    /* 交易主体证件类型  */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"");
    sprintf(strBuf,"\"identityType\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "交易主体证件类型 = [%s]",strValue);
    
    /* 交易主体证件号码  */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"");
    sprintf(strBuf,"\"identityNo\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "交易主体证件号码 = [%s]",strValue);
    
    /* 账户类型 0:卡；1:折  */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"0");
    sprintf(strBuf,"\"accounttype\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "账户类型 = [%s]",strValue);
    
   /* 交易状态 */    
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    
    TOOL_Log( LDEBUG, "标志 = [%d]",itype);
    if(itype == 0){
        strcpy(strValue,"1"); // 1：为请求状态
    }else if(itype == 1){
         iRet = FS_GetField(pfReqList, 2057, 0, strValue,  &iFldLen); // 取域的值
         TOOL_Log( LDEBUG, "响应码域2057值 = [%s]",strValue);
         if (SUCC != iRet)
         {
             TOOL_Log(LERROR, "取3域值失败 iRet[%d]", iRet);
             return -1;
         }
         if(0==memcmp(strValue,"0000",4)){
             strcpy(strValue,"0"); // 0:成功
         } else { 
             strcpy(strValue,"2"); // 2:失败
         }
    }
    sprintf(strBuf,"\"transStatus\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "交易状态 = [%s]",strValue);
    
    /* 交易对手账户名称 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    strcpy(strValue,"");
    sprintf(strBuf,"\"payeeAcName\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "交易对手账户名称 = [%s]",strValue);
    
    /* 交易对手账户号码 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));

    sprintf(strBuf,"\"payeeAcNo\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "交易对手账户号码 = [%s]",strValue);
    
    /* 支付类对应商户ID */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    iRet = FS_GetField(pfReqList, 42, 0, strValue,  &iFldLen); //42域值“受卡方标识码”
    if (SUCC != iRet)
    {
        TOOL_Log(LERROR, "取42域值失败 iRet[%d]", iRet);
        return -1;
    }
    sprintf(strBuf,"\"paycommercialtenant\":\"%s\",",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "支付类型对应商户ID = [%s]",strValue);
    
    /* 借贷标志 */
    memset(strBuf,0x00,sizeof(strBuf));
    memset(strValue,0x00,sizeof(strValue));
    memcpy(strValue,str_cardtype,2);
    sprintf(strBuf,"\"debitOrcredit\":\"%s\"}",strValue);
    strcat(szSendMessage,strBuf);
    TOOL_Log( LDEBUG, "借贷标志 = [%s]",strValue);
    
    // 风控接口报文
    TOOL_Log(LDEBUG,"组风控接口报文 szSendMessage=[%s]",szSendMessage);
    
    // 调用反欺诈系统API - ShortSendMsg 只发送不接收信息返回
    /* 读取配置文件cfca.cfg */
    memset(aczCfgFile,0x00,sizeof(aczCfgFile));
    snprintf(aczCfgFile, sizeof(aczCfgFile), "%s/etc/cfca.cfg", getenv("ELDIR"));
    TOOL_Log(LDEBUG, "配置文件CONFIG路径 = [%s]", aczCfgFile );
    
    memset(szServerIP,0x00,sizeof(szServerIP));
    iRet = ReadCfg(aczCfgFile, "CFCA", "IP", szServerIP, 1);
    if (0 != iRet)
    {
        TOOL_Log(LERROR, "取返回CFCA IP error[%d]", iRet);
        return -1;
    }
    TOOL_Log( LDEBUG, "风控系统IP = [%s]",szServerIP);

    memset(strPort,0x00,sizeof(strPort));
    iRet = ReadCfg(aczCfgFile, "CFCA", "PORT", strPort, 1);
    if (0 != iRet)
    {
        TOOL_Log(LERROR, "取返回CFCA Port error[%d]", iRet);
        return -1;
    }
    TOOL_Log( LDEBUG, "风控系统PORT = [%s]",strPort);
    
    nPort = atoi(strPort);
    
    memset(strValues,0x00,sizeof(strValues));
    iRet = ReadCfg(aczCfgFile, "CFCA", "FILEPATH", strValues, 1);
    if (0 != iRet)
    {
        TOOL_Log(LERROR, "取返回CFCA filePath error[%d]", iRet);
        return -1;
    }
    
    memset(szLogConfigFilePath,0x00,sizeof(szLogConfigFilePath));
    sprintf(szLogConfigFilePath, "%s%s", getenv("WORKDIR"),strValues);
    TOOL_Log( LDEBUG, "风控系统日志路径 = [%s]",szLogConfigFilePath);
    
    nResult = CFCA_Initialize(szServerIP, nPort, szLogConfigFilePath, 10); 
    DEMO_CHECK_ERROR(CFCA_OK != nResult, nResult, "CFCA_Initialize");
    nResult = CFCA_Connect(&pSession); 
    nResult = SendMsgtoServer(pSession, szSendMessage);
    DEMO_CHECK_ERROR(CFCA_OK != nResult, nResult, "CFCA_Connect");
    nResult = CFCA_Disconnect(&pSession);
    DEMO_CHECK_ERROR(CFCA_OK != nResult, nResult, "CFCA_Connect");
    pSession = NULL;
    
    nResult = CFCA_OK;

_err:
    if (NULL != pSession)
    {
        CFCA_Disconnect(&pSession);
        pSession = NULL;
    }
    
    return 0;
}

/*
 * END OF FILE
 */
