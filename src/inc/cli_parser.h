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


#ifndef __CLI_PARSER_H__
#define __CLI_PARSER_H__
#include "cli_utils.h"
#include "cli_vector.h"

typedef enum{
    CLI_LEFT_BRACKET,
    CLI_RIGHT_BRACKET,
    CLI_LEFT_PARENTHESE,
    CLI_RIGHT_PARENTHESE,
    CLI_BAR,
    CLI_OTHER
}cli_char_type_e;


typedef enum{
    CLI_P_S_INIT,
    CLI_P_S_START,
    CLI_P_S_IN_BRACKET,
    CLI_P_S_IN_PARENTHESE,
    CLI_P_S_END,
}cli_parser_state_e;

#define CLI_MAX_PARENTHESE_LOOP  20

#define CLI_MAX_TAG_NUM     10
#define CLI_MAX_PARAM_NUM   10

#define CLI_TAG_SPLIT_TAG   " "
#define CLI_HELP_SPLIT_TAG   "\n"

#define CLI_MAX_INPUT_NUM   128

    
#define IPV6_ADDR_STR   "0123456789abcdefABCDEF:.%"

#define STATE_START     1
#define STATE_COLON     2
#define STATE_DOUBLE    3
#define STATE_ADDR      4
#define STATE_DOT       5



typedef struct {
    cli_int8     *input[CLI_MAX_INPUT_NUM];
    cli_uint16     pos[CLI_MAX_INPUT_NUM];  /* pos in the line */
    cli_uint32     num;
    cli_boolean   prefix;
}cli_cmd_input_info_t;


cli_cmd_input_info_t * cli_cmd_input_info_new(
        const cli_int8 * line);

void cli_cmd_input_info_delete(
        cli_cmd_input_info_t * info);

cli_boolean cli_check_param_valid(
        cli_uint8 type,
        cli_int8 * format,
        cli_int8 * value);

typedef cli_boolean cli_param_check_handler_t(
        cli_int8 * format,
        cli_int8 * value);


void cli_split_string_with_token(
        cli_vector_t * vec,
        cli_int8 * string,
        cli_int8 * tok);

void cli_free_str_vector(
        cli_vector_t * v);


#endif
