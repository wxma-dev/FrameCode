#include <stdio.h>
#include <stdlib.h>
#include <Beai3Dict.h>

int main(int argc, char *argv[])
{
	DictKey key = "PaymentSerialNo";
	char * value = "100001";

	/* ��ʼ��һ�������ֵ� */
	DictHandle dictHandle = InitDict(10);

	/* ��ʼ��Ƕ���ֵ� */
	DictHandle headerHandle = InitDict(10);
	DictHandle bodyHandle = InitDict(10);

	/* �������ݵĻ��棬����ڽ��յ��ַ�����С */
	char buffer[64];
	long buffer_size;

	/* ����Ƕ�������ֵ�ľ�� */
	DictHandle myHandle;

	/* ���ñ���ͷ���� */
	SetDictString(&headerHandle, "bitmap", "1010101010101");

	/* ���ñ��������� */
	SetDictString(&bodyHandle, key, value);
	SetDictString(&bodyHandle, "amount", "1000000.00");

	/* ����Ƕ�������ֵ� */
	SetDictDict(&dictHandle, "header", headerHandle);
	SetDictDict(&dictHandle, "body", bodyHandle);

	/* ��ȡǶ�������ֵ��� */
	GetDictDict(dictHandle, "header", &myHandle);
	/* ��ȡǶ�������ֵ��ڵ����� */
	GetDictString(myHandle, "bitmap", buffer, sizeof(buffer));
	printf("bitmap -> %s\n", key, buffer);
	
	/* ��ȡǶ�������ֵ��� */
	GetDictDict(dictHandle, "body", &myHandle);
	/* ��ȡǶ�������ֵ��ڵ����� */
	GetDictString(myHandle, key, buffer, sizeof(buffer));
	printf("%s -> %s\n", key, buffer);
	GetDictString(dictHandle, "amount", buffer, sizeof(buffer));
	printf("amount -> %s\n", buffer);

	/* ���������ֵ䣬�ͷ��ڴ� */
	/*
	 * Ƕ�׵������ֵ䲻��Ҫ��ʽ�����٣�
	 * �����ϼ������ֵ��ͬʱ�����ڲ�Ƕ�׵����������ֵ�
	 */
	DestroyDict(dictHandle);
}


