#ifndef _TEST_DEBUG_H_
#define _TEST_DEBUG_H_

#include <stdio.h>
#include <stdlib.h>

FILE* g_pLog_Handle;
#define TEST_LOG_INIT()\
g_pLog_Handle = NULL;
#define TEST_DEBUG_OPEN(fname)\
 do {\
  if (g_pLog_Handle != NULL) {\
   printf("Test suite error @ file '%s' line %d: The lof file can't be opened!\n", __FILE__, __LINE__);\
   exit(-1);\
  }\
  g_pLog_Handle = fopen(fname, "wt");\
 } while(0)
#define TEST_DEBUG_CLOSE()\
 do {\
  if (g_pLog_Handle == NULL) {printf("Test suite error: Log file has already been closed!\n"); exit(-1);}\
  fclose(g_pLog_Handle);\
 } while(0)
#define TEST_DEBUG(format, ...)\
 do {\
  fprintf((FILE*)g_pLog_Handle, "[DEBUG][%s:%d][%s]    ", __FILE__, __LINE__, __FUNCTION__);\
  fprintf((FILE*)g_pLog_Handle, format, ##__VA_ARGS__);\
  fprintf((FILE*)g_pLog_Handle, "\n" );\
  fflush((FILE*)g_pLog_Handle);\
 } while (0);
#define TEST_ERROR(format, ...)\
 do {\
  fprintf((FILE*)g_pLog_Handle, "[ERROR][%s:%d][%s]    ", __FILE__, __LINE__, __FUNCTION__);\
  fprintf((FILE*)g_pLog_Handle, format, ##__VA_ARGS__);\
  fprintf((FILE*)g_pLog_Handle, "\n" );\
  fflush((FILE*)g_pLog_Handle);\
 } while (0)
#define TEST_ASSERT(cond, comments)\
 if ( !(cond) )\
 {\
  TEST_DEBUG("Test failed at file '%s' line %d\n", __FILE__, __LINE__);\
  TEST_DEBUG("Test content: '"#cond"'\n");\
  if (comments[0] != '\0')\
  {\
   TEST_DEBUG(comments);\
   TEST_DEBUG("\n");\
  }\
 }
 
#endif // _TEST_DEBUG_H_


/*

int main()
{
	TEST_DEBUG_OPEN("./1.log");

	TEST_DEBUG("Hello World: %d", 1);

	TEST_DEBUG_CLOSE();
}

*/
