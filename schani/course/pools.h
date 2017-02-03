/*
 * pools.h
 *
 * lispreader
 *
 * Copyright (C) 2002-2007 Mark Probst
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __POOLS_H__
#define __POOLS_H__

#include <stdlib.h>
#include <stdbool.h>

/* these settings allow a pool to grow to up to 16 GB (last chunk 8GB) */
#define POOLS_GRANULARITY                sizeof(long)
#define POOLS_FIRST_CHUNK_SIZE           ((size_t)2048)
#define POOLS_NUM_CHUNKS                 20

typedef struct
{
    int active_chunk;
    size_t fill_ptr;
    long *chunks[POOLS_NUM_CHUNKS];
} pool_t;

bool pool_init (pool_t *pool);
void pool_free (pool_t *pool);

#ifdef __GNUC__
void* _pool_alloc (pool_t *pool, size_t size);

static inline void*
pool_alloc (pool_t *pool, size_t size)
{
    void *p;
    size_t padded_size = (size + POOLS_GRANULARITY - 1) / POOLS_GRANULARITY;

    if (pool->fill_ptr + padded_size >= (POOLS_FIRST_CHUNK_SIZE << pool->active_chunk))
	return _pool_alloc(pool, size);

    p = pool->chunks[pool->active_chunk] + pool->fill_ptr;
    pool->fill_ptr += padded_size;

    return p;
}
#else
void* pool_alloc (pool_t *pool, size_t size);
#endif

#ifdef __GNUC__
static inline void
pool_reset (pool_t *pool)
{
    pool->active_chunk = 0;
    pool->fill_ptr = 0;
}
#else
void reset_pool (pool_t *pool);
#endif

#endif
