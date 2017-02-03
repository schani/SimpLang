/* -*- c -*- */

/*
 * dynstring.c
 *
 * chpp
 *
 * Copyright (C) 1997-1998 Mark Probst
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

#include "memory.h"

#include "dynstring.h"

#define ASSURE_LENGTH(ds,s)       if ((s) >= (ds)->allocated) \
                                      (ds)->data = reallocString((ds)->pool, (ds)->data, (s) + 1, \
                                                                 &(ds)->allocated)

#define DBG(s)

static char*
allocString (pool_t *pool, int minSize, int *exponent)
{
    *exponent = minSize;

    return pool_alloc(pool, minSize);
}

static char*
reallocString (pool_t *pool, char *string, int minSize, int *exponent)
{
    char *new_string;

    *exponent = minSize * 2;

    new_string = pool_alloc(pool, *exponent);
    memcpy(new_string, string, minSize);

    return new_string;
}

dynstring_t
ds_new (pool_t *pool)
{
    dynstring_t ds;

    ds.pool = pool;
    ds.data = allocString(pool, 1, &ds.allocated);
    ds.data[0] = '\0';
    ds.length = 0;

    return ds;
}

dynstring_t
ds_new_from (pool_t *pool, const char *s)
{
    dynstring_t ds;
    int length = strlen(s);

    ds.pool = pool;
    ds.data = allocString(pool, length + 1, &ds.allocated);
    strcpy(ds.data, s);
    ds.length = length;

    return ds;
}

dynstring_t
ds_new_from_bytes (pool_t *pool, const char *s, int num)
{
    dynstring_t ds;

    ds.pool = pool;
    ds.data = allocString(pool, num + 1, &ds.allocated);
    memcpy(ds.data, s, num);
    ds.data[num] = 0;
    ds.length = num;

    return ds;
}

dynstring_t
ds_empty (void)
{
    static dynstring_t ds;
    static char *empty_string = "";

    if (ds.data == 0)
    {
	ds.pool = NULL;
	ds.data = empty_string;
	ds.allocated = 1;
	ds.length = 0;
    }

    return ds;
}

dynstring_t
ds_copy (dynstring_t *dsOrig)
{
    dynstring_t ds;

    assert(dsOrig->data != 0);

    ds.pool = dsOrig->pool;
    ds.data = allocString(ds.pool, dsOrig->length + 1, &ds.allocated);
    strcpy(ds.data, dsOrig->data);
    ds.length = dsOrig->length;

    return ds;
}

void
ds_append_string (dynstring_t *ds, const char *s, int num)
{
    assert(ds->data != 0);

    ASSURE_LENGTH(ds, ds->length + num);

    strncpy(ds->data + ds->length, s, num);
    ds->length += num;
    ds->data[ds->length] = '\0';
}

void
ds_append_char (dynstring_t *ds, char c)
{
    assert(ds->data != 0);

    ASSURE_LENGTH(ds, ds->length + 1);

    ds->data[ds->length] = c;
    ds->data[++ds->length] = '\0';
}

void
ds_shrink_front (dynstring_t *ds, int num)
{
    assert(ds->data != 0);

    if (num > ds->length)
	num = ds->length;

    memmove(ds->data, ds->data + num, ds->length + 1 - num);
    ds->length -= num;
}

void
ds_shrink_rear (dynstring_t *ds, int num)
{
    assert(ds->data != 0);

    if (num > ds->length)
	num = ds->length;

    ds->length -= num;
    ds->data[ds->length] = '\0';
}

void
ds_split_path (dynstring_t *path, dynstring_t *dir, dynstring_t *file)
{
    char *lastSlash = strrchr(path->data, '/');

    if (lastSlash != 0)
    {
	*dir = ds_new_from_bytes(path->pool, path->data, lastSlash - path->data);
	*file = ds_new_from(path->pool, lastSlash + 1);
    }
    else
    {
	*dir = ds_new_from(path->pool, "");
	*file = ds_copy(path);
    }
}

void
ds_remove_rear_ws (dynstring_t *ds)
{
    int i = ds->length;

    assert(ds->data != 0);

    if( !ds->length ) return;

    while( (i > 0) && isspace((int)(ds->data[i-1])) )
    {
	ds->data[i-1] = 0;
	i--;
	ds->length--;
    }
}

void
ds_remove_front_ws (dynstring_t *ds)
{
    int i = 0;

    while( isspace((int)(ds->data[i])) ) {
	i++;
    }

    if( i ) ds_shrink_front( ds, i );
}

void
ds_remove_outer_ws (dynstring_t *ds)
{
    ds_remove_rear_ws( ds );
    ds_remove_front_ws( ds );
}

dynstring_t
ds_split_ws (dynstring_t *ds)
{
    dynstring_t ds2;
    int i = 0;

    assert(ds->data != 0);

    while( i < (ds->length-1) && ds->data[i] != ' ' && ds->data[i] != '\t' ) i++;
    if( i == ds->length - 1 ) return ds_new_from(ds->pool, "");

    ds2 = ds_new_from(ds->pool, &ds->data[i+1]);
    ds_shrink_rear(ds, ds->length - i);

    return ds2;
}
