/* Mini-cli is Copyright (c) 2000-2013 Geesun Xu

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991, or
   (at your option) version 3 dated 29 June, 2007.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __CLI_VECTOR_H__
#define __CLI_VECTOR_H__
#include "cli_utils.h"

typedef struct cli_vector {
    void** data;
    cli_uint32  size;
    cli_uint32  count;
} cli_vector_t;

void cli_vector_init(cli_vector_t*);
cli_uint32 cli_vector_count(cli_vector_t*);
void cli_vector_add(cli_vector_t*, void*);
void cli_vector_set(cli_vector_t*, cli_uint32, void*);
void *cli_vector_get(cli_vector_t*, cli_uint32);
void cli_vector_delete(cli_vector_t*, cli_uint32);
void cli_vector_free(cli_vector_t*);
void cli_vector_qsort(cli_vector_t *v, cli_uint32 elem_size,cli_int32 (*cmp_func)(const void *, const void *));

#endif
