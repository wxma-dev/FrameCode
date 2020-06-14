#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include "prime_number.h"

#include "comext-hashtable.h"

static int bsearch_number_of_buckets(int capacity)
{
	const int number_of_buckets = capacity << 1;
	int start = 0;
	int end = sizeof(prime_number)/sizeof(prime_number[0]) - 1;
	int i;

	while(end - start > 1)
	{
		i = (start + end) >> 1;

		/*
		 * because of number_of_buckets always is even number and prime number
		 * always is odd number except 2, prime_number[i] never equal to
		 * number_of_buckets.
		 */
		if(prime_number[i] < number_of_buckets)
			start = i;
		else if(prime_number[i] > number_of_buckets)
			end = i;
	}

	return prime_number[end];
}

static unsigned int hashcode(COMEXT_HASHTABLE *hashtable, char *refid)
{
	unsigned int hashcode = 0;
	char *p = refid;

	for(; *p; p++)
		hashcode = ((hashcode << 5) | (hashcode >> (sizeof(hashcode)-5))) + *p;

	return hashcode % hashtable->number_of_buckets;
}

COMEXT_HASHTABLE *comext_initialize_hashtable(int capacity)
{
	COMEXT_HASHTABLE *hashtable;
	unsigned int number_of_buckets = bsearch_number_of_buckets(capacity);
	unsigned int i;

	hashtable = (COMEXT_HASHTABLE *)malloc(
		((COMEXT_HASHTABLE*)0)->buckets + number_of_buckets);

	if(!hashtable) return NULL;

	hashtable->number_of_buckets = number_of_buckets;
	hashtable->inuse = 0;
	hashtable->empty = number_of_buckets;

	for(i = 0; i < number_of_buckets; i++)
		hashtable->buckets[i].info = EMPTY;

	return hashtable;
}

void comext_destroy_hashtable(COMEXT_HASHTABLE *hashtable)
{
	int i, j;

	for(i = 0; i < hashtable->number_of_buckets; i++)
	{
		if(hashtable->buckets[i].info == LEGITIMATE)
		{
			for(j = 0; j < sizeof(hashtable->buckets[i].fragments)
				/ sizeof(hashtable->buckets[i].fragments[0]); j++)
			{
				if(hashtable->buckets[i].fragments[j])
				{
					free(hashtable->buckets[i].fragments[j]);
					hashtable->buckets[i].fragments[j] = NULL;
				}
			}
		}
	}

	free(hashtable);
}

int comext_find_element(COMEXT_HASHTABLE *hashtable, char *refid)
{
	int position;
	int collision = 0;

	position = hashcode(hashtable, refid);
	while(hashtable->buckets[position].info != EMPTY 
		&& strncmp(hashtable->buckets[position].refid, refid,
			sizeof(hashtable->buckets[position].refid)))
	{
		position += (++collision << 1) - 1;
		if(position >= hashtable->number_of_buckets)
			position -= hashtable->number_of_buckets;
		if(position >= hashtable->number_of_buckets)
			position -= hashtable->number_of_buckets;
	}

	return position;
}

COMEXT_BUCKET *comext_retrieve_element(COMEXT_HASHTABLE *hashtable, int pos)
{
	return hashtable->buckets[pos].info == LEGITIMATE 
		? hashtable->buckets + pos : NULL;
}

int comext_delete_element(COMEXT_HASHTABLE *hashtable, int pos)
{
	int j;

	if(hashtable->buckets[pos].info == LEGITIMATE)
	{
		for(j = 0; j < sizeof(hashtable->buckets[pos].fragments)
			/ sizeof(hashtable->buckets[pos].fragments[0]); j++)
		{
			if(hashtable->buckets[pos].fragments[j])
			{
				free(hashtable->buckets[pos].fragments[j]);
				hashtable->buckets[pos].fragments[j] = NULL;
			}
		}
		hashtable->buckets[pos].info = DELETED;
		hashtable->inuse --;
		return pos;
	}
	else
		return -1;
}

COMEXT_BUCKET *comext_retrieve_element_by_refid(COMEXT_HASHTABLE *hashtable, char *refid)
{
	return comext_retrieve_element(hashtable,
		comext_find_element(hashtable, refid));
}

int comext_delete_element_by_refid(COMEXT_HASHTABLE *hashtable, char *refid)
{
	return comext_delete_element(hashtable,
		comext_find_element(hashtable, refid));
}

void comext_empty_hashtable(COMEXT_HASHTABLE *hashtable)
{
	unsigned int i, j;

	for(i = 0; i < hashtable->number_of_buckets; i++)
	{
		if(hashtable->buckets[i].info == LEGITIMATE)
		{
			for(j = 0; j < sizeof(hashtable->buckets[i].fragments)
				/ sizeof(hashtable->buckets[i].fragments[0]); j++)
			{
				if(hashtable->buckets[i].fragments[j])
				{
					free(hashtable->buckets[i].fragments[j]);
					hashtable->buckets[i].fragments[j] = NULL;
				}
			}
		}
		hashtable->buckets[i].info = EMPTY;
	}

	hashtable->inuse = 0;
	hashtable->empty = hashtable->number_of_buckets;
}

int comext_insert_element(COMEXT_HASHTABLE **hashtable, COMEXT_BUCKET *bucket)
{
	int pos;

	if(bucket->info != LEGITIMATE)
		return -1;

	if(((*hashtable)->empty - 1) <= (*hashtable)->number_of_buckets >> 1)
		if(comext_rehash_hashtable(hashtable) < 0)
			return -1;

	pos = comext_find_element(*hashtable, bucket->refid);
	if((*hashtable)->buckets[pos].info != LEGITIMATE)
	{
		if((*hashtable)->buckets[pos].info == EMPTY)
			(*hashtable)->empty--;
		(*hashtable)->buckets[pos] = *bucket;
		(*hashtable)->inuse ++;
		return pos;
	}
	else
		return -1;
}

int comext_rehash_hashtable(COMEXT_HASHTABLE **hashtable)
{
	int pos;
	COMEXT_HASHTABLE *new;

	new = comext_initialize_hashtable((*hashtable)->inuse << 1);
	if(!new) return -1;

	for(pos = 0; pos < (*hashtable)->number_of_buckets; pos++)
		if((*hashtable)->buckets[pos].info == LEGITIMATE)
			if(comext_insert_element(&new, (*hashtable)->buckets + pos) < 0)
				goto error;

	free(*hashtable);
	*hashtable = new;
	return 0;

error:
	free(new);
	return -1;
}

