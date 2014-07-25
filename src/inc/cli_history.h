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


#ifndef __CLI_HISTORY_H__
#define __CLI_HISTORY_H__
#include "cli_utils.h"
#include "cli_history_item.h"

#define CLI_MAX_HISTORY_NUM   20

typedef struct {
    cli_history_item_t*  items[CLI_MAX_HISTORY_NUM];
    cli_uint32   num; 
    cli_uint32   cur_index;
    cli_uint32   history_index;
}cli_history_t;


typedef struct{
    const cli_history_t *history;
    cli_uint32           offset;
}cli_histroy_iter_t;


cli_history_t *  cli_history_new();

void cli_history_delete(cli_history_t * history);

void cli_histroy_add(
        cli_history_t * history,
        const cli_int8 * line);

cli_history_item_t * cli_history_getfirst(
        cli_history_t * history,
        cli_histroy_iter_t * iter);

cli_history_item_t * cli_history_getlast(
        cli_history_t * history,
        cli_histroy_iter_t * iter);

cli_history_item_t * cli_history_getnext(
        cli_histroy_iter_t * iter);

cli_history_item_t * cli_history_getprevious(
        cli_histroy_iter_t * iter);

#endif
