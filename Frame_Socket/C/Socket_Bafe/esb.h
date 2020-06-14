#ifndef ESB_H
#define ESB_H

#include <sys/time.h>

#define ESBID_LEN   31

typedef struct im_header
{
	long   SessionID;
	struct timeval SessionTime;
	int    Length;
	short  Stage;
	short  Timeout;
	int    ErrorNO;
	char   Initiative;
	char   MessageType;
	char   IMType;
	char   IDType;
	long   ApplicationID;
	char   Sender[ESBID_LEN + 1];
	char   Receiver[ESBID_LEN + 1];
} IMHEADER;

#endif

