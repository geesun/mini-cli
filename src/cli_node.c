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
#include <string.h>
#include <stdlib.h>
#include "cli_api.h"
#include "cli_utils.h"
#include "cli_cmd.h"
#include "cli_parser.h"
#include "cli_node.h"
#include "cli_shell.h"

/*lint -e578*/
static cli_node_t * g_cli_nodes = NULL;
static cli_uint32    g_cli_node_num = 0;

cli_status_t  cli_node_data_init(cli_uint32 max_node)
{
    cli_uint32 i = 0;

    g_cli_nodes = (cli_node_t *) CLI_MALLOC(sizeof(cli_node_t) * max_node);

    if(g_cli_nodes == NULL){
        CLI_ERROR("No memory \n");
        return CLI_E_NO_MEMORY; 
    }

    memset(g_cli_nodes,0x00,sizeof(cli_node_t) * max_node);
    g_cli_node_num = max_node;

    for(i = 0; i < max_node; i++){
        g_cli_nodes[i].valid = FALSE;
    }

    return CLI_OK;
}



cli_status_t cli_node_add(
    cli_uint32   id,
    cli_func_t *enter,
    cli_func_t *exit)

{

    cli_node_t  * p_node = NULL;

    CLI_ASSERT_RET(
            id < g_cli_node_num 
            ,CLI_E_PARAM); 
    
    p_node = &g_cli_nodes[id];

    /* 
     * since we support add command before add node 
     * if the node already add, ignore it
     */
    if(p_node->valid){
        /*CLI_ERROR("Node already exist \n");*/
        return CLI_OK;
    }
    
    p_node->valid = TRUE;
    cli_vector_init(&p_node->cmd_list);
    p_node->enter = enter;
    p_node->exit = exit;

    return CLI_OK;
}


cli_node_t * cli_node_get_by_id(cli_uint32 index)
{
    if(index < g_cli_node_num){
        return &g_cli_nodes[index];
    }
    return NULL;
}

cli_status_t cli_node_add_cmd(
        cli_uint32   index,
        cli_uint8   pri,
        cli_func_t *fn,
        cli_int8 * cmd_str, 
        cli_int8 * cmd_help
        )
{
    cli_cmd_t * cmd = NULL;
    cli_node_t * node = NULL;

    CLI_ASSERT_RET(index < g_cli_node_num
            && fn != NULL
            , CLI_E_PARAM);

    node =  &g_cli_nodes[index];

    /* 
     * since we support add command before add node 
     * if the node not add, add it for user
     */
    if(!node->valid){
        cli_node_add(index,NULL,NULL);
    }

    cmd = cli_cmd_new(pri,fn,cmd_str,cmd_help);

    if(cmd == NULL){
        CLI_ERROR("cmd %s format is not correct\n",cmd_str);
        return CLI_E_FORMAT;
    }
    
    
    cli_vector_add(&node->cmd_list,cmd);
    
    return CLI_OK;
}

cli_int8 * cli_node_dump_str(cli_uint32 node_id)
{
    cli_int8 * str = NULL;
    cli_int8 * tmp = NULL;
    cli_node_t * node = NULL;
    cli_uint32   i = 0 ; 
    cli_cmd_t   * cmd = NULL;

    CLI_ASSERT_RET(node_id< g_cli_node_num, NULL);
    
    node =  &g_cli_nodes[node_id];
    
    str = cli_strdup("");
    for(i = 0 ; i < cli_vector_count(&node->cmd_list); i++){
        cmd = cli_vector_get(&node->cmd_list, i);
        tmp = cli_cmd_dump_str(cmd);
        if(tmp != NULL){
            cli_strcat(&str,"  ");
            cli_strcat(&str,tmp);
            cli_strcat(&str,"\r\n");
            cli_strfree(tmp); 
            tmp = NULL;
        }
    }

    return str;

}

cli_status_t cli_node_get_first_match_cmd(
        cli_uint8 pri,
        cli_uint32 node_id,
        cli_cmd_input_info_t * input,
        cli_node_match_cmd_info_t * match_info,
        cli_uint32  * next_index,
        cli_boolean complete_match
        )

{
    cli_node_t * node = NULL;
    cli_uint32   i = 0 ; 
    cli_cmd_t   * cmd = NULL;
    cli_cmd_tag_t* tag = NULL;
    cli_boolean  matched = FALSE;
    cli_boolean  full_matched = FALSE;
    
    CLI_ASSERT_RET(node_id< g_cli_node_num
            && input != NULL
            && next_index != NULL
            && match_info != NULL, CLI_E_PARAM);

    node =  &g_cli_nodes[node_id];

    if(!node->valid){
        return CLI_E_PARAM;
    }
    

    for(i = 0 ; i < cli_vector_count(&node->cmd_list); i++){
        cmd = cli_vector_get(&node->cmd_list, i);
        if(pri < cmd->pri){
            continue;
        }
        if(CLI_OK == cli_cmd_get_matched_tag(cmd,input,&full_matched,&tag)){
            matched = TRUE;
            break;
        }
    }

    if(complete_match && !full_matched){
        return CLI_E_NOT_FOUND;
    }

    if(matched){
        match_info->cmd = cmd; 
        match_info->last_tag = tag; 
        *next_index = i + 1;
        match_info->full_match = full_matched;
        return CLI_OK;
    }

    return CLI_E_NOT_FOUND;

}


cli_status_t cli_node_get_next_match_cmd(
        cli_uint8 pri,
        cli_uint32 node_id,
        cli_cmd_input_info_t * input,
        cli_node_match_cmd_info_t * match_info,
        cli_uint32  * next_index,
        cli_boolean complete_match
        )

{
    cli_node_t * node = NULL;
    cli_uint32   i = 0 ; 
    cli_cmd_t   * cmd = NULL;
    cli_cmd_tag_t* tag = NULL;
    cli_boolean  matched = FALSE;
    cli_boolean  full_matched = FALSE;
    
    CLI_ASSERT_RET(
            input != NULL
            && match_info != NULL, CLI_E_PARAM);

   
    node =  &g_cli_nodes[node_id];

    if(!node->valid){
        return CLI_E_PARAM;
    }

    for(i = *next_index ; i < cli_vector_count(&node->cmd_list); i++){
        cmd = cli_vector_get(&node->cmd_list, i);
        if(pri < cmd->pri){
            continue;
        }
        if(CLI_OK == cli_cmd_get_matched_tag(cmd,input,&full_matched,&tag)){
            matched = TRUE;
            break;
        }
    }
    
    if(complete_match && !full_matched){
        return CLI_E_NOT_FOUND;
    }
    if(matched){
        match_info->cmd = cmd; 
        match_info->last_tag = tag; 
        *next_index = i + 1;
        match_info->full_match = full_matched;
        return CLI_OK;
    }

    return CLI_E_NOT_FOUND;

}

void cli_shell_node_dump(
        cli_uint32   index)
{
    cli_node_t * node = NULL;
    cli_cmd_t * cmd = NULL;
    cli_uint32 i = 0;

    node =  &g_cli_nodes[index];

    for(i = 0; i < cli_vector_count(&node->cmd_list); i++){
        cmd = cli_vector_get(&node->cmd_list,i);
        printf("===================================\n");
        cli_cmd_dump(cmd);
    }

}
/*lint +e578*/

