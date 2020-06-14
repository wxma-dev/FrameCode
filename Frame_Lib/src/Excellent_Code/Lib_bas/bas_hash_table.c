#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "bas/bas_global.h"

BAS_EXPORT T_HashTable* basHashTableCreate(T_HashFunc pfHashFunc, T_HashEqualFunc pfHashEqualFunc, int iFreeKey, int iFreeValue)
{
	T_HashTable *ptHashTable = (T_HashTable *)malloc(sizeof(T_HashTable));
	if (ptHashTable == NULL)
	{
		return NULL;
	}
	ptHashTable->pptTable = (T_HashEntry **)malloc(sizeof(T_HashEntry*) * BAS_HASHTABLE_INIT_SIZE);
	if (ptHashTable->pptTable == NULL)
	{
		free(ptHashTable);
		return NULL;
	}
	memset(ptHashTable->pptTable, '\0', sizeof(T_HashEntry*) * BAS_HASHTABLE_INIT_SIZE);
	ptHashTable->iFill = 0;
	ptHashTable->iUsed = 0;
	ptHashTable->iMask = BAS_HASHTABLE_INIT_SIZE - 1;
	ptHashTable->iLimit = BAS_HASHTABLE_INIT_SIZE * BAS_HASHTABLE_LOAD_FACTOR;
	ptHashTable->pfHashFunc = pfHashFunc;
	ptHashTable->pfHashEqualFunc = pfHashEqualFunc;
	ptHashTable->iFreeKey = iFreeKey;
	ptHashTable->iFreeValue = iFreeValue;

	return ptHashTable;
}

BAS_EXPORT unsigned int basDefaultHashFunc(void *pvKey, int iKeyLength)
{
	int i;
	unsigned int k = 0;
	unsigned char *s = (unsigned char *)pvKey;
	
	for (i=0; i<iKeyLength; ++i,++s)
	{
		k += ~(k << 9);
		k ^= ((k >> 14) | (k << 18));
		k += (k << 4);
		k ^= ((k >> 10) | (k << 22));
		k += *s;
	}
	return k;
}

BAS_EXPORT int basDefaultHashEqual(void *pvKeyA, void *pvKeyB)
{
	return strcmp((char*)pvKeyA, (char*)pvKeyB);
}

static unsigned int hash(T_HashTable *ptHashTable, void *pvKey)
{
	unsigned int iHash = (*ptHashTable->pfHashFunc)(pvKey);
	iHash += ~(iHash << 9);
	iHash ^= ((iHash >> 14) | (iHash << 18));    /* >>> */
	iHash += (iHash << 4);
	iHash ^= ((iHash >> 10) | (iHash << 22));	/* <<< */
	return iHash;
}

static int basInnerHashTableSearch(T_HashTable *ptHashTable, void *pvKey)
{
	unsigned int iHashValue, iSlot, iPerturb;

	iHashValue = hash(ptHashTable, pvKey);
	iSlot = iHashValue & ptHashTable->iMask;
	iPerturb = iHashValue;

	while ((ptHashTable->pptTable[iSlot] != NULL) && 
			(ptHashTable->pptTable[iSlot]->iHashValue != iHashValue) &&
			((*ptHashTable->pfHashEqualFunc)(pvKey,ptHashTable->pptTable[iSlot]->pvKey) != 0))
	{
		iSlot = ((5 * iSlot) + 1 + iPerturb) & ptHashTable->iMask;
		iPerturb >>= 5;
	}

	if (ptHashTable->pptTable[iSlot] == NULL)
	{
		return -1;
	}
	else
	{
		return iSlot;
	}
}

BAS_EXPORT void *basHashTableSearch(T_HashTable *ptHashTable, void *pvKey)
{
	int iIndex;

	iIndex = basInnerHashTableSearch(ptHashTable, pvKey);
	if (iIndex >= 0)
	{
		return ptHashTable->pptTable[iIndex]->pvValue;
	}
	else
	{
		return NULL;
	}
}

static int basHashTableExpand(T_HashTable *ptHashTable)
{
	int iOldSize = ptHashTable->iMask+1;
	int iNewSize = iOldSize * BAS_HASHTABLE_EXPAND_MULTIPLICAND;
	T_HashEntry **pp;
	int i;
	T_HashEntry **pptTemp = (T_HashEntry **)malloc(sizeof(T_HashEntry*) * iOldSize);
	if (pptTemp == NULL)
	{
		return -1;
	}
	memcpy(pptTemp, ptHashTable->pptTable, sizeof(T_HashEntry*) * iOldSize);
	ptHashTable->pptTable = (T_HashEntry **)realloc(ptHashTable->pptTable, sizeof(T_HashEntry*) * iNewSize);
	if (ptHashTable->pptTable == NULL)
	{
		return -1;
	}
	memset(ptHashTable->pptTable, '\0', sizeof(T_HashEntry*) * iNewSize);
	ptHashTable->iMask = iNewSize-1;
	ptHashTable->iLimit = iNewSize * BAS_HASHTABLE_LOAD_FACTOR;

	for (i=0,pp=pptTemp; i<iOldSize; ++i,++pp)
	{
		unsigned int iHashValue, iSlot, iPerturb;
		if (*pp == NULL)
		{
			continue;
		}
		iSlot = (*pp)->iHashValue & ptHashTable->iMask;
		iPerturb = (*pp)->iHashValue;

		while ((ptHashTable->pptTable[iSlot] != NULL) && 
				(ptHashTable->pptTable[iSlot]->iHashValue != iHashValue) &&
				((*ptHashTable->pfHashEqualFunc)((*pp)->pvKey,ptHashTable->pptTable[iSlot]->pvKey) != 0))
		{
			iSlot = ((5 * iSlot) + 1 + iPerturb) & ptHashTable->iMask;
			iPerturb >>= 5;
		}
		ptHashTable->pptTable[iSlot] = *pp;
	}

	return 0;
}

BAS_EXPORT int basHashTableInsert(T_HashTable *ptHashTable, void *pvKey, void *pvValue)
{
	T_HashEntry *ptEntry;
	unsigned int iHashValue, iSlot, iPerturb;
	int iIndex;

	iIndex = basInnerHashTableSearch(ptHashTable, pvKey);
	if (iIndex >= 0)
	{	/* 替换原来的值 */
		ptHashTable->pptTable[iIndex]->pvValue = pvValue;
		return 0;
	}

	if (++ptHashTable->iUsed > ptHashTable->iLimit)
	{
		if (basHashTableExpand(ptHashTable) != 0)
		{
			return -1;
		}
	}
	ptEntry = (T_HashEntry *)malloc(sizeof(T_HashEntry));
	if (ptEntry == NULL)
	{
		return -1;
	}
	ptEntry->iHashValue = iHashValue = hash(ptHashTable, pvKey);
	ptEntry->pvKey = pvKey;
	ptEntry->pvValue = pvValue;

	iSlot = iHashValue & ptHashTable->iMask;
	iPerturb = ptEntry->iHashValue;

	while ((ptHashTable->pptTable[iSlot] != NULL) && 
			(ptHashTable->pptTable[iSlot]->iHashValue != iHashValue) &&
			((*ptHashTable->pfHashEqualFunc)(pvKey,ptHashTable->pptTable[iSlot]->pvKey) != 0))
	{
		iSlot = ((5 * iSlot) + 1 + iPerturb) & ptHashTable->iMask;
		iPerturb >>= 5;
	}
	ptHashTable->pptTable[iSlot] = ptEntry;

	return 0;
}

BAS_EXPORT void* basHashTableRemove(T_HashTable *ptHashTable, void *pvKey)
{
	int iIndex;

	iIndex = basInnerHashTableSearch(ptHashTable, pvKey);
	if (iIndex >= 0)
	{
		T_HashEntry *ptEntry = ptHashTable->pptTable[iIndex];
		void *pvValue = ptEntry->pvValue;
		if (ptHashTable->iFreeKey == BAS_TRUE)
		{
			free(ptEntry->pvKey);
		}
		free(ptEntry);
		ptHashTable->pptTable[iIndex] = NULL;
		--ptHashTable->iUsed;

		return pvValue;
	}
	else
	{
		return NULL;
	}
}

BAS_EXPORT void basHashTableDestroy(T_HashTable *ptHashTable)
{
	T_HashEntry **pptEntry;
	int i;
	for (pptEntry=ptHashTable->pptTable,i=0; i<ptHashTable->iMask+1; ++i, ++pptEntry)
	{
		if (*pptEntry != NULL)
		{
			if (ptHashTable->iFreeKey == BAS_TRUE)
			{
				free((*pptEntry)->pvKey);
			}
			if (ptHashTable->iFreeValue == BAS_TRUE)
			{
				free((*pptEntry)->pvValue);
			}
			free(*pptEntry);
		}
	}
	free(ptHashTable->pptTable);
	free(ptHashTable);
}

