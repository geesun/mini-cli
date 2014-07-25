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


#ifndef __CLI_HISTORY_ITEM_H__
#define __CLI_HISTORY_ITEM_H__
#include "cli_utils.h"

typedef struct{
    cli_int8*    line; 
    cli_uint32   index;
}cli_history_item_t;


cli_history_item_t * cli_history_item_new(
        const cli_int8 * line,
        cli_uint32 index);

void cli_history_item_delete(cli_history_item_t * item);

const cli_int8 * cli_history_item_get_line(cli_history_item_t * item);

cli_uint32 cli_history_item_get_index(cli_history_item_t * item);


#endif
