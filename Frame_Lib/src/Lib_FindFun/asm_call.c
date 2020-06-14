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
#include "asm_reg.h"

#include <stdio.h>

/* asm.h */
#define ASM_BASE                300
#define ASM_SUCC                0
#define ASM_FAIL                ASM_BASE
#define ASM_SRCNOFOUND          ASM_BASE + 1
#define ASM_STDSRCNOFOUND       ASM_BASE + 2
#define ASM_SUBREQCFGNOFOUND    ASM_BASE + 3
#define ASM_NODEMAPREQNOFOUND   ASM_BASE + 4
#define ASM_TYPE_ERR            ASM_BASE + 5
#define ASM_SRCTYPE_ERR         ASM_BASE + 6
#define ASM_FAIL_ERR            ASM_BASE + 7



#define CONVERT_FUN_LEN                       32


static int ASM_CompareFunc(const void * node1, const void * node2)
{
    return strcmp(((AsmFuncEntry *)node1)->pczFuncName,
                 ((AsmFuncEntry *)node2)->pczFuncName);
}


int ASM_Running( char *AsmFuncName, void * args )
{
    int iRet;
    int iMaxFunId;
    char aczAsmFunc[ CONVERT_FUN_LEN + 1 ];
    
    AsmFuncEntry stComAsm, *pstAsm;
    
    memset( aczAsmFunc, 0x00, sizeof( aczAsmFunc ) );
    
    /* ������麯���ŵĺϷ��� */
    if( AsmFuncName == NULL ) {
        //TOOL_Log( LERROR, "���麯���Ƿ�" );
        return ASM_FAIL;
    }
    
    iMaxFunId = sizeof( astAsmEntry ) / sizeof( AsmFuncEntry );
    
    strcpy( aczAsmFunc, AsmFuncName );
    //PUB_StrTrim( aczAsmFunc, ' ' );
    if( strlen( aczAsmFunc ) == 0 )
    //    strcpy( aczAsmFunc, "ASM_FldCopy" );
        return ASM_FAIL;
    
    stComAsm.pczFuncName = aczAsmFunc;
    
    pstAsm = bsearch( &stComAsm, astAsmEntry, iMaxFunId, sizeof(AsmFuncEntry),
                      ASM_CompareFunc );
    if( pstAsm == NULL ) {
        //TOOL_Log( LERROR, "���麯��[%s]������", aczAsmFunc );
        return ASM_FAIL;
    }
    
    /*
    ** �������麯��,���ݵ�ǰ������ĵ�ǰ�ε���Դ��ֵ����,
    ** ������ǰ������ĵ�ǰ�εĽ��ֵ���� 
    */
    iRet = pstAsm->pFunc( args );
    if( iRet != ASM_SUCC ) {
        //TOOL_Log( LERROR, "���麯��ִ�д���.����[%s].��[%d].����ֵΪ[%d]",
        //          aczAsmFunc, eiFldId, iRet );
        if( iRet == ASM_FAIL_ERR )
            return ASM_FAIL_ERR;
        return ASM_FAIL;
    }
    
    return ASM_SUCC;

}



/*
** EasyLink 3.6
*/

