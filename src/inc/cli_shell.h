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


#ifndef __CLI_SHELL_H__
#define __CLI_SHELL_H__
#include "cli_utils.h"
#include "cli_api.h"
#include "cli_node.h"
#include "cli_readline.h"

#define CLI_NODE_DEEP_MAX  20 
#define CLI_MAX_PROMPT_LEN      256

#define CLI_SHELL_MAX_CMD_ARGC  32

#define CLI_MAX_CONTEXT_NUM 8

#define CLI_DEFAULT_HOST   "MINI-CLI"

typedef enum
{
    CLI_SHELL_STATE_INITIALISING,
    CLI_SHELL_STATE_READY,
    CLI_SHELL_STATE_LOGIN_FAILED,
    CLI_SHELL_STATE_CLOSING
} shell_state_t;

struct cli_shell_s{
    cli_uint8     state;
    cli_uint8     user_pri;
    cli_int8    * shell_prompt;
    cli_rl_t    * readline;
    cli_int8    * host_name;
    cli_uint32    idx;
    cli_uint32    node_id_list[CLI_NODE_DEEP_MAX];
    cli_int8    * prompts[CLI_NODE_DEEP_MAX];
    cli_uint32    contexts[CLI_MAX_CONTEXT_NUM];
    void        * timer;
    cli_uint8     is_timeout;
};

typedef enum{
    CLI_SHELL_COMPLETE_NONE,  
    CLI_SHELL_COMPLETE_WORD, 
    CLI_SHELL_COMPLETE_PREFIX,
    CLI_SHELL_COMPLETE_PARAM,
}cli_shell_match_type_t;


typedef struct{
    cli_uint32 timeout;
    cli_timer_add_func_t  *timer_add;
    cli_timer_del_func_t  *timer_del;
}cli_shell_timer_t;

typedef struct{
    cli_uint32            root_node_id;
    cli_int8            * cli_logo;
    cli_int8            * cli_host;
    cli_shell_timer_t     timer;
    cli_uint32            shell_exit;
    cli_uint32            max_node;
    cli_uint32            *global_nodes;
}cli_shell_ctrl_t;

cli_shell_t * cli_shell_new(
        FILE   *instream,
        FILE   *outstream);

void cli_shell_delete(cli_shell_t * shell);

void cli_install_builtin_cmd(cli_uint32 node_id);
void cli_install_root_builtin_cmd(cli_uint32 node_id);
void cli_set_prompt(cli_shell_t * shell,cli_int8* prompt);

cli_status_t cli_add_root_node(
        cli_uint32 node_id
        );

cli_boolean cli_shell_check_cmd_line(
     cli_rl_t * this);


void cli_shell_set_user_pri(
        cli_shell_t * shell,
        cli_uint8 pri);

void cli_session_timeout_restart(
        cli_shell_t * this
        );

void cli_session_timeout(
        cli_shell_t * this
        );


cli_status_t cli_shell_ctrl_init(cli_uint32 max_node);
#endif
