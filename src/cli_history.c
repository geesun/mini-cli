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
#include <string.h>
#include "cli_api.h"
#include "cli_history_item.h"
#include "cli_history.h"

/*lint -e438*/
/*lint -e578*/
static void cli_history_init(cli_history_t * history)
{
    cli_uint32 i = 0; 
    for(i = 0; i < CLI_MAX_HISTORY_NUM; i++){
        history->items[i] = NULL;
    }

    history->num = 0; 
    history->cur_index = 0xffffffff;
    history->history_index = 0;
}

static void cli_history_finish(cli_history_t * history)
{
    cli_uint32 i = 0;
    for(i = 0; i < history->num; i++){
        cli_history_item_delete(history->items[i]);
    }
}

cli_history_t *  cli_history_new()
{
    cli_history_t * history = CLI_MALLOC(sizeof(cli_history_t));
    
    if(history != NULL){
        cli_history_init(history);
    }
    return history;
}

void cli_history_delete(cli_history_t * history)
{
    if(history == NULL){
        return;
    }
    
    cli_history_finish(history);
    CLI_FREE(history);
}

void cli_history_remove_duplicate(
         cli_history_t * history,
        const cli_int8 * line)
{
    cli_uint32  i = 0; 
    cli_uint32  index = 0;
    cli_uint32  start = 0;
    cli_history_item_t * item = NULL;
    cli_boolean found = FALSE;

    if(history->num == CLI_MAX_HISTORY_NUM){
        start = history->cur_index + 1;
    }
    for(i = 0; i < history->num; i++){
        index = (start  + i ) % CLI_MAX_HISTORY_NUM;
        
        item = history->items[index];
        
        if(item != NULL && strcmp(item->line,line) == 0){
            found = TRUE;
            break;
        }
    }

    if(!found){
        return;
    }

    cli_history_item_delete(item);

    /* shift the entry */
    for(; (i + 1) < history->num;i++){
        index = (start  + i ) % CLI_MAX_HISTORY_NUM;
        history->items[index] = history->items[(index + 1 )%CLI_MAX_HISTORY_NUM];
        history->items[(index + 1 )%CLI_MAX_HISTORY_NUM] = NULL;
    }

    history->num --;
    history->cur_index = (history->cur_index + CLI_MAX_HISTORY_NUM - 1 )%CLI_MAX_HISTORY_NUM;
}

void cli_histroy_add(
        cli_history_t * history,
        const cli_int8 * line)
{
    cli_history_item_t * item  = NULL;
    cli_uint32 index = 0;
    if(history == NULL){
        return;
    }

    cli_history_remove_duplicate(history,line);

    item = cli_history_item_new(line,history->history_index++);
    
    index = (history->cur_index + 1)%CLI_MAX_HISTORY_NUM;
    /* full, need remove oldest */
    if(history->num == CLI_MAX_HISTORY_NUM){
        cli_history_item_delete(history->items[index]);
    }else{
        history->num++;
    }

    history->items[index] = item;
    history->cur_index = index;
}


cli_history_item_t * cli_history_getfirst(
        cli_history_t * history,
        cli_histroy_iter_t * iter)
{
    cli_history_item_t * result = NULL; 

    CLI_ASSERT_RET(
            (history != NULL && iter != NULL),
            NULL);

    iter->history = history;

    if(history->num == 0){
        return NULL;
    }
    
    if(history->num < CLI_MAX_HISTORY_NUM){
        result = history->items[0];
        iter->offset  = 0;
    }else{
        iter->offset = (history->cur_index + 1)%CLI_MAX_HISTORY_NUM;
        result = history->items[iter->offset];
    }
    
    return result;
}

cli_history_item_t * cli_history_getlast(
        cli_history_t * history,
        cli_histroy_iter_t * iter)
{
    cli_history_item_t * result = NULL; 
    CLI_ASSERT_RET(
            (history != NULL && iter != NULL),
            NULL);

    if(history->num == 0){
        return NULL;
    }
    
    iter->history = history;
    iter->offset = history->cur_index;
    result = history->items[iter->offset];
    
    return result;
}

cli_history_item_t * cli_history_getnext(
        cli_histroy_iter_t * iter)
{
    cli_history_item_t * result = NULL; 
    cli_uint32 index ; 

    CLI_ASSERT_RET(
            (iter != NULL && iter->history != NULL),
            NULL);

    if(iter->history->num == 0){
        return NULL;
    }

    /* get to the last entry */
    if(iter->offset  == iter->history->cur_index){
        return NULL;
    }
    
    index = (iter->offset +  1 )%CLI_MAX_HISTORY_NUM;

    iter->offset = index;

    result = iter->history->items[index];
    
    return result;
}

cli_history_item_t * cli_history_getprevious(
        cli_histroy_iter_t * iter)
{
    cli_history_item_t * result = NULL; 
    cli_uint32 index; 
    

    CLI_ASSERT_RET(
            (iter != NULL && iter->history != NULL),
            NULL);

    if(iter->history->num == 0){
        return NULL;
    }

    if(iter->history->num < CLI_MAX_HISTORY_NUM){
        if(iter->offset == 0){
            return NULL;
        }
        index = iter->offset - 1 ;
    }else{
        index = (iter->offset + CLI_MAX_HISTORY_NUM - 1 )%CLI_MAX_HISTORY_NUM;

        if(index == iter->history->cur_index){
            return NULL;
        }
    }
    
    iter->offset = index; 

    result = iter->history->items[index];

    return result;
}
/*lint +e438*/
/*lint +e578*/

