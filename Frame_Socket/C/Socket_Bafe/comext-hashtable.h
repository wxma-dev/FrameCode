#ifndef COMEXT_HASHTABLE_H
#define COMEXT_HASHTABLE_H

enum
{
	LEGITIMATE,
	EMPTY,
	DELETED
};

typedef struct bucket
{
	int      info;
	char     refid[38 + 1];
	time_t   time;
	/*
	void     *fragments[16];
	*/
	void     *fragments[64];
} COMEXT_BUCKET;

typedef struct hashtable
{
	unsigned int      number_of_buckets;
	unsigned int      inuse;
	unsigned int      empty;
	COMEXT_BUCKET     buckets[0];
} COMEXT_HASHTABLE;

COMEXT_HASHTABLE *comext_initialize_hashtable(int capacity);
void comext_destroy_hashtable(COMEXT_HASHTABLE *hashtable);
int comext_find_element(COMEXT_HASHTABLE *hashtable, char *refid);
COMEXT_BUCKET *comext_retrieve_element(COMEXT_HASHTABLE *hashtable, int pos);
int comext_delete_element(COMEXT_HASHTABLE *hashtable, int pos);
COMEXT_BUCKET *comext_retrieve_element_by_refid(COMEXT_HASHTABLE *hashtable, char *refid);
int comext_delete_element_by_refid(COMEXT_HASHTABLE *hashtable, char *refid);
void comext_empty_hashtable(COMEXT_HASHTABLE *hashtable);
int comext_insert_element(COMEXT_HASHTABLE **hashtable, COMEXT_BUCKET *bucket);
int comext_rehash_hashtable(COMEXT_HASHTABLE **hashtable);

#endif

