#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include "im_vector.h"
#include "im_hashtable.h"

void im_destroy_hashtable_with_info(im_hashtable *hashtable, int info);
void im_destroy_vector_with_info(im_vector *vector, int info);

im_hashtable *im_duplicate_hashtable_with_info(im_hashtable *hashtable, int info);
im_vector *im_duplicate_vector_with_info(im_vector *vector, int info);

static void free_bucket(im_vector *vector, int position)
{
	void *tmp;

	switch(vector->buckets[position].info)
	{
	case IM_EXTERNAL_STRING:
		free(vector->buckets[position].value.external_string.object_alloc);
		break;
	case IM_EXTERNAL_BLOCK:
		free(vector->buckets[position].value.external_block.object_alloc);
		break;
	case IM_EXTERNAL_NESTED:
		im_destroy_hashtable_with_info(
			vector->buckets[position].value.external_nested.object_alloc,
			vector->buckets[position].info);
		break;
	case IM_EXTERNAL_VECTOR:
		im_destroy_vector_with_info(
			vector->buckets[position].value.external_vector.object_alloc,
			vector->buckets[position].info);
		break;
	case IM_ATTACHED_NESTED:
		tmp = (void *)(vector->buckets + vector->number_of_buckets)
			+ vector->buckets[position].value.attached_nested.object_offset;
		im_destroy_hashtable_with_info(tmp, vector->buckets[position].info);
		break;
	case IM_ATTACHED_VECTOR:
		tmp = (void *)(vector->buckets + vector->number_of_buckets)
			+ vector->buckets[position].value.attached_vector.object_offset;
		im_destroy_vector_with_info(tmp, vector->buckets[position].info);
		break;
	default:
		break;
	}
}

im_vector *im_initialize_vector(int capacity)
{
	im_vector *vector;

	vector = (im_vector *)calloc(1, ((im_vector*)0)->buckets + capacity);

	if(!vector) return NULL;

/*
	vector->parent.parent_object = NULL;
	vector->position = -1;
	IM_FLAGS_ZERO(vector);
*/

	vector->number_of_buckets = capacity;
	vector->inuse = 0;
	vector->attached_size = 0;
	vector->attached_offset = 0;

	return vector;
}

void im_destroy_vector(im_vector *vector)
{
	im_destroy_vector_with_info(vector, IM_EXTERNAL_VECTOR);
}

void im_destroy_vector_with_info(im_vector *vector, int info)
{
	unsigned int i;

	for(i = 0; i < vector->inuse; i++)
		free_bucket(vector, i);

	if(info == IM_EXTERNAL_VECTOR)
		free(vector);
}

#if 0
static void set_external_parent(im_vector *vector, int position)
{
	im_vector *tmp;

	if(vector->buckets[position].info == IM_EXTERNAL_NESTED)
		tmp = vector->buckets[position].value.external_nested.object_alloc;
	else if(vector->buckets[position].info == IM_EXTERNAL_VECTOR)
		tmp = vector->buckets[position].value.external_vector.object_alloc;
	else
		return;

	tmp->parent.parent_object = vector;
	tmp->position = position;
	IM_FLAGS_CLR(tmp, IM_ATTACHED_BIT);
}

static void set_attached_parent(im_vector *vector, int position)
{
	im_vector *tmp;
	long vector_size;
	long object_offset;

	if(vector->buckets[position].info == IM_ATTACHED_NESTED)
		object_offset =
			vector->buckets[position].value.attached_nested.object_offset;
	else if(vector->buckets[position].info == IM_ATTACHED_VECTOR)
		object_offset = 
			vector->buckets[position].value.attached_vector.object_offset;
	else
		return;

	vector_size = ((im_vector *)0)->buckets + vector->number_of_buckets;
	tmp = (void *)vector + vector_size + object_offset;

	tmp->parent.parent_offset = - (vector_size + object_offset);
	tmp->position = position;
	IM_FLAGS_SET(tmp, IM_ATTACHED_BIT);
}

static void clear_serialized_flag(im_vector *vector)
{
	im_vector *parent = vector;

	while(parent && IM_FLAGS_ISSET(parent, IM_SERIALIZED_BIT))
	{
		IM_FLAGS_CLR(parent, IM_SERIALIZED_BIT);

		if(IM_FLAGS_ISSET(parent, IM_ATTACHED_BIT))
			parent = (void *)parent + parent->parent.parent_offset;
		else
			parent = parent->parent.parent_object;
	}
}

static void adjust_parent_and_flags(im_vector *vector, int position)
{
	/* Adjust child's parent, clear serialized flag. */
	switch(vector->buckets[position].info)
	{
	case IM_EXTERNAL_NESTED:
	case IM_EXTERNAL_VECTOR:
		set_external_parent(vector, position);
		clear_serialized_flag(vector);
		break;
	case IM_ATTACHED_NESTED:
	case IM_ATTACHED_VECTOR:
		set_attached_parent(vector, position);
		break;
	case IM_EXTERNAL_STRING:
	case IM_EXTERNAL_BLOCK:
		clear_serialized_flag(vector);
		break;
	default:
		break;
	}
}
#endif

int im_add_element(im_vector **vector, im_vector_bucket *bucket)
{
	if(bucket->info == IM_EMPTY || bucket->info == IM_DELETED)
		return -1;
	
	if((*vector)->inuse >= (*vector)->number_of_buckets)
		if(im_resize_vector(vector) < 0)
			return -1;

	(*vector)->buckets[(*vector)->inuse] = *bucket;

	/*adjust_parent_and_flags(*vector, (*vector)->inuse);*/

	return (*vector)->inuse ++;
}

int im_del_element(im_vector *vector, int position)
{
	if(position >= 0 && position < vector->inuse)
	{
		free_bucket(vector, position);
		memmove(vector->buckets + position,
			vector->buckets + position + 1,
			sizeof(vector->buckets[0]) * (vector->inuse - position - 1));

		vector->inuse--;

		return position;
	}
	else
		return -1;
}

int im_set_element(im_vector *vector, int position, im_vector_bucket *bucket)
{
	if(bucket->info == IM_EMPTY || bucket->info == IM_DELETED)
		return -1;

	if(position >=0 && position < vector->inuse)
	{
		free_bucket(vector, position);
		vector->buckets[position] = *bucket;

		/*adjust_parent_and_flags(vector, position);*/

		return position;
	}
	else
		return -1;
}

im_vector_bucket *im_get_element(im_vector *vector, int position)
{
	if(position >=0 && position < vector->inuse)
		return vector->buckets + position;
	else
		return NULL;
}

void im_empty_vector(im_vector *vector)
{
	int i;

	for(i = 0; i < vector->inuse; i++)
		free_bucket(vector, i);

	vector->inuse = 0;
	vector->attached_offset = 0;
}

int im_resize_vector(im_vector **vector)
{
	unsigned int i;
	im_vector *tmp, *new;
	long old_vector_size, new_vector_size;
	void *attached_base;

	attached_base = (*vector)->buckets + (*vector)->number_of_buckets;

	new = im_initialize_vector((*vector)->number_of_buckets << 1);
	if(!new) return -1;

	old_vector_size = ((im_vector*)0)->buckets + (*vector)->number_of_buckets;
	new_vector_size = ((im_vector*)0)->buckets + new->number_of_buckets;

	if((*vector)->attached_offset)
	{
		tmp = realloc(new, new_vector_size + (*vector)->attached_offset);
		if(!tmp) goto error;
		new = tmp;
	}

	new->attached_size = (*vector)->attached_offset;
	memcpy((void *)new + new_vector_size, (void *)(*vector) + old_vector_size,
		(*vector)->attached_offset);
	new->attached_offset = (*vector)->attached_offset;

	for(i = 0; i < (*vector)->number_of_buckets; i++)
		if(im_add_element(&new, (*vector)->buckets + i) < 0)
			goto error;

/*
	new->flags = (*vector)->flags;
	if(IM_FLAGS_ISSET(new, IM_ATTACHED_BIT))
		new->parent.parent_offset = (*vector)->parent.parent_offset;
	else
		new->parent.parent_object = (*vector)->parent.parent_object;
*/

	free(*vector);
	*vector = new;
	return 0;

error:
	free(new);
	return -1;
}

static int serialize_bucket(im_vector *vector, im_vector_bucket *bucket, void *attached_buffer)
{
	long vector_size, length;
	long object_offset;
	void *alloc;

	if(bucket->info == IM_ATTACHED_NESTED)
	{
		vector_size = ((im_vector*)0)->buckets + vector->number_of_buckets;
		object_offset = bucket->value.attached_nested.object_offset;

		alloc = calloc(1, bucket->value_length);
		if(!alloc) return -1;

/*		memcpy(alloc, (void *)vector + vector_size+object_offset,
			bucket->value_length);
*/
		memcpy(alloc,  attached_buffer+object_offset, bucket->value_length);
		bucket->info = IM_EXTERNAL_NESTED;
		bucket->value.external_nested.object_size = bucket->value_length;
		bucket->value.external_nested.object_alloc = alloc;
	}
	else if(bucket->info == IM_ATTACHED_VECTOR)
	{
		vector_size = ((im_vector*)0)->buckets + vector->number_of_buckets;
		object_offset = bucket->value.attached_vector.object_offset;

		alloc = calloc(1, bucket->value_length);
		if(!alloc) return -1;

/*		memcpy(alloc, (void *)vector+vector_size + object_offset,
			bucket->value_length);
*/
		memcpy(alloc, attached_buffer + object_offset, bucket->value_length);
		bucket->info = IM_EXTERNAL_VECTOR;
		bucket->value.external_vector.object_size = bucket->value_length;
		bucket->value.external_vector.object_alloc = alloc;
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

static int attach_bucket(im_vector **vector, int position,
	void *attached_buffer)
{
	long vector_size, attached_size;
	void *tmp, *attached;
	im_vector_bucket *bucket;

	bucket = (*vector)->buckets + position;
	if(bucket->info == IM_EMPTY || bucket->info == IM_DELETED ||
		bucket->info == IM_SHORT || bucket->info == IM_INT ||
		bucket->info == IM_LONG || bucket->info == IM_LONGLONG ||
		bucket->info == IM_USHORT || bucket->info == IM_UINT ||
		bucket->info == IM_ULONG || bucket->info == IM_ULONGLONG ||
		bucket->info == IM_FLOAT || bucket->info == IM_DOUBLE ||
		bucket->info == IM_LONGDOUBLE || bucket->info == IM_STRING ||
		bucket->info == IM_BLOCK)
		return 0;

	if(bucket->info == IM_EXTERNAL_NESTED || bucket->info == IM_EXTERNAL_VECTOR)
		(*vector)->attached_offset +=
			im_align_offset((*vector)->attached_offset);

	vector_size = ((im_vector*)0)->buckets + (*vector)->number_of_buckets;

	attached_size = (*vector)->attached_size;
	while(bucket->value_length > attached_size - (*vector)->attached_offset)
		attached_size += IM_ALLOCSIZE;

	if(attached_size > (*vector)->attached_offset)
	{
		tmp = realloc((*vector), vector_size + attached_size);
		if(!tmp) return -1;

		*vector = tmp;
		(*vector)->attached_size = attached_size;
		bucket = (*vector)->buckets + position;
	}

	attached = (void *)(*vector) + vector_size;
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
			memcpy(attached + (*vector)->attached_offset,
				tmp, bucket->value_length);
			bucket->info = IM_ATTACHED_STRING;
			bucket->value.attached_string.object_offset
				= (*vector)->attached_offset;
			bucket->value.attached_string.object_size = bucket->value_length;
			(*vector)->attached_offset += bucket->value_length;
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
			memcpy(attached + (*vector)->attached_offset,
				tmp, bucket->value_length);
			bucket->info = IM_ATTACHED_BLOCK;
			bucket->value.attached_block.object_offset
				= (*vector)->attached_offset;
			bucket->value.attached_block.object_size = bucket->value_length;
			(*vector)->attached_offset += bucket->value_length;
		}
		free(tmp);
		break;
	case IM_EXTERNAL_NESTED:
		tmp = bucket->value.external_nested.object_alloc;
		memcpy(attached + (*vector)->attached_offset,
			tmp, bucket->value_length);
		bucket->info = IM_ATTACHED_NESTED;
		bucket->value.attached_nested.object_offset
			= (*vector)->attached_offset;
		bucket->value.attached_nested.object_size = bucket->value_length;
		(*vector)->attached_offset += bucket->value_length;
		im_destroy_hashtable(tmp);
		break;
	case IM_EXTERNAL_VECTOR:
		tmp = bucket->value.external_vector.object_alloc;
		memcpy(attached + (*vector)->attached_offset,
			tmp, bucket->value_length);
		bucket->info = IM_ATTACHED_VECTOR;
		bucket->value.attached_vector.object_offset
			= (*vector)->attached_offset;
		bucket->value.attached_vector.object_size = bucket->value_length;
		(*vector)->attached_offset += bucket->value_length;
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
			memcpy(attached + (*vector)->attached_offset,
				tmp, bucket->value_length);
			bucket->info = IM_ATTACHED_STRING;
			bucket->value.attached_string.object_offset
				= (*vector)->attached_offset;
			bucket->value.attached_string.object_size = bucket->value_length;
			(*vector)->attached_offset += bucket->value_length;
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
			memcpy(attached + (*vector)->attached_offset,
				tmp, bucket->value_length);
			bucket->info = IM_ATTACHED_BLOCK;
			bucket->value.attached_block.object_offset
				= (*vector)->attached_offset;
			bucket->value.attached_block.object_size = bucket->value_length;
			(*vector)->attached_offset += bucket->value_length;
		}
		break;
	default:
		break;
	}

	return 0;
}

int im_serialize_vector(im_vector **vector)
{
	int i;
	long vector_size;
	void *attached_buffer;
	void *tmp;

	vector_size = ((im_vector*)0)->buckets + (*vector)->number_of_buckets;

/*
	if(IM_FLAGS_ISSET(*vector, IM_SERIALIZED_BIT))
		return vector_size + (*vector)->attached_size;
*/

	/* Copy attached object */
	attached_buffer = alloca((*vector)->attached_offset);
	memcpy(attached_buffer,
		(void *)(*vector) + vector_size, (*vector)->attached_offset);
	(*vector)->attached_offset = 0;

	for(i = 0; i < (*vector)->inuse; i++)
	{
		/*
		 * If a bucket is nested hash or vector, serialize the bucket.
		 * The serialized bucket info must be IM_EXTERNAL_NESTED or
		 * IM_EXTERNAL_VECTOR.
		 */
		if((*vector)->buckets[i].info == IM_EXTERNAL_NESTED ||
			(*vector)->buckets[i].info == IM_EXTERNAL_VECTOR ||
			(*vector)->buckets[i].info == IM_ATTACHED_NESTED ||
			(*vector)->buckets[i].info == IM_ATTACHED_VECTOR)
			if(serialize_bucket(*vector, (*vector)->buckets + i, attached_buffer) < 0)
				return -1;
		
		/* Attach external object and attached object. */
		if(attach_bucket(vector, i, attached_buffer) < 0)
			return -1;

		/* Set child's parent. */
		/*set_attached_parent(*vector, i);*/
	}

	/* Set serialized flag. */
	/*IM_FLAGS_SET(*vector, IM_SERIALIZED_BIT);*/

	tmp = realloc(*vector, vector_size + (*vector)->attached_offset);
	if(!tmp)
		return -1;
	*vector = tmp;
	(*vector)->attached_size = (*vector)->attached_offset;

	return vector_size + (*vector)->attached_size;
}

static int copy_bucket(im_vector_bucket *bucket)
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

im_vector *im_duplicate_vector_with_info(im_vector *vector, int info)
{
	im_vector *new_vector;
	long vector_size;
	int i = 0;

	if(info == IM_EXTERNAL_VECTOR)
	{
		vector_size = sizeof(im_vector) + sizeof(vector->buckets[0]) *
			(vector->number_of_buckets - 1);

		new_vector = calloc(1, vector_size + vector->attached_size);
		if(!new_vector)
			return NULL;

		memcpy(new_vector, vector, vector_size + vector->attached_size);
	}
	else if(info == IM_ATTACHED_VECTOR)
		new_vector = vector;
	else
		return NULL;

	for(i = 0; i < vector->number_of_buckets; i++)
	{
		if(copy_bucket(vector->buckets + i) < 0)
			goto error;
	}

	return new_vector;
error:
	for(i--; i >=0; i--)
		free_bucket(new_vector, i);

	if(info == IM_EXTERNAL_NESTED)
		free(new_vector);

	return NULL;
}

im_vector *im_duplicate_vector(im_vector *vector)
{
	return im_duplicate_vector_with_info(vector, IM_EXTERNAL_VECTOR);
}

