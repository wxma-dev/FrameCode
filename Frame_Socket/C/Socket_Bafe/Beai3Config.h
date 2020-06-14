#ifndef BEAI3CONFIG_H
#define BEAI3CONFIG_H

#define NAME_LENGTH			63
#define VALUE_LENGTH		191

typedef void* HANDLE;

typedef struct ST_Variable
{
	char caName[NAME_LENGTH + 1];
	char caValue[VALUE_LENGTH + 1];
} ST_VARIABLE;


HANDLE LoadConfigure(char *pcFileName);
void FreeConfigure(HANDLE hConfigure);
int GetValue(HANDLE hConfigure, char *pcSection,
	char *pcName, char *pcValue, int iValueLen);
int GetVariableCount(HANDLE hConfigure, char *pcSection);
int NextVariable(HANDLE hConfigure, char *pcSection,
	int iPosition, ST_VARIABLE *pstVariable);

#endif
