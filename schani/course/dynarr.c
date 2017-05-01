#include <string.h>

#include "dynarr.h"

void
dynarr_init(dynarr_t *arr, pool_t *pool)
{
	arr->pool = pool;
	arr->length = 0;
	arr->capacity = 4;
	arr->data = pool_alloc(pool, sizeof(void*) * arr->capacity);
}

void
dynarr_append(dynarr_t *arr, void *p)
{
	if (arr->length >= arr->capacity) {
		arr->capacity *= 2;
		void **new_data = pool_alloc(arr->pool, sizeof(void*) * arr->capacity);
		memcpy(new_data, arr->data, sizeof(void*) * arr->length);
		arr->data = new_data;
	}

	arr->data[arr->length++] = p;
}

void
dynarr_remove (dynarr_t *arr, size_t i)
{
	assert(i < arr->length);
	memmove(&arr->data[i], &arr->data[i + 1], sizeof(void*) * (arr->length - i - 1));
	arr->length--;
}
