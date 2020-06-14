

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "../../../../../../Frame_Lib/src/Lib_File/lib_conf_file.h"


int main()
{
	char     sThreadNum_Init_Add_Work[1024];

	memset( sThreadNum_Init_Add_Work, 0x00, sizeof( sThreadNum_Init_Add_Work ) );
	if ( 0 != Get_ValuseString_ByFieldKey_InConfFile( "../Application_Demo/conf/frame_threadpool.conf", "THREAD_POOL_SET", "SET_ADD_WORK_THREAD_NUM_OF_POOL", sThreadNum_Init_Add_Work ) )
	{

	}


}








