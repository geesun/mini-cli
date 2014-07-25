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
#include <ctype.h>
#include "cli_api.h"
#include "cli_utils.h"
#include "cli_vector.h"
#include "cli_cmd.h"
#include "cli_parser.h"

/*lint -e438*/
/*lint -e613*/
/*lint -e676*/
/*lint -e14*/
/*lint -e574*/
/*lint -e19*/
/*lint -e659*/
/*lint -e124*/

void cli_split_string_with_token(
        cli_vector_t * vec,
        cli_int8 * string,
        cli_int8 * tok)
{
    cli_int8 * str = NULL;
    cli_int8 * p = NULL;
    cli_int8 * p1 = NULL;

    CLI_ASSERT(vec != NULL
            && string != NULL
            && tok != NULL);

    cli_vector_init(vec);

    str = cli_strdup(string);

    p1 = strtok(str,tok);
    while(p1 != NULL){
        p = cli_strdup(p1);
        cli_vector_add(vec,p);
        p1 = strtok(NULL,tok);
    }

    cli_strfree(str);
}

void cli_free_str_vector(
        cli_vector_t * v)
{
    cli_uint32 i = 0;
    
    for(i = 0; i < cli_vector_count(v); i++){
        cli_int8 * item = cli_vector_get(v,i);
        cli_strfree(item);
    }

    cli_vector_free(v);
}

cli_char_type_e  cli_get_char_type(cli_int8 c)
{
    switch(c){
        case '[':
            return CLI_LEFT_BRACKET;
        case ']':
            return CLI_RIGHT_BRACKET;
        case '(':
            return CLI_LEFT_PARENTHESE;
        case ')':
            return CLI_RIGHT_PARENTHESE;
        case '|':
            return CLI_BAR;
        default:
            return CLI_OTHER;
    }

}




static cli_status_t cli_parser_cmd_tag_item(
        cli_cmd_tag_item_t **item,
        cli_int8  * item_str,
        cli_boolean opt)
{
    cli_int8 * p = NULL;
    cli_int8 * tmp = NULL;
    cli_boolean is_upper = TRUE;
    cli_boolean is_valid_char = TRUE;
    cli_status_t ret = CLI_OK;

    CLI_ASSERT_RET(item != NULL && item_str != NULL, CLI_E_PARAM);

    *item = (cli_cmd_tag_item_t*)CLI_MALLOC(sizeof(cli_cmd_tag_item_t));

    if(*item == NULL){
        return CLI_E_NO_MEMORY;
    }

    memset(*item,0x00,sizeof(cli_cmd_tag_item_t));
    
    (*item)->tag = cli_strdup(item_str);
    (*item)->opt = opt;
    if(strcmp(item_str,"A.B.C.D") == 0){
        (*item)->format = CLI_TAG_FORMAT_IPV4;
        return CLI_OK;
    }

    if(strcmp(item_str,"A:B:C:D:E:F") == 0){
        (*item)->format = CLI_TAG_FORMAT_MAC;
        return CLI_OK;
    }

    if(strcmp(item_str,"A::B") == 0){
        (*item)->format = CLI_TAG_FORMAT_IPV6;
        return CLI_OK;
    }
    
    if(item_str[0] == '<' && item_str[strlen(item_str) - 1] == '>'){
        
        if(strchr(item_str,'-') == NULL){
            ret = CLI_E_FORMAT;
            goto Exit;
        }

        tmp = cli_strdup(item_str + 1);
        tmp[strlen(tmp) - 1 ] = '\0';
        p = strtok(tmp,"-");
        
        if(p == NULL || !cli_is_digital(p)){
            cli_strfree(tmp);
            tmp = NULL;
            ret = CLI_E_FORMAT;
            goto Exit;
        }

        p = strtok(NULL,"-");
        if(p == NULL || !cli_is_digital(p)){
            cli_strfree(tmp);
            tmp = NULL;
            ret = CLI_E_FORMAT;
            goto Exit;
        }
        (*item)->format = CLI_TAG_FORMAT_RANGE;
        cli_strfree(tmp);
        tmp = NULL;

        return CLI_OK;
    }

    is_upper = cli_isupper(item_str);

    is_valid_char = cli_iskeyword(item_str);

    /* all is upcase */
    if(is_upper){
        (*item)->format = CLI_TAG_FORMAT_STR;
        return CLI_OK;
    }
    
    /*all is valid char*/
    if(is_valid_char){
        (*item)->format = CLI_TAG_FORMAT_KEYWORD;
        return CLI_OK;
    }
Exit:
    CLI_FREE(*item);
    *item = NULL;
    return ret;
}

cli_status_t cli_parser_cmd_tag(
        cli_cmd_tag_t ** tag_out,
        cli_int8   * tag_str)
{
    cli_int8 * str = NULL;
    cli_int8 * start = NULL;
    cli_int8 * end = NULL;
    cli_int8 * tmp = NULL;
    cli_char_type_e type ;
    cli_parser_state_e state = CLI_P_S_INIT;
    cli_status_t ret = CLI_OK;
    cli_cmd_tag_item_t * item = NULL;
    cli_uint32  i = 0;
    cli_cmd_tag_t  * tag = NULL;
    cli_boolean   opt = FALSE;

    cli_parser_state_e state_stack[CLI_MAX_PARENTHESE_LOOP] = {CLI_P_S_INIT};
    cli_uint32 stack_head = 0;

    CLI_ASSERT_RET(tag_out != NULL && tag_str != NULL, CLI_E_PARAM);


    cli_strtrim(tag_str);
    
    *tag_out = tag = CLI_MALLOC(sizeof(cli_cmd_tag_t));

    if(tag == NULL){
        return CLI_E_NO_MEMORY;
    }

    memset(tag,0x00,sizeof(cli_cmd_tag_t));

    tag->key = cli_iskeyword(tag_str);
    cli_vector_init(&tag->items);
    
    str = tag_str;
    start = end = str;
    while(*str != '\0'){
        type = cli_get_char_type(*str);
        switch(type){
            case CLI_LEFT_BRACKET:
                if(state == CLI_P_S_INIT){
                    state = CLI_P_S_IN_BRACKET;
                    end = start = str + 1;
                    opt = TRUE;
                }else{ /* [ only exit at the start of line */
                    ret = CLI_E_FORMAT;
                    goto Err_Exit;
                }
                break;
            case CLI_RIGHT_BRACKET:
                if(state != CLI_P_S_IN_BRACKET){ /*Miss [*/
                    ret = CLI_E_FORMAT;
                    goto Err_Exit;
                }else{
                    if(start == str){ /*string like [] is error format*/
                        ret = CLI_E_FORMAT;
                        goto Err_Exit;
                    }

                    state = CLI_P_S_END;

                    if(start != end){
                        /* process the char */
                        tmp = cli_strndup(start,end-start + 1);
                        item = NULL;
                        ret = cli_parser_cmd_tag_item(&item,tmp,opt);
                        cli_strfree(tmp);
                        tmp = NULL;
                        if(ret == CLI_OK && item != NULL){
                            cli_vector_add(&tag->items,item);
                        }
                        start = end;
                        goto Exit;
                    }
                }
                break;

            case CLI_LEFT_PARENTHESE:
                state_stack[stack_head++] = state;
                state = CLI_P_S_IN_BRACKET;
                end = start = str + 1;
                break;
            case CLI_RIGHT_PARENTHESE:
                if(state == CLI_P_S_IN_BRACKET){
                    state = state_stack[--stack_head];
                    if(start != end){
                        tmp = cli_strndup(start,end-start + 1);
                        item = NULL;
                        ret = cli_parser_cmd_tag_item(&item,tmp,opt);
                        cli_strfree(tmp);
                        tmp = NULL;
                        if(ret == CLI_OK && item != NULL){
                            cli_vector_add(&tag->items,item);
                        }
                        start = end;
                    }
                }else{
                    ret = CLI_E_FORMAT;
                    goto Err_Exit;
                }
                break;
            case CLI_BAR:
                if(start != end){
                    tmp = cli_strndup(start,end-start + 1);
                    item = NULL;
                    ret = cli_parser_cmd_tag_item(&item,tmp,opt);
                    cli_strfree(tmp);
                    tmp = NULL;
                    if(ret == CLI_OK && item != NULL){
                        cli_vector_add(&tag->items,item);
                    }
                }
                start = end = str + 1;
                    
                break;
            case CLI_OTHER:
                end = str;
                if(state == CLI_P_S_INIT){
                    state = CLI_P_S_START;
                }
                break;
        }

        if(ret != CLI_OK){
            goto Err_Exit;
        }
        str ++;
    }
    
    if(state == CLI_P_S_IN_BRACKET 
            ||state == CLI_P_S_IN_PARENTHESE
            ){
        ret = CLI_E_FORMAT;
        goto Err_Exit;
    }

    /*process the last one it has*/
    if(start != end || state == CLI_P_S_START){
        tmp = cli_strndup(start,end-start + 1);
        item = NULL;
        ret = cli_parser_cmd_tag_item(&item,tmp,opt);
        cli_strfree(tmp);
        tmp = NULL;
        if(ret == CLI_OK && item != NULL){
            cli_vector_add(&tag->items,item);
        }
    }
    
    goto Exit;

Err_Exit:
    for(i = 0; i < cli_vector_count(&tag->items); i++){
        item = cli_vector_get(&tag->items,i);
        CLI_FREE(item);
    }

Exit:
    return ret;
}


cli_cmd_input_info_t * cli_cmd_input_info_new(
        const cli_int8 * line)
{
    cli_cmd_input_info_t * info = NULL; 
    cli_int8 * p = NULL;
    cli_int8 * tmp = NULL;


    CLI_ASSERT_RET(line != NULL, NULL);

    info = (cli_cmd_input_info_t *) CLI_MALLOC(sizeof(cli_cmd_input_info_t));

    if(info == NULL){
        return NULL;
    }
    
    memset(info,0x00,sizeof(cli_cmd_input_info_t));

    info->prefix = TRUE;        

    if(strlen(line) == 0 || isspace(line[strlen(line) - 1])){
        info->prefix = FALSE;        
    }

    info->num = 0;
    tmp = cli_strdup(line);
    p = strtok(tmp,CLI_TAG_SPLIT_TAG);
    while(p != NULL && info->num < CLI_MAX_INPUT_NUM){
        info->input[info->num] = cli_strdup(p);
        info->pos[info->num] = p - tmp;
        info->num ++; 

        p = strtok(NULL,CLI_TAG_SPLIT_TAG);
    }

    cli_strfree(tmp);
    tmp = NULL;
    return info;
}



void cli_cmd_input_info_delete(
        cli_cmd_input_info_t * info)
{
    cli_uint32 i = 0; 

    CLI_ASSERT(info != NULL);

    for(; i < info->num; i++){
        cli_strfree(info->input[i]);
        info->input[i] = NULL;
    }

    CLI_FREE(info);
}

cli_boolean cli_param_check_keyword(
        cli_int8 *format, 
        cli_int8 * value)
{
    if(strcmp(format, value) == 0){
        return TRUE;
    }
    return FALSE;
}


cli_boolean cli_param_check_number(
        cli_int8 *format, 
        cli_int8 * value)
{
    cli_int32 i = 0;
    
    if(value[0] == '0' 
            && (value[1] =='x' || value[1] == 'X')){
        for(i = 2; i < strlen(value); i++){
            if((value[i] >= '0' && value[i] <= '9')
                    ||(value[i] >= 'a' && value[i] <= 'f')
                    ||(value[i] >= 'A' && value[i] <= 'F')
                    ){
                continue;
            }else{
                return FALSE;
            }
        }

        return TRUE;
    }

    for(i = 0; i < strlen(value); i++){
        if(value[i] < '0' || value[i] > '9'){
            return FALSE;
        }
    }

    return TRUE;
}

cli_boolean cli_param_check_range(
        cli_int8 *format, 
        cli_int8 * value)
{
    cli_int8 * p = NULL;
    cli_boolean  ret = FALSE;
    cli_int64 min = 0, max = 0;
    cli_int8 * tmp = NULL; 
    cli_int64 v = 0;

    ret = cli_param_check_number(format,value);

    if(ret == FALSE){
        return ret;
    }
    
    ret = FALSE;

    tmp = cli_strdup(format + 1);
    tmp[strlen(tmp) -1 ] = '\0';

    p = strtok(tmp,"-");
    
    v = cli_str2num(value);

    if(p != NULL){
        
        min = cli_str2num(p);
        p = strtok(NULL,"-");
        if(p != NULL){
            max = cli_str2num(p);
            if(v >= min && v <= max){
                ret =  TRUE;
                goto Exit;
            }
        }
    }
Exit:
    cli_strfree(tmp);
    return ret;
}

cli_boolean cli_param_check_mac(
        cli_int8 *format, 
        cli_int8 * value)
{
    cli_int8 * tmp = value; 
    cli_int8 i = 0;
    cli_int8 s = 0;

    while(*tmp != '\0'){
        if(isxdigit(*tmp)){
            i++;
        }else if(*tmp == ':'){
           if(i == 0 || (i/2 -1) != s){
                return FALSE;
           }
           s++;
        }else{
            return FALSE;
        }
        tmp ++;
    }

    if(i == 12 && s == 5){
        return TRUE;
    }

    return FALSE;
}

cli_boolean cli_param_check_ipv4(
        cli_int8 *format, 
        cli_int8 * value)
{
    cli_int8 * tmp = value; 
    cli_int8 i = 0;
    cli_int8 s = 0;
    cli_boolean pre_digit = FALSE;
    cli_int8  ip[4] = { 0 };
    cli_uint32  pos = 0;

    while(*tmp != '\0'){
        if(isdigit(*tmp)){
            i++;
            if(pos > 3){
                return FALSE;
            }

            ip[pos] = *tmp;
            pos ++;
            pre_digit = TRUE;
        }else if(*tmp == '.' && pre_digit){
           if(i == 0 || *(tmp + 1) == '\0' ){
                return FALSE;
           }

           ip[pos] = '\0';
           
           if(atoi(ip) > 255){
                return FALSE;
           }

           pos = 0;
           
           s++;
           pre_digit = FALSE;
        }else{
            return FALSE;
        }
        tmp ++;
    }

    ip[pos] = '\0';

    if(atoi(ip) > 255){
        return FALSE;
    }

    if(s == 3){
        return TRUE;
    }

    return FALSE;
}

cli_boolean cli_param_check_ipv6(
        cli_int8 *format, 
        cli_int8 * str)
{
    cli_int8 state = STATE_START;
    cli_int32 colons = 0, nums = 0, double_colon = 0;
    cli_int8 *sp = NULL;


    if(strspn(str, IPV6_ADDR_STR) != strlen(str))
        return FALSE;

    while('\0' != *str)
    {
        switch(state)
        {
            case STATE_START:
                if(':' == *str)
                {
                    if(':' != *(str + 1) && '\0' != *(str + 1))
                        return FALSE;

                    colons--;
                    state = STATE_COLON;
                }
                else
                {
                    sp = str;
                    state = STATE_ADDR;
                }

                continue;
            case STATE_COLON:
                colons++;

                if(':' == *(str + 1))
                    state = STATE_DOUBLE;
                else
                {
                    sp = str + 1;
                    state = STATE_ADDR;
                }
                break;
            case STATE_DOUBLE:
                if(double_colon)
                    return FALSE;

                if(':' == *(str + 1))
                    return FALSE;
                else
                {
                    if('\0' != *(str + 1))
                        colons++;

                    sp = str + 1;
                    state = STATE_ADDR;
                }

                double_colon++;
                nums++;
                break;
            case STATE_ADDR:
                if(':' == *(str + 1) || '\0' == *(str + 1))
                {
                    if(str - sp > 3)
                        return FALSE;

                    nums++;
                    state = STATE_COLON;
                }

                if('.' == *(str + 1))
                    state = STATE_DOT;

                break;
            case STATE_DOT:
                state = STATE_ADDR;
                break;
            default:
                break;
        }

        if(nums > 8)
            return FALSE;

        if(colons > 7)
            return FALSE;

        str++;
    }

    return TRUE;

}

cli_param_check_handler_t  * g_param_check[] = \
{
    NULL,
    cli_param_check_keyword,
    NULL,
    cli_param_check_range,
    cli_param_check_mac,
    cli_param_check_ipv4,
    cli_param_check_ipv6,
};

cli_uint32 cli_get_char_count(cli_int8 * str,cli_int8 c)
{
    cli_int32 i ; 
    cli_uint32 cnt = 0; 
    for(i = 0; i < strlen(str); i++){
        if(str[i] == c){
            cnt ++;
        }
    }

    return cnt;
}


cli_boolean cli_check_param_valid(
        cli_uint8 type,
        cli_int8 * format,
        cli_int8 * value)
{
    cli_boolean  ret = TRUE;

    CLI_ASSERT_RET(
            format != NULL 
            && value != NULL, FALSE);

    if(g_param_check[type]!= NULL){
        ret = g_param_check[type](format,value);
    }

    return ret;
}

/*lint +e438*/
/*lint +e613*/
/*lint +e676*/
/*lint +e14*/
/*lint +e574*/
/*lint +e19*/
/*lint +e659*/
/*lint +e124*/


