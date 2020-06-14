#ifndef IM_HASHTABLE
#define IM_HASHTABLE

#include <sys/types.h>
#include "im_types.h"

/* key string maximum length */
#define KEY_MAX_LENGTH (38 + 64)

/* hashtable bucket */
typedef struct im_hash_bucket
{
	char            key[KEY_MAX_LENGTH + 1];
	char            info;
	long            value_length;
	im_value        value;
} im_hash_bucket;

/* hashtable */
typedef struct im_hashtable
{
/*
	im_parent       parent;
	int             position;
	unsigned int    flags;
*/
	unsigned int    number_of_buckets;
	unsigned int    inuse;
	unsigned int    empty;
	long            attached_size;
	long            attached_offset;
	im_hash_bucket  buckets[0];
} im_hashtable;

im_hashtable *im_initialize_hashtable(int capacity);
void im_destroy_hashtable(im_hashtable *hashtable);
int im_find_element(im_hashtable *hashtable, char *key);
im_hash_bucket *im_retrieve_element(im_hashtable *hashtable, int position);
int im_delete_element(im_hashtable *hashtable, int position);
im_hash_bucket *im_retrieve_element_by_key(im_hashtable *hashtable, char *key);
int im_delete_element_by_key(im_hashtable *hashtable, char *key);
int im_insert_element(im_hashtable **hashtable,im_hash_bucket *bucket);
void im_empty_hashtable(im_hashtable *hashtable);
int im_rehash_hashtable(im_hashtable **hashtable);
int im_serialize_hashtable(im_hashtable **hashtable);
im_hashtable *im_duplicate_hashtable(im_hashtable *hashtable);

#endif

