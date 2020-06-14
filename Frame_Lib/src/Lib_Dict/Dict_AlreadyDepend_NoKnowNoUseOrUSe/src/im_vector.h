#ifndef IM_VECTOR
#define IM_VECTOR

#include <sys/types.h>
#include "im_types.h"

typedef struct im_vector_bucket
{
	char             info;
	long             value_length;
	im_value         value;
} im_vector_bucket;

typedef struct im_vector
{
/*
	im_parent        parent;
	int              position;
	unsigned int     flags;
*/
	unsigned int     number_of_buckets;
	unsigned int     inuse;
	long             attached_size;
	long             attached_offset;
	im_vector_bucket buckets[0];
} im_vector;

im_vector *im_initialize_vector(int capacity);
void im_destroy_vector(im_vector *vector);
int im_add_element(im_vector **vector, im_vector_bucket *bucket);
int im_del_element(im_vector *vector, int position);
int im_set_element(im_vector *vector, int position, im_vector_bucket *bucket);
im_vector_bucket *im_get_element(im_vector *vector, int position);
void im_empty_vector(im_vector *vector);
int im_resize_vector(im_vector **vector);
int im_serialize_vector(im_vector **vector);
im_vector *im_duplicate_vector(im_vector *vector);

#endif

