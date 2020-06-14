#ifndef IM_TYPES
#define IM_TYPES

#include <sys/types.h>

/* bucket info */
enum
{
	IM_EMPTY = 0,        /*  0 */
	IM_DELETED,          /*  1 */

	IM_SHORT,            /*  2 */
	IM_INT,              /*  3 */
	IM_LONG,             /*  4 */
	IM_LONGLONG,         /*  5 */

	IM_USHORT,           /*  6 */
	IM_UINT,             /*  7 */
	IM_ULONG,            /*  8 */
	IM_ULONGLONG,        /*  9 */

	IM_FLOAT,            /* 10 */
	IM_DOUBLE,           /* 11 */
	IM_LONGDOUBLE,       /* 12 */

	IM_STRING,           /* 13 */
	IM_BLOCK,            /* 14 */

	IM_EXTERNAL_STRING,  /* 15 */
	IM_EXTERNAL_BLOCK,   /* 16 */

	IM_EXTERNAL_NESTED,  /* 17 */
	IM_EXTERNAL_VECTOR,  /* 18 */

	IM_ATTACHED_STRING, /* 19 */
	IM_ATTACHED_BLOCK,  /* 20 */

	IM_ATTACHED_NESTED, /* 21 */
	IM_ATTACHED_VECTOR  /* 22 */
};

typedef union im_parent
{
	void            *parent_object;
	long            parent_offset;
} im_parent;

typedef struct im_external_object
{
	long            object_size;
	void            *object_alloc;
} im_external_object;

typedef struct im_attached_object
{
	long            object_size;
	long            object_offset;
} im_attached_object;

/* value union */
typedef union im_value
{
	short           short_value;
	int             int_value;
	long            long_value;
	long long       longlong_value;

	unsigned short  unsigned_short_value;
	unsigned int    unsigned_int_value;
	unsigned long   unsigned_long_value;
	unsigned long long unsigned_longlong_value;

	float           float_value;
	double          double_value;
	long double     longdouble_value;

	char            string_value[0];
	unsigned char   block_value[0];

	im_external_object  external_string;
	im_external_object  external_block;
	im_external_object  external_nested;
	im_external_object  external_vector;

	im_attached_object attached_string;
	im_attached_object attached_block;
	im_attached_object attached_nested;
	im_attached_object attached_vector;
} im_value;


/* alignment macro */
#define IM_ALLOCSIZE  (8192U)
#define IM_ALIGNMENT  (8U)
#define IM_ALIGN_MASK (IM_ALIGNMENT - 1)
#define im_align_offset(A) \
	((IM_ALIGNMENT - ((long)(A) & IM_ALIGN_MASK)) & IM_ALIGN_MASK)


/* flags bit */
#define IM_ATTACHED_BIT     (0)
#define IM_SERIALIZED_BIT   (1)

/* type bit */
#define IM_HASHTABLE_BIT    (24)
#define IM_VECTOR_BIT       (25)

/* im_flags only used for macro IM_FLAGS_XXXX(). DO NOT reference it. */
typedef struct im_flags
{
	im_parent      parent;
	int            position;
	unsigned int   flags;
} im_flags;

/* bit macro */
#define IM_FLAGS_ZERO(o)     (((im_flags *)(o))->flags  = 0U)
#define IM_FLAGS_SET(o, b)   (((im_flags *)(o))->flags |=  (1 << (b)))
#define IM_FLAGS_CLR(o, b)   (((im_flags *)(o))->flags &= ~(1 << (b)))
#define IM_FLAGS_ISSET(o, b) (((im_flags *)(o))->flags &   (1 << (b)))

#endif

