#ifndef BEAI3LOG_H
#define BEAI3LOG_H

#define DEFAULTLOGLEVEL 4

#define   LOGMSGSIZE  4194304   /*消息队列的大小*/

#define PLT_FLAG 'p'
#define TRC_FLAG 't'
#define COM_FLAG 'c'
#define APP_FLAG 'a'

#define INFOLEVEL  1
#define ERRORLEVEL 2
#define DEBUGLEVEL 4
#define TRCLEVEL   1

extern int g_iTransTID;
extern long g_lSessionID;
extern int g_iLogLevel;
extern char gsSysName[10];
extern char  g_caSysJym[12+1];
extern char  gsPlatVer[20];            /*beai平台版本号   */
extern char  gsSysVer[20];             /*应用系统版本号   */
extern char  gsSysFullName[50];


int InitLogenv(char *sysname);
int PltTrcAppLog(char *file, int line,
	char flag, int level, char *tag, char *fmt, ...);

/*

#ifndef PERROR
#define PERROR(...) \
	PltTrcAppLog(__FILE__, __LINE__, \
	PLT_FLAG, ERRORLEVEL, "[ERROR]", __VA_ARGS__)
#endif

#ifndef PINFO
#define PINFO(...) \
	PltTrcAppLog(__FILE__, __LINE__, \
	PLT_FLAG, INFOLEVEL, "[INFO]", __VA_ARGS__)
#endif

#ifndef PDEBUG
#define PDEBUG(...) \
	PltTrcAppLog(__FILE__, __LINE__, \
	PLT_FLAG, DEBUGLEVEL, "[DEBUG]", __VA_ARGS__)
#endif


#ifndef ERROR
#define ERROR(...) \
	PltTrcAppLog( __FILE__, __LINE__, \
	APP_FLAG, ERRORLEVEL, "[ERROR]", __VA_ARGS__)
#endif

#ifndef INFO
#define INFO(...) \
	PltTrcAppLog( __FILE__, __LINE__, \
	APP_FLAG, INFOLEVEL, "[INFO ]", __VA_ARGS__)
#endif

#ifndef DEBUG
#define DEBUG(... ) \
	PltTrcAppLog( __FILE__, __LINE__, \
	APP_FLAG, DEBUGLEVEL, "[DEBUG]", __VA_ARGS__)
#endif

#ifndef TRCLOG
#define TRCLOG(...) \
	PltTrcAppLog( __FILE__,__LINE__, \
	TRC_FLAG,TRCLEVEL,"" , __VA_ARGS__)
#endif
*/

#endif

