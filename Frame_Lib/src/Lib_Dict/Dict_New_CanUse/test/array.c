#include <stdio.h>
#include <stdlib.h>
#include <Beai3Dict.h>

struct data_type
{
	char *account;
	char *amount;
} data[] = { {"10001", "1000.00"} };

int main(int argc, char *argv[])
{
	DictKey key = "PaymentSerialNo";
	char * value = "0123456578901234567890";
	int i;
	char countBuffer[64];
	char tmp[100];

	/* 初始化一个数据字典 */
	DictHandle dictHandle = InitDict(4);

	/* 初始化一个表格 */
	DictHandle tableHandle = InitArray(3);
	/* 保存表格新建行的数据字典句柄 */
	DictHandle rowHandle;
	/* 接受数据的缓存，需大于接收的字符串大小 */
	char buffer[64];
	long buffer_size;
	/* 接收嵌套表格的句柄 */
	DictHandle myArrayHandle;
	/* 接收表格内嵌套数据字典的句柄 */
	DictHandle myDictHandle;

	/* 设置表格数据 */
	for(i = 0; i < sizeof(data)/sizeof(data[0]); i++)
	{
		/* 创建行数据 */
		rowHandle = InitDict(2);
		SetDictString(&rowHandle, "account", data[i].account);
		SetDictString(&rowHandle, "amount", data[i].amount);
		
		/* 添加行数据 */
		AddArrayDict(&tableHandle, rowHandle);
	}
	SetDictArray(&dictHandle, "table", tableHandle);

	/* 设置表格数据行数 */
	snprintf(countBuffer, sizeof(countBuffer) - 1,
		"%d", GetArrayLength(tableHandle));
		
	countBuffer[sizeof(countBuffer) - 1] = '\0';
	SetDictString(&dictHandle, "count", countBuffer);

	/* 设置字典数据 */
	SetDictString(&dictHandle, key, value);

	if(GetDictString(dictHandle, "_locanode", tmp, sizeof(tmp)) < 0)
		fprintf(stderr, "can't not get _locanode\n");
	else
		fprintf(stderr, "_locanode = [%s]\n", tmp);

	SetDictString(&dictHandle, "_locanode", "beai");

	SerializeDict(&dictHandle);

	GetDictArray(dictHandle, "table", &tableHandle);

	/* 创建行数据 */
	rowHandle = InitDict(2);
	SetDictString(&rowHandle, "account", "ladkflakdfladsfalssdkfladkflasd");
	SetDictString(&rowHandle, "amount", "lakdslfkaldfkla871349174389712934719");
		
	/* 添加行数据 */
	AddArrayDict(&tableHandle, rowHandle);

	SerializeDict(&dictHandle);

	if(GetDictString(dictHandle, "_locanode", tmp, sizeof(tmp)) < 0)
		fprintf(stderr, "can't not get _locanode\n");
	else
		fprintf(stderr, "_locanode = [%s]\n", tmp);

	SerializeDict(&dictHandle);

	if(GetDictString(dictHandle, "_locanode", tmp, sizeof(tmp)) < 0)
		fprintf(stderr, "can't not get _locanode\n");
	else
		fprintf(stderr, "_locanode = [%s]\n", tmp);

	/*
	 * 取得数据字典里的字符串，并且输出到终端屏幕。
	 * 如果缓存不够大，则截掉超长部分字符串
	 */
	GetDictString(dictHandle, key, buffer, sizeof(buffer));
	printf("%s -> %s\n", key, buffer);
	GetDictString(dictHandle, "count", buffer, sizeof(buffer));
	printf("count -> %s\n", buffer);

	/* 取得表格内的数据 */
	GetDictArray(dictHandle, "table", &myArrayHandle);
	for(i = 0; i < GetArrayLength(myArrayHandle); i++)
	{
		GetArrayDict(myArrayHandle, i, &myDictHandle);
		GetDictString(myDictHandle, "account", buffer, sizeof(buffer));
		printf("account -> %s\n", buffer);
		GetDictString(myDictHandle, "amount", buffer, sizeof(buffer));
		printf("amount -> %s\n", buffer);
	}


	/* 销毁数据字典，释放内存 */
	/*
	 * 嵌套的数据字典不需要显式地销毁，
	 * 销毁上级数据字典会同时销毁内部嵌套的所有表格和数据字典
	 */
	DestroyDict(dictHandle);
}

