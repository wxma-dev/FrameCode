/*************************************************************
*
*    Copyright (c) 2000-2005 GIT Ltd.
*    All rights reserved
*
*    This is unpublished proprietary source code of GIT Ltd.
*    The copyright notice above does not evidence any actual
*    or intended publication of such source code.
*
*    NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
*    SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
*
*    Program: 
*    Version:
*    Description:
*    Date:      Version:      Author:      Description:
*    
*************************************************************/

#include "asm_fdef.h"

#ifndef __ASM_REG_H__
#define __ASM_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ADDASM( funname ) { "ASM_"#funname, ASM_##funname }

/* 重组函数地址结构 */
typedef struct
{
   char * pczFuncName;
   int  (* pFunc)( SVC_Content *epstService, PFS epcSrcList,
                   int eiMsgAppId, int eiFldId, PFS opcResutlList );
} AsmFuncEntry;

AsmFuncEntry astAsmEntry[] = {
   ADDASM( AddStr ),
   ADDASM( FldCopy ),
   ADDASM( FldCopy1 ),
   ADDASM( FldCopyToStrM100 ),
   ADDASM( GenPassword ),
   ADDASM( GenUUID ),
   ADDASM( GetCashAcctNo ),
   ADDASM( GetChlRtnCode ),
   ADDASM( GetChlRtnMsg ),
   ADDASM( GetDateTime ),
   ADDASM( GetElSerial ),
   ADDASM( GetHostRtnCode ),
   ADDASM( GetLast11Bit ),
   ADDASM( GetMediumType ),
   ADDASM( GetOFSLoopTime ),
   ADDASM( GetUUID ),
   ADDASM( StrM100ToStr ),
   ADDASM( StrToStrM100 ),
   ADDASM( SysDate ),
   ADDASM( SysTime ),
   ADDASM( TEST_001 ),
   ADDASM( Using_01 ),
   ADDASM( using_05 ),
};

#ifdef __cplusplus
}
#endif

#endif

/*
**end of file
*/
