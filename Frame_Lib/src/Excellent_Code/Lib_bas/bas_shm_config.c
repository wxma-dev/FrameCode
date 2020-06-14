#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include "libxml2/libxml/parser.h"
#include "libxml2/libxml/tree.h"
#include "libxml2/libxml/xpath.h"
#include "iconv.h"
#include "bas/bas_global.h"

#define DATA_BULK   10
#define MIN_SHM_SIZE 1

BAS_EXPORT T_ShmAppDesc* gptShmAppDesc=NULL;

static T_ShmSearchData* loadShmAppDesc(xmlNodePtr ptNode, T_ShmAppDesc *ptAppDesc)
{
    ptAppDesc->ptShmSearchData = (T_ShmSearchData*) malloc(sizeof(T_ShmSearchData)*DATA_BULK);
    if (ptAppDesc->ptShmSearchData == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
        return NULL;
    }
    ptAppDesc->iSearchDataCnt = 0;
    while (ptNode != NULL)
    {
        if (xmlStrcmp(ptNode->name, BAD_CAST "search_data") == 0)
        {
            xmlChar* pcName = xmlGetProp(ptNode, BAD_CAST "name");
            xmlChar* pcLoader = xmlGetProp(ptNode, BAD_CAST "loader");
            xmlChar* pcLib = xmlGetProp(ptNode, BAD_CAST "libpath");

            if (pcName == NULL)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "table: name property missing");
                return NULL;
            }
            if (pcLoader == NULL)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "table: loader property missing");
                return NULL;
            }
            if (pcLib == NULL)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "table: libpath property missing");
                return NULL;
            }
            if ((++ptAppDesc->iSearchDataCnt % DATA_BULK) == 0)
            {
                ptAppDesc->ptShmSearchData =
                    (T_ShmSearchData*) realloc(ptAppDesc->ptShmSearchData,
                                               sizeof(T_ShmSearchData)*(DATA_BULK+ptAppDesc->iSearchDataCnt));
                if (ptAppDesc->ptShmSearchData == NULL)
                {
                    BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
                    return NULL;
                }
            }
            {
                T_ShmSearchData* ptShmSearchData =
                    ptAppDesc->ptShmSearchData+ptAppDesc->iSearchDataCnt-1;
                ptShmSearchData->pcName = strdup((char*)pcName);
                ptShmSearchData->pcLoader = strdup((char*)pcLoader);
                ptShmSearchData->pcLib = strdup((char*)pcLib);
            }
        }
        ptNode = ptNode->next;
    }

    return 0;
}

static T_ShmAppDesc* loadShmConfig(const char* pcFileName,
                                   const char* pcCharacterSet,
                                   const char* pcAppName)
{
    xmlDocPtr ptDoc;
    xmlNodePtr ptRoot, ptNode;
    xmlXPathContextPtr ptContext;
    xmlXPathObjectPtr ptObject;
    xmlNodeSetPtr ptNodeSet;
    int i;
    T_ShmAppDesc *ptShmAppDesc;
    char caBuff[BAS_LDESC_LEN + 1];

    ptDoc = xmlReadFile(pcFileName, pcCharacterSet, XML_PARSE_RECOVER);
    if (ptDoc == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "open [%s] error", pcFileName);
        return NULL;
    }

    ptRoot = xmlDocGetRootElement(ptDoc);
    if (ptRoot == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "file[%s] is empty", pcFileName);
        return NULL;
    }
    if (xmlStrcmp(ptRoot->name, BAD_CAST "shm_config") != 0)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "root node is not shm_search");
        return NULL;
    }

    ptShmAppDesc = (T_ShmAppDesc*) malloc(sizeof(T_ShmAppDesc));
    if (ptShmAppDesc == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "not enough memory");
        return NULL;
    }
    memset(ptShmAppDesc, '\0', sizeof(T_ShmAppDesc));

    ptNode = ptRoot->xmlChildrenNode;
    while (ptNode != NULL)
    {
        if ((xmlStrcmp(ptNode->name, BAD_CAST "application") == 0) &&
            (xmlGetProp(ptNode, BAD_CAST "name") != NULL) &&
            (xmlStrcmp(xmlGetProp(ptNode, BAD_CAST "name"), BAD_CAST pcAppName) == 0))
        {
            xmlChar* pcId = xmlGetProp(ptNode, BAD_CAST "id");
            xmlChar* pcIPCKey =xmlGetProp(ptNode, BAD_CAST "ipckey");
            xmlChar* pcSize = xmlGetProp(ptNode, BAD_CAST "size");
            xmlChar* pcSupport7x24 = xmlGetProp(ptNode, BAD_CAST "support7x24");
            xmlChar* pcAddress = xmlGetProp(ptNode, BAD_CAST "address");
            xmlChar* pcDesc = xmlGetProp(ptNode, BAD_CAST "desc");

            if (pcId == NULL)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "application: id property missing");
                return NULL;
            }
            ptShmAppDesc->iId = atoi((char*)pcId);
            if (pcIPCKey == NULL)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "application: ipckey property missing");
                return NULL;
            }
            memset(caBuff, 0, sizeof(caBuff));
            memcpy(caBuff, basExpandMacro((char*)pcIPCKey), sizeof(caBuff) - 1);
            ptShmAppDesc->iIPCKey = (int)basAtolHex(caBuff, sizeof(caBuff) - 1);
            if (ptShmAppDesc->iIPCKey <= 0)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "application: ipckey must > 0");
                return NULL;
            }
            if (pcSize == NULL)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "application: size property missing");
                return NULL;
            }
            ptShmAppDesc->iSize = atoi((char*)pcSize);
            if (ptShmAppDesc->iSize < MIN_SHM_SIZE)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "application: size must > %dM", MIN_SHM_SIZE);
                return NULL;
            }
            ptShmAppDesc->iSize *= 1024*1024;
            if ((pcSupport7x24 != NULL) &&
                (xmlStrcmp(pcSupport7x24, BAD_CAST "true") == 0))
            {
                ptShmAppDesc->bSupport7x24 = BAS_TRUE;
            }
            else
            {
                ptShmAppDesc->bSupport7x24 = BAS_FALSE;
            }
            if (pcAddress != NULL)
            {
                if ((memcmp(pcAddress, "0x", 2) == 0) ||
                    (memcmp(pcAddress, "0X", 2) == 0))
                {
                    sscanf((char*)pcAddress+2, "%lx", &ptShmAppDesc->ulAddress);
                }
                else
                {
                    ptShmAppDesc->ulAddress = atol((char*)pcAddress);
                }
            }
            if (pcDesc != NULL)
            {
                ptShmAppDesc->pcDesc = strdup((char*)pcDesc);
            }

            if (loadShmAppDesc(ptNode->xmlChildrenNode, ptShmAppDesc) != 0)
            {
                BAS_LOG(BAS_LOGINFO, 0, 0, "load application error");
                return NULL;
            }
            return ptShmAppDesc;
        }
        ptNode = ptNode->next;
    }

    return NULL;
}

BAS_EXPORT int basShmInit(const char* pcFileName,
                          const char* pcCharacterSet,
                          const char* pcAppName)
{
    static char caFileName[1024];
    int iShmId;

    sprintf(caFileName, "%s/etc/%s", getenv("APPHOME"), pcFileName);
    gptShmAppDesc = loadShmConfig(caFileName, pcCharacterSet, pcAppName);
    if (gptShmAppDesc == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "load shm application desc error\n");
        return -1;
    }

    if ((iShmId = shmget(gptShmAppDesc->iIPCKey, gptShmAppDesc->iSize,
                        0660|IPC_EXCL|IPC_CREAT)) == -1)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "shmget failure!!!!\n");
        return -1;
    }

    gptShmAppDesc->pvShmBuf = shmat(iShmId, 0, 0);
    if ((long)gptShmAppDesc->pvShmBuf == -1L)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "shmat failure!!!!\n");
        return -1;
    }

    return 0;
}

BAS_EXPORT int basShmAttach(const char* pcFileName,
                            const char* pcCharacterSet,
                            const char* pcAppName)
{
    static char caFileName[1024];
    int iShmId;

    sprintf(caFileName, "%s/etc/%s", getenv("APPHOME"), pcFileName);
    gptShmAppDesc = loadShmConfig(caFileName, pcCharacterSet, pcAppName);
    if (gptShmAppDesc == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "load shm application desc error\n");
        return -1;
    }

    if ((iShmId = shmget(gptShmAppDesc->iIPCKey, gptShmAppDesc->iSize,
                        0660)) == -1)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "shmget failure!!!!\n");
        return -1;
    }

    gptShmAppDesc->pvShmBuf = shmat(iShmId, 0, 0);
    if ((long)gptShmAppDesc->pvShmBuf == -1L)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "shmat failure!!!!\n");
        return -1;
    }

    return 0;
}

BAS_EXPORT int basShmDetach(void)
{
    shmdt(gptShmAppDesc->pvShmBuf);
    gptShmAppDesc->pvShmBuf = NULL;
    return 0;
}

BAS_EXPORT int basShmRm(const char* pcFileName,
                        const char* pcCharacterSet,
                        const char* pcAppName)
{
    static char caFileName[1024];
    int iShmId;

    sprintf(caFileName, "%s/etc/%s", getenv("APPHOME"), pcFileName);
    gptShmAppDesc = loadShmConfig(caFileName, pcCharacterSet, pcAppName);
    if (gptShmAppDesc == NULL)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "load shm application desc error\n");
        return -1;
    }

    if ((iShmId = shmget(gptShmAppDesc->iIPCKey, gptShmAppDesc->iSize,
                        0660|IPC_CREAT)) == -1)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "shmget failure!!!!\n");
        return -1;
    }

    if (shmctl(iShmId, IPC_RMID, 0) < 0)
    {
        BAS_LOG(BAS_LOGINFO, 0, 0, "remove shm failed!!!!!\n");
        return -1;
    }

    return 0;
}
