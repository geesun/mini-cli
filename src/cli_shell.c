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


/*lint -e451*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#include "cli_api.h"
#include "cli_utils.h"
#include "cli_readline.h"
#include "cli_parser.h"
#include "cli_cmd.h"
#include "cli_node.h"
#include "cli_shell.h"
#include "cli_auth.h"
#include "cli_vector.h"
#include "cli_error.h"

/*lint -e578*/

/*lint -e64*/
/*lint -e438*/
/*lint -e550*/
/*lint -e574*/
/*lint -e530*/
/*lint -e18*/
/*lint -e14*/
/*lint -e516*/
/*lint -e532*/
/*lint -e533*/
/*lint -e686*/
static cli_int8 g_default_log[] = "\
                            \r\n\r\n\
            .88b  d88. d888888b d8b   db d888888b         .o88b. db      d888888b   \r\n \
            88'YbdP`88   `88'   888o  88   `88'          d8P  Y8 88        `88'     \r\n \
            88  88  88    88    88V8o 88    88           8P      88         88      \r\n \
            88  88  88    88    88 V8o88    88    C8888D 8b      88         88      \r\n \
            88  88  88   .88.   88  V888   .88.          Y8b  d8 88booo.   .88.     \r\n \
            YP  YP  YP Y888888P VP   V8P Y888888P         `Y88P' Y88888P Y888888P   \r\n \
\r\n\r\n";

typedef cli_status_t cli_node_change_t(
        cli_shell_t * ,
        cli_node_t * prev_node, 
        cli_node_t * node);

static cli_shell_ctrl_t g_cli_ctrl;

void cli_shell_show_error_mark(
         cli_shell_t          *  shell ,
         cli_uint32 loc)
{
    cli_print(shell,"%*s\n",loc + strlen(shell->shell_prompt) + 1,"^");
}


void cli_shell_show_cmd_error_info(
        cli_shell_t          *  shell ,
        cli_cmd_status_t  status)
{
    cli_print(shell,"  !%s\n\n",cli_get_error_str(status));
}

cli_status_t  cli_shell_run_cmd(
        cli_shell_t          *  shell ,
        cli_cmd_input_info_t *  input,
        cli_cmd_t * cmd)
{
    cli_uint32 err_loc = 0;
    cli_int8 *argv[CLI_SHELL_MAX_CMD_ARGC];
    cli_uint32 argc = 0;
    cli_uint32 i = 0;
    cli_cmd_tag_t * tag = NULL;
    cli_cmd_status_t ret = CLI_CMD_OK;

    if(!cli_cmd_verify(input,cmd,&err_loc)){
        /*show error mark*/
        //cli_print(shell,"%*s\n",input->pos[err_loc] + strlen(shell->shell_prompt),"^");
        cli_shell_show_error_mark(shell,input->pos[err_loc]);
        cli_shell_show_cmd_error_info(shell,CLI_CMD_E_PARAM);
        return  CLI_E_FORMAT;
    }
    
    for(i = 0 ; i < cli_vector_count(&cmd->tags) && i < input->num; i++){
        tag = cli_vector_get(&cmd->tags,i);
        if(tag->key){
            continue;
        }
        argv[argc++] = input->input[i];
    }

    if(cmd->handler != NULL){
        ret = cmd->handler(shell,argc,argv);
        if(ret != CLI_CMD_OK){
            cli_shell_show_cmd_error_info(shell,ret);
            return CLI_E_ERROR;
        }
        return CLI_OK;
    }

    return CLI_E_NOT_FOUND;
}

cli_status_t cli_shell_get_match_cmds(
        cli_uint8  pri,
        cli_uint32 node_id,
        cli_cmd_input_info_t *  input ,
        cli_node_match_cmd_info_t  *matches,
        cli_uint32 *num)
{
    cli_int32 i = 0;
    cli_uint32 index = 0; 
    cli_status_t ret = CLI_OK;

    ret = cli_node_get_first_match_cmd(pri,node_id,input,&matches[i],&index,FALSE);
    while(ret == CLI_OK){
        i++;
        if(i >= CLI_RL_MAX_MATCH_NUM){
            return CLI_E_ERROR;
        }
        ret = cli_node_get_next_match_cmd(pri,node_id,input,&matches[i],&index,FALSE);
    }

    *num = i;

    return CLI_OK;
}


cli_status_t cli_shell_key_enter(
        cli_rl_t *this,
        cli_int32 key)
{
    cli_cmd_input_info_t *  input = NULL;
    cli_shell_t          *  shell = (cli_shell_t*) this->context; 
    cli_node_match_cmd_info_t  matches[CLI_RL_MAX_MATCH_NUM];
    cli_uint32            match_num = 0;
    cli_uint32            node_id;
    cli_status_t         ret = CLI_OK;
    cli_uint32            i = 0;
    cli_uint32            full_match = 0;
    cli_uint32            key_full_match = 0;
    cli_node_match_cmd_info_t  *matched = NULL;
    
    CLI_ASSERT_RET(this != NULL, CLI_E_PARAM);

    cli_session_timeout_restart(shell);
    
    if(this->line == NULL){
        return CLI_E_ERROR;
    }
    
    if(!cli_shell_check_cmd_line(this)){
        return CLI_E_ERROR;
    }

    /*nothing input, just move the the line to next line*/
    if(*this->line == '\0'){
        cli_rl_crlf(this); 
        cli_rl_reset_line_state(this);
        return CLI_OK;
    }

    
    node_id = shell->node_id_list[shell->idx];
    input = cli_cmd_input_info_new(this->line);

    cli_shell_get_match_cmds(shell->user_pri,node_id,input,matches,&match_num);

#if 0
    printf("match num = %d \n", match_num);
    for(i = 0; i < match_num; i++){
        printf("match = %d ",matches[i].full_match);
        if(matches[i].last_tag == NULL){
            continue;
        }
        for(j = 0; j < cli_vector_count(&matches[i].last_tag->items); j++){
            cli_cmd_tag_item_t * _item = cli_vector_get(&matches[i].last_tag->items,j);

            printf("%s\t-\t%s\n",_item->tag,_item->desc);
        }
    }
#endif
    
    if(match_num == 0){
        cli_rl_crlf(this); 
        cli_shell_show_cmd_error_info(shell,CLI_CMD_E_CMD_NOT_FOUND);
        this->done = TRUE;
        ret = CLI_E_NOT_FOUND;
        goto Exit;
    }


    for(i = 0; i < match_num; i++){
        if(cli_cmd_matched(input,matches[i].cmd,FALSE)){
            full_match ++;
            matched = &matches[i];
        }
    }
    

    if(full_match == 0){
        cli_rl_crlf(this); 
        this->done = TRUE;
        cli_shell_show_cmd_error_info(shell,CLI_CMD_E_CMD_NOT_COMPLETE);
        goto Exit;
    }else if(full_match > 1){
        for(i = 0; i < match_num; i++){
            if(cli_cmd_matched(input,matches[i].cmd,TRUE)){
                key_full_match ++;
                matched = &matches[i];
            }
        }
        if(key_full_match != 1){
            cli_rl_crlf(this); 
            this->done = TRUE;
            cli_shell_show_cmd_error_info(shell,CLI_CMD_E_AMBIGUOUS);
            goto Exit;
        }
    }

    if(matched != NULL){
        cli_rl_crlf(this); 
        cli_shell_run_cmd(shell,input,matched->cmd);
        this->done = TRUE;
        goto Exit;
    }
Exit:
    cli_cmd_input_info_delete(input);
    return ret;

}




cli_uint32 cli_shell_get_prev_space(const cli_int8* text,cli_uint32 end)
{
    cli_int8 start = end;
    cli_int8 len = 0;

    CLI_ASSERT_RET(text != NULL, 0);
    len = strlen(text);
    if(start > len){
        start = len ;
    }

    while(start > 0 && !isspace(text[start -1])){
        start --;        
    }

    return start;

}

cli_int32  cli_shell_cmd_item_cmp(
         const void *a , const void *b )
{
    cli_cmd_tag_item_t * item1 = *(cli_cmd_tag_item_t **) a ; 
    cli_cmd_tag_item_t * item2 = *(cli_cmd_tag_item_t **) b;
    cli_int32 ret = 0; 
    
    if(item1 == NULL){
        return 1;
    }
    if(item2 == NULL){
        return -1;
    }
    
    ret = strcmp(item1->tag,item2->tag);

    if(ret == 0){
        if(item1->desc == NULL){
            return 1;
        }
        if(item2->desc == NULL){
            return -1;
        }
        ret = strcmp(item1->desc,item2->desc);
    }

    return ret;
}


static cli_status_t cli_shell_get_match_tag_items(
        cli_vector_t *vec,
        cli_node_match_cmd_info_t* matches,
        cli_uint32            match_num,
        cli_int8 * prefix
        )
{
    cli_uint32 i = 0, j = 0; 
    cli_cmd_tag_t * tag = NULL; 
    cli_cmd_tag_item_t * item = NULL;
    
    cli_vector_init(vec);

    for(i = 0; i < match_num; i++){
        tag = matches[i].last_tag;
        if(tag == NULL){
            continue;
        }
        for(j = 0 ; j < cli_vector_count(&tag->items); j++){
            item = cli_vector_get(&tag->items,j);

            if(prefix == NULL
                    || (item->format == CLI_TAG_FORMAT_KEYWORD 
                        && strncasecmp(item->tag,prefix,strlen(prefix)) == 0)
                    || (
                        item->format != CLI_TAG_FORMAT_KEYWORD
                        && prefix != NULL 
                        && cli_check_param_valid(item->format,item->tag,prefix)
                        )
                    ){
                cli_vector_add(vec,item);
            }
        }
    }

    cli_vector_qsort(vec,sizeof(cli_cmd_tag_item_t *),cli_shell_cmd_item_cmp);
    return CLI_OK;
}


static cli_status_t cli_shell_show_help(
        cli_rl_t * this,
        cli_cmd_input_info_t *  input,
        cli_vector_t   * vec
        )
{
    cli_vector_t  desc;
    cli_uint32 i = 0; 
    cli_uint32 j = 0; 
    cli_uint32 k = 0; 
    cli_int8 * p = NULL;
    cli_cmd_tag_item_t *item = NULL;
    cli_cmd_tag_item_t *tmp = NULL;
    cli_boolean duplicate = FALSE;
    cli_boolean desc_null = FALSE;
    cli_int8 *  tmp_str = NULL;
    cli_shell_t          *  shell = (cli_shell_t*) this->context; 

    for(i = 0; i < cli_vector_count(vec); ){
        item = cli_vector_get(vec,i);
        if(item->opt){
            tmp_str = cli_strdup("[");
            cli_strcat(&tmp_str,item->tag);
            cli_strcat(&tmp_str,"]");
            cli_print(shell,"  %-*s\t",20,tmp_str);
            cli_strfree(tmp_str);
            tmp_str = NULL;
        }else{
            cli_print(shell,"  %-*s\t",20,item->tag);
        }
        desc_null = FALSE;
        cli_vector_init(&desc);
        cli_vector_add(&desc,item->desc);
        for(j = i + 1 ; j<  cli_vector_count(vec); j++){
            tmp = cli_vector_get(vec,j);
            duplicate = FALSE;
            if(strcmp(item->tag,tmp->tag) != 0){
                break;
            }
            for(k = 0; k < cli_vector_count(&desc); k++){
                p = cli_vector_get(&desc,k);
                if(tmp->desc == NULL){
                    break;
                }
                if(strcmp(tmp->desc,p) == 0){
                    duplicate = TRUE;
                    break;
                }
            }
            if(!duplicate){
                if(tmp->desc == NULL){ 
                    if(desc_null){
                        continue;
                    }
                    desc_null = TRUE;
                }
                cli_vector_add(&desc,tmp->desc);
            }
        }
        
        for(k = 0; k < cli_vector_count(&desc); k++){
            p = cli_vector_get(&desc,k);
            if(k == 0){
                if(p != NULL){
                    cli_print(shell,"- %s\n",p);
                }else{
                    cli_print(shell,"- %s\n","N/A");
                }
            }else{
                if(p != NULL){
                    cli_print(shell,"  %-*s\t- %s\n",20," ",p);
                }
            }
        }
        cli_vector_free(&desc);
        i = j;

    }
    
    cli_print(shell,"\n");

    return CLI_OK;
}



static cli_uint8 cli_shell_do_complete(
        cli_rl_t * this,
        cli_cmd_input_info_t *  input,
        cli_vector_t   * vec 
        )
{
    cli_uint32 i = 0;
    cli_uint32 j = 0;
    cli_cmd_tag_item_t * item = NULL;
    cli_int8   *tmp = NULL;
    cli_int8   *complete = NULL;
    cli_uint32 start;
    cli_uint8   type = CLI_SHELL_COMPLETE_NONE;


    for(i = 0; i < cli_vector_count(vec); i++){
        item = cli_vector_get(vec,i);
        
        if(item->format != CLI_TAG_FORMAT_KEYWORD){
            if(input->prefix){
                type = CLI_SHELL_COMPLETE_PARAM;
            }
            goto Exit;
        }
    }


    for(i = 0; i < cli_vector_count(vec); i++){
        item = cli_vector_get(vec,i);
        if(complete == NULL){
            complete = cli_strdup(item->tag);
            continue;
        }
        tmp = item->tag; 
        
        if(strlen(tmp) < strlen(complete)){
            complete[strlen(tmp) - 1] = '\0';
        }
        j = 0;
        while(j < strlen(complete)){
            if(complete[j] != tmp[j]){
                complete[j] = '\0';
                break;
            }
            j++;
        }
    }

    if(complete != NULL){
        start = cli_shell_get_prev_space(this->line,strlen(this->line));

        cli_rl_delete_text(this,start,this->end);
        cli_rl_insert_text(this,complete);
    

        for(i = 0 ; i < cli_vector_count(vec); i++){
            item = cli_vector_get(vec,i);
            if(item->format == CLI_TAG_FORMAT_KEYWORD){
                if(type == CLI_SHELL_COMPLETE_WORD 
                        && strncasecmp(item->tag,complete,strlen(complete)) == 0){
                    type = CLI_SHELL_COMPLETE_PREFIX;
                }

                if(strcmp(item->tag,complete) == 0){
                    type = CLI_SHELL_COMPLETE_WORD;
                }
            }
        }
    }else{
        if(input->prefix){
            type = CLI_SHELL_COMPLETE_PARAM;
        }
    }
Exit:
    if(complete != NULL){
        cli_strfree(complete);
        complete = NULL;
    }

    return type;
}


cli_boolean cli_shell_check_cmd_line(
     cli_rl_t * this)
{
    cli_cmd_input_info_t *  input = NULL;
    cli_shell_t          *  shell = (cli_shell_t*) this->context; 
    cli_uint32            node_id;
    cli_node_match_cmd_info_t  matches[CLI_RL_MAX_MATCH_NUM];
    cli_uint32            match_num = 0;
    cli_uint32            i = 0;
    cli_uint32            num = 0;
    cli_uint32            err = 0;
    cli_uint32            err_loc = 0;
    cli_boolean           ret = FALSE;
    cli_boolean           is_bad_cmd = FALSE;

    if(this->line == NULL){
        return FALSE;
    }

    node_id = shell->node_id_list[shell->idx];

    input = cli_cmd_input_info_new(this->line);
    if(input == NULL){
        return FALSE;
    }

    num = input->num;

    for(i = num; i > 0; i--){
        input->num = i;
        match_num = 0;
        cli_shell_get_match_cmds(shell->user_pri,node_id,input,matches,&match_num);
        if(match_num > 0){
            break;
        }
    }
    
    input->num = num;

    if(i != num){
        err = i;
        is_bad_cmd = TRUE;
    }else{
        if(input->prefix){
            input->num --;
        }

        if(input->num == 0){
            ret = TRUE;
            goto Exit;
        }

        for(i = 0; i < match_num; i++){
            if(!cli_cmd_verify(input,matches[i].cmd,&err_loc)){
                if(err_loc > err){
                    err = err_loc;
                }
            }else{
                err = num;
            }
        }

        if(err == num){
            ret =  TRUE; 
            goto Exit;
        }
    }
    cli_rl_crlf(this); 
    
    cli_shell_show_error_mark(shell,input->pos[err]);

    if(is_bad_cmd){
        cli_shell_show_cmd_error_info(shell,CLI_CMD_E_CMD_NOT_FOUND);
        //this->done = TRUE;
    }else{
        cli_shell_show_cmd_error_info(shell,CLI_CMD_E_PARAM);
    }
    cli_rl_reset_line_state(this);
Exit:
    input->num = num;
    cli_cmd_input_info_delete(input);
    return ret;
}
     

cli_status_t cli_shell_do_help_complete(
        cli_rl_t * this,
        cli_boolean complete
        )
{
    cli_cmd_input_info_t *  input = NULL;
    cli_shell_t          *  shell = (cli_shell_t*) this->context; 
    cli_node_match_cmd_info_t  matches[CLI_RL_MAX_MATCH_NUM];
    cli_uint32            match_num = 0;
    cli_uint32            i = 0;
    cli_uint32            node_id;
    cli_status_t         ret = CLI_OK;
    cli_vector_t         item_vec;
    cli_int8          *   prefix = NULL;
    cli_uint32            err_loc = 0;

    if(this->line == NULL){
        return CLI_E_ERROR;
    }

    if(!cli_shell_check_cmd_line(this)){
        return CLI_E_ERROR;
    }
    
    node_id = shell->node_id_list[shell->idx];
    input = cli_cmd_input_info_new(this->line);
    if(input == NULL){
        return CLI_E_ERROR;
    }

    cli_shell_get_match_cmds(shell->user_pri,node_id,input,matches,&match_num);

#if  0
    printf("match num = %d \n", match_num);
    for(i = 0; i < match_num; i++){
        printf("match = %d ",matches[i].full_match);
        if(matches[i].last_tag == NULL){
            continue;
        }
        for(j = 0; j < cli_vector_count(&matches[i].last_tag->items); j++){
            cli_cmd_tag_item_t * _item = cli_vector_get(&matches[i].last_tag->items,j);

            printf("%s\t-\t%s\n",_item->tag,_item->desc);
        }
    }
#endif

    if(match_num == 0){
        ret = CLI_E_NOT_FOUND;
        cli_shell_show_cmd_error_info(shell,CLI_CMD_E_CMD_NOT_FOUND);
        cli_rl_reset_line_state(this);

        goto Exit;
    }


    if(input->prefix){
        prefix = input->input[input->num -1];
    }

    cli_shell_get_match_tag_items(&item_vec,
            matches,
            match_num,
            prefix
            );

    cli_uint8 type  = CLI_SHELL_COMPLETE_NONE;
    if(complete){
        /*do complete */
        type = cli_shell_do_complete(this,input,&item_vec);
    }
    
    if(match_num == 1 && cli_vector_count(&item_vec) <= 1) {
        cli_cmd_input_info_delete(input);
        input = cli_cmd_input_info_new(this->line);
        if(!cli_cmd_verify(input,matches[0].cmd,&err_loc)){
            /*show error mark*/
            cli_rl_crlf(this);
            
            cli_shell_show_error_mark(shell,input->pos[err_loc]);

            cli_shell_show_cmd_error_info(shell,CLI_CMD_E_PARAM);
            cli_rl_reset_line_state(this);
            goto Exit2;
        }
    }


    cli_rl_crlf(this);

    /*show cr when found a full match*/
    for(i = 0; i < match_num; i++){
        if(matches[i].full_match 
                && type == CLI_SHELL_COMPLETE_NONE
                && matches[i].last_tag == NULL
                && cli_cmd_verify(input,matches[i].cmd,&err_loc)){
            cli_print(shell,"  <cr>\n");
            break;
        }
    }
    
    if(type == CLI_SHELL_COMPLETE_WORD || type == CLI_SHELL_COMPLETE_PARAM){
        cli_rl_insert_text(this," ");
    }
    
    if((match_num != 1 && cli_vector_count(&item_vec) > 1)
            || type != CLI_SHELL_COMPLETE_WORD) 
    {
        cli_shell_show_help(this,input,&item_vec);
    }

    cli_rl_reset_line_state(this);

Exit2:
    cli_vector_free(&item_vec);

Exit:
    cli_cmd_input_info_delete(input);

    return ret;
}


cli_status_t cli_shell_key_help(
        cli_rl_t * this,
        cli_int32 key)
{
    /* not at the end of the text */
    if(strlen(this->line) != this->point){
        return CLI_OK;
    }

    cli_shell_do_help_complete(this,FALSE);

    return CLI_OK;
}

cli_status_t cli_shell_key_tab(
        cli_rl_t * this,
        cli_int32 key)
{
    /* not at the end of the text */
    if(strlen(this->line) != this->point){
        this->point = strlen(this->line);
    }

    cli_shell_do_help_complete(this,TRUE);

    return CLI_OK;
}


cli_status_t cli_add_root_node(
        cli_uint32 node_id
        )
{
    cli_status_t ret = CLI_OK;

    ret = cli_node_add(node_id,NULL,NULL);
    
    if(ret != CLI_OK){
        return CLI_OK;
    }
    
    g_cli_ctrl.root_node_id = node_id;

    cli_install_root_builtin_cmd(node_id);

    return CLI_OK;
}



cli_status_t cli_add_node(
        cli_uint32 node_id
        )
{
    cli_status_t ret = CLI_OK;

    ret = cli_node_add(node_id,NULL,NULL);

    if(ret != CLI_OK){
        return CLI_OK;
    }

    cli_install_builtin_cmd(node_id);

    return CLI_OK;
}

cli_status_t cli_global_cmd_node_range(
        cli_uint32 min_node,
        cli_uint32 max_node)
{   
    cli_uint32 i = 0; 
    cli_uint32 j = 0; 

    CLI_ASSERT_RET(
            min_node < max_node
            && max_node < g_cli_ctrl.max_node,CLI_E_PARAM);

    for(i = 0 ; i < g_cli_ctrl.max_node; i++){
        g_cli_ctrl.global_nodes[i] = CLI_NODE_ID_INVALID;
    }

    for(i = min_node; i <  max_node; i++, j++){
        g_cli_ctrl.global_nodes[j] = i;
    }
    return CLI_OK;
}

cli_status_t cli_global_cmd_node_list(
        cli_uint32 node,
        ...)
{
    cli_uint32 i = 0;
    cli_uint32 value = 0;
    va_list argp;   
    va_start(argp,node); 

    /*remove next use data */
    for(i = 0 ; i < g_cli_ctrl.max_node; i++){
        g_cli_ctrl.global_nodes[i] = CLI_NODE_ID_INVALID;
    }

    i = 0;
    g_cli_ctrl.global_nodes[i] = node;
    i++;

    do{
        value = va_arg(argp,cli_uint32);
        if(value == CLI_NODE_ID_INVALID){
            break;
        }
        g_cli_ctrl.global_nodes[i] = value;
        i++;
    }while(i < g_cli_ctrl.max_node);

    va_end(argp);
    return CLI_OK;
}


cli_status_t cli_install_pri_global_cmd(
        cli_uint8  pri,
        cli_shell_cmd_def_t *cmd_def)
{
    cli_uint32 i = 0; 

    CLI_ASSERT_RET(cmd_def != NULL,CLI_E_PARAM);

    for(i = 0; i < g_cli_ctrl.max_node; i++){
        if(g_cli_ctrl.global_nodes[i] == CLI_NODE_ID_INVALID){
            break;
        }
        cli_node_add_cmd(g_cli_ctrl.global_nodes[i],pri,(cli_func_t*)cmd_def->handler,cmd_def->cmd,cmd_def->cmd_help);
    }

    return CLI_OK;
}

cli_status_t cli_install_global_cmd(
        cli_shell_cmd_def_t *cmd_def)
{
    CLI_ASSERT_RET(cmd_def != NULL,CLI_E_PARAM);

    return cli_install_pri_global_cmd(CLI_PRI_NORMAL,cmd_def);
}


cli_status_t cli_install_cmd(
        cli_uint32 node_id,
        cli_shell_cmd_def_t *cmd_def)
{
    CLI_ASSERT_RET(cmd_def != NULL,CLI_E_PARAM);
    
    return cli_node_add_cmd(node_id,CLI_PRI_NORMAL,(cli_func_t*)cmd_def->handler,cmd_def->cmd,cmd_def->cmd_help);
}

cli_status_t cli_install_pri_cmd(
        cli_uint8  pri,
        cli_uint32 node_id,
        cli_shell_cmd_def_t *cmd_def)
{
    CLI_ASSERT_RET(cmd_def != NULL,CLI_E_PARAM);
    
    return cli_node_add_cmd(node_id,pri,(cli_func_t*)cmd_def->handler,cmd_def->cmd,cmd_def->cmd_help);
}

void cli_shell_set_user_pri(
        cli_shell_t * shell,
        cli_uint8 pri)
{
    CLI_ASSERT(shell != NULL);

    shell->user_pri = pri;
}

cli_uint8  cli_get_user_pri(
        cli_shell_t * shell)
{
    CLI_ASSERT_RET(shell != NULL,CLI_PRI_INVALID);

    return shell->user_pri;
}

/*lint -e40*/
/*lint -e10*/
cli_status_t cli_enter_node(
        cli_shell_t * shell,
        cli_uint32 node_id,
        const cli_int8 * prompt_fmt,
        ...)
{
    cli_int8  prompt[CLI_MAX_PROMPT_LEN] = {0};
    cli_int8  tmp[CLI_MAX_PROMPT_LEN] = {0};
    va_list args;

    CLI_ASSERT_RET(shell != NULL
            && prompt_fmt != NULL, 
            CLI_E_PARAM);

    if((shell->idx + 1 )>= CLI_NODE_DEEP_MAX){
        return CLI_E_ERROR;
    }
    

    va_start(args, prompt_fmt);

    vsnprintf(prompt,sizeof(prompt),prompt_fmt,args);
    /*connect the prompt*/
    if(*prompt_fmt == '-'){
        snprintf(tmp,sizeof(tmp),"%s%s", shell->prompts[shell->idx],prompt);
    }else{
        snprintf(tmp,sizeof(tmp),"%s", prompt);
    }
    
    va_end(args);
    
    shell->node_id_list[++shell->idx] = node_id;
    shell->prompts[shell->idx] = cli_strdup(tmp);
    cli_set_prompt(shell,shell->prompts[shell->idx]);

    return CLI_OK;
}
/*lint +e40*/
/*lint +e10*/

cli_status_t cli_exit_node(
        cli_shell_t * shell
        )
{
    CLI_ASSERT_RET(shell != NULL,CLI_E_PARAM);

    if(shell->idx == 0){
        return CLI_E_ERROR;
    }

    if(shell->prompts[shell->idx] != NULL){
        cli_strfree(shell->prompts[shell->idx]);
        shell->prompts[shell->idx] = NULL;
    }

    shell->idx --;
    
    cli_set_prompt(shell,shell->prompts[shell->idx]);

    return CLI_OK;
}


cli_status_t cli_context_set(
            cli_shell_t * shell,
            cli_uint32  ctx_index,
            cli_uint32  ctx)
{
    CLI_ASSERT_RET(shell != NULL
            && ctx_index < CLI_MAX_CONTEXT_NUM,CLI_E_PARAM);
    
    shell->contexts[ctx_index] = ctx;

    return CLI_OK;
}

cli_uint32  cli_context_get(
        cli_shell_t * shell,
        cli_uint32  ctx_index)
{
    CLI_ASSERT_RET(shell != NULL
             && ctx_index < CLI_MAX_CONTEXT_NUM,0xffffffff);

    return shell->contexts[ctx_index] ;
}


void cli_shell_init(
        cli_shell_t * shell,
        FILE   *instream,
        FILE   *outstream)
{
    cli_uint32 i = 0;

    shell->readline = cli_rl_new(instream,outstream);
    shell->state = CLI_SHELL_STATE_INITIALISING;
    if(g_cli_ctrl.cli_host == NULL){
        shell->host_name = cli_strdup(CLI_DEFAULT_HOST);
    }else{
        shell->host_name = cli_strdup(g_cli_ctrl.cli_host);
    }

    shell->idx = 0;
    for(i = 0 ; i < CLI_NODE_DEEP_MAX; i++){
        shell->prompts[i] = NULL;
    }
    
    for(i = 0; i < CLI_MAX_CONTEXT_NUM; i++){
        shell->contexts[i] = 0xffffffff;
    }

    shell->shell_prompt = cli_strdup(shell->host_name);
    cli_strcat(&shell->shell_prompt,">");

    shell->prompts[0] = cli_strdup("");
    shell->node_id_list[0] = g_cli_ctrl.root_node_id;

    shell->is_timeout = FALSE;
    shell->timer = NULL;

   /* cli_rl_bind_key(shell->readline,'\n',cli_shell_key_enter);*/
    cli_rl_bind_key(shell->readline,'\r',cli_shell_key_enter);
    cli_rl_bind_key(shell->readline,'?',cli_shell_key_help);
    cli_rl_bind_key(shell->readline,'\t',cli_shell_key_tab);
}

cli_shell_t * cli_shell_new(
        FILE   *instream,
        FILE   *outstream)
{
    cli_shell_t * shell = (cli_shell_t *) CLI_MALLOC(sizeof(cli_shell_t));

    if(shell == NULL){
        return NULL;
    }

    memset(shell,0x00,sizeof(cli_shell_t));
    cli_shell_init(shell,instream,outstream);

    return shell;
}

void cli_shell_delete(cli_shell_t * shell)
{
    cli_uint32 i = 0 ; 

    CLI_ASSERT(shell != NULL);

    cli_rl_delete(shell->readline);
    shell->readline = NULL;

    if(shell->shell_prompt != NULL){
        cli_strfree(shell->shell_prompt);
        shell->shell_prompt = NULL;
    }
    
    if(shell->host_name  != NULL){
        cli_strfree(shell->host_name);
        shell->host_name  = NULL;
    }

    for(i = 0; i <= shell->idx; i++){
        if(shell->prompts[i] != NULL){
            cli_strfree(shell->prompts[i]);
            shell->prompts[i] = NULL;
        }
    }

    CLI_FREE(shell);
}

void cli_set_prompt(cli_shell_t * shell,cli_int8* prompt)
{
    cli_int8  tmp[CLI_MAX_PROMPT_LEN] = {0};

    CLI_ASSERT(shell != NULL&&prompt != NULL);
    if(shell->shell_prompt != NULL){
        cli_strfree(shell->shell_prompt);
        shell->shell_prompt = NULL;
    }

    /*need reconsider with permession*/
    if(strlen(prompt) != 0){
        snprintf(tmp,sizeof(tmp),"%s(%s)#",shell->host_name,prompt);
    }else{
        if(shell->idx == 0){
            snprintf(tmp,sizeof(tmp),"%s>",shell->host_name);
        }else{
            snprintf(tmp,sizeof(tmp),"%s#",shell->host_name);
        }
    }

    shell->shell_prompt = cli_strdup(tmp);
}

void cli_loop(cli_shell_t * shell)
{
    cli_int8 *line = NULL;

    CLI_ASSERT(shell != NULL);

    while(shell->state == CLI_SHELL_STATE_READY && !shell->readline->session_end){
        line = cli_rl_readline(
                shell->readline,
                shell,
                shell->shell_prompt);

        if(line != NULL){
            cli_histroy_add(shell->readline->history,line);
            CLI_FREE(line);
            line = NULL;
        }
    }
}

void cli_echo_enable(
    cli_shell_t * this,
    cli_boolean enable)
{
    CLI_ASSERT(this != NULL);

    cli_rl_echo_enable(this->readline,enable);
    
}

cli_int32 cli_get_input(
        cli_shell_t * this,
        cli_int8 * prompt,
        cli_int8 * buf, 
        cli_int32  max_len
        )
{
    cli_uint32 len;
    
    CLI_ASSERT_RET(this != NULL
            && prompt != NULL
            && buf != NULL, 0);
    
    len = cli_rl_get_input(this->readline,prompt,buf,max_len);

    return len;

}


/*lint -e40*/
/*lint -e10*/
cli_int32 cli_print(
        cli_shell_t * this,
        const cli_int8 *fmt,...
        )
{

    va_list args;
    cli_int32 len = 0;

    va_start(args, fmt);
    len = cli_rl_printf(this->readline, fmt, args);
    va_end(args);

    return len;

}
/*lint +e40*/
/*lint +e10*/


void cli_main(
        cli_session_type_t type,
        FILE   *in,
        FILE   *out)
{
    cli_shell_t * shell = NULL;
    do{
        shell = cli_shell_new(in,out);
        if(shell == NULL){
            break;
        }

        if(g_cli_ctrl.cli_logo == NULL){
            cli_print(shell,"%s",g_default_log);
        }else{
            cli_print(shell,"%s",g_cli_ctrl.cli_logo);
        }

        if(cli_user_login(shell,3)){
            shell->state = CLI_SHELL_STATE_READY;
            shell->is_timeout = FALSE;

            if(g_cli_ctrl.timer.timer_add != NULL){
                shell->timer = g_cli_ctrl.timer.timer_add(
                        g_cli_ctrl.timer.timeout,
                        shell,
                        cli_session_timeout);
            }

            cli_loop(shell);

            if(g_cli_ctrl.timer.timer_del != NULL && !shell->is_timeout
              ){
                g_cli_ctrl.timer.timer_del(shell->timer);
            }

        }else{
            shell->state = CLI_SHELL_STATE_LOGIN_FAILED;
        }

        cli_shell_delete(shell);

        shell = NULL;
    }while(type == CLI_SESSION_CONSOLE && g_cli_ctrl.shell_exit != 1);
}

void cli_hostname_set(cli_int8 * host, cli_uint32 len)
{
    if(g_cli_ctrl.cli_host != NULL){
        CLI_FREE(g_cli_ctrl.cli_host);
        g_cli_ctrl.cli_host = NULL;
    }
    g_cli_ctrl.cli_host = CLI_MALLOC(len + 1);
    
    if(g_cli_ctrl.cli_host == NULL){
       return; 
    }
    
    memset(g_cli_ctrl.cli_host,0x00,len + 1);
    strncpy(g_cli_ctrl.cli_host,host,len);
}

void cli_logo_set(cli_int8 * logo, cli_uint32 len)
{
    if(g_cli_ctrl.cli_logo != NULL){
        CLI_FREE(g_cli_ctrl.cli_logo);
        g_cli_ctrl.cli_logo = NULL;
    }
    g_cli_ctrl.cli_logo = CLI_MALLOC(len + 1);
    
    if(g_cli_ctrl.cli_logo == NULL){
       return; 
    }
    
    memset(g_cli_ctrl.cli_logo,0x00,len + 1);
    strncpy(g_cli_ctrl.cli_logo,logo,len);
}

/* for session timeout */
void cli_session_timeout_restart(
        cli_shell_t * shell)
{
    CLI_ASSERT(shell != NULL);

    if(shell->is_timeout){
        return;
    }

    if(g_cli_ctrl.timer.timer_del != NULL){
        g_cli_ctrl.timer.timer_del(shell->timer);
    }

    if(g_cli_ctrl.timer.timer_add != NULL){
        shell->timer = g_cli_ctrl.timer.timer_add(
                g_cli_ctrl.timer.timeout,
                shell,
                cli_session_timeout);
    }
}

void cli_session_timeout(
        cli_shell_t * this
        )
{
    CLI_ASSERT(this != NULL);
    cli_print(this,"Session timeout, please login again\n");
    this->state = CLI_SHELL_STATE_CLOSING;
    this->readline->done = TRUE;
    this->readline->session_end = TRUE;
    this->is_timeout = TRUE;
    this->timer = NULL;
}

cli_status_t cli_session_timer_set(
        cli_timer_add_func_t * timer_add,
        cli_timer_del_func_t * timer_del)
{
    g_cli_ctrl.timer.timer_add = timer_add;
    g_cli_ctrl.timer.timer_del = timer_del;

    return CLI_OK;
}

cli_status_t cli_session_idle_time_set(
         cli_shell_t * cli,
        cli_uint32 timeout)
{
    g_cli_ctrl.timer.timeout = timeout;
    if(cli != NULL){
        cli_session_timeout_restart(cli);
    }
    return CLI_OK;
}

cli_status_t cli_shell_ctrl_init(cli_uint32 max_node)
{
    cli_status_t ret = CLI_CMD_OK;
    cli_uint32 i = 0;

    ret = cli_node_data_init(max_node);
    if(ret != CLI_OK){
        return ret;
    }

    g_cli_ctrl.root_node_id = 0;
    g_cli_ctrl.cli_logo = NULL;
    g_cli_ctrl.cli_host = NULL;
    g_cli_ctrl.timer.timer_add = NULL;
    g_cli_ctrl.timer.timer_del = NULL;
    g_cli_ctrl.timer.timeout = 1000*300;
    g_cli_ctrl.shell_exit = 1;
    
    g_cli_ctrl.max_node = max_node;
    g_cli_ctrl.global_nodes = CLI_MALLOC(sizeof(cli_uint32) * max_node);
    if(g_cli_ctrl.global_nodes == NULL){
        return CLI_E_NO_MEMORY;
    }

    for(i = 0 ; i < max_node; i++){
        g_cli_ctrl.global_nodes[i] = CLI_NODE_ID_INVALID;
    }

    return CLI_OK;
}

void cli_console_exit_disable()
{
    g_cli_ctrl.shell_exit = 0;
}



/*lint +e578*/

/*lint +e64*/
/*lint +e438*/
/*lint +e550*/
/*lint +e574*/
/*lint +e530*/
/*lint +e18*/
/*lint +e14*/
/*lint +e516*/
/*lint +e532*/
/*lint +e533*/
/*lint +e451*/
/*lint +e686*/

