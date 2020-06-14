#ifndef BEAI3DICT_H
#define BEAI3DICT_H

typedef void * DictHandle;
typedef char * DictKey;


/* int GET_SHORT(DictHandle,DictKey,short*) */
#define GET_SHORT(handle,key,value) GetDictShort(handle,key,value)
/* int GET_INT(DictHandle,DictKey,int*) */
#define GET_INT(handle,key,value) GetDictInt(handle,key,value)
/* int GET_LONG(DictHandle,DictKey,long*) */
#define GET_LONG(handle,key,value) GetDictLong(handle,key,value)
/* int GET_LONGLONG(DictHandle,DictKey,long long*) */
#define GET_LONGLONG(handle,key,value) GetDictLongLong(handle,key,value)

/* int GET_USHORT(DictHandle,DictKey,unsigned short*) */
#define GET_USHORT(handle,key,value) GetDictUShort(handle,key,value)
/* int GET_UINT(DictHandle,DictKey,unsigned int*) */
#define GET_UINT(handle,key,value) GetDictUInt(handle,key,value)
/* int GET_ULONG(DictHandle,DictKey,unsigned long*) */
#define GET_ULONG(handle,key,value) GetDictULong(handle,key,value)
/* int GET_ULONGLONG(DictHandle,DictKey,unsigned long long*) */
#define GET_ULONGLONG(handle,key,value) Get_ULongLong(handle,key,value)

/* int GET_FLOAT(DictHandle,DictKey,float*) */
#define GET_FLOAT(handle,key,value) GetDictFloat(handle,key,value)
/* int GET_DOUBLE(DictHandle,DictKey,double*) */
#define GET_DOUBLE(handle,key,value) GetDictDouble(handle,key,value)
/* int GET_LONGDOUBLE(DictHandle,DictKey,long double*) */
#define GET_LONGDOUBLE(handle,key,value) GetDictLongDouble(handle,key,value)

/* int GET_STRING(DictHandle,DictKey,char[]) */
#define GET_STRING(handle,key,value) GetDictString(handle,key,value,sizeof(value))
/* int GET_BLOCK(DictHandle,DictKey,char[]) */
#define GET_BLOCK(handle,key,value) GetDictBlock(handle,key,value,sizeof(value))

/* int GET_DICT(DictHandle,DictKey,DictHandle*) */
#define GET_DICT(handle,key,value) GetDictDict(handle,key,value)
/* int GET_ARRAY(DictHandle,DictKey,DictHandle*) */
#define GET_ARRAY(handle,key,value) GetDictArray(handle,key,value)

/* int SET_SHORT(DictHandle,DictKey,short) */
#define SET_SHORT(handle,key,value) SetDictShort(&handle,key,value)
/* int SET_INT(DictHandle,DictKey,int) */
#define SET_INT(handle,key,value) SetDictInt(&handle,key,value)
/* int SET_LONG(DictHandle,DictKey,long) */
#define SET_LONG(handle,key,value) SetDictLong(&handle,key,value)
/* int SET_LONGLONG(DictHandle,DictKey,long long) */
#define SET_LONGLONG(handle,key,value) SetDictLongLong(&handle,key,value)

/* int SET_USHORT(DictHandle,DictKey,unsigned short) */
#define SET_USHORT(handle,key,value) SetDictUShort(&handle,key,value)
/* int SET_UINT(DictHandle,DictKey,unsigned int) */
#define SET_UINT(handle,key,value) SetDictUInt(&handle,key,value)
/* int SET_ULONG(DictHandle,DictKey,unsigned long) */
#define SET_ULONG(handle,key,value) SetDictULong(&handle,key,value)
/* int SET_ULONGLONG(DictHandle,DictKey,unsigned long long) */
#define SET_ULONGLONG(handle,key,value) SetDictULongLong(&handle,key,value)

/* int SET_FLOAT(DictHandle,DictKey,float) */
#define SET_FLOAT(handle,key,value) SetDictFloat(&handle,key,value)
/* int SET_DOUBLE(DictHandle,DictKey,double) */
#define SET_DOUBLE(handle,key,value) SetDictDouble(&handle,key,value)
/* int SET_LONGDOUBLE(DictHandle,DictKey,long double) */
#define SET_LONGDOUBLE(handle,key,value) SetDictLongDouble(&handle,key,value)

/* int SET_STRING(DictHandle,DictKey,char*) */
#define SET_STRING(handle,key,value) SetDictString(&handle,key,value)
/* int SET_BLOCK(DictHandle,DictKey,void*,int) */
#define SET_BLOCK(handle,key,value,size) SetDictBlock(&handle,key,value,size)

/* int SET_DICT(DictHandle,DictKey,DictHandle) */
#define SET_DICT(handle,key,value) SetDictDict(&handle,key,value)
/* int SET_ARRAY(DictHandle,DictKey,DictHandle) */
#define SET_ARRAY(handle,key,value) SetDictArray(&handle,key,value)

/* int ARRAY_AT(DictHandle,int,DictHandle*) */
#define ARRAY_AT(handle,index,value) GetArrayDict(handle,index,value)
/* int ARRAY_ADD(DictHandle,DictHandle) */
#define ARRAY_ADD(handle,value) AddArrayDict(&handle,value)



DictHandle InitDict(int capacity);
void DestroyDict(DictHandle handle);

DictHandle InitArray(int capacity);
void DestroyArray(DictHandle handle);

void ClearDict(DictHandle handle);
void ClearArray(DictHandle handle);

int GetDictShort(DictHandle handle, DictKey key, short *value);
int GetDictInt(DictHandle handle, DictKey key, int *value);
int GetDictLong(DictHandle handle, DictKey key, long *value);
int GetDictLongLong(DictHandle handle, DictKey key, long long *value);

int GetDictUShort(DictHandle handle, DictKey key, unsigned short *value);
int GetDictUInt(DictHandle handle, DictKey key, unsigned int *value);
int GetDictULong(DictHandle handle, DictKey key, unsigned long *value);
int GetDictULongLong(DictHandle handle, DictKey key, unsigned long long *value);

int GetDictFloat(DictHandle handle, DictKey key, float *value);
int GetDictDouble(DictHandle handle, DictKey key, double *value);
int GetDictLongDouble(DictHandle handle, DictKey key, long double *value);

int GetDictString(DictHandle handle, DictKey key, char *value, int size);
int GetDictBlock(DictHandle handle, DictKey key, void *value, int size);

int GetDictDict(DictHandle handle, DictKey key, DictHandle *value);
int GetDictArray(DictHandle handle, DictKey key, DictHandle *value);

int SetDictShort(DictHandle *handle, DictKey key, short value);
int SetDictInt(DictHandle *handle, DictKey key, int value);
int SetDictLong(DictHandle *handle, DictKey key, long value);
int SetDictLongLong(DictHandle *handle, DictKey key, long long value);

int SetDictUShort(DictHandle *handle, DictKey key, unsigned short value);
int SetDictUInt(DictHandle *handle, DictKey key, unsigned int value);
int SetDictULong(DictHandle *handle, DictKey key, unsigned long value);
int SetDictULongLong(DictHandle *handle, DictKey key, unsigned long long value);

int SetDictFloat(DictHandle *handle, DictKey key, float value);
int SetDictDouble(DictHandle *handle, DictKey key, double value);
int SetDictLongDouble(DictHandle *handle, DictKey key, long double value);

int SetDictString(DictHandle *handle, DictKey key, char *value);
int SetDictBlock(DictHandle *handle, DictKey key, void *value, int size);

int SetDictDict(DictHandle *handle, DictKey key, DictHandle value);
int SetDictArray(DictHandle *handle, DictKey key, DictHandle value);

int DeleteDict(DictHandle handle, DictKey key);

int GetArrayLength(DictHandle handle);
int GetArrayDict(DictHandle handle, int index, DictHandle *value);
int AddArrayDict(DictHandle *handle, DictHandle value);

int SerializeDict(DictHandle *handle);

int GetDictNElement(DictHandle handle);
int CloneDict(DictHandle *dest, DictHandle *src, DictKey key);
int CloneDictl(DictHandle *dest, DictHandle *src, ...);
int CloneDictv(DictHandle *dest, DictHandle *src, int keyc, DictKey keyv[]);
int GetDictKeyv(DictHandle handle, int keyc, DictKey keyv[]);

#endif

