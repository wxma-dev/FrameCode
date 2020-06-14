#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

#include <Beai3Config.h>
#include <Beai3Log.h>

#include "com-extension.h"
#include "comext-hashtable.h"

#define HEADERLEN (5)

#define PROTOCOL_HEADER_LENGTH   (5)
#define BANCS_HEADER_LENGTH      (122)

#define BRANCHNO_OFFSET          (37)
#define BRANCHNO_LENGTH          (5)

#define FLAG2_OFFSET             (78)

#define UUID_OFFSET              (90)
#define UUID_LENGTH              (32)
#define UUID_LENGTH2             (12)

#define MSGMAX            (262144)
#define MESSAGE_PAGE_SIZE (16384)
#define SYSNAME           "BANCSCARD"

COMEXT_HASHTABLE *response;
time_t last_clean_time;


void InitComExt(void)
{
	if(!(response = comext_initialize_hashtable(500)))
	{
		PERROR("%s: comext_initialize_hashtable() error.", SYSNAME);
		exit(EXIT_FAILURE);
	}

	last_clean_time = time(NULL);
}

void FreeComExt(void)
{
	comext_destroy_hashtable(response);
}

int GetMessageLength(char *message, int length)
{
	int  len;
	char lenbuf[HEADERLEN + 1];
	char fhh[32];
	char wwqdlsh[32];
	char refid[35 + 1];
	int  number;
	IMHEADER imheader;

	if(!message)
		return -1;

	if(length < HEADERLEN)
		return 0;

	memcpy(lenbuf, message + 1, 4);
	lenbuf[4] = '\0';

	len = atoi(lenbuf);
	PDEBUG(" length %d len %d",length,len);
	if(len <= length)
	{
		memcpy(fhh, message + 5 + 37, 5);
		fhh[5] = '\0';
		memcpy(wwqdlsh, message + 5 + 8, 12);
		wwqdlsh[12] = '\0';
		snprintf(refid, sizeof(refid), "%s%s", fhh, wwqdlsh);

		PDEBUG("%s: refid = [%s]", SYSNAME, refid);
		if(RetrieveTransaction(SYSNAME, refid, &number, &imheader) < 0)
		{
			PERROR("%s: RetrieveTransaction() error.", SYSNAME);
		}
		else
			g_lSessionID = imheader.SessionID;
	}
	
	PDEBUG("%d", len >= 0 && len <= MSGMAX - HEADERLEN ? len + HEADERLEN: -1);

	return len >= 0 && len <= MSGMAX - HEADERLEN ? len + HEADERLEN: -1;
}

int GetReferenceID(DictHandle handle, char *refid, int size)
{
	char fhh[32];
	char wwqdlsh[32];

	if(GetDictString(handle, "FHH", fhh, sizeof(fhh)) < 0)
	{
		PERROR("%s: can not get \"FHH\".", SYSNAME);
		return -1;
	}

	if(GetDictString(handle, "WWQDLSH", wwqdlsh, sizeof(wwqdlsh)) < 0)
	{
		PERROR("%s: can not get \"WWQDLSH\".", SYSNAME);
		return -1;
	}

	snprintf(refid, size, "%s%s", fhh, wwqdlsh);

	PINFO("%s: refid = [%s]", SYSNAME, refid);

	return 0;
}

static void CleanTimeout()
{
	int i, j;
	time_t cur_time = time(NULL);

	PDEBUG("%s: Before cleanup, inuse = [%d]", SYSNAME, response->inuse);

	for(i = 0; i < response->number_of_buckets; i++)
	{
		if(response->buckets[i].info == LEGITIMATE)
		{
			if(cur_time - response->buckets[i].time > 20)
			{
				for(j = 0; j < sizeof(response->buckets[i].fragments)
					/ sizeof(response->buckets[i].fragments[0]); j++)
				{
					if(response->buckets[i].fragments[j])
					{
						free(response->buckets[i].fragments[j]);
						response->buckets[i].fragments[j] = NULL;
					}
				}
				response->buckets[i].info = DELETED;
				response->inuse --;
			}
		}
	}

	PDEBUG("%s: After cleanup, inuse = [%d]", SYSNAME, response->inuse);
}


static int Handle039001(char **message, COMEXT_BUCKET *bucket)
{
	int i;
	int message_length, body_length, length = 0, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	int xhcs=0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;

PDEBUG("bucket->fragments[0][%p]",bucket->fragments[0]);
PDEBUG("bucket->fragments[ %d] [%p] bucket->fragments[%d][%s]", 0,bucket->fragments[0],0,bucket->fragments[0]);
	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}
PDEBUG("bucket->fragments[ %d] [%p] bucket->fragments[%d][%s]", 0,bucket->fragments[0],0,bucket->fragments[0]);
	memcpy(*message, bucket->fragments[0], 5 + 122 + 2 + 19);
	length = 5 + 122 + 2 + 19;

	nitems_offset = length;
	length += 6;
	
	memcpy(bodylen_str, bucket->fragments[0] + 5 + 4, 4);
	bodylen_str[4] = '\0';
	body_length = atoi(bodylen_str);

PDEBUG("xhcs %d length %d body_length  %d",xhcs,length ,body_length);
	memcpy(*message + length,bucket->fragments[0] + 5 + 122 + 2 + 19, body_length - 19);
	length += body_length - 19;

	
	PDEBUG("xhcs %d length %d",xhcs,length);

	for(i = 1; i < sizeof(bucket->fragments) / sizeof(bucket->fragments[0])
		&& bucket->fragments[i]; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str);
	PDEBUG("xhcs %d length %d",xhcs,length);
		if(size < length + message_length)
		{
			size += MESSAGE_PAGE_SIZE;

			if(!(tmp = realloc(*message, size)))
			{
				PERROR("%s: realloc() error: %s", SYSNAME, strerror(errno));
				free(*message);
				*message = NULL;
				comext_delete_element_by_refid(response, bucket->refid);
				return -1;
			}
			*message = tmp;
		}

		memcpy(bodylen_str, bucket->fragments[i] + 5 + 4, 4);
		bodylen_str[4] = '\0';
		body_length = atoi(bodylen_str);

		memcpy(*message + length,
			bucket->fragments[i] + 5 + 122 + 2, body_length);
		length += body_length;
			PDEBUG("xhcs %d length %d",xhcs,length);

		if(*((char *)bucket->fragments[i] + 5 + 78) == '2'
			|| *((char *)bucket->fragments[i] + 5 + 78) == '6')
		{
			break;
		}
	}

	comext_delete_element_by_refid(response, bucket->refid);
	
	xhcs = (length - 5 - 122 - 2 - 19 - 6 )/ 27;
	PDEBUG("xhcs %d length %d",xhcs,length);
	snprintf(nitems_str, sizeof(nitems_str), "%06d", xhcs);
	memcpy(*message + nitems_offset, nitems_str, 6);
	return length;
}
static int HandleDefault(char **message, COMEXT_BUCKET *bucket)
{
	int i;
	int message_length, body_length, length = 0;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], *tmp;

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}

/*
	memcpy(*message, bucket->fragments[0], 5 + 131 + 2);
	length = 5 + 131 + 2;
*/
	memcpy(*message, bucket->fragments[0], 5 + 122 + 2);
	length = 5 + 122 + 2;

	for(i = 0; i < sizeof(bucket->fragments) / sizeof(bucket->fragments[0])
		&& bucket->fragments[i]; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str);

		if(size < length + message_length)
		{
			size += MESSAGE_PAGE_SIZE;

			if(!(tmp = realloc(*message, size)))
			{
				PERROR("%s: realloc() error: %s", SYSNAME, strerror(errno));
				free(*message);
				*message = NULL;
				comext_delete_element_by_refid(response, bucket->refid);
				return -1;
			}
			*message = tmp;
		}

		memcpy(bodylen_str, bucket->fragments[i] + 5 + 4, 4);
		bodylen_str[4] = '\0';
		body_length = atoi(bodylen_str);

		memcpy(*message + length,
/*
			bucket->fragments[i] + 5 + 131 + 2, body_length);
*/
			bucket->fragments[i] + 5 + 122 + 2, body_length);
		length += body_length;

/*
		if(*((char *)bucket->fragments[i] + 5 + 70) == '2'
			|| *((char *)bucket->fragments[i] + 5 + 70) == '6')
*/
		if(*((char *)bucket->fragments[i] + 5 + 78) == '2'
			|| *((char *)bucket->fragments[i] + 5 + 78) == '6')
		{
			comext_delete_element_by_refid(response, bucket->refid);
			return length;
		}
	}

	free(*message);
	*message = NULL;
	comext_delete_element_by_refid(response, bucket->refid);
	return -1;
}

static int HandleInputMessage(char **message, COMEXT_BUCKET *bucket)
{
	char jym[32];
	int i, length;
PDEBUG("bucket->fragments[ %d] [%p] bucket->fragments[%d][%s]", 0,bucket->fragments[0],0,bucket->fragments[0]);
	for(i = 0; i < sizeof(bucket->fragments)/sizeof(bucket->fragments[0]); i++)
	{
		if(bucket->fragments[i])
		{

			if(((char *)bucket->fragments[i])[5 + 78] == '2'
				|| ((char *)bucket->fragments[i])[5 + 78] == '6')
				break;
			else if(((char *)bucket->fragments[i])[5 + 78] == '0'
				|| ((char *)bucket->fragments[i])[5 + 78] == '4')
				continue;
			else
			{

				PERROR("%s: Message at 5 + 78 not in 0, 2, 4, 6.", SYSNAME);
				comext_delete_element_by_refid(response, bucket->refid);
				return -1;
			}
		}
		else
			return 0;
	}

	if(i == sizeof(bucket->fragments)/sizeof(bucket->fragments[0]))
	{
		PERROR("%s: Number of message over maxinum.", SYSNAME);
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}

	memset(jym, 0, sizeof(jym));
/*
	memcpy(jym, bucket->fragments[0] + 5 + 49, 6);
*/
	memcpy(jym, bucket->fragments[0] + 5 + 52, 6);
	PDEBUG("%s: JYM = [%s]", SYSNAME, jym);


PDEBUG("bucket->fragments[0][%p]",bucket->fragments[0]);
PDEBUG("bucket->fragments[ %d] [%p] bucket->fragments[%d][%s]", 0,bucket->fragments[0],0,bucket->fragments[0]);
	if(!strncmp(jym, "039001", 6))
		return Handle039001(message, bucket);
	else
		return HandleDefault(message, bucket);
}

int InputMessageProcess(char **message, int length)
{
	char fhh[32];
	char wwqdlsh[32];
	char refid[35 + 1];
	char sequence_str[32];
	int  sequence;
	int  position;
	COMEXT_BUCKET *bucket, new_bucket;

	memcpy(fhh, *message + 5 + 37, 5);
	fhh[5] = '\0';
	memcpy(wwqdlsh, *message + 5 + 8, 12);
	wwqdlsh[12] = '\0';
	snprintf(refid, sizeof(refid), "%s%s", fhh, wwqdlsh);
	memcpy(sequence_str, *message + 5 + 24, 4);
	sequence_str[4] = '\0';
	sequence = atoi(sequence_str);

	PDEBUG("%s: REFID = [%s], sequence = [%d]", SYSNAME, refid, sequence);

/*
	if(sequence == 1 && ((*message)[5+70] == '2' || (*message)[5+70] == '6')
		&& strncmp(*message + 5 + 49, "084022", 6))
		return length;
	else
	{
*/
		if(!(bucket = comext_retrieve_element_by_refid(response, refid)))
		{
			memset(&new_bucket, 0, sizeof(new_bucket));

			new_bucket.info = LEGITIMATE;
			strncpy(new_bucket.refid, refid, sizeof(new_bucket.refid) - 1);
			new_bucket.refid[sizeof(new_bucket.refid) - 1] = '\0';
			new_bucket.time = time(NULL);
			new_bucket.fragments[sequence - 1] = *message;
			PDEBUG("new_bucket.fragments[ %d] [%p] new_bucket.fragments[%d][%s]",sequence - 1,new_bucket.fragments[sequence - 1],sequence - 1,new_bucket.fragments[sequence - 1]);
			if(new_bucket.time - last_clean_time > 20)
			{
				CleanTimeout();
				last_clean_time = new_bucket.time;
			}

			if(comext_insert_element(&response, &new_bucket) < 0)
				return -1;

			PDEBUG("%s: inuse = [%d], number_of_buckets = [%d]",
				SYSNAME, response->inuse, response->number_of_buckets);

			bucket = &new_bucket;
			PDEBUG("bucket->fragments[ %d] [%p] bucket->fragments[%d][%s]",sequence - 1,bucket->fragments[sequence - 1],sequence - 1,bucket->fragments[sequence - 1]);
		}
		else
			bucket->fragments[sequence - 1] = *message;

		*message = NULL;
		return HandleInputMessage(message, bucket);

}



