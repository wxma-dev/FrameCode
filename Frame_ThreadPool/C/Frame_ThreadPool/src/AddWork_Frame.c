#include "AddWork_Frame.h"
//#include "Application_Demo/Application.h"
#include "Common.h"

int AddWork_Appliction( void *arg )
{
	return AddWork( arg );
}


/*********************************************************************
* Function：                                                         *
*    int Call_Addwork_Thread( void *arg)                             *
* Description：                                                      *
*    往CList_Task链表中添加任务（函数，参数）                        *
*    自旋锁、互斥锁成为可选择的方式                                  *
*    添加任务的部分，可以作为一个公共的部分，具体的发现任务的再做一个子接口来表现*
* In Params:                                                         *
*                                                                    *
* Out Params:                                                        *
*                                                                    *
* Return:                                                            *
*                                                                    *
*********************************************************************/
void *Call_Addwork_Thread( void *arg )
{
	AddWork_Appliction( arg );
}






