#ifndef __DYNARR_H__
#define __DYNARR_H__

#include <assert.h>

#include "pools.h"

typedef struct {
	pool_t *pool;
	size_t length;
	size_t capacity;
	void **data;
} dynarr_t;

void dynarr_init (dynarr_t *arr, pool_t *pool);
void dynarr_append (dynarr_t *arr, void *p);
void dynarr_remove (dynarr_t *arr, size_t i);

static inline size_t
dynarr_length (dynarr_t *arr)
{
	return arr->length;
}

static inline void*
dynarr_nth (dynarr_t *arr, size_t i)
{
	assert(i < arr->length);
	return arr->data[i];
}

static inline void
dynarr_set (dynarr_t *arr, size_t i, void *p)
{
	assert(i < arr->length);
	arr->data[i] = p;
}

// NOTE: don't use the dynarr after messing with the data
static inline void**
dynarr_data (dynarr_t *arr)
{
	return arr->data;
}

#endif
