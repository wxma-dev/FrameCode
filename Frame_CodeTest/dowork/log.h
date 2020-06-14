#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <stdlib.h>

extern FILE* g_pLog_Handle;

#define DEBUG_OPEN(fname)\
 do {\
  if (g_pLog_Handle != NULL) {\
   printf("Test suite error @ file '%s' line %d: The lof file can't be opened!\n", __FILE__, __LINE__);\
   exit(-1);\
  }\
  g_pLog_Handle = fopen(fname, "wt");\
 } while(0)
#define DEBUG_CLOSE()\
 do {\
  if (g_pLog_Handle == NULL) {printf("Test suite error: Log file has already been closed!\n"); exit(-1);}\
  fclose(g_pLog_Handle);\
 } while(0)
#define DEBUG(format, ...)\
 do {\
  fprintf((FILE*)g_pLog_Handle, "[DEBUG][%s:%d][%s]    ", __FILE__, __LINE__, __FUNCTION__);\
  fprintf((FILE*)g_pLog_Handle, format, ##__VA_ARGS__);\
  fprintf((FILE*)g_pLog_Handle, "\n");\
  fflush((FILE*)g_pLog_Handle);\
 } while (0);
#define INFO(format, ...)\
 do {\
  fprintf((FILE*)g_pLog_Handle, "[INFO][%s:%d][%s]    ", __FILE__, __LINE__, __FUNCTION__);\
  fprintf((FILE*)g_pLog_Handle, format, ##__VA_ARGS__);\
  fprintf((FILE*)g_pLog_Handle, "\n");\
  fflush((FILE*)g_pLog_Handle);\
 } while (0);
#define ERROR(format, ...)\
 do {\
  fprintf((FILE*)g_pLog_Handle, "[ERROR][%s:%d][%s]    ", __FILE__, __LINE__, __FUNCTION__);\
  fprintf((FILE*)g_pLog_Handle, format, ##__VA_ARGS__);\
  fprintf((FILE*)g_pLog_Handle, "\n");\
  fflush((FILE*)g_pLog_Handle);\
 } while (0)
#define TEST_ASSERT(cond, comments)\
 if ( !(cond) )\
 {\
  DEBUG("Test failed at file '%s' line %d\n", __FILE__, __LINE__);\
  DEBUG("Test content: '"#cond"'\n");\
  if (comments[0] != '\0')\
  {\
   DEBUG(comments);\
   DEBUG("\n");\
  }\
 }
 
#endif // _DEBUG_H_


/*

int main()
{
	DEBUG_OPEN("./1.log");

	DEBUG("Hello World: %d", 1);

	DEBUG_CLOSE();
}

*/
