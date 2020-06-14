#ifndef BEAI3IC_H
#define BEAI3IC_H

#include "Beai3Dict.h"

#include "esb.h"

#include "esbid.h"


long BEAIComIMHeader(IMHEADER *imheader,
	char *sender, char *receiver);

long BEAITransIMHeader(IMHEADER *imheader,
	char *sender, char *receiver, int timeout, char ack, char rsp);

int BEAISessionStart();
int BEAISessionEnd();

int BEAIComStart(IMHEADER *imheader, DictHandle *request);
int BEAIComEnd(IMHEADER *imheader, DictHandle *response);

int BEAIComSend(IMHEADER *imheader, DictHandle *response);
int BEAIComReceive(IMHEADER *imheader, DictHandle *request);

int BEAITransCall(IMHEADER *imheader, DictHandle *request,DictHandle *response);
int BEAITransSend(IMHEADER *imheader, DictHandle *request);
int BEAITransReceive(IMHEADER *imheader, DictHandle *response);

int BEAITransStart(IMHEADER *imheader, DictHandle *request);
int BEAITransEnd(IMHEADER *imheader, DictHandle *response);

int BEAIDispatch(IMHEADER *imheader, DictHandle *forward);

#endif

