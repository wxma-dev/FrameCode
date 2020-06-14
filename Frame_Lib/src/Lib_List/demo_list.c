

/*
	例子：lib_list.h 的使用demo

*/




#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"../include/lib_list.h"

struct list_item
{
	int id;
	struct list_head list;
};

#if 0
int main()
{
	struct list_head list;
	struct list_head *pos, *p;
	int i;

	//定义成员
	struct list_item mylist;
	//初始化链表
	INIT_LIST_HEAD(&mylist.list);

	for( i = 0; i < 5; i++)
	{
		tmp = ( struct list_item*)malloc(sizeof(struct list_item));
		tmp->id = i;
		list_add(&tmp->list, &mylist.list);
	}

	//遍历
	list_for_each(pos, &mylist.list)
	{
		tmp = list_entry(pos, struct list_item, list);
		printf("id = %d\n",tmp->id);
	}
	
	return 0;
}

#endif