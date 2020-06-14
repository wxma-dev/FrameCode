#ifndef ASYNCREG_H
#define ASYNCREG_H

#include <sys/types.h>

#include "esb.h"

int InitializeAsyncRegister();
int DestroyAsyncRegister();
int RegisterTransaction(char *sysname, char *refid, int number, IMHEADER *imheader);
int RetrieveTransaction(char *sysname, char *refid, int *number, IMHEADER *imheader);
int UnregisterTransaction(char *sysname, char *refid);

#endif

