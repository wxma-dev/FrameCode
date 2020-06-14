#ifndef __LIB_FILE_H__
#define __LIB_FILE_H__

#include <stdio.h>

typedef struct st_ReadLine{
	FILE  *pPoint_fp;                       /* �ļ���ָ�� */
	char  sStr_Line[40960 + 1];             /* ��ȡ��ĳһ�е��ڴ棬�޶��ļ���ĳһ�д�С������40960 */
	int   iFlagFirst;                       /*  -2 �쳣������-1 �ļ���ȡ��ϣ�0-��һ��ʹ��δ���ļ���1-��һ�ζ�ȡ�����, 2-�ڶ��δ��ļ���ɣ��Դ����� */
	char  sStr_FilePathName[256 + 1];       /* �ļ���·���� */
} ST_ReadLine;

/* ��ȡ�ļ���С(�ֽ�) */
off_t Get_FileSize( char * filename );

/* ��ȡ�ļ����� */
int GetFileRowCount( char *sFilename, int * pNum );

/* ѭ����ȡ�ļ���ÿһ�� */
ST_ReadLine * While_ReadLine_FromFile( ST_ReadLine ** pPoint_StReadLine );




/* �ļ�����ش��� */
int Do_File( char * sInfile, char * sOutFile, char iFlag );
/*start**************Do_File����iFlag****************/
/*
ע�⣺���¸��÷�����_WINDOWS_HH_DO_REPLACE_UNIX_HH_ �⣬�������÷�����Ĭ���ļ����з�Ϊ\n��
*/
#define _IF_LINE_ONLY_TAB_DO_DELETE_ALL_TAB_      't'     /* t:һ����ֻ��tab�ַ�      ����ȥ��Tab�ַ���\n�ַ�Ҳ��Ĭ��д�����ļ��У� */
#define _IF_LINE_ONLY_KG_DO_DELETE_ALL_KG_        's'     /* s:һ����ֻ�пո��ַ�     ����ȥ���ո�   ��\n�ַ�Ҳ��Ĭ��д�����ļ��У� */
#define _IF_LINE_ONLY_KG_AND_TABL_DO_DELETE_ALL_  'a'     /* a:һ����ֻ�пո��TAB�ַ�����ȥ��     ��\n�ַ�Ҳ��Ĭ��д�����ļ��У� */
                                                                                                             
#define _LINE_HEAD_ALL_TAB_DO_REPLACE_FOUR_KG_    'b'     /* b:һ���п�ͷ������tab����4���ո�                                       */
#define _LINE_HEAD_ALL_FOUR_KG_DO_REPLACE_TAB_    'c'     /* c:һ���п�ͷ������4���ո񻻳�tab                                       */
#define _MULTIPLE_HH_DO_REPLACE_TWO_HH_           'd'     /* d:unix�Ķ��л���2�л��з�                                              */
                                                                                                                                    
#define _WINDOWS_HH_DO_REPLACE_UNIX_HH_           'u'     /* u:windows���з��滻��unix���з���                                      */
#define _UNIX_HH_DO_REPLACE_WINDOWS_HH_           'w'     /* w:unix���з��滻��windows���з���                                      */
                                                                                                                                    
#define _TAB_DQBQ_                                'e'     /* e:һ���ļ���tab��Ϊ���䲹��                                            */
#define _FOUR_KG_DQBQ_                            'f'     /* f:һ���ļ���4���ո���Ϊ���䲹��                                        */
#define _NO_HAVA_ANY_SJ_DO_ADD_FOUR_KG_DQBQ_      'z'     /* z:û���κ��������ļ�����4���ո���䲹��                                */
                                                                                                                                    
#define _DELETE_LINE_LEFT_RIGHT_KG_               'g'     /* g:ȥ���ļ������ݵ�һ�е����ҿո�                                       */
                                                                                                                                    
#define _DELETE_ALL_SXG_ZS_                       'h'     /* h:ȥ���ļ������е�˫б�˵�ע��                                         */
#define _DELETE_ALL_SX_XH_ZS_                     'i'     /* i:ȥ���ļ������е�б����˫�Ǻŵ�ע��                                   */
#define _DELETE_ALL_SX_XH_SXG_ZS_                 'j'     /* j:ȥ���ļ������е�б����˫�Ǻš�˫б�˵�ע��                           */

#define _DELETE_ALL_TAB_EMPTY_LINE_               'k'     /* k:ɾ���ļ������е�tab����                                              */
#define _DELETE_ALL_KG_EMPTY_LINE_                'm'     /* m:ɾ���ļ������еĿո����                                             */
#define _DELETE_ALL_HHFH_EMPTY_LINE_              'n'     /* n:ɾ���ļ������е�ֻ�л��з��ŵĿ���                                   */
#define _DELETE_ALL_TAB_KG_EMPTY_LINE_            'q'     /* q:ɾ���ļ������еĿ��У�����tab���С��ո���С�ֻ�л��з��ŵĿ��У�    */

#define _PUT_FILE_MORE_NOT_USE_VARIABLE_          'r'     /* r:���Դ�ļ���δʹ�õı�����ָ���ļ�                                   */
#define _DELETE_MORE_NOT_USE_VARIABLE_            'v'     /* v:ɾ��Դ�ļ���δʹ�õı���                                             */
#define _PUT_FILE_ALL_FUN_FROM_SRC_               'x'     /* x:���Դ�ļ��е����еĺ�����ָ���ļ�                                   */
#define _PUT_FILE_SHOW_ALL_FUN_FROM_HEADFILE_     'y'     /* y:���ͷ�ļ��е����еĺ�����ָ���ļ�                                   */
/*end**************Do_File����iFlag******************/


#endif



