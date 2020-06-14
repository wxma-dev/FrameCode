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

	/* ��ʼ��һ�������ֵ� */
	DictHandle dictHandle = InitDict(4);

	/* ��ʼ��һ����� */
	DictHandle tableHandle = InitArray(3);
	/* �������½��е������ֵ��� */
	DictHandle rowHandle;
	/* �������ݵĻ��棬����ڽ��յ��ַ�����С */
	char buffer[64];
	long buffer_size;
	/* ����Ƕ�ױ��ľ�� */
	DictHandle myArrayHandle;
	/* ���ձ����Ƕ�������ֵ�ľ�� */
	DictHandle myDictHandle;

	/* ���ñ������ */
	for(i = 0; i < sizeof(data)/sizeof(data[0]); i++)
	{
		/* ���������� */
		rowHandle = InitDict(2);
		SetDictString(&rowHandle, "account", data[i].account);
		SetDictString(&rowHandle, "amount", data[i].amount);
		
		/* ��������� */
		AddArrayDict(&tableHandle, rowHandle);
	}
	SetDictArray(&dictHandle, "table", tableHandle);

	/* ���ñ���������� */
	snprintf(countBuffer, sizeof(countBuffer) - 1,
		"%d", GetArrayLength(tableHandle));
		
	countBuffer[sizeof(countBuffer) - 1] = '\0';
	SetDictString(&dictHandle, "count", countBuffer);

	/* �����ֵ����� */
	SetDictString(&dictHandle, key, value);

	if(GetDictString(dictHandle, "_locanode", tmp, sizeof(tmp)) < 0)
		fprintf(stderr, "can't not get _locanode\n");
	else
		fprintf(stderr, "_locanode = [%s]\n", tmp);

	SetDictString(&dictHandle, "_locanode", "beai");

	SerializeDict(&dictHandle);

	GetDictArray(dictHandle, "table", &tableHandle);

	/* ���������� */
	rowHandle = InitDict(2);
	SetDictString(&rowHandle, "account", "ladkflakdfladsfalssdkfladkflasd");
	SetDictString(&rowHandle, "amount", "lakdslfkaldfkla871349174389712934719");
		
	/* ��������� */
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
	 * ȡ�������ֵ�����ַ���������������ն���Ļ��
	 * ������治������ص����������ַ���
	 */
	GetDictString(dictHandle, key, buffer, sizeof(buffer));
	printf("%s -> %s\n", key, buffer);
	GetDictString(dictHandle, "count", buffer, sizeof(buffer));
	printf("count -> %s\n", buffer);

	/* ȡ�ñ���ڵ����� */
	GetDictArray(dictHandle, "table", &myArrayHandle);
	for(i = 0; i < GetArrayLength(myArrayHandle); i++)
	{
		GetArrayDict(myArrayHandle, i, &myDictHandle);
		GetDictString(myDictHandle, "account", buffer, sizeof(buffer));
		printf("account -> %s\n", buffer);
		GetDictString(myDictHandle, "amount", buffer, sizeof(buffer));
		printf("amount -> %s\n", buffer);
	}


	/* ���������ֵ䣬�ͷ��ڴ� */
	/*
	 * Ƕ�׵������ֵ䲻��Ҫ��ʽ�����٣�
	 * �����ϼ������ֵ��ͬʱ�����ڲ�Ƕ�׵����б��������ֵ�
	 */
	DestroyDict(dictHandle);
}

