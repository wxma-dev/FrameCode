#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include "im_hashtable.h"
#include "im_vector.h"
#include "prime_number.h"

void im_destroy_hashtable_with_info(im_hashtable *hashtable, int info);
void im_destroy_vector_with_info(im_vector *vector, int info);

im_hashtable *im_duplicate_hashtable_with_info(im_hashtable *hashtable, int info);
im_vector *im_duplicate_vector_with_info(im_vector *vector, int info);

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

static unsigned int im_hashcode(im_hashtable *hashtable, char *key)
{
	unsigned int i;
	unsigned int hashcode = 0;

	for(i = 0; key[i] && i < sizeof(hashtable->buckets[0].key) - 1; i++)
		hashcode = (hashcode << 5) + key[i];

	return hashcode % hashtable->number_of_buckets;
}

static void free_bucket(im_hashtable *hashtable, int position)
{
	void *tmp;

	switch(hashtable->buckets[position].info)
	{
	case IM_EXTERNAL_STRING:
		free(hashtable->buckets[position].value.external_string.object_alloc);
		break;
	case IM_EXTERNAL_BLOCK:
		free(hashtable->buckets[position].value.external_block.object_alloc);
		break;
	case IM_EXTERNAL_NESTED:
		im_destroy_hashtable_with_info(
			hashtable->buckets[position].value.external_nested.object_alloc,
			hashtable->buckets[position].info);
		break;
	case IM_EXTERNAL_VECTOR:
		im_destroy_vector_with_info(
			hashtable->buckets[position].value.external_vector.object_alloc,
			hashtable->buckets[position].info);
		break;
	case IM_ATTACHED_NESTED:
		tmp = (void *)(hashtable->buckets + hashtable->number_of_buckets)
			+ hashtable->buckets[position].value.attached_nested.object_offset;
		im_destroy_hashtable_with_info(tmp, hashtable->buckets[position].info);
		break;
	case IM_ATTACHED_VECTOR:
		tmp = (void *)(hashtable->buckets + hashtable->number_of_buckets)
			+ hashtable->buckets[position].value.attached_vector.object_offset;
		im_destroy_vector_with_info(tmp, hashtable->buckets[position].info);
		break;
	default:
		break;
	}
}

im_hashtable *im_initialize_hashtable(int capacity)
{
	im_hashtable *hashtable;
	unsigned int number_of_buckets = bsearch_number_of_buckets(capacity);
	unsigned int i;

	hashtable = (im_hashtable *)calloc(1, 
		((im_hashtable*)0)->buckets + number_of_buckets);

	if(!hashtable) return NULL;

/*
	hashtable->parent.parent_object = NULL;
	hashtable->position = -1;
	IM_FLAGS_ZERO(hashtable);
*/

	hashtable->number_of_buckets = number_of_buckets;
	hashtable->inuse = 0;
	hashtable->empty = number_of_buckets;
	hashtable->attached_size = 0;
	hashtable->attached_offset = 0;

	for(i = 0; i < number_of_buckets; i++)
		hashtable->buckets[i].info = IM_EMPTY;

	return hashtable;
}

void im_destroy_hashtable(im_hashtable *hashtable)
{
	im_destroy_hashtable_with_info(hashtable, IM_EXTERNAL_NESTED);
}

void im_destroy_hashtable_with_info(im_hashtable *hashtable, int info)
{
	unsigned int i;

	for(i = 0; i < hashtable->number_of_buckets; i++)
		free_bucket(hashtable, i);

	if(info == IM_EXTERNAL_NESTED)
		free(hashtable);
}

int im_find_element(im_hashtable *hashtable, char *key)
{
	int position;
	int collision = 0;

	position = im_hashcode(hashtable, key);
	while(hashtable->buckets[position].info != IM_EMPTY
		&& strncmp(hashtable->buckets[position].key, key,
			sizeof(hashtable->buckets[position].key) - 1))
	{
		position += (++collision << 1) - 1;
		if(position >= hashtable->number_of_buckets)
			position -= hashtable->number_of_buckets;
		if(position >= hashtable->number_of_buckets)
			position -= hashtable->number_of_buckets;
	}

	return position;
}

im_hash_bucket *im_retrieve_element(im_hashtable *hashtable, int position)
{
	if(hashtable->buckets[position].info == IM_EMPTY
		|| hashtable->buckets[position].info == IM_DELETED)
		return NULL;

	return hashtable->buckets + position;
}

int im_delete_element(im_hashtable *hashtable, int position)
{
	if(hashtable->buckets[position].info == IM_EMPTY
		|| hashtable->buckets[position].info == IM_DELETED)
		return -1;

	free_bucket(hashtable, position);
	hashtable->buckets[position].info = IM_DELETED;
	hashtable->inuse --;

	return position;
}

im_hash_bucket *im_retrieve_element_by_key(im_hashtable *hashtable, char *key)
{
	int position = im_find_element(hashtable, key);

	return im_retrieve_element(hashtable, position);
}

int im_delete_element_by_key(im_hashtable *hashtable, char *key)
{
	int position = im_find_element(hashtable, key);

	return im_delete_element(hashtable, position);
}

void im_empty_hashtable(im_hashtable *hashtable)
{
	unsigned int i;

	for(i = 0; i < hashtable->number_of_buckets; i++)
	{
		free_bucket(hashtable, i);
		hashtable->buckets[i].info = IM_EMPTY;
	}

	hashtable->inuse = 0;
	hashtable->empty = hashtable->number_of_buckets;
	hashtable->attached_offset = 0;
}

#if 0
static void set_external_parent(im_hashtable *hashtable, int position)
{
	im_hashtable *tmp;

	if(hashtable->buckets[position].info == IM_EXTERNAL_NESTED)
		tmp = hashtable->buckets[position].value.external_nested.object_alloc;
	else if(hashtable->buckets[position].info == IM_EXTERNAL_VECTOR)
		tmp = hashtable->buckets[position].value.external_vector.object_alloc;
	else
		return;

	tmp->parent.parent_object = hashtable;
	tmp->position = position;
	IM_FLAGS_CLR(tmp, IM_ATTACHED_BIT);
}

static void set_attached_parent(im_hashtable *hashtable, int position)
{
	im_hashtable *tmp;
	long hash_size;
	long object_offset;

	if(hashtable->buckets[position].info == IM_ATTACHED_NESTED)
		object_offset =
			hashtable->buckets[position].value.attached_nested.object_offset;
	else if(hashtable->buckets[position].info == IM_ATTACHED_VECTOR)
		object_offset =
			hashtable->buckets[position].value.attached_vector.object_offset;
	else
		return;

	hash_size = ((im_hashtable *)0)->buckets + hashtable->number_of_buckets;
	tmp = (void *)hashtable + hash_size + object_offset;

	tmp->parent.parent_offset = - (hash_size + object_offset);
	tmp->position = position;
	IM_FLAGS_SET(tmp, IM_ATTACHED_BIT);
}

static void clear_serialized_flag(im_hashtable *hashtable)
{
	im_hashtable *parent = hashtable;

	while(parent && IM_FLAGS_ISSET(parent, IM_SERIALIZED_BIT))
	{
		IM_FLAGS_CLR(parent, IM_SERIALIZED_BIT);

		if(IM_FLAGS_ISSET(parent, IM_ATTACHED_BIT))
			parent = (void *)parent + parent->parent.parent_offset;
		else
			parent = parent->parent.parent_object;
	}
}

static void adjust_parent_and_flags(im_hashtable *hashtable, int position)
{
	/* Adjust child's parent, clear serialized flag */
	switch(hashtable->buckets[position].info)
	{
	case IM_EXTERNAL_NESTED:
	case IM_EXTERNAL_VECTOR:
		set_external_parent(hashtable, position);
		clear_serialized_flag(hashtable);
		break;
	case IM_ATTACHED_NESTED:
	case IM_ATTACHED_VECTOR:
		set_attached_parent(hashtable, position);
		break;
	case IM_EXTERNAL_STRING:
	case IM_EXTERNAL_BLOCK:
		clear_serialized_flag(hashtable);
		break;
	default:
		break;
	}
}
#endif

int im_insert_element(im_hashtable **hashtable, im_hash_bucket *bucket)
{
	int position;

	if(bucket->info == IM_EMPTY || bucket->info == IM_DELETED)
		return -1;

	if((*hashtable)->empty - 1 <= (*hashtable)->number_of_buckets >> 1)
		if(im_rehash_hashtable(hashtable) < 0)
			return -1;

	position = im_find_element(*hashtable, bucket->key);
	if((*hashtable)->buckets[position].info == IM_EMPTY
		|| (*hashtable)->buckets[position].info == IM_DELETED)
	{
		if((*hashtable)->buckets[position].info == IM_EMPTY)
			(*hashtable)->empty --;
		(*hashtable)->buckets[position] = *bucket;
		(*hashtable)->inuse ++;
	}
	else
		return -1;

	/*adjust_parent_and_flags(*hashtable, position);*/

	return position;
}

int im_rehash_hashtable(im_hashtable **hashtable)
{
	unsigned int i;
	im_hashtable *tmp, *new;
	long old_hash_size, new_hash_size;

	new = im_initialize_hashtable((*hashtable)->inuse << 1);
	if(!new) return -1;

	old_hash_size = ((im_hashtable *)0)->buckets
		+ (*hashtable)->number_of_buckets;
	new_hash_size = ((im_hashtable *)0)->buckets + new->number_of_buckets;

	if((*hashtable)->attached_offset)
	{
		tmp = realloc(new, new_hash_size + (*hashtable)->attached_offset);
		if(!tmp) goto error;
		new = tmp;
	}

	new->attached_size = (*hashtable)->attached_offset;
	memcpy((void *)new + new_hash_size, (void *)(*hashtable) + old_hash_size,
		(*hashtable)->attached_offset);
	new->attached_offset = (*hashtable)->attached_offset;

	for(i = 0; i < (*hashtable)->number_of_buckets; i++)
		if((*hashtable)->buckets[i].info != IM_EMPTY
			&& (*hashtable)->buckets[i].info != IM_DELETED)
			if(im_insert_element(&new, (*hashtable)->buckets + i) < 0)
				goto error;
/*
	new->flags = (*hashtable)->flags;
	if(IM_FLAGS_ISSET(new, IM_ATTACHED_BIT))
		new->parent.parent_offset = (*hashtable)->parent.parent_offset;
	else
		new->parent.parent_object = (*hashtable)->parent.parent_object;
*/

	free(*hashtable);
	*hashtable = new;
	return 0;

error:
	free(new);
	return -1;
}

static int serialize_bucket(im_hashtable *hashtable, im_hash_bucket *bucket,void *attached_buffer)
{
	long hash_size, length;
	long object_offset;
	void *object_alloc;

	if(bucket->info == IM_ATTACHED_NESTED)
	{
		hash_size = ((im_hashtable*)0)->buckets + hashtable->number_of_buckets;
		object_offset = bucket->value.attached_nested.object_offset;

		object_alloc = calloc(1, bucket->value_length);
		if(!object_alloc) return -1;
/*
		memcpy(object_alloc,
			(void *)hashtable + hash_size+object_offset, bucket->value_length);
*/
		memcpy(object_alloc,
			attached_buffer+object_offset, bucket->value_length);
		bucket->info = IM_EXTERNAL_NESTED;
		bucket->value.external_nested.object_size = bucket->value_length;
		bucket->value.external_nested.object_alloc = object_alloc;
	}
	else if(bucket->info == IM_ATTACHED_VECTOR)
	{
		hash_size = ((im_hashtable*)0)->buckets + hashtable->number_of_buckets;
		object_offset = bucket->value.attached_vector.object_offset;

		object_alloc = calloc(1, bucket->value_length);
		if(!object_alloc) return -1;
/*
		memcpy(object_alloc, (void *)hashtable + hash_size + object_offset,
			bucket->value_length);
*/
		memcpy(object_alloc, attached_buffer + object_offset,
			bucket->value_length);
		bucket->info = IM_EXTERNAL_VECTOR;
		bucket->value.external_vector.object_size = bucket->value_length;
		bucket->value.external_vector.object_alloc = object_alloc;
	}

	if(bucket->info == IM_EXTERNAL_NESTED)
	{
		length = im_serialize_hashtable(
			(im_hashtable **) &bucket->value.external_nested.object_alloc);
		if(length < 0)
			return -1;
		else
			bucket->value_length = length;
	}
	else if(bucket->info == IM_EXTERNAL_VECTOR)
	{
		length = im_serialize_vector(
			(im_vector **) &bucket->value.external_vector.object_alloc);
		if(length < 0)
			return -1;
		else
			bucket->value_length = length;
	}

	return 0;
}

static int attach_bucket(im_hashtable **hashtable, int position, void *attached_buffer)
{
	long hash_size, attached_size;
	void *tmp, *attached;
	im_hash_bucket *bucket;

	bucket = (*hashtable)->buckets + position;
	if(bucket->info == IM_EMPTY || bucket->info == IM_DELETED ||
		bucket->info == IM_SHORT || bucket->info == IM_INT ||
		bucket->info == IM_LONG || bucket->info == IM_LONGLONG ||
		bucket->info == IM_USHORT || bucket->info == IM_UINT ||
		bucket->info == IM_ULONG || bucket->info == IM_ULONGLONG ||
		bucket->info == IM_FLOAT || bucket->info == IM_DOUBLE ||
		bucket->info == IM_LONGDOUBLE || bucket->info == IM_STRING ||
		bucket->info == IM_BLOCK)
		return 0;

	if(bucket->info == IM_EXTERNAL_NESTED
		|| bucket->info == IM_EXTERNAL_VECTOR)
		(*hashtable)->attached_offset +=
			im_align_offset((*hashtable)->attached_offset);

	hash_size = ((im_hashtable*)0)->buckets + (*hashtable)->number_of_buckets;

	attached_size = (*hashtable)->attached_size;
	while(bucket->value_length > attached_size - (*hashtable)->attached_offset)
		attached_size += IM_ALLOCSIZE;

	if(attached_size > (*hashtable)->attached_size)
	{
		tmp = realloc((*hashtable), hash_size + attached_size);
		if(!tmp) return -1;

		*hashtable = tmp;
		(*hashtable)->attached_size = attached_size;
		bucket = (*hashtable)->buckets + position;
	}

	attached = (void *)(*hashtable) + hash_size;
	switch(bucket->info)
	{
	case IM_EXTERNAL_STRING:
		tmp = bucket->value.external_string.object_alloc;
		if(bucket->value_length <= sizeof(bucket->value))
		{
			memcpy(bucket->value.string_value, tmp, bucket->value_length);
			bucket->info = IM_STRING;
		}
		else
		{
			memcpy(attached + (*hashtable)->attached_offset,
				tmp, bucket->value_length);
			bucket->info = IM_ATTACHED_STRING;
			bucket->value.attached_string.object_offset
				= (*hashtable)->attached_offset;
			bucket->value.attached_string.object_size = bucket->value_length;
			(*hashtable)->attached_offset += bucket->value_length;
		}
		free(tmp);
		break;
	case IM_EXTERNAL_BLOCK:
		tmp = bucket->value.external_block.object_alloc;
		if(bucket->value_length <= sizeof(bucket->value))
		{
			memcpy(bucket->value.block_value, tmp, bucket->value_length);
			bucket->info = IM_BLOCK;
		}
		else
		{
			memcpy(attached + (*hashtable)->attached_offset,
				tmp, bucket->value_length);
			bucket->info = IM_ATTACHED_BLOCK;
			bucket->value.attached_block.object_offset
				= (*hashtable)->attached_offset;
			bucket->value.attached_block.object_size = bucket->value_length;
			(*hashtable)->attached_offset += bucket->value_length;
		}
		free(tmp);
		break;
	case IM_EXTERNAL_NESTED:
		tmp = bucket->value.external_nested.object_alloc;
		memcpy(attached + (*hashtable)->attached_offset,
			tmp, bucket->value_length);
		bucket->info = IM_ATTACHED_NESTED;
		bucket->value.attached_nested.object_offset
			= (*hashtable)->attached_offset;
		bucket->value.attached_nested.object_size = bucket->value_length;
		(*hashtable)->attached_offset += bucket->value_length;
		im_destroy_hashtable(tmp);
		break;
	case IM_EXTERNAL_VECTOR:
		tmp = bucket->value.external_vector.object_alloc;
		memcpy(attached + (*hashtable)->attached_offset,
			tmp, bucket->value_length);
		bucket->info = IM_ATTACHED_VECTOR;
		bucket->value.attached_vector.object_offset
			= (*hashtable)->attached_offset;
		bucket->value.attached_vector.object_size = bucket->value_length;
		(*hashtable)->attached_offset += bucket->value_length;
		im_destroy_vector(tmp);
		break;
	case IM_ATTACHED_STRING:
		tmp = attached_buffer + bucket->value.attached_string.object_offset;
		if(bucket->value_length <= sizeof(bucket->value))
		{
			memcpy(bucket->value.string_value, tmp, bucket->value_length);
			bucket->info = IM_STRING;
		}
		else
		{
			memcpy(attached + (*hashtable)->attached_offset,
				tmp, bucket->value_length);
			bucket->info = IM_ATTACHED_STRING;
			bucket->value.attached_string.object_offset
				= (*hashtable)->attached_offset;
			bucket->value.attached_string.object_size = bucket->value_length;
			(*hashtable)->attached_offset += bucket->value_length;
		}
		break;
	case IM_ATTACHED_BLOCK:
		tmp = attached_buffer + bucket->value.attached_block.object_offset;
		if(bucket->value_length <= sizeof(bucket->value))
		{
			memcpy(bucket->value.block_value, tmp, bucket->value_length);
			bucket->info = IM_BLOCK;
		}
		else
		{
			memcpy(attached + (*hashtable)->attached_offset,
				tmp, bucket->value_length);
			bucket->info = IM_ATTACHED_BLOCK;
			bucket->value.attached_block.object_offset
				= (*hashtable)->attached_offset;
			bucket->value.attached_block.object_size = bucket->value_length;
			(*hashtable)->attached_offset += bucket->value_length;
		}
		break;
	default:
		break;
	}

	return 0;
}

int im_serialize_hashtable(im_hashtable **hashtable)
{
	int i;
	long hash_size;
	void *attached_buffer;
	void *tmp;

	hash_size = ((im_hashtable*)0)->buckets + (*hashtable)->number_of_buckets;
		
	/*
	if(IM_FLAGS_ISSET(*hashtable, IM_SERIALIZED_BIT))
		return hash_size + (*hashtable)->attached_size;
	*/

	/* Copy attached object. */
	attached_buffer = alloca((*hashtable)->attached_offset);
	memcpy(attached_buffer,
		(void *)(*hashtable) + hash_size, (*hashtable)->attached_offset);
	(*hashtable)->attached_offset = 0;

	for(i = 0; i < (*hashtable)->number_of_buckets; i++)
	{
		/*
		 * If a bucket is nested hash or vector, serialize the bucket.
		 * The serialized bucket info must be IM_EXTERNAL_NESTED or
		 * IM_EXTERNAL_VECTOR.
		 */
		if((*hashtable)->buckets[i].info == IM_EXTERNAL_NESTED ||
			(*hashtable)->buckets[i].info == IM_EXTERNAL_VECTOR ||
			(*hashtable)->buckets[i].info == IM_ATTACHED_NESTED ||
			(*hashtable)->buckets[i].info == IM_ATTACHED_VECTOR)
			if(serialize_bucket(*hashtable, (*hashtable)->buckets + i, attached_buffer) < 0)
				return -1;

		/* Attach external object and attached object. */
		if(attach_bucket(hashtable, i, attached_buffer) < 0)
			return -1;

		/* Set child's parent. */
		/*set_attached_parent(*hashtable, i);*/
	}

	/* Set serialized flag. */
	/*IM_FLAGS_SET(*hashtable, IM_SERIALIZED_BIT);*/

	tmp = realloc(*hashtable, hash_size + (*hashtable)->attached_offset);
	if(!tmp)
		return -1;
	*hashtable = tmp;
	(*hashtable)->attached_size = (*hashtable)->attached_offset;

	return hash_size + (*hashtable)->attached_size;
}

static int copy_bucket(im_hash_bucket *bucket)
{
	void *object_alloc;

	switch(bucket->info)
	{
	case IM_EMPTY:
	case IM_DELETED:
	case IM_SHORT:
	case IM_INT:
	case IM_LONG:
	case IM_LONGLONG:
	case IM_USHORT:
	case IM_UINT:
	case IM_ULONG:
	case IM_ULONGLONG:
	case IM_FLOAT:
	case IM_DOUBLE:
	case IM_LONGDOUBLE:
	case IM_STRING:
	case IM_BLOCK:
	case IM_ATTACHED_STRING:
	case IM_ATTACHED_BLOCK:
		break;
	case IM_EXTERNAL_STRING:
		object_alloc = calloc(1, bucket->value_length);
		if(!object_alloc)
			return -1;
		memcpy(object_alloc, bucket->value.external_string.object_alloc,
			bucket->value.external_string.object_size);
		bucket->value.external_string.object_size = bucket->value_length;
		bucket->value.external_string.object_alloc = object_alloc;
		break;
	case IM_EXTERNAL_BLOCK:
		object_alloc = calloc(1, bucket->value_length);
		if(!object_alloc)
			return -1;
		memcpy(object_alloc, bucket->value.external_block.object_alloc,
			bucket->value.external_block.object_size);
		bucket->value.external_block.object_size = bucket->value_length;
		bucket->value.external_block.object_alloc = object_alloc;
		break;
	case IM_EXTERNAL_NESTED:
		object_alloc = im_duplicate_hashtable_with_info(
			bucket->value.external_nested.object_alloc, IM_EXTERNAL_NESTED);
		if(!object_alloc)
			return -1;
		bucket->value.external_nested.object_size = bucket->value_length;
		bucket->value.external_nested.object_alloc = object_alloc;
		break;
	case IM_EXTERNAL_VECTOR:
		object_alloc = im_duplicate_vector_with_info(
			bucket->value.external_vector.object_alloc, IM_EXTERNAL_VECTOR);
		if(!object_alloc)
			return -1;
		bucket->value.external_vector.object_size = bucket->value_length;
		bucket->value.external_vector.object_alloc = object_alloc;
		break;
	case IM_ATTACHED_NESTED:
		object_alloc = im_duplicate_hashtable_with_info(
			bucket->value.external_nested.object_alloc, IM_ATTACHED_NESTED);
		if(!object_alloc)
			return -1;
		break;
	case IM_ATTACHED_VECTOR:
		object_alloc = im_duplicate_vector_with_info(
			bucket->value.external_vector.object_alloc, IM_ATTACHED_VECTOR);
		if(!object_alloc)
			return -1;
		break;
	default:
		break;
	}

	return 0;
}

im_hashtable *im_duplicate_hashtable_with_info(im_hashtable *hashtable, int info)
{
	im_hashtable *new_hashtable;
	long hash_size;
	int i;

	if(info == IM_EXTERNAL_NESTED)
	{
		hash_size = sizeof(im_hashtable) + sizeof(hashtable->buckets[0]) *
			(hashtable->number_of_buckets - 1);

		new_hashtable = calloc(1, hash_size + hashtable->attached_size);
		if(!new_hashtable)
			return NULL;

		memcpy(new_hashtable, hashtable, hash_size + hashtable->attached_size);
	}
	else if(info == IM_ATTACHED_NESTED)
		new_hashtable = hashtable;
	else
		return NULL;

	for(i = 0; i < hashtable->number_of_buckets; i++)
	{
		if(copy_bucket(hashtable->buckets + i) < 0)
			goto error;
	}

	return new_hashtable;
error:
	for(i--; i >=0; i--)
		free_bucket(new_hashtable, i);

	if(info == IM_EXTERNAL_NESTED)
		free(new_hashtable);

	return NULL;
}

im_hashtable *im_duplicate_hashtable(im_hashtable *hashtable)
{
	return im_duplicate_hashtable_with_info(hashtable, IM_EXTERNAL_NESTED);
}

