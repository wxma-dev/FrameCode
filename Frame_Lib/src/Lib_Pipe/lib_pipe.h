/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    lib_string.h                              *
* Description:    关于内存、字符串的lib                     *
* Author:         mawx                                      *
* Create Date:    2015/05/14                                *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/

#ifndef __LIB_STRING_H__
#define __LIB_STRING_H__

#ifdef __cplusplus
extern "C" {
#endif


/* 将shell执行的结果输出 */
char * DoShell_ByPipe( char *aczCommand, char * sOut_Buf );



#ifdef __cplusplus
}
#endif

#endif

