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

#define __EXTERN_VAR__
#include "asm.h"
#include "asm_reg.h"

static int ASM_CompareFunc(const void * node1, const void * node2)
{
   return strcmp(((AsmFuncEntry *)node1)->pczFuncName,
                 ((AsmFuncEntry *)node2)->pczFuncName);
}

int ASM_Running( char *epczAsmFunc, SVC_Content * epstService, PFS epcSrcList,
                 int eiFldId, int eiFieldType, PFS opcResultList )
{
   int iRet;
   int iMaxFunId;
   char aczAsmFunc[ CONVERT_FUN_LEN + 1 ];

   AsmFuncEntry stComAsm, *pstAsm;

   memset( aczAsmFunc, 0x00, sizeof( aczAsmFunc ) );

   /* 检查重组函数号的合法性 */
   if( epczAsmFunc == NULL ) {
      TOOL_Log( LERROR, "重组函数非法" );
      return ASM_FAIL;
   }

   iMaxFunId = sizeof( astAsmEntry ) / sizeof( AsmFuncEntry );

   strcpy( aczAsmFunc, epczAsmFunc );
   PUB_StrTrim( aczAsmFunc, ' ' );
   if( strlen( aczAsmFunc ) == 0 )
      strcpy( aczAsmFunc, "ASM_FldCopy" );

   stComAsm.pczFuncName = aczAsmFunc;

   pstAsm = bsearch( &stComAsm, astAsmEntry, iMaxFunId, sizeof(AsmFuncEntry),
                     ASM_CompareFunc );
   if( pstAsm == NULL ) {
      TOOL_Log( LERROR, "重组函数[%s]不存在", aczAsmFunc );
      return ASM_FAIL;
   }

   /*
   ** 调用重组函数,根据当前请求域的当前次的来源域值链表,
   ** 创建当前请求域的当前次的结果值链表 
   */
   iRet = pstAsm->pFunc( epstService, epcSrcList, eiFieldType, eiFldId, 
                         opcResultList );
   if( iRet != ASM_SUCC ) {
      TOOL_Log( LERROR, "重组函数执行错误.函数[%s].域[%d].返回值为[%d]",
                aczAsmFunc, eiFldId, iRet );
      if( iRet == ASM_FAIL_ERR )
         return ASM_FAIL_ERR;
      return ASM_FAIL;
   }

   return ASM_SUCC;

}

/*
** EasyLink 3.6
*/

