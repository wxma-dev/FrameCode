#include <stdio.h>
#include <stdlib.h>
//#include <Beai3Dict.h>
#include "../utils/beai3/Beai3Dict.h"

int main(int argc, char *argv[])
{
	DictKey key = "PaymentSerialNo";
	char * value = "100001";

	/* 初始化一个数据字典 */
	DictHandle dictHandle = InitDict(10);

	/* 接受数据的缓存，需大于接收的字符串大小 */
	char buffer[64];
	long  buffer_size;

	/* 设置字典数据 */
	SetDictString(&dictHandle, key, value);
	SetDictString(&dictHandle, "amount", "1000000.00");

	/*
	 * 取得数据字典里的字符串，并且输出到终端屏幕。 
	 * 如果缓存不够大，则截掉超长部分字符串
	 */
	GetDictString(dictHandle, key, buffer, sizeof(buffer));
	printf("%s -> %s\n", key, buffer);
	GetDictString(dictHandle, "amount", buffer, sizeof(buffer));
	printf("amount -> %s\n", buffer);

	/* 销毁数据字典，释放内存 */
	DestroyDict(dictHandle);
}

