#include "AddWork_Frame.h"
//#include "Application_Demo/Application.h"
#include "Common.h"

int AddWork_Appliction( void *arg )
{
	return AddWork( arg );
}


/*********************************************************************
* Function��                                                         *
*    int Call_Addwork_Thread( void *arg)                             *
* Description��                                                      *
*    ��CList_Task������������񣨺�����������                        *
*    ����������������Ϊ��ѡ��ķ�ʽ                                  *
*    �������Ĳ��֣�������Ϊһ�������Ĳ��֣�����ķ������������һ���ӽӿ�������*
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






