#ifndef _TEST_DEBUG_H_
#define _TEST_DEBUG_H_

#include <stdio.h>
#include <stdlib.h>


FILE* g_pLog_Handle;
#define DEBUG_OPEN_FRAME(fname)\
 do {\
  if (g_pLog_Handle != NULL) {\
   printf("Test suite error @ file '%s' line %d: The lof file can't be opened!\n", __FILE__, __LINE__);\
   exit(-1);\
  }\
  g_pLog_Handle = fopen(fname, "a+");\
 } while(0)
#define DEBUG_CLOSE_FRAME()\
 do {\
  if (g_pLog_Handle == NULL) {printf("Test suite error: Log file has already been closed!\n"); exit(-1);}\
  fclose(g_pLog_Handle);\
 } while(0)
#define DEBUG_FRAME(format, ...)\
 do {\
  fprintf((FILE*)g_pLog_Handle, "[DEBUG][%s:%d][%s]    ", __FILE__, __LINE__, __FUNCTION__);\
  fprintf((FILE*)g_pLog_Handle, format, ##__VA_ARGS__);\
  fprintf((FILE*)g_pLog_Handle, "\n" );\
  fflush((FILE*)g_pLog_Handle);\
 } while (0);
#define ERROR_FRAME(format, ...)\
 do {\
  fprintf((FILE*)g_pLog_Handle, "[ERROR][%s:%d][%s]    ", __FILE__, __LINE__, __FUNCTION__);\
  fprintf((FILE*)g_pLog_Handle, format, ##__VA_ARGS__);\
  fprintf((FILE*)g_pLog_Handle, "\n" );\
  fflush((FILE*)g_pLog_Handle);\
 } while (0);
#define INFO_FRAME(format, ...)\
 do {\
  fprintf((FILE*)g_pLog_Handle, "[INFO][%s:%d][%s]    ", __FILE__, __LINE__, __FUNCTION__);\
  fprintf((FILE*)g_pLog_Handle, format, ##__VA_ARGS__);\
  fprintf((FILE*)g_pLog_Handle, "\n" );\
  fflush((FILE*)g_pLog_Handle);\
 } while (0)
#define ASSERT_FRAME(cond, comments)\
 if ( !(cond) )\
 {\
  DEBUG_FRAME("Test failed at file '%s' line %d\n", __FILE__, __LINE__);\
  DEBUG_FRAME("Test content: '"#cond"'\n");\
  if (comments[0] != '\0')\
  {\
   DEBUG_FRAME(comments);\
   DEBUG_FRAME("\n");\
  }\
 }
 
#endif // _TEST_DEBUG_H_


/*

int main()
{
	DEBUG_OPEN_FRAME("./1.log");

	DEBUG_FRAME("Hello World: %d", 1);

	DEBUG_CLOSE_FRAME();
}

*/
