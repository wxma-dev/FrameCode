#include <stdio.h>
#include <stdlib.h>
//#include <Beai3Dict.h>
#include "../utils/beai3/Beai3Dict.h"

int main(int argc, char *argv[])
{
	DictKey key = "PaymentSerialNo";
	char * value = "100001";

	/* ��ʼ��һ�������ֵ� */
	DictHandle dictHandle = InitDict(10);

	/* �������ݵĻ��棬����ڽ��յ��ַ�����С */
	char buffer[64];
	long  buffer_size;

	/* �����ֵ����� */
	SetDictString(&dictHandle, key, value);
	SetDictString(&dictHandle, "amount", "1000000.00");

	/*
	 * ȡ�������ֵ�����ַ���������������ն���Ļ�� 
	 * ������治������ص����������ַ���
	 */
	GetDictString(dictHandle, key, buffer, sizeof(buffer));
	printf("%s -> %s\n", key, buffer);
	GetDictString(dictHandle, "amount", buffer, sizeof(buffer));
	printf("amount -> %s\n", buffer);

	/* ���������ֵ䣬�ͷ��ڴ� */
	DestroyDict(dictHandle);
}

