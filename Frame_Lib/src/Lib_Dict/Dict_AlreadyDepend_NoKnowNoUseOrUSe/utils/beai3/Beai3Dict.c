#include <stdlib.h>
#include <string.h>

#include "../../src/im_hashtable.h"
#include "../../src/im_vector.h"

#include "Beai3Dict.h"

DictHandle InitDict(int capacity)
{
	if(capacity < 0)
		return NULL;

	return im_initialize_hashtable(capacity);
}

void DestroyDict(DictHandle handle)
{
	if(!handle)
		return;

	im_destroy_hashtable(handle);
}

DictHandle InitArray(int capacity)
{
	if(capacity < 0)
		return NULL;

	return im_initialize_vector(capacity);
}

void DestroyArray(DictHandle handle)
{
	if(!handle)
		return;

	im_destroy_vector(handle);
}

void ClearDict(DictHandle handle)
{
	if(!handle)
		return;

	im_empty_hashtable(handle);
}

void ClearArray(DictHandle handle)
{
	if(!handle)
		return;

	im_empty_vector(handle);
}

int GetDictShort(DictHandle handle, DictKey key, short *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atoi(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atoi(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atoi(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictInt(DictHandle handle, DictKey key, int *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atoi(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atoi(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atoi(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictLong(DictHandle handle, DictKey key, long *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atol(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atol(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atol(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictLongLong(DictHandle handle, DictKey key, long long *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atoll(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atoll(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atoll(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictUShort(DictHandle handle, DictKey key, unsigned short *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atoi(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atoi(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atoi(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictUInt(DictHandle handle, DictKey key, unsigned int *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atoi(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atoi(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atoi(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictULong(DictHandle handle, DictKey key, unsigned long *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atol(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atol(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atol(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictULongLong(DictHandle handle, DictKey key, unsigned long long *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atoll(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atoll(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atoll(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictFloat(DictHandle handle, DictKey key, float *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atof(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atof(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atof(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictDouble(DictHandle handle, DictKey key, double *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atof(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atof(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atof(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictLongDouble(DictHandle handle, DictKey key, long double *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		*value = bucket->value.short_value;
		break;
	case IM_INT:
		*value = bucket->value.int_value;
		break;
	case IM_LONG:
		*value = bucket->value.long_value;
		break;
	case IM_LONGLONG:
		*value = bucket->value.longlong_value;
		break;
	case IM_USHORT:
		*value = bucket->value.unsigned_short_value;
		break;
	case IM_UINT:
		*value = bucket->value.unsigned_int_value;
		break;
	case IM_ULONG:
		*value = bucket->value.unsigned_long_value;
		break;
	case IM_ULONGLONG:
		*value = bucket->value.unsigned_longlong_value;
		break;
	case IM_FLOAT:
		*value = bucket->value.float_value;
		break;
	case IM_DOUBLE:
		*value = bucket->value.double_value;
		break;
	case IM_LONGDOUBLE:
		*value = bucket->value.longdouble_value;
		break;
	case IM_STRING:
		*value = atof(bucket->value.string_value);
		break;
	case IM_EXTERNAL_STRING:
		*value = atof(bucket->value.external_string.object_alloc);
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = atof(tmp + bucket->value.attached_string.object_offset);
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictString(DictHandle handle, DictKey key, char *value, int size)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value || size <= 0)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_SHORT:
		snprintf(value, size, "%d", bucket->value.short_value);
		break;
	case IM_INT:
		snprintf(value, size, "%d", bucket->value.int_value);
		break;
	case IM_LONG:
		snprintf(value, size, "%ld", bucket->value.long_value);
		break;
	case IM_LONGLONG:
		snprintf(value, size, "%lld", bucket->value.longlong_value);
		break;
	case IM_USHORT:
		snprintf(value, size, "%u", bucket->value.unsigned_short_value);
		break;
	case IM_UINT:
		snprintf(value, size, "%u", bucket->value.unsigned_int_value);
		break;
	case IM_ULONG:
		snprintf(value, size, "%lu", bucket->value.unsigned_long_value);
		break;
	case IM_ULONGLONG:
		snprintf(value, size, "%llu", bucket->value.unsigned_longlong_value);
		break;
	case IM_FLOAT:
		snprintf(value, size, "%.2f", bucket->value.float_value);
		break;
	case IM_DOUBLE:
		snprintf(value, size, "%.2f", bucket->value.double_value);
		break;
	case IM_LONGDOUBLE:
		snprintf(value, size, "%.2Lf", bucket->value.longdouble_value);
		break;
	case IM_STRING:
		strncpy(value, bucket->value.string_value, size);
		value[size - 1] = '\0';
		break;
	case IM_EXTERNAL_STRING:
		strncpy(value, bucket->value.external_string.object_alloc, size);
		value[size - 1] = '\0';
		break;
	case IM_ATTACHED_STRING:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		strncpy(value, tmp+bucket->value.attached_string.object_offset, size);
		value[size - 1] = '\0';
		break;
	default:
		return -1;
	}

	return strlen(value);
}

int GetDictBlock(DictHandle handle, DictKey key, void *value, int size)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	long copy_size;
	void *tmp;

	if(!handle || !key || !value || size <= 0)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	copy_size = size < bucket->value_length ? size : bucket->value_length;
	switch(bucket->info)
	{
	case IM_BLOCK:
		memcpy(value, bucket->value.block_value, copy_size);
		break;
	case IM_EXTERNAL_BLOCK:
		memcpy(value, bucket->value.external_block.object_alloc, copy_size);
		break;
	case IM_ATTACHED_BLOCK:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		memcpy(value, tmp + bucket->value.attached_block.object_offset,
			copy_size);
		break;
	default:
		return -1;
	}

	return copy_size;
}

int GetDictDict(DictHandle handle, DictKey key, DictHandle *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_EXTERNAL_NESTED:
		*value = bucket->value.external_nested.object_alloc;
		break;
	case IM_ATTACHED_NESTED:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = tmp + bucket->value.attached_nested.object_offset;
		break;
	default:
		return -1;
	}

	return 0;
}

int GetDictArray(DictHandle handle, DictKey key, DictHandle *value)
{
	im_hashtable *hashtable = handle;
	im_hash_bucket *bucket;
	void *tmp;

	if(!handle || !key || !value)
		return -1;

	bucket = im_retrieve_element_by_key(hashtable, key);

	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_EXTERNAL_VECTOR:
		*value = bucket->value.external_vector.object_alloc;
		break;
	case IM_ATTACHED_VECTOR:
		tmp = (void *)hashtable +
			(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets);
		*value = tmp + bucket->value.attached_vector.object_offset;
		break;
	default:
		return -1;
	}

	return 0;
}

int SetDictShort(DictHandle *handle, DictKey key, short value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_SHORT;
	inserting.value_length = sizeof(value);
	inserting.value.short_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictInt(DictHandle *handle, DictKey key, int value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_INT;
	inserting.value_length = sizeof(value);
	inserting.value.int_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictLong(DictHandle *handle, DictKey key, long value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_LONG;
	inserting.value_length = sizeof(value);
	inserting.value.long_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictLongLong(DictHandle *handle, DictKey key, long long value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_LONGLONG;
	inserting.value_length = sizeof(value);
	inserting.value.longlong_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictUShort(DictHandle *handle, DictKey key, unsigned short value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_USHORT;
	inserting.value_length = sizeof(value);
	inserting.value.unsigned_short_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictUInt(DictHandle *handle, DictKey key, unsigned int value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_UINT;
	inserting.value_length = sizeof(value);
	inserting.value.unsigned_int_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictULong(DictHandle *handle, DictKey key, unsigned long value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_ULONG;
	inserting.value_length = sizeof(value);
	inserting.value.unsigned_long_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictULongLong(DictHandle *handle, DictKey key, unsigned long long value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_ULONGLONG;
	inserting.value_length = sizeof(value);
	inserting.value.unsigned_longlong_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictFloat(DictHandle *handle, DictKey key, float value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_FLOAT;
	inserting.value_length = sizeof(value);
	inserting.value.float_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictDouble(DictHandle *handle, DictKey key, double value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_DOUBLE;
	inserting.value_length = sizeof(value);
	inserting.value.double_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictLongDouble(DictHandle *handle, DictKey key, long double value)
{
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_LONGDOUBLE;
	inserting.value_length = sizeof(value);
	inserting.value.longdouble_value = value;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictString(DictHandle *handle, DictKey key, char *value)
{
	im_hash_bucket inserting;
	int existing;
	void *object_alloc;

	if(!handle || !*handle || !key || !value)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.value_length = strlen(value) + 1;

	if(inserting.value_length > 1048576)
		return -1;

	if(inserting.value_length <= sizeof(inserting.value))
	{
		strncpy(inserting.value.string_value, value, inserting.value_length -1);
		inserting.value.string_value[inserting.value_length - 1] = '\0';
		inserting.info = IM_STRING;
	}
	else
	{
		object_alloc = malloc(inserting.value_length);
		strncpy(object_alloc, value, inserting.value_length - 1);
		*((char *)object_alloc + inserting.value_length - 1) = '\0';
		inserting.value.external_string.object_alloc = object_alloc;
		inserting.value.external_string.object_size = inserting.value_length;
		inserting.info = IM_EXTERNAL_STRING;
	}

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictBlock(DictHandle *handle, DictKey key, void *value, int size)
{
	im_hash_bucket inserting;
	int existing;
	void *object_alloc;

	if(!handle || !*handle || !key || !value || size < 0)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.value_length = size;

	if(inserting.value_length > 1048576)
		return -1;

	if(inserting.value_length <= sizeof(inserting.value))
	{
		memcpy(inserting.value.block_value, value, inserting.value_length);
		inserting.info = IM_BLOCK;
	}
	else
	{
		object_alloc = malloc(inserting.value_length);
		memcpy(object_alloc, value, inserting.value_length);
		inserting.value.external_string.object_alloc = object_alloc;
		inserting.value.external_string.object_size = inserting.value_length;
		inserting.info = IM_EXTERNAL_BLOCK;
	}

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictDict(DictHandle *handle, DictKey key, DictHandle value)
{
	im_hashtable *hashtable = value;
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_EXTERNAL_NESTED;
	inserting.value_length = 
		(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets) +
		hashtable->attached_offset;

	if(inserting.value_length > 1048576)
		return -1;

	inserting.value.external_nested.object_alloc = hashtable;
	inserting.value.external_nested.object_size = 
		(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets) +
		hashtable->attached_size;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int SetDictArray(DictHandle *handle, DictKey key, DictHandle value)
{
	im_vector *vector = value;
	im_hash_bucket inserting;
	int existing;

	if(!handle || !*handle || !key)
		return -1;

	existing = im_find_element(*handle, key);

	if(existing >= 0)
		im_delete_element(*handle, existing);

	strncpy(inserting.key, key, sizeof(inserting.key) - 1);
	inserting.key[sizeof(inserting.key) - 1] = '\0';
	inserting.info = IM_EXTERNAL_VECTOR;
	inserting.value_length =
		(long)(((im_vector*)0)->buckets + vector->number_of_buckets) +
		vector->attached_offset;

	if(inserting.value_length > 1048576)
		return -1;

	inserting.value.external_nested.object_alloc = vector;
	inserting.value.external_nested.object_size =
		(long)(((im_vector*)0)->buckets + vector->number_of_buckets) +
		vector->attached_size;

	return im_insert_element((im_hashtable **)handle, &inserting);
}

int DeleteDict(DictHandle handle, DictKey key)
{
	return im_delete_element_by_key((im_hashtable *)handle, key);
}

int GetArrayLength(DictHandle handle)
{
	if(!handle)
		return -1;

	return ((im_vector *)handle)->inuse;
}

int GetArrayDict(DictHandle handle, int index, DictHandle *value)
{
	im_vector *vector = handle;
	im_vector_bucket *bucket;
	void *tmp;

	if(!handle || !value)
		return -1;

	bucket = im_get_element(handle, index);
	if(!bucket)
		return -1;

	switch(bucket->info)
	{
	case IM_EXTERNAL_NESTED:
		*value = bucket->value.external_nested.object_alloc;
		return index;
	case IM_ATTACHED_NESTED:
		tmp = (void *)vector +
			(long)(((im_vector*)0)->buckets + vector->number_of_buckets);
			
		*value = tmp + bucket->value.attached_nested.object_offset;
		return index;
	default:
		return -1;
	}

	return 0;
}

int AddArrayDict(DictHandle *handle, DictHandle value)
{
	im_hashtable *hashtable = value;
	im_vector_bucket adding;

	if(!handle || !*handle)
		return -1;

	adding.info = IM_EXTERNAL_NESTED;
	adding.value_length = 
		(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets) +
		hashtable->attached_offset;

	if(adding.value_length > 1048576)
		return -1;

	adding.value.external_nested.object_alloc = hashtable;
	adding.value.external_nested.object_size =
		(long)(((im_hashtable*)0)->buckets + hashtable->number_of_buckets) +
		hashtable->attached_size;

	return im_add_element((im_vector **)handle, &adding);
}

int SerializeDict(DictHandle *handle)
{
	if(!handle)
		return -1;

	if(!*handle)
		return 0;

	return im_serialize_hashtable((im_hashtable **)handle);
}

int GetDictNElement(DictHandle handle)
{
	if(!handle)
		return -1;

	return ((im_hashtable *)handle)->inuse;
}

#if 0
int CloneDict(DictHandle *dest, DictHandle src, DictKey key)
{
	im_hashtable **dest_hashtable, *src_hashtable;
	im_hash_bucket dest_bucket, *src_bucket;
	void *tmp;

	if(!dest || !*dest || !src || !key)
		return -1;

	dest_hash = dest;
	src_hash = src;

	src_bucket = im_retrieve_element_by_key(src_hashtable, key);

	if(!src_bucket)
		return -1;

	switch(src_bucket->info)
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
		dest_bucket = *src_bucket;
		break;
    case IM_EXTERNAL_STRING:
		object_alloc = malloc(src_bucket->value_length);
		if(!object_alloc)
			return -1;
		memcpy(object_alloc, src_bucket->value.external_string.object_alloc,
			src_bucket->value.external_string.object_size);
		strncpy(dest_bucket.key, key, sizeof(dest_bucket.key) - 1);
		dest_bucket.key[sizeof(dest_bucket.key) - 1] = '\0';
		dest_bucket.info = IM_EXTERNAL_STRING;
		dest_bucket.value_length = src_bucket->value_length;
		dest_bucket.value.external_string.object_size =dest_bucket.value_length;
		dest_bucket.value.external_string.object_alloc = object_alloc;
		break;
	case IM_EXTERNAL_BLOCK:
		object_alloc = malloc(src_bucket->value_length);
		if(!object_alloc)
			return -1;
		memcpy(object_alloc, src_bucket->value.external_block.object_alloc,
			src_bucket->value.external_block.object_size);
		strncpy(dest_bucket.key, key, sizeof(dest_bucket.key) - 1);
		dest_bucket.info = IM_EXTERNAL_BLOCK;
		dest_bucket.value_length = src_bucket->value_length;
		dest_bucket.value.external_block.object_size = dest_bucket.value_length;
		dest_bucket.value.external_block.object_alloc = object_alloc;
		break;
	case IM_ATTACHED_STRING:
		tmp = src_hashtable->buckets + src_hashtable->number_of_buckets;
		break;
	case IM_ATTACHED_BLOCK:
		tmp = src_hashtable->buckets + src_hashtable->number_of_buckets;
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

	return im_insert_element(&to_hashtable, &to_bucket);
}

/*
int CloneDictl(DictHandle from, DictHandle to, ...)
{

	return 0;
}

int CloneDictv(DictHandle from, DictHandle to, int keyc, char *keyv[])
{
	int i, count;
	im_hash_bucket *bucket;
	im_hashtable *srci3Dict.cBeai3Dict.c;
	im_hashtable *dst;

	if(!from || !to)
		return -1;

	count = 0;
	src = from;
	dst = to;

	for(i = 0; i < keyc; i++)
	{
		bucket = im_retrive_element_by_key(src, argv[i]);
	}

	return 0;
}
*/
#endif

int GetDictKeyv(DictHandle handle, int keyc, char *keyv[])
{
	int i, count = 0;
	im_hashtable *hashtable = handle;

	for(i = 0; i < hashtable->number_of_buckets && count < keyc; i++)
		if(hashtable->buckets[i].info != IM_EMPTY
			&& hashtable->buckets[i].info != IM_DELETED)
			keyv[count ++] = hashtable->buckets[i].key;

	return count;
}

