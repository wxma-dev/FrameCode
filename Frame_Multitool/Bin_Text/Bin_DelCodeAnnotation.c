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
*/

/*
*    ProgramName : Bin_DelCodeAnnotation.c
*    SystemName  : 
*    Version     : 1.0
*    Description : used by rz_06.sh
*    History     :
*    YYYY/MM/DD        Position        Author         Description
*    -------------------------------------------------------------
*    2015/11/06        JiaXing         zhr            create
*/

#include <stdio.h>



/************************************************************
* Function Name: main                                       *
* Description:                                              *
* In Params:     int argc                                   *
*                char** argv                                *
*                argv[1]  日期 YYYY/MM/DD                   *
*                argv[2]  日志文件                          *
* Out Params:                                               *
*                                                           *
* Return:        0: 成功                                    *
*               -1: 其他失败                                *
*               -2: 失败，执行System失败                    *
*               -3：输入参数错误                            *
*                                                           *
* explain:      应该是                                            *
************************************************************/
int main(int argc, char* argv[]) {
    enum {
        literal,
        single,
        multiple,
        string
    } mode = literal;
    char last = 0, current;

    while ((current = getchar()) != EOF) {
        switch (mode) {
            case single: {
                if (last != '\\' && (current == '\n' || current == '\r')) {
                    putchar(current);
                    current = 0;
                    mode = literal;
                }
            } break;
            case multiple: {
                if (last == '*' && current == '/') {
                    current = 0;
                    mode = literal;
                }
            } break;
            case string: {
                if (last == '\\') {
                    putchar(last);
                    putchar(current);
                } else if (current != '\\') {
                    putchar(current);
                    if (current == '"') {
                        mode = literal;
                    }
                }
            } break;
            default: {
                if (last == '/') {
                    if (current == '/') {
                        mode = single;
                    } else if (current == '*') {
                        mode = multiple;
                    } else {
                        putchar(last);
                        putchar(current);
                    }
                } else if (current != '/') {
                    putchar(current);
                    if (current == '"') {
                        mode = string;
                    }
                }
            } break;
        }
        last = current;
    }

    return 0;
}


/*
** end of file
*/