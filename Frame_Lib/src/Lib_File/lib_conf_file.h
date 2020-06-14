#ifndef __LIB_CONF_FILE_H__
#define __LIB_CONF_FILE_H__

#include "lib_file.h"

/* 读取配置文件某一行，添加内存，存储字符串  
	如果是第一次读取某一行则malloc，如果一次  
	以上读取某一行则realloc，并将新地址输出 */
char * AddMem_For_AddString( const int iCount_ReadLine, const char * sSrc_String, char ** pPoint_AddMem );

/* 释放AddMem_For_AddString添加的内存 */
void FreeMem_For_AddString( char ** pPoint_NeedFree );

/* 从配置文件中获取目标Key的内容 */
int Get_ValuseString_ByKey_InConfFile( const char * sFile_Name, const char * sKey, char * sDst_String );

/* 根据Field、Key的在配置文件中获取对应的值 */
int Get_ValuseString_ByFieldKey_InConfFile( const char * sFile_Name, const char * sField, const char * sKey, char * sDst_String );

/* 循环读取配置文件的有效行 */
ST_ReadLine * Wile_ReadLine_FromConfFile( ST_ReadLine ** pPoint_StReadLine );


int GetValue_ByStr( FILE * const pFile, const char * sIn, char * sOut);



/* 修改配置文件中某Field/Key的值 */
/* 修改配置文件中某Key的值(以文件中第一次出现的Key为准)*/




#endif


/*************************************************************************/








/*************************************************************
*    FileName : config.h                                     *
*    FileFunc : 定义头文件                                   *
*    Version  : V0.1                                         *
*    Author   : Sunrier                                      *
*    Date     : 2012-05-09                                   *
*    Descp    : Linux下获取配置文件信息                      *
*************************************************************/  
#ifndef   _CONFIG_H  
#define   _CONFIG_H  
  
#ifdef __cplusplus  
extern "C" {  
#endif  
  
#define  SUCCESS           0x00 /*成功*/  
#define  FAILURE           0x01 /*失败*/  
  
#define  FILENAME_NOTEXIST      0x02 /*配置文件名不存在*/  
#define  SECTIONNAME_NOTEXIST    0x03 /*节名不存在*/  
#define  KEYNAME_NOTEXIST      0x04 /*键名不存在*/  
#define  STRING_LENNOTEQUAL     0x05 /*两个字符串长度不同*/  
#define  STRING_NOTEQUAL       0x06 /*两个字符串内容不相同*/  
#define  STRING_EQUAL        0x00 /*两个字符串内容相同*/  
  
  
int CompareString(char *pInStr1,char *pInStr2);  
int GetKeyValue(FILE *fpConfig,char *pInKeyName,char *pOutKeyValue);  
int GetConfigIntValue(char *pInFileName,char *pInSectionName,char *pInKeyName,int *pOutKeyValue);  
int GetConfigStringValue(char *pInFileName,char *pInSectionName,char *pInKeyName,char *pOutKeyValue);  
  
#ifdef __cplusplus  
}  
#endif  
  
#endif


