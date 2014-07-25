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
#include "cli_utils.h"
#include "cli_parser.h"
#include "cli_cmd.h"

extern cli_status_t cli_parser_cmd_tag(
        cli_cmd_tag_t ** tag_out,
        cli_int8   * tag_str);

cli_cmd_t * cli_cmd_new(
        cli_uint8   pri,
        cli_func_t * handler,
        cli_int8 * cmd_str,
        cli_int8 * desc_str)
{
    cli_vector_t v; 
    cli_cmd_t *cmd = NULL;
    cli_cmd_tag_t *tag = NULL;
    cli_cmd_tag_item_t * item = NULL; 
    cli_uint32 i = 0;
    cli_uint32 j = 0 , k = 0;
    cli_status_t ret = CLI_OK;
    cli_int8 * desc = NULL;
    cli_int8 * str_tag = NULL;
 

    cmd = (cli_cmd_t *) CLI_MALLOC(sizeof(cli_cmd_t));

    if(cmd == NULL){
        return NULL;
    }
    
    memset(cmd,0x00,sizeof(cli_cmd_t));

    cmd->handler = handler;
    cmd->pri = pri;

    cli_vector_init(&cmd->tags);

    cli_split_string_with_token(&v,cmd_str," ");

    for(i = 0; i < cli_vector_count(&v); i++){
        str_tag = cli_vector_get(&v,i);
        ret =  cli_parser_cmd_tag(&tag,str_tag);
        if(ret == CLI_OK){
            cli_vector_add(&cmd->tags,tag);
        }
    }

    cli_free_str_vector(&v);

    cli_split_string_with_token(&v,desc_str,"\n");

    for(i = 0; i < cli_vector_count(&cmd->tags); i++){
        tag = cli_vector_get(&cmd->tags,i);
        for(j =0; j< cli_vector_count(&tag->items); j++){
            item = cli_vector_get(&tag->items,j);
            if(k < cli_vector_count(&v)){
                desc = cli_vector_get(&v,k);
                item->desc = cli_strdup(desc);
                cli_strtrim(item->desc);
                k++;
            }
        }
    }

    cli_free_str_vector(&v);

    return cmd;
}



cli_boolean cli_cmd_verify(
        cli_cmd_input_info_t *  input,
        cli_cmd_t * cmd,
        cli_uint32 * err_loc)
{
    cli_uint32 i = 0;
    cli_uint32 j = 0;
    cli_cmd_tag_t * tag = NULL;
    cli_uint32   tag_cnt = 0;
    cli_cmd_tag_item_t  * item = NULL;
    cli_uint32   item_cnt;
    cli_boolean  match = TRUE;
    cli_boolean  full_match = TRUE;

    CLI_ASSERT_RET(
            cmd != NULL 
            && input != NULL,
            FALSE);

    tag_cnt = cli_vector_count(&cmd->tags);
    for(i = 0; i < tag_cnt && i < input->num; i++){
        tag = cli_vector_get(&cmd->tags,i);
        item_cnt = cli_vector_count(&tag->items);
        match = FALSE;
        full_match = FALSE;
        for(j = 0; j < item_cnt; j++){
            item = cli_vector_get(&tag->items, j);
             if(item->format == CLI_TAG_FORMAT_KEYWORD){
                 if(strncasecmp(item->tag,input->input[i],strlen(item->tag)) == 0){
                     full_match = TRUE;
                 }
                 //if(tag->key){
                 if(strncasecmp(item->tag,input->input[i],strlen(input->input[i])) == 0){
                     /* double match without full match*/
                     if(match && !full_match){
                         *err_loc = i;
                         return FALSE;
                     }
                     match = TRUE;
                 }
                 /*
                 }else{
                     if(strcmp(item->tag,input->input[i]) == 0){
                         match = TRUE;
                     }
                 }*/
             }else{
                 /* verify */ 
                 if(cli_check_param_valid(item->format,item->tag,input->input[i])){
                     match = TRUE;
                 }
             }
        }

        if(match == FALSE){
            *err_loc = i;
            return FALSE;
        }
    }

    return TRUE;
}

cli_boolean cli_cmd_matched(
        cli_cmd_input_info_t *  input,
        cli_cmd_t * cmd,
        cli_boolean key_full_match
        )
{
    cli_uint32 i = 0;
    cli_uint32 j = 0;
    cli_cmd_tag_t * tag = NULL;
    cli_uint32   tag_cnt = 0;
    cli_cmd_tag_item_t  * item = NULL;
    cli_uint32   item_cnt;
    cli_boolean  match = TRUE;

    CLI_ASSERT_RET(
            cmd != NULL 
            && input != NULL,
            FALSE);

    tag_cnt = cli_vector_count(&cmd->tags);
    for(i = 0; i < tag_cnt && i < input->num; i++){
        tag = cli_vector_get(&cmd->tags,i);
        item_cnt = cli_vector_count(&tag->items);
        match = FALSE;
        for(j = 0; j < item_cnt; j++){
            item = cli_vector_get(&tag->items, j);

             if(item->format == CLI_TAG_FORMAT_KEYWORD){
                 if(key_full_match){
                    if(strncasecmp(item->tag,input->input[i],strlen(item->tag)) == 0){
                        match = TRUE;
                    }
                 }else{
                    if(strncasecmp(item->tag,input->input[i],strlen(input->input[i])) == 0){
                        match = TRUE;
                    }
                 }
             }else{
                match = TRUE;
             }
        }

        if(match == FALSE){
            return FALSE;
        }
    }

    /*check the user not input tag is option or not*/
    for(; i < tag_cnt; i++){
        tag = cli_vector_get(&cmd->tags,i);
        item_cnt = cli_vector_count(&tag->items);
        for(j = 0; j < item_cnt; j++){
            item = cli_vector_get(&tag->items, j);
            if(!item->opt){
               return FALSE;
            }
        }
    }

    return TRUE;
}


cli_status_t cli_cmd_get_matched_tag(
        cli_cmd_t * cmd, 
        cli_cmd_input_info_t * input,
        cli_boolean * full_match,
        cli_cmd_tag_t **last_match_tag)
{
    cli_uint32 i = 0;
    cli_uint32 j = 0;
    cli_boolean match = TRUE;
    cli_boolean tag_match = FALSE;
    cli_cmd_tag_t * tag = NULL;
    cli_uint32   tag_cnt = 0;
    cli_cmd_tag_item_t  * item = NULL;
    cli_uint32   item_cnt;


    CLI_ASSERT_RET(
            cmd != NULL 
            && input != NULL
            && full_match != NULL
            && last_match_tag != NULL, 
            CLI_E_PARAM);
    
    tag_cnt = cli_vector_count(&cmd->tags);
    for(i = 0; i < tag_cnt && i < input->num; i++){
        tag = cli_vector_get(&cmd->tags,i);
        tag_match = FALSE;
        item_cnt = cli_vector_count(&tag->items);
        for(j = 0; j < item_cnt; j++){
            item = cli_vector_get(&tag->items, j);
            if(item->format == CLI_TAG_FORMAT_KEYWORD){
                if(strncasecmp(item->tag,input->input[i],strlen(input->input[i])) == 0){
                    tag_match = TRUE;
                    break;
                }
            }else{
                if(cli_check_param_valid(item->format,item->tag,input->input[i])){
                    tag_match= TRUE;
                    break;
                }
            }
        }

        if(tag_match == FALSE){
            match = FALSE;
            break;
        }
    }


    /* found a match */
    if(i == input->num && match){
        //if(i >= must_tag_num){
        if(i == tag_cnt ){
            if(input->prefix){
                *last_match_tag = tag;
            }else{
                *last_match_tag = NULL;
            }
            *full_match = TRUE;
            return CLI_OK;
        }
        
        if(input->prefix){
            *last_match_tag = tag;
            *full_match = FALSE;
            return CLI_OK;
        }

        if(i < tag_cnt){
            *last_match_tag = cli_vector_get(&cmd->tags,i);
            *full_match = FALSE;
            return CLI_OK;
        }
    }

    return CLI_E_NOT_FOUND;
}


cli_int8 * cli_cmd_dump_str(
    cli_cmd_t   * cmd)
{
    cli_uint32 i = 0;
    cli_uint32 j = 0;
    cli_cmd_tag_t *tag = NULL;
    cli_cmd_tag_item_t * item = NULL; 
    cli_int8 * str = NULL;
    cli_boolean  key_str = FALSE;

    CLI_ASSERT_RET(cmd != NULL, NULL);
    
    str = cli_strdup("");

    for(i = 0; i < cli_vector_count(&cmd->tags); i++){
        tag = cli_vector_get(&cmd->tags,i);
        key_str = FALSE;
        for(j =0; j< cli_vector_count(&tag->items); j++){
            item = cli_vector_get(&tag->items,j);
            if(j == 0 && item->opt){
                cli_strcat(&str,"[");
            }
            if(j == 0 && !tag->key && item->format == CLI_TAG_FORMAT_KEYWORD){
                cli_strcat(&str,"(");
                key_str = TRUE;
            }

            if(j != 0){
                cli_strcat(&str,"|");
            }
            
            cli_strcat(&str,item->tag);
            if(key_str && (j + 1) == cli_vector_count(&tag->items)){
                cli_strcat(&str,")");
            }

            if(item->opt && (j + 1) == cli_vector_count(&tag->items)){
                cli_strcat(&str,"]");
            }
        }
        cli_strcat(&str," ");
    }

    return str;
}

void cli_cmd_dump(
        cli_cmd_t * cmd)
{
    cli_uint32 i = 0;
    cli_uint32 j = 0;
    cli_cmd_tag_t *tag = NULL;
    cli_cmd_tag_item_t * item = NULL; 

    for(i = 0; i < cli_vector_count(&cmd->tags); i++){
        tag = cli_vector_get(&cmd->tags,i);
        j =0;
        printf("key = %d \n",tag->key);
        for(j =0; j< cli_vector_count(&tag->items); j++){
            item = cli_vector_get(&tag->items,j);
            printf("\tf:%d %s\t-\t%s \n",item->format,item->tag,item->desc);
        }

    }
}

