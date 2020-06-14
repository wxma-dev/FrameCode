/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    lib_string.h                              *
* Description:    �����ڴ桢�ַ�����lib                     *
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


/* ����һ���µ��ڴ棬���ַ����������� */
char * AddStr_Malloc( const char * sAdd_String, char ** pDst_Mem );

/* ���·�����ڴ��У���β������ַ���        *
* ԭ�ڴ�Ҳ������malloc���ͺ�������Ķ�̬�ڴ�*
* �·�����ڴ泤�� ԭ�ڴ��ַ�������+����ַ����ĳ���+1 */
char * AddStr_Realloc( char * pSrc_Mem, const char * sAdd_String, char ** pDst_Mem );

/* �ͷ�AddStr_Realloc��AddStr_Malloc����ڴ� */
void AddStr_Free( char ** pPoint_Mem );

/* �ж�Դ�ַ����е��Ӵ��ĸ��� */
int Count_ChildStr_Num( char * sSrc_String, char * sChild_String );

/* �ж�Դ�ַ����е�ĳ���ַ��ĸ��� */
int Count_Char_Num( char * sSrc_String, char sChild_Char );

/* �ַ�ת�����ַ��� */
char * Char_To_Str( const char cSrc_Char, char * sDst_String );

/* �ַ���ת�����ַ����ַ�����һλΪ׼��*/
char * Str_To_Char( const char * sSrc_String, char sDst_Char );

/* ��ȡ��ǰʱ���YYYYMMDD */
void GetString_YYYYMMDD( char * pPoint_TimeMem );

/* ��ȡ��ǰʱ���HHMMSS */
void GetString_HHMMSS( char * pPoint_TimeMem );

/* ��ȡ��ǰʱ��ļ������ʱ�䣨������*/
void GetString_ComputerSec( char * pPoint_TimeMem );

/* �ַ������� *
* ���Ŀ���ַ�������С��Դ�ַ����ĳ��ȣ����δ��֪*/
char *Dx_String( char * sSrc_string, char * sDst_string );

/* �ַ�������(ֱ����Դ�ַ����Ŀռ��ϵ���) */
char *Dx_String_Yzfc( char * sSrc_string );

/* ȥ���ַ����е�ĳһ�ַ�(ȥ��ĳ�ַ���Դ�ַ����ϲ�)*
* �������ַ����ַ����в����ڣ�����Ϊɾ���ɹ� */
char * Del_Char( char * sSrc_String, char cChar );

/* ȥ���ַ����е�ĳһ�ַ�(ȥ��ĳ�ַ���Ŀ���ַ����ϲ�)*
* �������ַ����ַ����в����ڣ�����Ϊɾ���ɹ�*/
char * Del_Char_CopyToDst( char * sSrc_string, char * sDst_string, char sCh );

/* ȥ���ַ����еĻ��з� */
char * Del_Char_Hh( char * sSrc_string);

/*
��Դ�ַ����� �滻 ��ƥ���ϵĵڼ����ַ���  *
���û���ҵ���Ҫ�滻���ַ������򷵻�NULL  *
sDst_string Ŀ���ַ������Ȳ����Ļ������δ��֪ */
char * Replace_Str_To_Str_OrderByCount( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String, int iOrderCount );

/* ��Դ�ַ������滻����һ��ƥ���Ӧ���ַ��� *
* ���û���ҵ���Ҫ�滻���ַ������򷵻�NULL  *
* sDst_string Ŀ���ַ������Ȳ����Ļ������δ��֪ */
char * Replace_Str_To_Str_First( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String );

#if 0
/*
��Դ�ַ������滻�����ж�Ӧ���ַ���        *
���û���ҵ���Ҫ�滻���ַ������򷵻�NULL  *
sDst_string Ŀ���ַ������Ȳ����Ļ������δ��֪ */ ???����û��д���
char * Replace_Str_To_Str_All( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String );

/*
ɾ���ַ����е�����ƥ�䵽���Ӵ� */ ???����û��д���
char * Del_Str_To_Str_All( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String );
#endif

/* ����ɾ��ĳ�ļ��е�ĳһ�ַ� */
int Del_Char_File( char * sFilename, char cCh );

/* ȥ���ַ���ǰ��Ŀո� */
void Trim_Before( char * sString );

/* ȥ���ַ�������Ŀո� */
char *Trim_Behind( char * sString );
 
/* ɾ���ַ���ǰ��Ŀո� */
void Trim( char * sString );

/* ȥ���ַ����еĻ��з����ո� */
char * Trim_Kg_Hh( char * sString );

/* ȡ���ַ�cChar����ַ���(����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)*
* Ŀ���ַ����ռ䲻�������δ��֪ */
char * Get_String_Behind_Char( const char * sSrc_string , char cChar, char * sDst_string );

/* ȡ���ַ�cCharǰ���ַ���(����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)*
* Ŀ���ַ����ռ䲻�������δ��֪ */
char * Get_String_Before_Char( const char * sSrc_string , char cChar, char * sDst_string );

/* ȡ���ַ�'='����ַ���(ԭ���ģ�û��ȥ���ո񡢻��з�)*
* (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��) */
char * Get_String_Behind_Dh( char * src_string , char * dst_string );

/* ȡ���ַ�'='ǰ���ַ���(ԭ���ģ�û��ȥ���ո񡢻��з�)*
* (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��) */
char * Get_String_Before_Dh( char * src_string , char * dst_string );

/* ȡ���ַ�'='����ַ���(ȥ���ո񡢻��з�)   *
* (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��) */
char * Get_String_Behind_Dh_QdKgHh( char * sSrc_String , char * sDst_String );

/* ȡ���ַ�'='ǰ���ַ���(ȥ���ո񡢻��з�)   *
* (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��) */
char * Get_String_Before_Dh_QdKgHh( char * sSrc_String , char * sDst_String );

/* ȡ���ַ�'='����ַ���(ȥ���ո�) *
* (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��)*/
char * Get_String_Behind_Dh_QdKg( char * sSrc_String , char * sDst_String );

/* ȡ���ַ�'='ǰ���ַ���(ȥ���ո�) *
* (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��) */
char * Get_String_Before_Dh_QdKg( char * sSrc_String , char * sDst_String );

/* ȡ���ַ�'='���ߵ��ַ���(ȥ���ո�)������ȡkey-value��ֵ*
* (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��) */
char * Get_KeyValue_Dh_QdKg( char * sSrc_String , char * sDst_KeyString, char * sDst_ValueString );

/* ȡ���������ڵĵ��ַ���                    *
* (����Դ�ַ�����û�ж�Ӧ�ַ�������Ϊʧ��) */
char * Get_String_Between_Zkh( const char * sSrc_String , char * sDst_String );

/* �ַ��������Ƿ��Ƿ������ֹ���(�ɴ�С����) */
int Is_Digit_ByString( const char * sDigit_Str );

/* �ַ������Ƿ��Ƿ������ֹ���(�ɴ�С����) */
int Is_Digit_ByChar( const char sDigit_Str );

/* �ַ��������Ƿ���Ӣ����ĸ�Ĺ��� */
int Is_EnglishChar_ByString( const char * sEnglishChar_Str );

/* �ַ������Ƿ���Ӣ����ĸ�Ĺ��� */
int Is_EnglishChar_ByChar( const char sEnglishChar_Str );

/* �ַ��������Ƿ���Ӣ����ĸ�����֣���С���㣩�Ĺ��� */
int Is_EnglishChar_Digit_ByString( const char * sSrc_Str );

/* ��windows�Ļ��з� ��unix�Ļ��з��Ļ��� */

/* ���ļ��еģ�ǰ���tab ���� 4���ո� */

#ifdef __cplusplus
}
#endif

#endif

