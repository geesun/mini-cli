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


#include <stdlib.h>
#include "cli_api.h"
#include "cli_utils.h"
#include "cli_history_item.h"


/*lint -e438*/
/*lint -e578*/
static void cli_history_item_init(
        cli_history_item_t * item,
        const cli_int8 * line, 
        cli_uint32 index)
{
    item->line = cli_strdup(line);
    item->index = index; 
}

static void cli_history_item_finish(cli_history_item_t * item)
{
    cli_strfree(item->line);
    item->line = NULL;
}

cli_history_item_t * cli_history_item_new(
        const cli_int8 * line,
        cli_uint32 index)
{
    cli_history_item_t * item = CLI_MALLOC(sizeof(cli_history_item_t));

    if(item != NULL){
        cli_history_item_init(item,line,index);
    }

    return item;
}

void cli_history_item_delete(cli_history_item_t * item)
{
    if(item == NULL){
        return;
    }

    cli_history_item_finish(item);
    CLI_FREE(item);
}

const cli_int8 *  cli_history_item_get_line(cli_history_item_t * item)
{
    if(item == NULL){
        return NULL;
    }

    return item->line;
}


cli_uint32  cli_history_item_get_index(cli_history_item_t * item)
{
    if(item == NULL){
        return 0;
    }

    return item->index;
}

/*lint +e438*/
/*lint +e578*/


