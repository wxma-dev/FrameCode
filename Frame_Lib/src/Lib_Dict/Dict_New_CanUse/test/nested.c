#include <stdio.h>
#include <stdlib.h>
#include <Beai3Dict.h>

int main(int argc, char *argv[])
{
	DictKey key = "PaymentSerialNo";
	char * value = "100001";

	/* 初始化一个数据字典 */
	DictHandle dictHandle = InitDict(10);

	/* 初始化嵌套字典 */
	DictHandle headerHandle = InitDict(10);
	DictHandle bodyHandle = InitDict(10);

	/* 接收数据的缓存，需大于接收的字符串大小 */
	char buffer[64];
	long buffer_size;

	/* 接收嵌套数据字典的句柄 */
	DictHandle myHandle;

	/* 设置报文头数据 */
	SetDictString(&headerHandle, "bitmap", "1010101010101");

	/* 设置报文体数据 */
	SetDictString(&bodyHandle, key, value);
	SetDictString(&bodyHandle, "amount", "1000000.00");

	/* 设置嵌套数据字典 */
	SetDictDict(&dictHandle, "header", headerHandle);
	SetDictDict(&dictHandle, "body", bodyHandle);

	/* 获取嵌套数据字典句柄 */
	GetDictDict(dictHandle, "header", &myHandle);
	/* 获取嵌套数据字典内的数据 */
	GetDictString(myHandle, "bitmap", buffer, sizeof(buffer));
	printf("bitmap -> %s\n", key, buffer);
	
	/* 获取嵌套数据字典句柄 */
	GetDictDict(dictHandle, "body", &myHandle);
	/* 获取嵌套数据字典内的数据 */
	GetDictString(myHandle, key, buffer, sizeof(buffer));
	printf("%s -> %s\n", key, buffer);
	GetDictString(dictHandle, "amount", buffer, sizeof(buffer));
	printf("amount -> %s\n", buffer);

	/* 销毁数据字典，释放内存 */
	/*
	 * 嵌套的数据字典不需要显式地销毁，
	 * 销毁上级数据字典会同时销毁内部嵌套的所有数据字典
	 */
	DestroyDict(dictHandle);
}


