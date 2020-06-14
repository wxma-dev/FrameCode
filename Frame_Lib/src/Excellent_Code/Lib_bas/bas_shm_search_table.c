#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stddef.h>
#include <stdarg.h>

#include "bas/bas_global.h"

#define RELATIVE_ADDRESS(pt)    ((void*)(((long)pt - (long)gptShmAppDesc->pvShmBuf)))
#define ABSOLUTE_ADDRESS(pt)    ((void*)(((long)pt + (long)gptShmAppDesc->pvShmBuf)))

#define SHM_MIN_HOLE    256
#define SHM_MAGIC_WORD    "PSEUDO"
#define SHM_NULL    ((void*)-1L)

typedef struct MemoryBlock T_MemoryBlock;
struct MemoryBlock
{
    char caMagicWord[6];
    size_t iSize;
    size_t iActualSize;
    T_MemoryBlock *ptPrev;
    T_MemoryBlock *ptNext;
};

static T_MemoryBlock *ptFreeBlock = NULL;
BAS_EXPORT int    iLogFlag = 1; /* 打印剩余内存参数 0--不打印   1---打印 */

BAS_EXPORT void basInitShmBlock()
{
    long    lHeapSize;
    long    lPad;
    T_ShmSearchBuf *ptSSB = gptShmAppDesc->pvShmBuf;
    void *pvBuf =
        ((char*)gptShmAppDesc->pvShmBuf)+sizeof(*ptSSB);

    /* 边界对齐 */
    lPad = sizeof(double) - ((long)pvBuf) % sizeof(double);
    pvBuf = (void*)(((long)pvBuf) + lPad);
    lHeapSize = gptShmAppDesc->iSize - sizeof(*ptSSB) - lPad;
    if (gptShmAppDesc->bSupport7x24 == BAS_TRUE)
    {
        lHeapSize /= 2;
        ptSSB->aptHashTable[1-ptSSB->iSection] = SHM_NULL;
    }
    else
    {
        ptSSB->aptHashTable[0] = SHM_NULL;
    }
    /* 需要对齐到最大边界 */
    lHeapSize -= lHeapSize % sizeof(double);

    if (gptShmAppDesc->bSupport7x24 == BAS_TRUE)
    {
        ptFreeBlock = (T_MemoryBlock *)(((long)pvBuf) +
                                        lHeapSize * (1 - ptSSB->iSection));
    }
    else
    {
        ptFreeBlock = (T_MemoryBlock *)pvBuf;
    }
    ptFreeBlock->iSize = lHeapSize;
    ptFreeBlock->ptPrev = NULL;
    ptFreeBlock->ptNext = NULL;
}

static T_MemoryBlock* findFreeBlock(size_t size)
{
    T_MemoryBlock *ptMB = ptFreeBlock;
    while (ptMB != NULL)
    {
        if (ptMB->iSize >= size)
        {
            return ptMB;
        }
        ptMB = ptMB->ptNext;
    }
    return NULL;
}

static size_t calcActualSize(size_t size)
{
    size_t actualSize = size + sizeof(T_MemoryBlock);
    /* 需要对齐到最大边界 */
    actualSize += (sizeof(double) - actualSize % sizeof(double));
    return actualSize;
}

static void *shmMalloc(size_t size)
{
    size_t actualSize = calcActualSize(size);
    T_MemoryBlock *ptMB = findFreeBlock(actualSize);
    if (ptMB == NULL)
    {
        return NULL;
    }

    if (ptMB->ptNext == NULL)
    {    /* 链表最后一个节点 */
        T_MemoryBlock *ptLast = (T_MemoryBlock *)((char *)ptMB + actualSize);
        ptLast->iSize = ptMB->iSize - actualSize;
        ptLast->iActualSize = 0;
        ptLast->ptNext = NULL;
        ptLast->ptPrev = ptMB->ptPrev;
        if (ptMB->ptPrev == NULL)
        {
            ptFreeBlock = ptLast;
        }
        else
        {
            ptMB->ptPrev->ptNext = ptLast;
        }

        ptMB->iSize = actualSize;
    }
    else
    {    /* 链表的中间节点 */
        if ((ptMB->iSize - actualSize) < SHM_MIN_HOLE)
        {    /* 分配整个内存块 */
            ptMB->ptNext->ptPrev = ptMB->ptPrev;
            if (ptMB->ptPrev == NULL)
            {
                ptFreeBlock = ptMB->ptNext;
            }
            else
            {
                ptMB->ptPrev->ptNext = ptMB->ptNext;
            }
        }
        else
        {
            T_MemoryBlock *ptNext = (T_MemoryBlock *)((char *)ptMB + actualSize);
            ptNext->iSize = ptMB->iSize - actualSize;
            ptNext->iActualSize = 0;
            ptNext->ptNext = ptMB->ptNext;
            ptNext->ptPrev = ptMB->ptPrev;
            if (ptMB->ptPrev == NULL)
            {
                ptFreeBlock = ptNext;
            }
            else
            {
                ptMB->ptPrev->ptNext = ptNext;
            }

        }
    }

    memcpy(ptMB->caMagicWord, SHM_MAGIC_WORD, sizeof(ptMB->caMagicWord));
    ptMB->iActualSize = size;
    ptMB->ptPrev = ptMB->ptNext = NULL;
    return RELATIVE_ADDRESS((char*)ptMB + sizeof(T_MemoryBlock));
}

static void addToFreeChain(T_MemoryBlock *ptFree)
{
    BOOL iMerged = BAS_FALSE;
    T_MemoryBlock *ptLast = NULL;
    T_MemoryBlock *ptMB = ptFreeBlock;

    memset(ptFree->caMagicWord, '\0', sizeof(ptFree->caMagicWord));
    if (ptFree < ptMB)
    {
        if ((T_MemoryBlock*)((char*)ptFree + ptFree->iSize) == ptMB)
        {    /* 与链表开始的空闲块相邻，合并 */
            ptFree->iSize += ptMB->iSize;
            ptFree->ptNext = ptMB->ptNext;
            ptFree->ptPrev = NULL;
        }
        else
        {    /* 加入列表的最开始 */
            ptFree->ptNext = ptMB;
            ptFree->ptPrev = NULL;
        }
        ptFreeBlock = ptFree;
        return ;
    }
    while (ptMB != NULL)
    {
        if ((ptFree > ptMB) && (ptFree < ptMB->ptNext))
        {    /* 在两个相邻的空闲块之间 */
            if ((T_MemoryBlock*)((char*)ptFree + ptFree->iSize) == ptMB->ptNext)
            {    /* 如果与后一个内存块相邻，进行合并 */
                ptFree->iSize += ptMB->ptNext->iSize;
                ptFree->ptNext = ptMB->ptNext->ptNext;
                ptFree->ptPrev = ptMB->ptNext->ptPrev;
                ptMB->ptNext = ptFree;
                if ((T_MemoryBlock*)((char*)ptMB + ptMB->iSize) == ptFree)
                {    /* 与前一个内存块之间没有空隙，继续合并 */
                    ptMB->iSize += ptFree->iSize;
                    ptMB->ptNext = ptFree->ptNext;
                    if (ptFree->ptNext != NULL)
                    {
                        ptFree->ptNext->ptPrev = ptMB;
                    }
                }
            }
            else if ((T_MemoryBlock*)((char*)ptMB + ptMB->iSize) == ptFree)
            {    /* 与前一个内存块之间没有空隙，合并 */
                ptMB->iSize += ptFree->iSize;
            }
            else
            {    /* 插入列表中 */
                ptFree->ptPrev = ptMB;
                ptFree->ptNext = ptMB->ptNext;
                ptMB->ptNext->ptPrev = ptFree;
                ptMB->ptNext = ptFree;
            }
            return;
        }
        ptLast = ptMB;
        ptMB = ptMB->ptNext;
    }

    if ((T_MemoryBlock*)((char*)ptLast + ptLast->iSize) == ptFree)
    {    /* 与最后一个空闲块相邻，合并 */
        ptLast->iSize += ptFree->iSize;
    }
}

/* pv - relative address
   return - relative address */
static void* shmRealloc(void *pv, size_t size)
{
    BOOL bMerged;
    T_MemoryBlock *ptOld = (T_MemoryBlock *)((char*)ABSOLUTE_ADDRESS(pv) - sizeof(T_MemoryBlock));
    size_t iOldSize = ptOld->iActualSize;
    void *pvNew = NULL;

    if (memcmp(ptOld->caMagicWord, SHM_MAGIC_WORD, sizeof(ptOld->caMagicWord)) != 0)
    {
        fprintf(stderr, "corrupted memory block [0x%8lx]\n", (unsigned long)pv);
        exit(1);
    }
    addToFreeChain(ptOld);

    pvNew = shmMalloc(size);
    if (pvNew == NULL)
    {
        return NULL;
    }
    memmove(ABSOLUTE_ADDRESS(pvNew), ABSOLUTE_ADDRESS(pv), iOldSize);

    return pvNew;
}

/* return - relative address */
static T_HashTable* shmHashTableCreate(void)
{
    int i;
    T_HashEntry **pptTable;
    T_HashTable *ptAbsoluteHT;
    T_HashTable *ptHashTable =
        (T_HashTable *)shmMalloc(sizeof(T_HashTable));
    if (ptHashTable == SHM_NULL)
    {
        return NULL;
    }
    ptAbsoluteHT = ABSOLUTE_ADDRESS(ptHashTable);
    ptAbsoluteHT->pptTable =
        (T_HashEntry **)shmMalloc(sizeof(T_HashEntry*) *
                                  BAS_HASHTABLE_INIT_SIZE);
    if (ptAbsoluteHT->pptTable == SHM_NULL)
    {
        return NULL;
    }
    pptTable = (T_HashEntry**)ABSOLUTE_ADDRESS(ptAbsoluteHT->pptTable);
    for (i=0; i<BAS_HASHTABLE_INIT_SIZE; ++i)
    {
        pptTable[i] = SHM_NULL;
    }
    ptAbsoluteHT->iFill = 0;
    ptAbsoluteHT->iUsed = 0;
    ptAbsoluteHT->iMask = BAS_HASHTABLE_INIT_SIZE-1;
    ptAbsoluteHT->iLimit = BAS_HASHTABLE_INIT_SIZE * BAS_HASHTABLE_LOAD_FACTOR;
    ptAbsoluteHT->pfHashFunc = NULL;
    ptAbsoluteHT->pfHashEqualFunc = NULL;
    ptAbsoluteHT->iFreeKey = BAS_FALSE;
    ptAbsoluteHT->iFreeValue = BAS_FALSE;

    return ptHashTable;
}

static unsigned int hashkey(const void *pvKey)
{
	int i;
	unsigned int k = 0;
	unsigned char *s = (unsigned char *)pvKey;
	int iKeyLength = strlen((char*)pvKey);

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

/* pvKeyA, pvKeyB - absolute address */
static int equalkeys(const void *pvKeyA, const void *pvKeyB)
{
    return strcmp((char*)pvKeyA, (char*)pvKeyB);
}

/* ptHashTable - absolute address
   pvKey - absolute address */
static unsigned int hash(T_HashTable *ptHashTable, const void *pvKey)
{
    unsigned int iHash = hashkey(pvKey);
    iHash += ~(iHash << 9);
    iHash ^= ((iHash >> 14) | (iHash << 18));    /* >>> */
    iHash += (iHash << 4);
    iHash ^= ((iHash >> 10) | (iHash << 22));    /* <<< */
    return iHash;
}

/* ptHashTable - absolute address
   pvKey - absolute address */
static int shmInnerHashTableSearch(T_HashTable *ptHashTable, const void *pvKey)
{
    unsigned int iHashValue, iSlot, iPerturb;
    T_HashEntry **pptTable =
        (T_HashEntry**)ABSOLUTE_ADDRESS(ptHashTable->pptTable);

    iHashValue = hash(ptHashTable, pvKey);
    iSlot = iHashValue & ptHashTable->iMask;
    iPerturb = iHashValue;

    while (BAS_TRUE)
    {
        T_HashEntry *ptEntry = pptTable[iSlot];
        if (ptEntry == SHM_NULL)
            return -1;
        ptEntry = (T_HashEntry*)ABSOLUTE_ADDRESS(ptEntry);
        if ((ptEntry->iHashValue == iHashValue) &&
            (equalkeys(pvKey, ABSOLUTE_ADDRESS(ptEntry->pvKey)) == 0))
        {
            return iSlot;
        }
        iSlot = ((5 * iSlot) + 1 + iPerturb) & ptHashTable->iMask;
        iPerturb >>= 5;
    }

    return -1;
}

/* ptHashTable - absolute address
   pvKey - absolute address
   return - absolute address */
static void* shmHashTableSearch(T_HashTable *ptHashTable, const void *pvKey)
{
    int iIndex;

    iIndex = shmInnerHashTableSearch(ptHashTable, pvKey);
    if (iIndex >= 0)
    {
        T_HashEntry **pptTable =
            (T_HashEntry**)ABSOLUTE_ADDRESS(ptHashTable->pptTable);
        return ABSOLUTE_ADDRESS(((T_HashEntry*)ABSOLUTE_ADDRESS(pptTable[iIndex]))->pvValue);
    }
    else
    {
        return NULL;
    }
}

/* ptHashTable - absolute address */
static int shmHashTableExpand(T_HashTable *ptHashTable)
{
    int iOldSize = ptHashTable->iMask+1;
    int iNewSize = iOldSize * BAS_HASHTABLE_EXPAND_MULTIPLICAND;
    T_HashEntry **pp;
    int i;
    T_HashEntry **pptTable =
        (T_HashEntry**)ABSOLUTE_ADDRESS(ptHashTable->pptTable);
    T_HashEntry **pptTemp =
        (T_HashEntry **)shmMalloc(sizeof(T_HashEntry*) * iOldSize);
    if (pptTemp == SHM_NULL)
    {
        return -1;
    }
    pptTemp = (T_HashEntry **)ABSOLUTE_ADDRESS(pptTemp);

    memcpy(pptTemp, pptTable, sizeof(T_HashEntry*) * iOldSize);
    ptHashTable->pptTable = (T_HashEntry **)shmRealloc(ptHashTable->pptTable,
                                                       sizeof(T_HashEntry*) * iNewSize);
    if (ptHashTable->pptTable == SHM_NULL)
    {
        return -1;
    }
    pptTable = (T_HashEntry**)ABSOLUTE_ADDRESS(ptHashTable->pptTable);
    for (i=0; i<iNewSize; ++i)
    {
        pptTable[i] = SHM_NULL;
    }
    ptHashTable->iMask = iNewSize-1;
    ptHashTable->iLimit = iNewSize * BAS_HASHTABLE_LOAD_FACTOR;

    for (i=0,pp=pptTemp; i<iOldSize; ++i,++pp)
    {
        unsigned int iHashValue, iSlot, iPerturb;
        T_HashEntry *ptSrc = *pp;
        if (ptSrc == SHM_NULL)
        {
            continue;
        }
        ptSrc = (T_HashEntry*)ABSOLUTE_ADDRESS(ptSrc);
        iSlot = ptSrc->iHashValue & ptHashTable->iMask;
        iPerturb = ptSrc->iHashValue;

        while (BAS_TRUE)
        {
            T_HashEntry *ptDst = pptTable[iSlot];
            if (ptDst == SHM_NULL)
                break;
            ptDst = (T_HashEntry*)ABSOLUTE_ADDRESS(ptDst);
            if ((ptDst->iHashValue == ptSrc->iHashValue) &&
                (equalkeys(ABSOLUTE_ADDRESS(ptSrc->pvKey),
                           ABSOLUTE_ADDRESS(ptDst->pvKey)) == 0))
            {
                break;
            }
            iSlot = ((5 * iSlot) + 1 + iPerturb) & ptHashTable->iMask;
            iPerturb >>= 5;
        }
        pptTable[iSlot] = (T_HashEntry*)RELATIVE_ADDRESS(ptSrc);
    }

    return 0;
}

/* ptHashTable - relative address
   pvKey - relative address
   pvValue - relative address */
static int shmHashTableInsert(T_HashTable *ptHashTable, void *pvKey, void *pvValue)
{
    T_HashEntry **pptTable;
    T_HashEntry *ptEntry;
    unsigned int iHashValue, iSlot, iPerturb;
    int iIndex;
    void* pvAbsoluteKey;

    ptHashTable = ABSOLUTE_ADDRESS(ptHashTable);
    pvAbsoluteKey = ABSOLUTE_ADDRESS(pvKey);

    iIndex = shmInnerHashTableSearch(ptHashTable, pvAbsoluteKey);
    pptTable = (T_HashEntry**)ABSOLUTE_ADDRESS(ptHashTable->pptTable);
    if (iIndex >= 0)
    {    /* 替换原来的值 */
        ((T_HashEntry*)ABSOLUTE_ADDRESS(pptTable[iIndex]))->pvValue = pvValue;
        return 0;
    }

    if (++ptHashTable->iUsed > ptHashTable->iLimit)
    {
        if (shmHashTableExpand(ptHashTable) != 0)
        {
            return -1;
        }
        /* expand之后pptTable的地址已经发生变化，因此需要重新定位 */
        pptTable = (T_HashEntry**)ABSOLUTE_ADDRESS(ptHashTable->pptTable);
    }
    /* find a empty slot */
    iHashValue = hash(ptHashTable, pvAbsoluteKey);
    iSlot = iHashValue & ptHashTable->iMask;
    iPerturb = iHashValue;

    while (BAS_TRUE)
    {
        T_HashEntry *ptDst = pptTable[iSlot];
        if (ptDst == SHM_NULL)
            break;
        ptDst = (T_HashEntry*)ABSOLUTE_ADDRESS(ptDst);
        if ((ptDst->iHashValue == iHashValue) &&
            (equalkeys(pvAbsoluteKey, ABSOLUTE_ADDRESS(ptDst->pvKey)) == 0))
        {
            break;
        }
        iSlot = ((5 * iSlot) + 1 + iPerturb) & ptHashTable->iMask;
        iPerturb >>= 5;
    }
    ptEntry = (T_HashEntry *)shmMalloc(sizeof(T_HashEntry));
    if (ptEntry == SHM_NULL)
    {
        return -1;
    }
    pptTable[iSlot] = ptEntry;
    ptEntry = ABSOLUTE_ADDRESS(ptEntry);
    ptEntry->iHashValue = iHashValue;
    ptEntry->pvKey = pvKey;
    ptEntry->pvValue = pvValue;

    return 0;
}

/* pvKey - absolute address */
#define    MAX_TABLE_KEY_LENGTH    6

static char* formKey(const T_SearchTableDef *ptSearchTableDef, const void *pvContent)
{
    static char mcaKey[512];
    char *s;
    int i;
    T_SearchFieldDef *ptField;

    memset(mcaKey, '\0', sizeof(mcaKey));
    for (i=0,ptField=ptSearchTableDef->ptKeyDefArray,s=mcaKey;
         i<ptSearchTableDef->iKeyDefArrayLength; ++i,++ptField)
    {
        switch (ptField->eType)
        {
        case FIELD_TYPE_CHAR:
            sprintf(s, "%-*.*s", ptField->iLength, ptField->iLength, (char*)pvContent+ptField->iOffset);
            break;
        case FIELD_TYPE_LONG:
            sprintf(s, "%0*ld", ptField->iLength, *(long*)((char*)pvContent+ptField->iOffset));
            break;
        case FIELD_TYPE_DOUBLE:
            sprintf(s, "%0*.*f", ptField->iLength-ptField->iScale, ptField->iScale, *(double*)((char*)pvContent+ptField->iOffset));
            break;
        default:
            return NULL;
        }
        s += strlen(s);
    }
    return mcaKey;
}

static char* formKeyVararg(const T_SearchTableDef *ptSearchTableDef, va_list ap)
{
    static char mcaKey[512];
    char *s;
    int i;
    T_SearchFieldDef *ptField;

    memset(mcaKey, '\0', sizeof(mcaKey));
    for (i=0,ptField=ptSearchTableDef->ptKeyDefArray,s=mcaKey;
         i<ptSearchTableDef->iKeyDefArrayLength; ++i,++ptField)
    {
        switch (ptField->eType)
        {
        case FIELD_TYPE_CHAR:
            sprintf(s, "%-*.*s", ptField->iLength, ptField->iLength, va_arg(ap, char*));
            break;
        case FIELD_TYPE_LONG:
            sprintf(s, "%0*ld", ptField->iLength, va_arg(ap, long));
            break;
        case FIELD_TYPE_DOUBLE:
            sprintf(s, "%0*.*f", ptField->iLength-ptField->iScale, ptField->iScale, va_arg(ap, double));
            break;
        default:
            return NULL;
        }
        s += strlen(s);
    }
    return mcaKey;
}

BAS_EXPORT int basLoadSearchTable(const T_SearchTableDef *ptSearchTableDef)
{
    int i;
    int iRet;
    int iIndex = 0;
    void *ptCursor;
    char *pcRelKey;
    void *pvContent;
    char **ppcKeys;
    void **pptTable;
    T_SearchTableDef *ptRelSTD, *ptAbsoluteSTD;
    T_ShmSearchBuf *ptSSB = (T_ShmSearchBuf*)gptShmAppDesc->pvShmBuf;
    T_HashTable *ptRelHT;

    if (ptSSB->bSupport7x24 == BAS_TRUE)
    {
        ptRelHT = ptSSB->aptHashTable[1-ptSSB->iSection];
    }
    else
    {
        ptRelHT = ptSSB->aptHashTable[ptSSB->iSection];
    }

    if (ptRelHT == SHM_NULL)
    {
        if ((ptRelHT = shmHashTableCreate()) == SHM_NULL)
        {
            BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
            return -1;
        }
        if (ptSSB->bSupport7x24 == BAS_TRUE)
        {
            ptSSB->aptHashTable[1-ptSSB->iSection] = ptRelHT;
        }
        else
        {
            ptSSB->aptHashTable[ptSSB->iSection] = ptRelHT;
        }
    }

    ptCursor = malloc(ptSearchTableDef->iStdSize);
    if (ptCursor == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
        return -1;
    }

    pcRelKey = (char*)shmMalloc(strlen(ptSearchTableDef->pcSearchName) + 1);
    if (pcRelKey == SHM_NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
        return -1;
    }
    strcpy((char*)ABSOLUTE_ADDRESS(pcRelKey), ptSearchTableDef->pcSearchName);

    ptRelSTD = (T_SearchTableDef*)shmMalloc(sizeof(T_SearchTableDef));
    if (ptRelSTD == SHM_NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
        return -1;
    }
    ptAbsoluteSTD = ABSOLUTE_ADDRESS(ptRelSTD);
    memcpy(ptAbsoluteSTD, ptSearchTableDef, sizeof(T_SearchTableDef));
    ptAbsoluteSTD->pcSearchName =
        (char*)shmMalloc(strlen(ptSearchTableDef->pcSearchName)+1);
    if (ptAbsoluteSTD->pcSearchName == SHM_NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
        return -1;
    }
    strcpy(ABSOLUTE_ADDRESS(ptAbsoluteSTD->pcSearchName),
           ptSearchTableDef->pcSearchName);
    ptAbsoluteSTD->ptKeyDefArray =
        (T_SearchFieldDef *)shmMalloc(sizeof(T_SearchFieldDef)*ptAbsoluteSTD->iKeyDefArrayLength);
    if (ptAbsoluteSTD->ptKeyDefArray == SHM_NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
        return -1;
    }
    {
        T_SearchFieldDef *ptKD = ABSOLUTE_ADDRESS(ptAbsoluteSTD->ptKeyDefArray);
        for (i=0; i<ptAbsoluteSTD->iKeyDefArrayLength; ++i,++ptKD)
        {
            memcpy(ptKD, &ptSearchTableDef->ptKeyDefArray[i], sizeof(T_SearchFieldDef));
        }
    }


    if (shmHashTableInsert(ptRelHT, pcRelKey, ptRelSTD) != 0)
    {
        return -1;
    }
    (*ptSearchTableDef->pfStdInit)(ptCursor);
    if ((*ptSearchTableDef->pfCursorOper)(BAS_DBCUROPEN, ptCursor) != BAS_DBOK)
    {
        return -1;
    }
    ptAbsoluteSTD->pvContent = shmMalloc(ptSearchTableDef->iStdSize*10);
    if (ptAbsoluteSTD->pvContent == SHM_NULL)
    {
        return -1;
    }
    pvContent = ABSOLUTE_ADDRESS(ptAbsoluteSTD->pvContent);
    ptAbsoluteSTD->iContentSize = 10;
    ptAbsoluteSTD->iContentCnt = 0;
    while ((iRet = (*ptSearchTableDef->pfCursorOper)(BAS_DBCURFETCH, ptCursor)) == BAS_DBOK)
    {
        if (iIndex > ptAbsoluteSTD->iContentSize-1)
        {
            ptAbsoluteSTD->iContentSize *= 2;
            ptAbsoluteSTD->pvContent = shmRealloc(ptAbsoluteSTD->pvContent,
                                                  ptAbsoluteSTD->iStdSize*ptAbsoluteSTD->iContentSize);
            if (ptAbsoluteSTD->pvContent == NULL)
            {
				BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
                return -1;
            }
            pvContent = ABSOLUTE_ADDRESS(ptAbsoluteSTD->pvContent);
        }
        memcpy((char*)pvContent+ptAbsoluteSTD->iStdSize*iIndex++,
               ptCursor, ptAbsoluteSTD->iStdSize);
    }
    ptAbsoluteSTD->iContentCnt = iIndex;
    (*ptSearchTableDef->pfCursorOper)(BAS_DBCURCLOSE, ptCursor);
    if (iRet != BAS_DBNOTFOUND)
    {
        return -1;
    }
    free(ptCursor);

    switch (ptAbsoluteSTD->eSearchType)
    {
    case SEARCH_TYPE_HASH:
        ptAbsoluteSTD->ptHashTable = shmHashTableCreate();
        if (ptAbsoluteSTD->ptHashTable == SHM_NULL)
        {
            BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
            return -1;
        }

        pvContent = ABSOLUTE_ADDRESS(ptAbsoluteSTD->pvContent);
        for (iIndex=0; iIndex<ptAbsoluteSTD->iContentCnt; ++iIndex)
        {
            void *ptValue = (char*)pvContent+ptAbsoluteSTD->iStdSize*iIndex;
            char *s = formKey(ptSearchTableDef, ptValue);
            char *pcKey = (char*)shmMalloc(sizeof(char)*(strlen(s)+1));
            if (pcKey == SHM_NULL)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
                return -1;
            }
            strcpy(ABSOLUTE_ADDRESS(pcKey), s);
            if (shmHashTableInsert(ptAbsoluteSTD->ptHashTable, pcKey,
                                   RELATIVE_ADDRESS(ptValue)) != 0)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
                return -1;
            }
        }
        break;
    case SEARCH_TYPE_BINARY:
    default:
        ptAbsoluteSTD->ppcKeys =
            (char **)shmMalloc(sizeof(char*) * ptAbsoluteSTD->iContentCnt);
        if (ptAbsoluteSTD->ppcKeys == SHM_NULL)
        {
            BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
            return -1;
        }
        ppcKeys = (char**)ABSOLUTE_ADDRESS(ptAbsoluteSTD->ppcKeys);

        pvContent = ABSOLUTE_ADDRESS(ptAbsoluteSTD->pvContent);
        for (iIndex=0; iIndex<ptAbsoluteSTD->iContentCnt; ++iIndex)
        {
            void *ptValue = (char*)pvContent+ptAbsoluteSTD->iStdSize*iIndex;
            char *s = formKey(ptSearchTableDef, ptValue);
            char *pcKey = (char*)shmMalloc(sizeof(char)*(strlen(s)+1));
            if (pcKey == SHM_NULL)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
                return -1;
            }
            strcpy(ABSOLUTE_ADDRESS(pcKey), s);
            ppcKeys[iIndex] = pcKey;
        }
        break;
    case SEARCH_TYPE_TABLE:
        {
            T_SearchFieldDef *ptPK =
                (T_SearchFieldDef*)ABSOLUTE_ADDRESS(ptAbsoluteSTD->ptKeyDefArray);

            if ((ptAbsoluteSTD->iKeyDefArrayLength != 1) ||
                (ptPK->eType != FIELD_TYPE_LONG) ||
                (ptPK->iLength > MAX_TABLE_KEY_LENGTH))
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "key is too long");
                return -1;
            }
            {
                int iSize, i;
                for (i=0,iSize=1; i<ptPK->iLength; ++i)
                {
                    iSize *= 10;
                }
                ptAbsoluteSTD->pptTable = (void **)shmMalloc(sizeof(void*) * iSize);
                if (ptAbsoluteSTD->pptTable == SHM_NULL)
                {
                    BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
                    return -1;
                }

                pptTable = ABSOLUTE_ADDRESS(ptAbsoluteSTD->pptTable);
                for (i=0; i<iSize; ++i)
                {
                    pptTable[i] = SHM_NULL;
                }
            }

            pvContent = ABSOLUTE_ADDRESS(ptAbsoluteSTD->pvContent);
            for (iIndex=0; iIndex<ptAbsoluteSTD->iContentCnt; ++iIndex)
            {
                void *ptValue = (char*)pvContent+ptAbsoluteSTD->iStdSize*iIndex;
                long lKey = *(long *)((char*)ptValue + ptPK->iOffset);
                pptTable[lKey] = RELATIVE_ADDRESS(ptValue);
            }
        }
        break;
    }

	if( iLogFlag == 1)
	{
		if(memcmp(gptShmAppDesc->pcDesc, "online", 6) == 0)
		{
			BAS_LOG(BAS_LOGINFO, 0, 0, "ptSearchTableDef->pcSearchName=[%s], iFreeBlock->iSize=[%ld], gptShmAppDesc->iSize=[%ld], USAGE=[%d%%]\n",
			                        ptSearchTableDef->pcSearchName, ptFreeBlock->iSize*2, gptShmAppDesc->iSize, ptFreeBlock->iSize*2*100/gptShmAppDesc->iSize);
		}
		else
		{
			BAS_LOG(BAS_LOGINFO, 0, 0, "ptSearchTableDef->pcSearchName=[%s], iFreeBlock->iSize=[%ld], gptShmAppDesc->iSize=[%ld], USAGE=[%d%%]\n",
			                        ptSearchTableDef->pcSearchName, ptFreeBlock->iSize, gptShmAppDesc->iSize, ptFreeBlock->iSize*100/gptShmAppDesc->iSize);
		}
	}
		
    return 0;
}

BAS_EXPORT T_SearchTableDef* basGetShmSearchTableDef(const char *pcSearchName, T_SearchTableDef *ptFixedSTD)
{
    T_ShmSearchBuf *ptSSB;
    T_HashTable *ptRelHT;
    T_SearchTableDef *ptSTD;

	iLogFlag =0;
    if (gptShmAppDesc == NULL)
    {
        return NULL;
    }
    ptSSB = (T_ShmSearchBuf*)gptShmAppDesc->pvShmBuf;
    ptRelHT = ptSSB->aptHashTable[ptSSB->iSection];

    if (ptRelHT == SHM_NULL)
    {
        return NULL;
    }
    if (pcSearchName == NULL)
    {
        return NULL;
    }

    ptSTD =
        (T_SearchTableDef*)shmHashTableSearch(ABSOLUTE_ADDRESS(ptRelHT),
                                              pcSearchName);
    if (ptSTD == NULL)
    {
        return NULL;
    }

    memcpy(ptFixedSTD, ptSTD, sizeof(T_SearchTableDef));
    ptFixedSTD->pcSearchName = ABSOLUTE_ADDRESS(ptFixedSTD->pcSearchName);
    ptFixedSTD->ptKeyDefArray = ABSOLUTE_ADDRESS(ptFixedSTD->ptKeyDefArray);
    ptFixedSTD->ptHashTable = ABSOLUTE_ADDRESS(ptFixedSTD->ptHashTable);
    ptFixedSTD->pvContent = ABSOLUTE_ADDRESS(ptFixedSTD->pvContent);

    return ptFixedSTD;
}

BAS_EXPORT void* basSearchTableByShmDef(const T_SearchTableDef *ptSTD, ...)
{
    char *pcKey;
    void *pvValue;
    va_list ap;

    va_start(ap, ptSTD);
    if (ptSTD == NULL)
    {
        return NULL;
    }

    switch (ptSTD->eSearchType)
    {
    case SEARCH_TYPE_HASH:
        pcKey = formKeyVararg(ptSTD, ap);
        pvValue = shmHashTableSearch(ptSTD->ptHashTable, pcKey);
        va_end(ap);
        return pvValue;
    case SEARCH_TYPE_BINARY:
    default:
        pcKey = formKeyVararg(ptSTD, ap);
        {
            int high = ptSTD->iContentCnt;
            int low = -1;
            int probe;
            while (high - low > 1)
            {
                int probe = (low + high) >> 1;
                if (strcmp(ABSOLUTE_ADDRESS(ptSTD->ppcKeys[probe]), pcKey) > 0)
                {
                    high = probe;
                }
                else
                {
                    low = probe;
                }
            }
            if ((low == -1) ||
                (strcmp(ABSOLUTE_ADDRESS(ptSTD->ppcKeys[low]), pcKey) != 0))
            {
                va_end(ap);
                return NULL;
            }
            else
            {
                pvValue = (char*)ptSTD->pvContent + ptSTD->iStdSize * low;
                va_end(ap);
                return pvValue;
            }
        }
    case SEARCH_TYPE_TABLE:
        {
            long lKey = va_arg(ap, long);
            if (ptSTD->pptTable[lKey] == SHM_NULL)
            {
                va_end(ap);
                return NULL;
            }
            else
            {
                pvValue = ABSOLUTE_ADDRESS(ptSTD->pptTable[lKey]);
                va_end(ap);
                return pvValue;
            }
        }
    }
    va_end(ap);
    return NULL;
}

BAS_EXPORT void* basShmSearchTable(const char *pcSearchName, ...)
{
    T_SearchTableDef tSTD;
    void *ptResult;
    va_list ap;

    va_start(ap, pcSearchName);
    if (basGetShmSearchTableDef(pcSearchName, &tSTD) == NULL)
    {
        va_end(ap);
        return NULL;
    }

    ptResult = basSearchTableByShmDef(&tSTD, ap);
    va_end(ap);
    return ptResult;
}

static int binarySearch(const T_SearchTableDef *ptSTD, const char* pcKey)
{
    int high = ptSTD->iContentCnt;
    int low = -1;
    int iRet;

    while (high - low > 1)
    {
        int probe = (low + high) >> 1;
        if (strcmp(ABSOLUTE_ADDRESS(ptSTD->ppcKeys[probe]), pcKey) > 0)
        {
            high = probe;
        }
        else
        {
            low = probe;
        }
    }

    iRet = strcmp(ABSOLUTE_ADDRESS(ptSTD->ppcKeys[low]), pcKey);
    if (low == -1)
    {
        return 0;
    }
    else if (iRet == 0)
    {
        return low;
    }
    else if (iRet > 0)
    {
        return low;
    }
    else
    {
        return low+1;
    }
}

static int lowbinarySearch(const T_SearchTableDef *ptSTD, const char* pcKey )
{
    int high = ptSTD->iContentCnt;
    int low = -1;
    int i;
    int here=0;
    int iRet;

    for (i=0;i<ptSTD->iContentCnt;i++)
    {
        if (strcmp(ABSOLUTE_ADDRESS(ptSTD->ppcKeys[i]), pcKey) >= 0)
        {
				break;
		}
    }
	return i;
}

static int highbinarySearch(const T_SearchTableDef *ptSTD, const char* pcKey )
{
    int high = ptSTD->iContentCnt;
    int low = -1;
    int i;
    int here=0;
    int iRet;

    for (i=0;i< ptSTD->iContentCnt;i++)
    {
        if (strcmp(ABSOLUTE_ADDRESS(ptSTD->ppcKeys[i]), pcKey) > 0)
        {
				break;
		}
    }
	return i;
}

static int orderSearch(const T_SearchTableDef *ptSTD, const char* pcKey,int flag )
{
    int high = ptSTD->iContentCnt;
    int low = -1;
    int i;
    int here=0;
    int iRet;

    for (i=0;i<ptSTD->iContentCnt;i++)
    {
        if (strcmp(ABSOLUTE_ADDRESS(ptSTD->ppcKeys[i]), pcKey) == 0)
        {
			if ( flag == 0 )
			{
				break;
			} else if ( flag == 1 )
			{
				here = 1;
				continue;
			}
		} else if ( (flag == 1) && (here == 1) ) {
			i--;
			break;
		} else if ( (here == 0) && (i==ptSTD->iContentCnt)) {
			return 0;
		}
    }
	if ( i == ptSTD->iContentCnt )
	   i--;
	return i;
}


BAS_EXPORT T_ShmIterator* basShmIterator(const char* pcSearchName, int iKeyCnt, ...)
{
    T_ShmIterator *ptItr;
    T_SearchTableDef tSTD;
    va_list ap;
	int iflag;

    va_start(ap, iKeyCnt);
    if (basGetShmSearchTableDef(pcSearchName, &tSTD) == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0,
                "can't find table[%s] define", pcSearchName);
        va_end(ap);
        return NULL;
    }
    if (tSTD.eSearchType != SEARCH_TYPE_BINARY)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "iterator must used in binary search");
        va_end(ap);
        return NULL;
    }

    ptItr = (T_ShmIterator*)malloc(sizeof(T_ShmIterator));
    if (ptItr == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "out of memory");
        va_end(ap);
        return NULL;
    }
    ptItr->iValueSize = tSTD.iStdSize;

    {
        static char mcaKey[512];
        T_SearchFieldDef *ptField;
        int i;
        char *s;
        int low,high;

        if (iKeyCnt > tSTD.iKeyDefArrayLength)
        {
            iKeyCnt = tSTD.iKeyDefArrayLength;
        }
        // form the lowest key
        memset(mcaKey, '\0', sizeof(mcaKey));
        for (i=0,ptField=tSTD.ptKeyDefArray,s=mcaKey;
             i<iKeyCnt; ++i,++ptField)
        {
            switch (ptField->eType)
            {
            case FIELD_TYPE_CHAR:
                sprintf(s, "%-*.*s", ptField->iLength, ptField->iLength, va_arg(ap, char*));
                break;
            case FIELD_TYPE_LONG:
                sprintf(s, "%0*ld", ptField->iLength, va_arg(ap, long));
                break;
            case FIELD_TYPE_DOUBLE:
                sprintf(s, "%0*.*f", ptField->iLength-ptField->iScale, ptField->iScale, va_arg(ap, double));
                break;
            default:
                return NULL;
            }
            s += strlen(s);
        }
        //low = binarySearch(ptSTD, mcaKey);
        low = lowbinarySearch(&tSTD, mcaKey);
        ptItr->pvBeginValue = (char*)tSTD.pvContent + tSTD.iStdSize * low;
        // form the highest key
        mcaKey[strlen(mcaKey)] = 0xff;
        //high = binarySearch(ptSTD, mcaKey);
        high = highbinarySearch(&tSTD, mcaKey);
        ptItr->pvEndValue = (char*)tSTD.pvContent + tSTD.iStdSize * high;
    }
    ptItr->pvCurrValue = ptItr->pvBeginValue;
    va_end(ap);
    return ptItr;
}

