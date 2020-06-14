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

#define PROTOCOL_HEADER_LENGTH   (5)
#define BANCS_HEADER_LENGTH      (122)
#define BODY_LENGTH_OFFSET       (4)

#define FLAG2_OFFSET             (78)
#define JYDM_OFFSET              (52)

#define UUID_OFFSET              (90)

#define MSGMAX            (1048576)
/*
#define MSGMAX            (262144)
*/

#define MESSAGE_PAGE_SIZE (16384)

#define SYSNAME           "BANCS" /*同步于异步同时存在时，为了测试异步的系统暂未BANCS_1*/

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
	char lenbuf[5+ 1];
	char wwqdlsh[32+1];
	char refid[38 + 1];
	char jydm[6+1];
	int  number;
	IMHEADER imheader;

	if(!message)
		return -1;

	if(length < 5)
		return 0;

	if(message[0] != '-' && message[0] != 'A')
		return -1;

	memcpy(lenbuf, message + 1, 4);
	lenbuf[4] = '\0';

	len = atoi(lenbuf);

	if(len < length)
	{
		memcpy(jydm, message+PROTOCOL_HEADER_LENGTH+JYDM_OFFSET, 6);
		jydm[6] = '\0';
		memcpy(wwqdlsh, message + PROTOCOL_HEADER_LENGTH + UUID_OFFSET, 32);
		wwqdlsh[32] = '\0';
		if(!iCheckSafJYDM(jydm))
		{
			snprintf(refid, sizeof(refid), "%s%s", jydm,wwqdlsh);
		}else{
			snprintf(refid, sizeof(refid), "%s", wwqdlsh);
		}

		PDEBUG("%s: refid = [%s]", SYSNAME, refid);
		if(RetrieveTransaction(SYSNAME, refid, &number, &imheader) < 0)
		{
			PERROR("%s: RetrieveTransaction() error.", SYSNAME);
		}
		else
			g_lSessionID = imheader.SessionID;
	}

	return len >= 0 && len <= MSGMAX - 5? len + 5: -1;
}

int GetReferenceID(DictHandle handle, char *refid, int size)
{
	char wwqdlsh[32+1];
	char jydm[6+1];
	
	if(GetDictString(handle, "WWXTGZH", wwqdlsh, sizeof(wwqdlsh)) < 0)
	{
		PERROR("%s: can not get \"WWXTGZH\".", SYSNAME);
		return -1;
	}

	if(GetDictString(handle, "JYDM", jydm, sizeof(jydm)) < 0)
	{
		PERROR("%s: can not get \"JYDM\".", SYSNAME);
		return -1;
	}
	if(!iCheckSafJYDM(jydm))
	{
		snprintf(refid, size, "%s%s", jydm,wwqdlsh);
	}else{
		snprintf(refid, size, "%s", wwqdlsh);
	}

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

static int Handle010450(char **message, COMEXT_BUCKET *bucket,int number)
{
	int i;
	int length = 0;
	int message_length, body_length, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;
	char outputcode[2+1];
	

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}

	memcpy(outputcode, bucket->fragments[number]+PROTOCOL_HEADER_LENGTH+BANCS_HEADER_LENGTH,2);
	outputcode[2]=0;
	
	if(!memcmp(outputcode,"01",2))
	{
		memcpy(msglen_str, bucket->fragments[0] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) + 5;

		if(size < message_length)
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
		memcpy(*message, bucket->fragments[0], message_length );
		length = message_length;
		comext_delete_element_by_refid(response, bucket->refid);
		return length;
	}
	
	memcpy(*message, bucket->fragments[number], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2);
	length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2;
	
	nitems_offset = length;
	length += 6;
	
	for(i = 0; i < number + 1 ; i++)
	{
		message_length = 268;

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

		memcpy(*message + length,bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2, 268);
		length += 268;
	}
	
	for(i = 0; i < number + 1 ; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) - BANCS_HEADER_LENGTH - 270;
		
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
		
		memcpy(*message + length, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 270, message_length);
		length += message_length;
		
		if(*((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '2'
			|| *((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '6')
		{
			comext_delete_element_by_refid(response, bucket->refid);

			snprintf(nitems_str, sizeof(nitems_str), "%06d", number + 1);
			memcpy(*message + nitems_offset, nitems_str, 6);
			return length;
		}
	}


	free(*message);
	*message = NULL;
	comext_delete_element_by_refid(response, bucket->refid);
	return -1;
}
static int Handle000450(char **message, COMEXT_BUCKET *bucket,int number)
{
	int i;
	int length = 0;
	int message_length, body_length, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;
	char outputcode[2+1];
	

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}

	memcpy(outputcode, bucket->fragments[number]+PROTOCOL_HEADER_LENGTH+BANCS_HEADER_LENGTH,2);
	outputcode[2]=0;
	
	if(!memcmp(outputcode,"01",2))
	{
		memcpy(msglen_str, bucket->fragments[0] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) + 5;

		if(size < message_length)
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
		memcpy(*message, bucket->fragments[0], message_length );
		length = message_length ;
		comext_delete_element_by_refid(response, bucket->refid);
		return length;
	}
	
	memcpy(*message, bucket->fragments[number], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2);
	length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2;
	
	nitems_offset = length;
	length += 6;
	
	for(i = 0; i < number + 1 ; i++)
	{
		message_length = 45;

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

		memcpy(*message + length,bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2, 45);
		length += 45;
	}
	
	for(i = 0; i < number + 1 ; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) - BANCS_HEADER_LENGTH - 47;
		
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
		
		memcpy(*message + length, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 47, message_length);
		length += message_length;
		
		if(*((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '2'
			|| *((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '6')
		{
			comext_delete_element_by_refid(response, bucket->refid);

			snprintf(nitems_str, sizeof(nitems_str), "%06d", number + 1);
			memcpy(*message + nitems_offset, nitems_str, 6);
			return length;
		}
	}


	free(*message);
	*message = NULL;
	comext_delete_element_by_refid(response, bucket->refid);
	return -1;
}
static int HandleWithOutputcode(char **message, COMEXT_BUCKET *bucket,int number)
{
	int i;
	int length = 0;
	int message_length, body_length, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;
	char *pmsgprint=NULL;
	int  imsgprintLength = 0, fristFlag = 0;
	char warning[1024+1];

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}
	if(memcmp(bucket->fragments[0]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "28", 2) == 0
		|| memcmp(bucket->fragments[0]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "55", 2) == 0)
	{
		fristFlag = 0;
	}else{
		fristFlag = 1;
		memcpy(*message, bucket->fragments[0], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH );
		length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH ;
	}
	
	for(i = 0; i < number + 1 ; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		memcpy(bodylen_str, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH+BODY_LENGTH_OFFSET, 4);
		bodylen_str[4] = '\0';
		message_length = atoi(msglen_str)  - BANCS_HEADER_LENGTH;
		/*打印信息判断 output code 为 28  55包同样处理*/
		if(memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "28", 2) == 0
			|| memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "55", 2) == 0)
		{
			if(!pmsgprint)
			{
				pmsgprint = malloc(MESSAGE_PAGE_SIZE);
				if(!pmsgprint)
				{
					PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
					free(*message);
					*message = NULL;
					comext_delete_element_by_refid(response, bucket->refid);
					return -1;
				}
				memset(pmsgprint, 0x20, MESSAGE_PAGE_SIZE);
				memcpy(pmsgprint,"2800", 4);
				pmsgprint[MESSAGE_PAGE_SIZE-1]=0;
				imsgprintLength = 4;
			}
			if(imsgprintLength < MESSAGE_PAGE_SIZE-1024)
			{
				if(memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "55", 2) == 0)
				{
					strcpy(warning, "55WARNING:");
					memcpy(warning+10,bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH+2,atoi(bodylen_str));
					memcpy(pmsgprint+imsgprintLength,warning, 10+atoi(bodylen_str));
				}else{
					memcpy(pmsgprint+imsgprintLength, bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH+2,atoi(bodylen_str));
				}
				imsgprintLength += 1024;
			}
		}else if(memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "01", 2) == 0
			|| memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "03", 2) == 0
			|| memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "04", 2) == 0
			|| memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "08", 2) == 0){
			if(fristFlag == 0)
			{
				fristFlag = 1;
				memcpy(*message, bucket->fragments[i], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH );
				length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH ;
			}
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
			memcpy(*message + length, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH , message_length);
			length += message_length;
		}
		if(*((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '2'
			|| *((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '6')
		{
			if(pmsgprint)
			{
				sprintf(nitems_str,"%02d",  (imsgprintLength-4)/1024);
				memcpy(pmsgprint+2, nitems_str, 2);
				if(size < length+imsgprintLength)
				{
					size += imsgprintLength;
					if(!(tmp = realloc(*message, size)))
					{
						PERROR("%s: realloc() error: %s", SYSNAME, strerror(errno));
						free(*message);
						free(pmsgprint);
						pmsgprint= NULL;
						*message = NULL;
						comext_delete_element_by_refid(response, bucket->refid);
						return -1;
					}
					*message = tmp;
				}
				memcpy(*message+length, pmsgprint, imsgprintLength);
				length += imsgprintLength;
				free(pmsgprint);
				pmsgprint= NULL;
			}
			comext_delete_element_by_refid(response, bucket->refid);
			return length;
		}
	}


	free(*message);
	*message = NULL;
	comext_delete_element_by_refid(response, bucket->refid);
	return -1;
}
static int Handle020430(char **message, COMEXT_BUCKET *bucket,int number)
{
	int i;
	int length = 0;
	int message_length, body_length, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;
	char outputcode[2+1];
	

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}

	memcpy(outputcode, bucket->fragments[number]+PROTOCOL_HEADER_LENGTH+BANCS_HEADER_LENGTH,2);
	outputcode[2]=0;
	
	if(!memcmp(outputcode,"01",2))
	{
		memcpy(msglen_str, bucket->fragments[0] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) + 5;

		if(size < message_length)
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
		memcpy(*message, bucket->fragments[number], message_length);
		length = message_length ;
		comext_delete_element_by_refid(response, bucket->refid);
		return length;
	}
	
	memcpy(*message, bucket->fragments[number], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2);
	length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2;
	
	memcpy(msglen_str, bucket->fragments[number] + 1, 4);
	msglen_str[4] = '\0';
	message_length = atoi(msglen_str);
	
	memcpy(*message + length , bucket->fragments[number]+message_length + 5 - 80,  80);
	length += 80;
	
	for(i = 0; i < number +1 ; i++)
	{
		if(i == number)
		{
			memcpy(msglen_str, bucket->fragments[i] + 1, 4);
			msglen_str[4] = '\0';
			message_length = atoi(msglen_str)  - BANCS_HEADER_LENGTH - 82;
		}
		else
		{
			memcpy(msglen_str, bucket->fragments[i] + 1, 4);
			msglen_str[4] = '\0';
			message_length = atoi(msglen_str)  - BANCS_HEADER_LENGTH - 2;
		}
		
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
		
		memcpy(*message + length, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2, message_length);
		length += message_length;
		
		if(*((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '2'
			|| *((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '6')
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
static int Handle000600(char **message, COMEXT_BUCKET *bucket,int number)
{
	int i;
	int length = 0;
	int message_length, body_length, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}

	
	if(number == 0)
	{
		memcpy(msglen_str, bucket->fragments[0] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) + 5;

		if(size < message_length)
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
		memcpy(*message, bucket->fragments[0], message_length );
		length = message_length ;
		comext_delete_element_by_refid(response, bucket->refid);
		return length;
	}
	
	memcpy(*message, bucket->fragments[0], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2);
	length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2;
	
	nitems_offset = length;
	length += 6;
	
	for(i = 0; i < number + 1 ; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str)  - BANCS_HEADER_LENGTH - 2;
		
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
		
		memcpy(*message + length, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2, message_length);
		length += message_length;
		
		if(*((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '2'
			|| *((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '6')
		{
			comext_delete_element_by_refid(response, bucket->refid);

			snprintf(nitems_str, sizeof(nitems_str), "%06d", number);
			memcpy(*message + nitems_offset, nitems_str, 6);
			return length;
		}
	}


	free(*message);
	*message = NULL;
	comext_delete_element_by_refid(response, bucket->refid);
	return -1;
}
static int Handle060451(char **message, COMEXT_BUCKET *bucket,int number)
{
	int i;
	int length = 0;
	int message_length, body_length, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}

	
	if(number == 0)
	{
		memcpy(msglen_str, bucket->fragments[0] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) + 5;

		if(size < message_length)
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
		memcpy(*message, bucket->fragments[number], message_length );
		length = message_length ;
		comext_delete_element_by_refid(response, bucket->refid);
		return length;
	}
	
	memcpy(*message, bucket->fragments[number], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH );
	length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH ;
	
	for(i = 0; i < number + 1 ; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str)  - BANCS_HEADER_LENGTH;
		
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
		
		memcpy(*message + length, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH , message_length);
		length += message_length;
		
		if(*((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '2'
			|| *((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '6')
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

static int Handle020450(char **message, COMEXT_BUCKET *bucket,int number)
{
	int i;
	int length = 0;
	int message_length, body_length, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}

	if(number == 0)
	{
		memcpy(msglen_str, bucket->fragments[0] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) + 5;

		if(size < message_length)
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
		memcpy(*message, bucket->fragments[0], message_length );
		length = message_length ;
		comext_delete_element_by_refid(response, bucket->refid);
		return length;
	}
	
	memcpy(*message, bucket->fragments[0], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2);
	length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2;
	length += 19;/*9位记录号+1位下一页标志+9交易日期*/
	
	for(i = 0; i < number + 1 ; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str)  - BANCS_HEADER_LENGTH - 2 -19;
		
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
		
		memcpy(*message + length, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2+19, message_length);
		length += message_length;
		
		if(*((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '2'
			|| *((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '6')
		{
			memcpy(*message +PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2 ,
				bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2, 19);
			comext_delete_element_by_refid(response, bucket->refid);
			return length;
		}
	}

	free(*message);
	*message = NULL;
	comext_delete_element_by_refid(response, bucket->refid);
	return -1;
}

static int Handle007017(char **message, COMEXT_BUCKET *bucket,int number)
{
	int i;
	int length = 0;
	int message_length, body_length, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}

	if(number == 0)
	{
		memcpy(msglen_str, bucket->fragments[0] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) + 5;

		if(size < message_length)
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
		memcpy(*message, bucket->fragments[0], message_length );
		length = message_length ;
		comext_delete_element_by_refid(response, bucket->refid);
		return length;
	}
	
	memcpy(*message, bucket->fragments[0], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2);
	length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2;
	length += 148;
	
	for(i = 0; i < number + 1 ; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str)  - BANCS_HEADER_LENGTH - 2 -148;
		
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
		
		memcpy(*message + length, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2+148, message_length);
		length += message_length;
		
		if(*((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '2'
			|| *((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '6')
		{
			memcpy(*message +PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2 ,
				bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2, 148);
			comext_delete_element_by_refid(response, bucket->refid);
			return length;
		}
	}

	free(*message);
	*message = NULL;
	comext_delete_element_by_refid(response, bucket->refid);
	return -1;
}

static int iHandleNoOutputcode(char **message, COMEXT_BUCKET *bucket,int number)
{
	int i;
	int length = 0;
	int message_length, body_length, nitems_offset;
	int size = MESSAGE_PAGE_SIZE;
	int nitems = 0;
	char msglen_str[32], bodylen_str[32], nitems_str[32], *tmp;
	char *pmsgprint=NULL;
	int  imsgprintLength = 0, fristFlag = 0;
	char warning[1024+1];

	*message = malloc(size);
	if(!*message)
	{
		PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
		*message = NULL;
		comext_delete_element_by_refid(response, bucket->refid);
		return -1;
	}
	
	if(number == 0)
	{
		memcpy(msglen_str, bucket->fragments[0] + 1, 4);
		msglen_str[4] = '\0';
		message_length = atoi(msglen_str) + 5;

		if(size < message_length)
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
		memcpy(*message, bucket->fragments[0], message_length);
		length = message_length;
		comext_delete_element_by_refid(response, bucket->refid);
		return length;
	}
	/*第一个为28包 或55包*/
	if(memcmp(bucket->fragments[0]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "28", 2) == 0
		|| memcmp(bucket->fragments[0]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "55", 2) == 0)
	{
		fristFlag = 0;
	}else{
		fristFlag = 1;
		memcpy(*message, bucket->fragments[0], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2  );
		length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH  +2 ;
	}
	
	for(i = 0; i < number + 1 ; i++)
	{
		memcpy(msglen_str, bucket->fragments[i] + 1, 4);
		msglen_str[4] = '\0';
		memcpy(bodylen_str, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH+BODY_LENGTH_OFFSET, 4);
		bodylen_str[4] = '\0';
		message_length = atoi(msglen_str)  - BANCS_HEADER_LENGTH - 2 ;
		/*打印信息判断 output code 为 28 或 55包*/
		if(memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "28", 2) == 0
			|| memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "55", 2) == 0)
		{
			if(!pmsgprint)
			{
				pmsgprint = malloc(MESSAGE_PAGE_SIZE);
				if(!pmsgprint)
				{
					PERROR("%s: malloc() error: %s", SYSNAME, strerror(errno));
					free(*message);
					*message = NULL;
					comext_delete_element_by_refid(response, bucket->refid);
					return -1;
				}
				memset(pmsgprint, 0x20, MESSAGE_PAGE_SIZE);
				memcpy(pmsgprint,"2800", 4);
				pmsgprint[MESSAGE_PAGE_SIZE-1]=0;
				imsgprintLength = 4;
			}
			if(imsgprintLength < MESSAGE_PAGE_SIZE-1024)
			{
				if(memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "55", 2) == 0)
				{
					strcpy(warning, "55WARNING:");
					memcpy(warning+10, bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH+2,atoi(bodylen_str));
					memcpy(pmsgprint+imsgprintLength, warning, 10+atoi(bodylen_str));
				}else{
					memcpy(pmsgprint+imsgprintLength, bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH+2,atoi(bodylen_str));
				}
				imsgprintLength += 1024;
			}
			
		}else if(memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "01", 2) == 0
		|| memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "03", 2) == 0
		|| memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "08", 2) == 0
		|| memcmp(bucket->fragments[i]+PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH, "04", 2) == 0){
			if(fristFlag == 0)
			{
				memcpy(*message, bucket->fragments[i], PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH + 2  );
				length = PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH  +2 ;
				fristFlag = 1;
			}
			if(size < length + message_length)
			{
				size += MESSAGE_PAGE_SIZE;
    	
				if(!(tmp = realloc(*message, size)))
				{
					PERROR("%s: realloc() error: %s", SYSNAME, strerror(errno));
					free(*message);
					free(pmsgprint);
					pmsgprint= NULL;
					*message = NULL;
					comext_delete_element_by_refid(response, bucket->refid);
					return -1;
				}
				*message = tmp;
			}
		
			memcpy(*message + length, bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + BANCS_HEADER_LENGTH +2, message_length);
			length += message_length;
		}
		
		if(*((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '2'
			|| *((char *)bucket->fragments[i] + PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET) == '6')
		{
			if(pmsgprint)
			{
				sprintf(nitems_str,"%02d",  (imsgprintLength-4)/1024);
				memcpy(pmsgprint+2, nitems_str, 2);
				if(size < length+imsgprintLength)
				{
					size += imsgprintLength;
					if(!(tmp = realloc(*message, size)))
					{
						PERROR("%s: realloc() error: %s", SYSNAME, strerror(errno));
						free(*message);
						free(pmsgprint);
						pmsgprint= NULL;
						*message = NULL;
						comext_delete_element_by_refid(response, bucket->refid);
						return -1;
					}
					*message = tmp;
				}
				memcpy(*message+length, pmsgprint, imsgprintLength);
				length += imsgprintLength;
				free(pmsgprint);
				pmsgprint= NULL;
			}
			comext_delete_element_by_refid(response, bucket->refid);
			return length;
		}
	}

	free(*message);
	free(pmsgprint);
	pmsgprint= NULL;
	*message = NULL;
	comext_delete_element_by_refid(response, bucket->refid);
	return -1;
}


static int HandleInputMessage(char **message, COMEXT_BUCKET *bucket)
{
	char jym[32];
	int i, length;

	for(i = 0; i < sizeof(bucket->fragments)/sizeof(bucket->fragments[0]); i++)
	{
		if(bucket->fragments[i])
		{

			if(((char *)bucket->fragments[i])[PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET] == '2'
				|| ((char *)bucket->fragments[i])[PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET] == '6')
				break;
			else if(((char *)bucket->fragments[i])[PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET] == '0'
				|| ((char *)bucket->fragments[i])[PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET] == '4')
				continue;
			else
			{
				PERROR("%s: Message at PROTOCOL_HEADER_LENGTH + FLAG2_OFFSET not in 0, 2, 4, 6.", SYSNAME);
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
	memcpy(jym, bucket->fragments[0] + PROTOCOL_HEADER_LENGTH  + JYDM_OFFSET, 6);
	PDEBUG("%s: JYM = [%s]", SYSNAME, jym);     
	
	if(!strncmp(jym, "010450", 6))      
    	return Handle010450(message, bucket,i);
    	
    if(!memcmp(jym,"000450",6))
		return Handle000450(message, bucket,i);
    
    if(!memcmp(jym,"013031",6))
		return HandleWithOutputcode(message, bucket,i);
		
	if(!memcmp(jym,"020430",6))
		return Handle020430(message, bucket,i);
		
	if(!memcmp(jym,"007017",6))
		return Handle007017(message, bucket,i);
			
	if(!memcmp(jym,"000600",6))
		return Handle000600(message, bucket,i);
	
	if(!memcmp(jym,"032001",6))
		return HandleWithOutputcode(message,bucket,i);
		
	if(!memcmp(jym,"000409",6))
		return HandleWithOutputcode(message,bucket,i);
		
	if(!memcmp(jym,"060451",6))
		return Handle060451(message, bucket,i);
		
	if(!memcmp(jym,"020450",6))
		return Handle020450(message, bucket,i);
	else 
		return iHandleNoOutputcode(message, bucket,i);
    
    	
}

int InputMessageProcess(char **message, int length)
{
	char wwqdlsh[32+1];
	char refid[38 + 1];
	char sequence_str[32];
	char jydm[6+1];
	int  sequence;
	int  position;
	COMEXT_BUCKET *bucket, new_bucket;

	memcpy(jydm, *message + PROTOCOL_HEADER_LENGTH + JYDM_OFFSET, 6);
	jydm[6] = '\0';
	memcpy(wwqdlsh, *message + PROTOCOL_HEADER_LENGTH + UUID_OFFSET, 32);
	wwqdlsh[32] = '\0';
	if(!iCheckSafJYDM(jydm))
	{
		snprintf(refid, sizeof(refid), "%s%s", jydm,wwqdlsh);
	}else{
		snprintf(refid, sizeof(refid), "%s", wwqdlsh);
	}
	memcpy(sequence_str, *message + 5 + 24, 4);
	sequence_str[4] = '\0';
	sequence = atoi(sequence_str);

	PDEBUG("%s: REFID = [%s], sequence = [%d]", SYSNAME, refid, sequence);

	if(!(bucket = comext_retrieve_element_by_refid(response, refid)))
	{
		memset(&new_bucket, 0, sizeof(new_bucket));

		new_bucket.info = LEGITIMATE;
		strncpy(new_bucket.refid, refid, sizeof(new_bucket.refid) - 1);
		new_bucket.refid[sizeof(new_bucket.refid) - 1] = '\0';
		new_bucket.time = time(NULL);
		new_bucket.fragments[sequence - 1] = *message;

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
	}
	else
		bucket->fragments[sequence - 1] = *message;

	*message = NULL;
	return HandleInputMessage(message, bucket);

}

static int iCheckSafJYDM(char *JYDM)
{
	char safjydm[60][6]={"001110","001145","001150","001160","001445","003145",
	                     "009193","009195","011110","011144","011145","011146",
	                     "011148","011155","011157","011160","013110","013131",
	                     "013145","013146","016110","016144","016145","016146",
	                     "016148","020110","020145","020160","021131","021151",
	                     "021431","026131","026151","041130","041150","084115",
	                     "090101","001130","021153","020154","020130","020150",
	                     "013130","016157","011136","016136","016134","043130",
	                     "022112","021127","011134","058145","021135","024162",
	                     "021150","024112","013111","020134","004160",""     };
	int i;
	for(i=0; i<60; i++)
	{
		if(memcmp(JYDM, safjydm[i], 6) == 0)
			return 0;
	}
	return 1;
}
