#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Beai3Log.h"

#include "esbconfig.h"

static HANDLE config;

void esbinit()
{
	config = LoadConfigure("esb.conf");

	if(!config)
	{
		PERROR("can not load configure file.");
		exit(1);
	}
}

void esbfini()
{
	FreeConfigure(config);
	config = NULL;
}

HANDLE GetESBConfig()
{
	return config;
}

