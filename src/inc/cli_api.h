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


#ifndef __CLI_API_H__
#define __CLI_API_H__
#include <stdio.h>
#include "cli_error.h"
typedef unsigned long long      cli_uint64;
typedef long long               cli_int64;
typedef unsigned int            cli_uint32;
typedef int       	            cli_int32;
typedef unsigned short          cli_uint16;
typedef short	                cli_int16;
typedef unsigned char       	cli_uint8;
typedef char	                cli_int8;
typedef unsigned char       	cli_boolean;
typedef double                 	cli_double;


typedef enum{
    CLI_OK = 0,
    CLI_E_FORMAT,
    CLI_E_NO_MEMORY,
    CLI_E_PARAM,
    CLI_E_NOT_FOUND,
    CLI_E_ERROR,
}cli_status_t;


typedef struct cli_shell_s cli_shell_t;

typedef cli_cmd_status_t cli_handle_func_t(
        cli_shell_t *,
        cli_uint32   argc,
        cli_int8 ** argv);

typedef struct{
    cli_handle_func_t  *handler;
    cli_int8            *cmd;
    cli_int8            *cmd_help;
}cli_shell_cmd_def_t;


#define CLI_DEFINE(func,cmd, cmd_str,cmd_desc) \
    cli_cmd_status_t func(cli_shell_t *,cli_uint32   argc,cli_int8 ** argv); \
    cli_shell_cmd_def_t  cmd = { \
        func, \
        cmd_str, \
        cmd_desc \
    };\
    cli_cmd_status_t func (cli_shell_t *cli,cli_uint32   argc,cli_int8 ** argv)


#define CLI_DEFINE_ALIAS(func,cmd,cmd_str,cmd_desc) \
    cli_cmd_status_t func(cli_shell_t *,cli_uint32   argc,cli_int8 ** argv); \
    cli_shell_cmd_def_t  cmd = { \
        func, \
        cmd_str, \
        cmd_desc \
    };\


typedef enum{
    CLI_NODE_ID_VIEW = 0,
    CLI_NODE_ID_ENABLE,
    CLI_NODE_ID_CONFIG,
    CLI_NODE_ID_USER_START
}cli_node_id_t;

typedef enum{
    CLI_PRI_INVALID,
    CLI_PRI_NORMAL,
    CLI_PRI_POWER,
    CLI_PRI_ADMIN,
    CLI_PRI_DEBUG,
}cli_privilege_t;

typedef enum{
    CLI_SESSION_CONSOLE,
    CLI_SESSION_TELNET
}cli_session_type_t;

void cli_main(
        cli_session_type_t type,
        FILE   *in,
        FILE   *out);

cli_int32 cli_print(
        cli_shell_t * cli,
        const cli_int8 *fmt,...);

cli_int32 cli_get_input(
        cli_shell_t * cli,
        cli_int8 * prompt,
        cli_int8 * buf, 
        cli_int32  max_len
        );

void cli_echo_enable(
    cli_shell_t * this,
    cli_boolean enable);

cli_status_t cli_install_cmd(
        cli_uint32 node_id,
        cli_shell_cmd_def_t *cmd_def);

cli_status_t cli_install_pri_cmd(
        cli_uint8 pri,
        cli_uint32 node_id,
        cli_shell_cmd_def_t *cmd_def);

cli_status_t cli_add_node(
        cli_uint32 node_id
        );

cli_status_t cli_enter_node(
        cli_shell_t * shell,
        cli_uint32 node_id,
        const cli_int8 * prompt_fmt,
        ...);

cli_status_t cli_exit_node(
        cli_shell_t * shell
        );

cli_status_t cli_context_set(
            cli_shell_t * shell,
            cli_uint32  ctx_index,
            cli_uint32  ctx);

cli_uint32  cli_context_get(
        cli_shell_t * shell,
        cli_uint32  ctx_index);

cli_status_t cli_init(cli_uint32 max_node,cli_boolean auth);

void cli_server_start(cli_boolean telnetd);
void cli_telnetd_start(cli_int16 port);

cli_cmd_status_t cli_user_add(
        cli_int8  perm,
        cli_int8 * name,
        cli_int8 * passwd);

cli_cmd_status_t cli_user_del(
        cli_int8 * name
        );

cli_cmd_status_t cli_user_passwd_modify(
        cli_int8 * name,
        cli_int8 * old_passwd,
        cli_int8 * new_passwd);

cli_cmd_status_t cli_user_passwd_reset(
        cli_int8 * name,
        cli_int8 * new_passwd);

cli_uint8  cli_get_user_pri(
        cli_shell_t * shell);


void cli_logo_set(
        cli_int8 * logo, 
        cli_uint32 len);

void cli_hostname_set(
        cli_int8 * hostname, 
        cli_uint32 len);

/* for session timer */
typedef void cli_timeout_handle_func_t(
        cli_shell_t * shell
        );

typedef void * cli_timer_add_func_t(
        cli_uint32 timeout,
        cli_shell_t * shell,
        cli_timeout_handle_func_t * hdr
        );

typedef void  cli_timer_del_func_t(
        void * timer
        );

cli_status_t  cli_session_timer_set(
        cli_timer_add_func_t * timer_add,
        cli_timer_del_func_t * timer_del);

cli_status_t cli_session_idle_time_set(
        cli_shell_t * shell,
        cli_uint32 timeout);

void cli_console_exit_disable();

/*
 * Global command API 
 */
#define CLI_NODE_ID_INVALID  0xffffffff
cli_status_t cli_global_cmd_node_range(
        cli_uint32 min_node,
        cli_uint32 max_node);

/* The parameter ends with CLI_NODE_ID_INVALID*/
cli_status_t cli_global_cmd_node_list(
        cli_uint32 node,
        ...);

cli_status_t cli_install_pri_global_cmd(
        cli_uint8  pri,
        cli_shell_cmd_def_t *cmd_def);

cli_status_t cli_install_global_cmd(
        cli_shell_cmd_def_t *cmd_def);
#endif

