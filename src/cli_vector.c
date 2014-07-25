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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli_api.h"
#include "cli_vector.h"

/*lint -e578*/
void cli_vector_init(cli_vector_t *v)
{
    v->data = NULL;
    v->size = 0;
    v->count = 0;
}

cli_uint32 cli_vector_count(cli_vector_t *v)
{
    return v->count;
}

void cli_vector_add(cli_vector_t *v, void *e)
{
    if (v->size == 0) {
        v->size = 1; 
        /*
           since most of cli cmd has one element
           to reduce the memory used, we alloc one element at the first time
           */
        v->data = CLI_MALLOC(sizeof(void*) * v->size);
        memset(v->data, '\0', sizeof(void*) * v->size);
    }

    if (v->size == v->count) {
        if(v->size == 1){
            v->size = 4;
        }else{
            v->size *= 2;
        }
        v->data = CLI_REALLOC(v->data, sizeof(void*) * v->size);
    }

    v->data[v->count] = e;
    v->count++;
}

void cli_vector_set(cli_vector_t *v, cli_uint32 index, void *e)
{
    if (index >= v->count) {
        return;
    }

    v->data[index] = e;
}

void *cli_vector_get(cli_vector_t *v, cli_uint32 index)
{
    if (index >= v->count) {
        return NULL;
    }

    return v->data[index];
}

void cli_vector_delete(cli_vector_t *v, cli_uint32 index)
{
    if (index >= v->count) {
        return;
    }

    v->data[index] = NULL;

    cli_uint32 i, j;
    void **newarr = (void**)CLI_MALLOC(sizeof(void*) * v->count * 2);

    if(newarr == NULL){
        return;
    }

    for (i = 0, j = 0; i < v->count; i++) {
        if (v->data[i] != NULL) {
            newarr[j] = v->data[i];
            j++;
        }		
    }

    CLI_FREE(v->data);

    v->data = newarr;
    v->count--;
}


void cli_vector_qsort(cli_vector_t *v, cli_uint32 elem_size,cli_int32 (*cmp_func)(const void *, const void *))
{
    qsort(v->data, v->count, elem_size, cmp_func);
}

void cli_vector_free(cli_vector_t *v)
{
    CLI_FREE(v->data);
}
/*lint +e578*/

