#ifndef __MONITOR_CHILDPROCESS_H__
#define __MONITOR_CHILDPROCESS_H__

#include "Lib/lib_list.h"

/*
	定义子进程的相关的信息
	为每一个子进程分配进结构体
*/
typedef struct St_Childprocess
{
	int    iPocessStatus;                   /* 进程的状态，0-初始化状态(插入list链表之中的状态)，1-开启，2-关闭 */

	char   sProcess_Label[ 256 + 1 ];       /* 进程的标签名 */
	char   sBin_PathName[ 256 + 1 ];        /* bin文件的路径名称 */
	pid_t  iPid;                            /* 子进程id */

	int argc;                               /* 该进程的入参argc */
	char ** argv;                           /* 该进程的入参argv */

	void *  (*func)(void *);                /* 函数指针 */

	struct list_head list;                  /* list链表使用 */
}ST_ChildProcess; 


/*
	本框架使用的结构体
*/
typedef struct struct_pool
{
	char    sFullPathName_F[ 256 + 1 ];     /* 配置文件的全量文件名 */
	ST_ChildProcess * plist_head_F;         /* 指向工作链表的头结点，临界区 */
	int     cur_list_size_F;                /* 当前链表的大小 */
} ST_Frame;


/*
	工作链表的结构
*/
typedef struct clistnode {
	ST_ChildProcess * pList_Node;
	struct worker *next;                    /* 链表使用 */
}CList_Node;


void func_waitpid( int signo );
void sig_func( int signo );
int Init_Frame( ST_Frame * pPoint_StFrame, char * sConfFullPathName );
void IntoNode_List( void * pPoint_Node );
CList_Node *GetNode_List();
int Init_Open_Log();
void Close_Log();

#endif
