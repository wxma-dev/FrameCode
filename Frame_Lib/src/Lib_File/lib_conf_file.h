#ifndef __LIB_CONF_FILE_H__
#define __LIB_CONF_FILE_H__

#include "lib_file.h"

/* ��ȡ�����ļ�ĳһ�У�����ڴ棬�洢�ַ���  
	����ǵ�һ�ζ�ȡĳһ����malloc�����һ��  
	���϶�ȡĳһ����realloc�������µ�ַ��� */
char * AddMem_For_AddString( const int iCount_ReadLine, const char * sSrc_String, char ** pPoint_AddMem );

/* �ͷ�AddMem_For_AddString��ӵ��ڴ� */
void FreeMem_For_AddString( char ** pPoint_NeedFree );

/* �������ļ��л�ȡĿ��Key������ */
int Get_ValuseString_ByKey_InConfFile( const char * sFile_Name, const char * sKey, char * sDst_String );

/* ����Field��Key���������ļ��л�ȡ��Ӧ��ֵ */
int Get_ValuseString_ByFieldKey_InConfFile( const char * sFile_Name, const char * sField, const char * sKey, char * sDst_String );

/* ѭ����ȡ�����ļ�����Ч�� */
ST_ReadLine * Wile_ReadLine_FromConfFile( ST_ReadLine ** pPoint_StReadLine );


int GetValue_ByStr( FILE * const pFile, const char * sIn, char * sOut);



/* �޸������ļ���ĳField/Key��ֵ */
/* �޸������ļ���ĳKey��ֵ(���ļ��е�һ�γ��ֵ�KeyΪ׼)*/




#endif


/*************************************************************************/








/*************************************************************
*    FileName : config.h                                     *
*    FileFunc : ����ͷ�ļ�                                   *
*    Version  : V0.1                                         *
*    Author   : Sunrier                                      *
*    Date     : 2012-05-09                                   *
*    Descp    : Linux�»�ȡ�����ļ���Ϣ                      *
*************************************************************/  
#ifndef   _CONFIG_H  
#define   _CONFIG_H  
  
#ifdef __cplusplus  
extern "C" {  
#endif  
  
#define  SUCCESS           0x00 /*�ɹ�*/  
#define  FAILURE           0x01 /*ʧ��*/  
  
#define  FILENAME_NOTEXIST      0x02 /*�����ļ���������*/  
#define  SECTIONNAME_NOTEXIST    0x03 /*����������*/  
#define  KEYNAME_NOTEXIST      0x04 /*����������*/  
#define  STRING_LENNOTEQUAL     0x05 /*�����ַ������Ȳ�ͬ*/  
#define  STRING_NOTEQUAL       0x06 /*�����ַ������ݲ���ͬ*/  
#define  STRING_EQUAL        0x00 /*�����ַ���������ͬ*/  
  
  
int CompareString(char *pInStr1,char *pInStr2);  
int GetKeyValue(FILE *fpConfig,char *pInKeyName,char *pOutKeyValue);  
int GetConfigIntValue(char *pInFileName,char *pInSectionName,char *pInKeyName,int *pOutKeyValue);  
int GetConfigStringValue(char *pInFileName,char *pInSectionName,char *pInKeyName,char *pOutKeyValue);  
  
#ifdef __cplusplus  
}  
#endif  
  
#endif


