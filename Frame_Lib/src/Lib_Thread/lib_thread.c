
#include "lib_thread.h"



/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    Thread_Priority_Scheduling                *
* Description:    判断是否支持线程优先级调度                *
* Author:         mawx                                      *
* Create Date:    2020/05/19                                *
* In:                                                       *
*                                                           *
* Out:                                                      *
*                                                           *
* Return:         1 支持线程优先级调度                      *
*                 0 不支持线程优先级调度                    *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
int Thread_Priority_Scheduling()
{
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
    return 1;
#else
    return 0;
#endif
}