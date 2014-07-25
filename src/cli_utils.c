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


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "cli_api.h"
#include "cli_utils.h"

/*lint -e438*/
/*lint -e574*/
cli_int8 * cli_strtrim(cli_int8 * str)
{
    cli_int32 i = 0;
    cli_int8 * ret = str;
    cli_int32  empty = 0;

    if(str == NULL){
        return str;
    }

    while(str[i] == ' '
            || str[i] == '\t'
            || str[i] == '\0'){
        i++;
        ret = str + i;
    }
    empty = i;

    i = strlen(str) - 1;

    while(i >= 0
            && (str[i] == ' '
            || str[i] == '\t')){
        str[i] = '\0';
        i--;
    }

    i = 0;
    while(i < (strlen(ret))){
        str[i] = str[i + empty];
        i++;
    }
    str[i] = '\0';

    return str;
}


void cli_strncat(
        cli_int8     **string,
        const cli_int8   *text,
        cli_uint32  len)
{
    cli_int8 *q = NULL;
    cli_uint32 length , initlen , textlen; 
    
    if(text == NULL){
        return;
    }

    textlen = strlen(text);
    /* make sure the client cannot give us duff details */
    len = (len < textlen) ? len : textlen;

    /* remember the size of the original string */
    initlen = *string != NULL ? strlen(*string) : 0;


    /* account for '\0' */
    length = initlen + len + 1;

    /* allocate the memory for the result */
    q = CLI_REALLOC(*string,length);
    if(NULL != q){
        *string = q;
        /* move to the end of the initial string */
        q += initlen;

        while(len--){
            *q++ = *text++;
        }
        *q = '\0';
    }
}

void cli_strcat(
        cli_int8 **string,
        const cli_int8*text)
{
	cli_uint32 len = text ? strlen(text) : 0;
	cli_strncat(string,text,len);
}

cli_int8 *cli_strdup(const cli_int8 *string)
{
	cli_int8 *result = NULL;
	if(NULL != string){
		cli_strcat(&result,string);
	}
	return result;
}


cli_int8 * cli_strndup(const cli_int8 *string, cli_uint32 length)
{
    char *result=NULL;
    if(NULL != string){
        result = CLI_MALLOC(length+1);
        if(NULL != result){
            strncpy(result,string,length);
            result[length] = '\0';
        }
    }
    return result;
}

cli_int8 * cli_strrealloc(cli_int8 *str, cli_uint32 new_len)
{
    return CLI_REALLOC(str,new_len);
}

void cli_strfree(cli_int8 * string)
{
    CLI_FREE(string);
}

cli_boolean cli_isupper(cli_int8 * str)
{
    cli_int32 i = 0; 
    
    if(str == NULL){
        return FALSE;
    }

    for(i = 0; i < strlen(str); i++){
        if(isupper(str[i])){
            continue;
        }

        if(str[i] == '_' || str[i] == '-'){
            continue;
        }

        return FALSE;
    }

    return TRUE;
}

cli_boolean cli_iskeyword(
        cli_int8 * str)
{
    cli_int32 i = 0; 
    
    if(str == NULL){
        return FALSE;
    }

    for(i = 0; i < strlen(str); i++){
        if(islower(str[i])){
            continue;
        }

        if(isdigit(str[i])){
            continue;
        }

        if(str[i] == '_' || str[i] == '-'){
            continue;
        }
        
        return FALSE;
    }

    return TRUE;
}


cli_boolean cli_is_digital(
        cli_int8 * str)
{
    cli_int32 i = 0;
    cli_boolean hex = FALSE;
    
    if(str == NULL){
        return FALSE;
    }

    if(strlen(str) > 2 
        && str[0] == '0' 
        && (str[1] == 'x' || str[1] == 'X')){
        i = 2;
        hex = TRUE;
    }
    
    for(; i < strlen(str); i++){
        if(hex && isxdigit(str[i])){
            continue;
        }
        
        if(isdigit(str[i])){
            continue;
        }

        return FALSE;
    }

    return TRUE;
}

cli_int64 cli_str2num(cli_int8 * str)
{
    cli_int64 v = 0; 

    if(str == NULL){
        return 0;
    }

    sscanf(str,"%llu",&v);

    if(strlen(str) > 2){
        if(str[0] == '0' 
                && (str[1] == 'x' || str[1] == 'X')){
            sscanf(str,"%llx",&v);
        }
    }
    
    return v;
}

/*lint +e438*/
/*lint +e574*/

