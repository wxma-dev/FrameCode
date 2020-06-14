#include "bas/bas_global.h"
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xpath.h>

#define XML_VERSION										"1.0"
static  xmlDocPtr	mpXmlDocPtr = NULL;			/* XML数据文件文档实例 */
static	xmlNodePtr	mpXmlRoot = NULL;			/* XML数据文件根节点 */ /* Level 1 */
static	xmlNodePtr	mpXmlNodeLevel2 = NULL;
static	xmlNodePtr	mpXmlNodeLevel3 = NULL;

/*****************************************************************************/
/* FUNC:   int code_convert(char *from_charset, char *to_charset, char *inbuf,int inlen, char *outbuf, int outlen) */
/* PARAMS: from_charset - 源编码方式                                         */
/*         to_charset - 目标编码方式                                         */
/*         inbuf - in buffer                                                 */
/*         outbuf - out buffer                                               */
/* RETURN:  0 - 成功                                                         */
/*         -1 - 失败                                                         */
/* DESC:   代码转换:从一种编码转为另一种编码                                 */
/*****************************************************************************/
BAS_EXPORT int basXmlCodeConvert(char *from_charset, char *to_charset, char *inbuf,int inlen, char *outbuf, int outlen)
{
    iconv_t cd;
    char *errmsg = NULL;
    char **pin = &inbuf;
    char **pout = &outbuf;


    cd = iconv_open(to_charset, from_charset);
    if (cd == (iconv_t)-1)
    {
        errmsg=strerror(errno);
        BAS_LOG(BAS_LOGINFO, 0, 0, "iconv_open error! errmsg=[%s]\n", errmsg);
        return -1;
    }

    memset(outbuf, 0, outlen);
    if (iconv(cd, pin, (size_t *)&inlen, pout, (size_t *)&outlen) == -1)
    {
        errmsg=strerror(errno);
        BAS_LOG(BAS_LOGINFO, 0, 0, "iconv error! errmsg=[%s]\n", errmsg);
        return -1;
    }
    iconv_close(cd);

    return 0;
}

/*****************************************************************************/
/* FUNC:   int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)   */
/* PARAMS: inbuf - in buffer                                                 */
/*         inlen - in buffer len                                             */
/*         outbuf - out buffer                                               */
/*         outlen - outlen                                                   */
/* RETURN:  0 - 成功                                                         */
/*         -1 - 失败                                                         */
/* DESC:   GBK码转为UNICODE码                                                */
/*****************************************************************************/
BAS_EXPORT int basXmlCodeGbk2Unicde(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return basXmlCodeConvert("GBK", "UTF-8", inbuf, inlen, outbuf, outlen);
}

BAS_EXPORT char* basXmlConvertEncoding(const char *caInputString, const char* caFromEncoding, const char* caToEncoding)
{
    static char *caOutputString = NULL;
    size_t inputSize = 0;
    size_t outputSize = 0;
    const char *pcInPtr = NULL;
    char *pcOutPtr = NULL;
    iconv_t cd;
    int iRet = 0;

    if (NULL == caInputString)
    {
        return NULL;
    }

    inputSize = strlen(caInputString) + 1;
    outputSize = inputSize * 2 - 1;
    if (caOutputString != NULL) 
    {
        free(caOutputString);
        caOutputString = NULL;
    }
    caOutputString = (char*)malloc(outputSize);

    pcInPtr = caInputString;
    pcOutPtr = caOutputString;

    if ((cd = iconv_open(caToEncoding, caFromEncoding)) == (iconv_t)-1)
    {
        BAS_LOG(BAS_LOGERROR, 0, 0,
            "basXmlConvertEncoding: iconv_open error[%s]", strerror(errno));
        return NULL;
    }

    iRet = iconv(cd, (char**)&pcInPtr, &inputSize, &pcOutPtr, &outputSize);
    if (iRet != 0)
    {
        BAS_LOG(BAS_LOGERROR, 0, 0,
            "basXmlConvertEncoding: iconv error[%s]", strerror(errno)); 
        return NULL;
    }

    iconv_close(cd);

    return caOutputString;
}

BAS_EXPORT void basFreeXpath(xmlDocPtr pXmlDoc, xmlXPathContextPtr ctxPtr, xmlXPathObjectPtr objPtr)
{
	xmlXPathFreeContext(ctxPtr);

	xmlXPathFreeObject(objPtr);

	xmlFreeDoc(pXmlDoc);
}

BAS_EXPORT int basGetXmlValue(xmlDocPtr	pXmlDoc, xmlNodePtr curNode, const char *pcaName, char *pcaValue, long lValueLen)
{
	xmlChar *pcaTmp;	
	if (xmlStrcmp(curNode->name, (const xmlChar *)pcaName) == 0)
	{
		pcaTmp = xmlNodeListGetString(pXmlDoc, curNode->xmlChildrenNode, 1);
		if (basXmlCodeGbk2Unicde((char *)pcaTmp, strlen((char *)pcaTmp), pcaValue, lValueLen) != 0)
		{
			BAS_LOG(BAS_LOGINFO, 0, 0, "basGetXmlValue: basXmlCodeGbk2Unicde error");
			return -1;
		}
	
		xmlFree(pcaTmp);
	}

	return 0;
}

