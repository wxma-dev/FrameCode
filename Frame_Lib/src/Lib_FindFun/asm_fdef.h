/*
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
 *    PROGRAM_NAME :
 *    SYSTEM_NAME  : EasyLink
 *    VERSION      : 3.x
 *    DESCRIPTION  :
 *    HISTORY      : 
 *    YYYY/MM/DD        Position        Author         Description
 *    -------------------------------------------------------------
 *    2004/03/04        Beijing         ZWL            Creation
 */


/* Unix Head File */
//#include "sysunix.h"

#ifndef __ASM_FDEF_H__
#define __ASM_FDEF_H__
//#include "pubhead.h"
//#include "pkg.h"

/*
int ASM_AddStr( SVC_Content *epstService, PFS epcSrcList,
              int eiMsgAppId, int eiFldId, PFS opcResutlList );

int ASM_FldCopy( SVC_Content *epstService, PFS epcSrcList,
              int eiMsgAppId, int eiFldId, PFS opcResutlList );

int ASM_FldCopy1( SVC_Content *epstService, PFS epcSrcList,
              int eiMsgAppId, int eiFldId, PFS opcResutlList );

int ASM_FldCopyToStrM100( SVC_Content *epstService, PFS epcSrcList,
              int eiMsgAppId, int eiFldId, PFS opcResutlList );
*/
int ASM_Personprintf( void * args );

int ASM_Personprintfb( void * args );

#endif

/*
** end of file
*/

