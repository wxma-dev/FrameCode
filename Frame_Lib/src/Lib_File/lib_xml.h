#ifndef __LIB_XML_H__
#define __LIB_XML_H__

/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Get_FindString                            *
* Description:    寻找字符串中第一个str1与str2中的字符串    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
*                 fnd_str 存放查到到的字符串                *
*                 str1 查找的开始字符串位置                 *
*                 str2 查询的结束字符串位置                 *
* Out:            NULL 查询无                               *
*                 非空 查询到字符串                         *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Get_FindString( char * src_str, char * fnd_str , char * str1, char * str2 );


/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Get_FirstBracket                          *
* Description:    寻找字符串中第一个<>中的字符串            *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
*                 fnd_str 存放查到到的字符串                *
* Out:            NULL 查询无                               *
*                 非空 查询到字符串                         *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Get_FirstBracket( char * src_str, char * fnd_str );



/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Is_FirstSpace_String                      *
* Description:    判断字符串首部中是否有空格                *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
* Out:            0 无                                      *
*                 1 有                                      *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Is_FirstSpace_String( char * str );


/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Is_EndSpace_String                        *
* Description:    判断字符串尾部是否有空格                  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
* Out:            0 无                                      *
*                 1 有                                      *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Is_EndSpace_String( char * str );


/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Is_Space_String                           *
* Description:    判断字符串中是否有空格                    *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
* Out:            0 无                                      *
*                 1 有                                      *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Is_Space_String( char * str );




/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Check_BracketString                       *
* Description:    校验<>中的内容是否有空格                  *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             str 源字符串                              *
* Out:            -1 有空格                                 *
*                 0 无空格                                  *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Check_BracketString( char * str );



/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Check_XmlString                           *
* Description:    校验字符串中是否有对应字段str的</str>的部分*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             src_str 源字符串                          *
*                 str 检验字符串                            *
* Out:            -1 无</str>部分                           *
*                 0  有</str>部分                           *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
int Check_XmlString( char * src_str, char * str );



/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Find_ZdXml                                *
* Description:    寻找<fnd_str的完整部分，比如<fnd_str class="listitem">*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             str 源字符串                              *
*                 fnd_str 字段字符串                        *
* Out:            NULL    失败                              *
*                非NULL  对应查询到字符串值                 *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Find_ZdXml( char * str, char * fnd_str );




/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Get_ChildString                           *
* Description:    将<fnd_str>与</fnd_str>之间的字符串拿出来 *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             str 源字符串                              *
*                 fnd_str 字段字符串                        *
* Out:            NULL    失败                              *
*                非NULL  对应查询到字符串值                 *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Get_ChildString( char * str , char * fnd_str );


/************************************************************
* Copyright (c) 2015 北京联银通科技有限公司                 *
* Source File:    Get_Check_ChildString                     *
* Description:    将<fnd_str>与</fnd_str>之间的字符串拿出来，并且校验是否合法*
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             str 源字符串                              *
*                 fnd_str 字段字符串                        *
* Out:            NULL    失败                              *
*                非NULL  对应查询到字符串值                 *  
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
char * Get_Check_ChildString( char * str , char * fnd_str );

#endif


























