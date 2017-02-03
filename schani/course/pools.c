/*
 * pools.c
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

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "pools.h"

bool
pool_init (pool_t *pool)
{
    int i;

    pool->active_chunk = 0;
    pool->fill_ptr = 0;

    for (i = 0; i < POOLS_NUM_CHUNKS; ++i)
	pool->chunks[i] = 0;

    pool->chunks[0] = (long*)malloc(POOLS_GRANULARITY * POOLS_FIRST_CHUNK_SIZE);
    if (pool->chunks[0] == 0)
	return false;

    memset(pool->chunks[0], 0, POOLS_GRANULARITY * POOLS_FIRST_CHUNK_SIZE);

    return true;
}

#ifndef __GNUC__
void
pool_reset (pool_t *pool)
{
    pool->active_chunk = 0;
    pool->fill_ptr = 0;
}
#endif

void
pool_free (pool_t *pool)
{
    int i;

    /* printf("alloced %d pools\n", active_chunk + 1); */
    for (i = 0; i < POOLS_NUM_CHUNKS; ++i)
	if (pool->chunks[i] != 0)
	    free(pool->chunks[i]);
}

#ifdef __GNUC__
void*
_pool_alloc (pool_t *pool, size_t byte_size)
#else
void*
pool_alloc (pool_t *pool, size_t byte_size)
#endif
{
    size_t pool_size, size;
    void *p;

    pool_size = POOLS_FIRST_CHUNK_SIZE << pool->active_chunk;
    size = (byte_size + POOLS_GRANULARITY - 1) / POOLS_GRANULARITY;

    while (pool->fill_ptr + size >= pool_size)
    {
	size_t new_pool_size;

	++pool->active_chunk;
	assert(pool->active_chunk < POOLS_NUM_CHUNKS);

	pool->fill_ptr = 0;

	new_pool_size = POOLS_FIRST_CHUNK_SIZE << pool->active_chunk;
	/* TODO: if the requested block is too big to fit into the
	   pool to be allocated, it should simply be skipped, which
	   would save memory. */
	if (pool->chunks[pool->active_chunk] == 0)
	{
	    size_t new_pool_byte_size = POOLS_GRANULARITY * new_pool_size;

	    /* printf("allocing pool %d with size %ld\n", pool->active_chunk, (long)new_pool_byte_size); */

	    pool->chunks[pool->active_chunk] = (long*)malloc(new_pool_byte_size);
	    if (pool->chunks[pool->active_chunk] == 0)
		return 0;
	    /* FIXME: either remove the memset here or memset the pool
	       even if it's not newly allocated, because pools can be
	       reset. */
	    memset(pool->chunks[pool->active_chunk], 0, new_pool_byte_size);
	}
	pool_size = new_pool_size;
    }

    assert(pool->fill_ptr + size < pool_size);

    p = pool->chunks[pool->active_chunk] + pool->fill_ptr;
    pool->fill_ptr += size;

    return p;
}
