#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "bas/bas_global.h"
#include "bas/bas_genathcd.h"

BAS_EXPORT int iGenAthcd(void)
{
    time_t  tTime;
    int     iVal;

    // ���������ֻ���ڳ���������ʱ���ʼ��һ��
	// ����Ļ������ͬһ���ڵ��ö�ξͻ�������ͬ����Ȩ��
//    time(&tTime);
//    srandom(tTime);
    //while ((iVal = random()%999999) < 9999);
	while (1)
	{
		iVal = random()%999999;
		if (iVal != 0) break;
	}
    return iVal;
}
