#ifndef COM_EXTENSION_H
#define COM_EXTENSION_H

#include <esb.h>
#include <Beai3Dict.h>

int GetMessageLength(char *message, int length);
int GetReferenceID(DictHandle handle, char *refid, int size);
int InputMessageProcess(char **message, int length);
int OutputMessageProcess(char **message, int length);

#endif

