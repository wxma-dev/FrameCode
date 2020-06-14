#ifndef __THREAD_DOWORK_H__
#define __THREAD_DOWORK_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <pthread.h>

#include "struct.h"
#include "log.h"

extern CThread_pool *pool;

void *thread_routine_dowork( void *arg );


#endif
