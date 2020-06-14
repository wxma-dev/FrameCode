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


/* 分配一块新的内存，将字符串拷贝至此 */
char * AddStr_Malloc( const char * sAdd_String, char ** pDst_Mem );

/* 在新分配的内存中，在尾部添加字符串        *
* 原内存也必须是malloc类型函数分配的动态内存*
* 新分配的内存长度 原内存字符串长度+添加字符串的长度+1 */
char * AddStr_Realloc( char * pSrc_Mem, const char * sAdd_String, char ** pDst_Mem );

/* 释放AddStr_Realloc、AddStr_Malloc添加内存 */
void AddStr_Free( char ** pPoint_Mem );

/* 判断源字符串中的子串的个数 */
int Count_ChildStr_Num( char * sSrc_String, char * sChild_String );

/* 判断源字符串中的某个字符的个数 */
int Count_Char_Num( char * sSrc_String, char sChild_Char );

/* 字符转换成字符串 */
char * Char_To_Str( const char cSrc_Char, char * sDst_String );

/* 字符串转换成字符（字符串第一位为准）*/
char * Str_To_Char( const char * sSrc_String, char sDst_Char );

/* 获取当前时间的YYYYMMDD */
void GetString_YYYYMMDD( char * pPoint_TimeMem );

/* 获取当前时间的HHMMSS */
void GetString_HHMMSS( char * pPoint_TimeMem );

/* 获取当前时间的计算机的时间（秒数）*/
void GetString_ComputerSec( char * pPoint_TimeMem );

/* 字符串倒序 *
* 如果目的字符串长度小于源字符串的长度，结果未可知*/
char *Dx_String( char * sSrc_string, char * sDst_string );

/* 字符串倒序(直接在源字符串的空间上倒叙) */
char *Dx_String_Yzfc( char * sSrc_string );

/* 去掉字符串中的某一字符(去掉某字符后，源字符串合并)*
* 如果这个字符在字符串中不存在，则认为删除成功 */
char * Del_Char( char * sSrc_String, char cChar );

/* 去掉字符串中的某一字符(去掉某字符后，目标字符串合并)*
* 如果这个字符在字符串中不存在，则认为删除成功*/
char * Del_Char_CopyToDst( char * sSrc_string, char * sDst_string, char sCh );

/* 去掉字符串中的换行符 */
char * Del_Char_Hh( char * sSrc_string);

/*
在源字符串中 替换 掉匹配上的第几个字符串  *
如果没有找到需要替换的字符串，则返回NULL  *
sDst_string 目标字符串长度不够的话，结果未可知 */
char * Replace_Str_To_Str_OrderByCount( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String, int iOrderCount );

/* 在源字符串中替换掉第一个匹配对应的字符串 *
* 如果没有找到需要替换的字符串，则返回NULL  *
* sDst_string 目标字符串长度不够的话，结果未可知 */
char * Replace_Str_To_Str_First( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String );

#if 0
/*
在源字符串中替换掉所有对应的字符串        *
如果没有找到需要替换的字符串，则返回NULL  *
sDst_string 目标字符串长度不够的话，结果未可知 */ ???函数没有写完成
char * Replace_Str_To_Str_All( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String );

/*
删除字符串中的所有匹配到的子串 */ ???函数没有写完成
char * Del_Str_To_Str_All( char * sSrc_string, char * sDst_string, char * sTh_Src_String, char * sTh_Dst_String );
#endif

/* 批量删除某文件中的某一字符 */
int Del_Char_File( char * sFilename, char cCh );

/* 去掉字符串前面的空格 */
void Trim_Before( char * sString );

/* 去掉字符串后面的空格 */
char *Trim_Behind( char * sString );
 
/* 删除字符串前后的空格 */
void Trim( char * sString );

/* 去掉字符串中的换行符、空格 */
char * Trim_Kg_Hh( char * sString );

/* 取得字符cChar后的字符串(若在源字符串中没有对应字符，则认为失败)*
* 目标字符串空间不够，结果未可知 */
char * Get_String_Behind_Char( const char * sSrc_string , char cChar, char * sDst_string );

/* 取得字符cChar前的字符串(若在源字符串中没有对应字符，则认为失败)*
* 目标字符串空间不够，结果未可知 */
char * Get_String_Before_Char( const char * sSrc_string , char cChar, char * sDst_string );

/* 取得字符'='后的字符串(原生的，没有去掉空格、换行符)*
* (若在源字符串中没有对应字符，则认为失败) */
char * Get_String_Behind_Dh( char * src_string , char * dst_string );

/* 取得字符'='前的字符串(原生的，没有去掉空格、换行符)*
* (若在源字符串中没有对应字符，则认为失败) */
char * Get_String_Before_Dh( char * src_string , char * dst_string );

/* 取得字符'='后的字符串(去掉空格、换行符)   *
* (若在源字符串中没有对应字符，则认为失败) */
char * Get_String_Behind_Dh_QdKgHh( char * sSrc_String , char * sDst_String );

/* 取得字符'='前的字符串(去掉空格、换行符)   *
* (若在源字符串中没有对应字符，则认为失败) */
char * Get_String_Before_Dh_QdKgHh( char * sSrc_String , char * sDst_String );

/* 取得字符'='后的字符串(去掉空格) *
* (若在源字符串中没有对应字符，则认为失败)*/
char * Get_String_Behind_Dh_QdKg( char * sSrc_String , char * sDst_String );

/* 取得字符'='前的字符串(去掉空格) *
* (若在源字符串中没有对应字符，则认为失败) */
char * Get_String_Before_Dh_QdKg( char * sSrc_String , char * sDst_String );

/* 取得字符'='两边的字符串(去掉空格)，即获取key-value的值*
* (若在源字符串中没有对应字符，则认为失败) */
char * Get_KeyValue_Dh_QdKg( char * sSrc_String , char * sDst_KeyString, char * sDst_ValueString );

/* 取得中括号内的的字符串                    *
* (若在源字符串中没有对应字符，则认为失败) */
char * Get_String_Between_Zkh( const char * sSrc_String , char * sDst_String );

/* 字符串内容是否都是符合数字规则(可带小数点) */
int Is_Digit_ByString( const char * sDigit_Str );

/* 字符内容是否都是符合数字规则(可带小数点) */
int Is_Digit_ByChar( const char sDigit_Str );

/* 字符串内容是否都是英文字母的规则 */
int Is_EnglishChar_ByString( const char * sEnglishChar_Str );

/* 字符内容是否都是英文字母的规则 */
int Is_EnglishChar_ByChar( const char sEnglishChar_Str );

/* 字符串内容是否都是英文字母、数字（带小数点）的规则 */
int Is_EnglishChar_Digit_ByString( const char * sSrc_Str );

/* 将windows的换行符 、unix的换行符的互换 */

/* 将文件中的，前面的tab 换成 4个空格 */

#ifdef __cplusplus
}
#endif

#endif

