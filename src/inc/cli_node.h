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


#ifndef __CLI_NODE_H__
#define __CLI_NODE_H__
#include "cli_utils.h"
#include "cli_cmd.h"
#include "cli_vector.h"

typedef struct cli_node_s  cli_node_t;

struct cli_node_s{
    cli_boolean           valid;
    cli_int8             *name;
    cli_vector_t         cmd_list;
    cli_func_t          *enter;
    cli_func_t          *exit;
}; 


typedef struct{
    cli_cmd_t * cmd;
    cli_cmd_tag_t * last_tag;
    cli_boolean  full_match;
}cli_node_match_cmd_info_t;

cli_status_t  cli_node_data_init(cli_uint32 max_node);

cli_status_t cli_node_add(
        cli_uint32   id,
        cli_func_t *enter,
        cli_func_t *exit);

cli_status_t cli_node_get_first_match_cmd(
        cli_uint8  pri,
        cli_uint32 node_id,
        cli_cmd_input_info_t * input,
        cli_node_match_cmd_info_t * match_info,
        cli_uint32  * next_index,
        cli_boolean complete_match
        );


cli_status_t cli_node_get_next_match_cmd(
        cli_uint8  pri,
        cli_uint32 node_id,
        cli_cmd_input_info_t * input,
        cli_node_match_cmd_info_t * match_info,
        cli_uint32  * next_index,
        cli_boolean complete_match
        );
cli_status_t cli_node_add_cmd(
        cli_uint32   index,
        cli_uint8   pri,
        cli_func_t *fn,
        cli_int8 * cmd_str, 
        cli_int8 * cmd_help
        );


cli_status_t cli_node_set_prompt(cli_uint32 node_id, cli_int8 * prompt);

cli_int8* cli_node_get_prompt(cli_uint32 node_id);

cli_int8 * cli_node_dump_str(cli_uint32 node_id);
#endif

