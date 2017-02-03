/* -*- c -*- */

/*
 * dynstring.h
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

#ifndef __DYNSTRING_H__
#define __DYNSTRING_H__

#include "pools.h"

typedef struct
{
    pool_t *pool;
    char *data;
    int length;
    int allocated;
} dynstring_t;

dynstring_t ds_new (pool_t *pool);
dynstring_t ds_new_from (pool_t *pool, const char *s);
dynstring_t ds_new_from_bytes (pool_t *pool, const char *s, int num);
dynstring_t ds_empty ();

dynstring_t ds_copy (dynstring_t *ds);

void ds_append_string (dynstring_t *ds, const char *s, int num);
void ds_append_char (dynstring_t *ds, char c);

void ds_shrink_front (dynstring_t *ds, int num);
void ds_shrink_rear (dynstring_t *ds, int num);

void ds_split_path (dynstring_t *path, dynstring_t *dir, dynstring_t *file);

void ds_remove_outer_ws (dynstring_t *ds);
void ds_remove_front_ws (dynstring_t *ds);
void ds_remove_rear_ws (dynstring_t *ds);

dynstring_t ds_split_ws (dynstring_t *ds);

#endif
