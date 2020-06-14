#ifndef __LIB_FILE_H__
#define __LIB_FILE_H__

#include <stdio.h>

typedef struct st_ReadLine{
	FILE  *pPoint_fp;                       /* 文件的指针 */
	char  sStr_Line[40960 + 1];             /* 读取的某一行的内存，限定文件的某一行大小不超过40960 */
	int   iFlagFirst;                       /*  -2 异常结束，-1 文件读取完毕，0-第一次使用未打开文件，1-第一次读取文完成, 2-第二次打开文件完成，以此类推 */
	char  sStr_FilePathName[256 + 1];       /* 文件名路径名 */
} ST_ReadLine;

/* 获取文件大小(字节) */
off_t Get_FileSize( char * filename );

/* 获取文件行数 */
int GetFileRowCount( char *sFilename, int * pNum );

/* 循环读取文件的每一行 */
ST_ReadLine * While_ReadLine_FromFile( ST_ReadLine ** pPoint_StReadLine );




/* 文件的相关处理 */
int Do_File( char * sInfile, char * sOutFile, char iFlag );
/*start**************Do_File参数iFlag****************/
/*
注意：以下各用法，除_WINDOWS_HH_DO_REPLACE_UNIX_HH_ 外，其他的用法，均默认文件换行符为\n。
*/
#define _IF_LINE_ONLY_TAB_DO_DELETE_ALL_TAB_      't'     /* t:一行中只有tab字符      ，则去掉Tab字符（\n字符也是默认写到新文件中） */
#define _IF_LINE_ONLY_KG_DO_DELETE_ALL_KG_        's'     /* s:一行中只有空格字符     ，则去掉空格   （\n字符也是默认写到新文件中） */
#define _IF_LINE_ONLY_KG_AND_TABL_DO_DELETE_ALL_  'a'     /* a:一行中只有空格和TAB字符，则都去掉     （\n字符也是默认写到新文件中） */
                                                                                                             
#define _LINE_HEAD_ALL_TAB_DO_REPLACE_FOUR_KG_    'b'     /* b:一行中开头的所有tab换成4个空格                                       */
#define _LINE_HEAD_ALL_FOUR_KG_DO_REPLACE_TAB_    'c'     /* c:一行中开头的所有4个空格换成tab                                       */
#define _MULTIPLE_HH_DO_REPLACE_TWO_HH_           'd'     /* d:unix的多行换成2行换行符                                              */
                                                                                                                                    
#define _WINDOWS_HH_DO_REPLACE_UNIX_HH_           'u'     /* u:windows换行符替换成unix换行符号                                      */
#define _UNIX_HH_DO_REPLACE_WINDOWS_HH_           'w'     /* w:unix换行符替换成windows换行符号                                      */
                                                                                                                                    
#define _TAB_DQBQ_                                'e'     /* e:一个文件中tab作为对其补齐                                            */
#define _FOUR_KG_DQBQ_                            'f'     /* f:一个文件中4个空格作为对其补齐                                        */
#define _NO_HAVA_ANY_SJ_DO_ADD_FOUR_KG_DQBQ_      'z'     /* z:没有任何缩进的文件，以4个空格对其补齐                                */
                                                                                                                                    
#define _DELETE_LINE_LEFT_RIGHT_KG_               'g'     /* g:去掉文件有内容的一行的左右空格                                       */
                                                                                                                                    
#define _DELETE_ALL_SXG_ZS_                       'h'     /* h:去掉文件中所有的双斜杆的注释                                         */
#define _DELETE_ALL_SX_XH_ZS_                     'i'     /* i:去掉文件中所有的斜线与双星号的注释                                   */
#define _DELETE_ALL_SX_XH_SXG_ZS_                 'j'     /* j:去掉文件中所有的斜线与双星号、双斜杆的注释                           */

#define _DELETE_ALL_TAB_EMPTY_LINE_               'k'     /* k:删除文件中所有的tab空行                                              */
#define _DELETE_ALL_KG_EMPTY_LINE_                'm'     /* m:删除文件中所有的空格空行                                             */
#define _DELETE_ALL_HHFH_EMPTY_LINE_              'n'     /* n:删除文件中所有的只有换行符号的空行                                   */
#define _DELETE_ALL_TAB_KG_EMPTY_LINE_            'q'     /* q:删除文件中所有的空行（包括tab空行、空格空行、只有换行符号的空行）    */

#define _PUT_FILE_MORE_NOT_USE_VARIABLE_          'r'     /* r:输出源文件中未使用的变量至指定文件                                   */
#define _DELETE_MORE_NOT_USE_VARIABLE_            'v'     /* v:删除源文件中未使用的变量                                             */
#define _PUT_FILE_ALL_FUN_FROM_SRC_               'x'     /* x:输出源文件中的所有的函数至指定文件                                   */
#define _PUT_FILE_SHOW_ALL_FUN_FROM_HEADFILE_     'y'     /* y:输出头文件中的所有的函数至指定文件                                   */
/*end**************Do_File参数iFlag******************/


#endif



