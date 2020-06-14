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

    // 随机数种子只能在程序启动的时候初始化一次
	// 否则的话如果在同一秒内调用多次就会生成相同的授权码
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
