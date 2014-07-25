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
#include "cli_api.h"
#include "cli_utils.h"
#include "cli_readline.h"
#include "cli_history.h"
#include "cli_shell.h"
/*lint -e438*/
CLI_DEFINE(cli_node_exit,cli_cmd_node_exit,"exit","Exit this node")
{
    cli_exit_node(cli);
    return CLI_CMD_OK;
}


CLI_DEFINE(cli_node_end,cli_cmd_node_end,"end","End to top node")
{
    while(CLI_OK == cli_exit_node(cli));

    return CLI_CMD_OK;
}

CLI_DEFINE(cli_show_history,cli_cmd_history,"history","Show command history")
{
    cli_history_t * history  = cli->readline->history;
    cli_histroy_iter_t iter;
    cli_history_item_t *item; 
    cli_uint32  i = 0;

    for(item = cli_history_getfirst(history,&iter); 
            item != NULL; 
            item = cli_history_getnext(&iter)){
        i++;
        cli_print(cli,"  %d.\t%s \n",i,item->line); 
    }

    return CLI_CMD_OK;
}

CLI_DEFINE(cli_show_cmd_list,cli_cmd_list,"list","Show command list")
{
    cli_int8 * str; 

    str = cli_node_dump_str(cli->node_id_list[cli->idx]);

    cli_print(cli,"%s\n",str);

    cli_strfree(str); 
    str = NULL;

    return CLI_CMD_OK;
}


CLI_DEFINE(cli_cmd_logout,cli_cmd_cli_logout,"logout","Exit this CLI session")
{
#ifndef CLI_OS_LINUX
    /*in serial port, no logout support*/
    if(cli->readline->term->istream != stdin){
#endif
        cli->state = CLI_SHELL_STATE_CLOSING;
#ifndef CLI_OS_LINUX
    }
#endif
    return CLI_CMD_OK;
}

void cli_install_root_builtin_cmd(cli_uint32 node_id)
{
     cli_install_cmd(node_id,&cli_cmd_history);
     cli_install_cmd(node_id,&cli_cmd_list);
     cli_install_cmd(node_id,&cli_cmd_cli_logout);
}

void cli_install_builtin_cmd(cli_uint32 node_id)
{
     cli_install_cmd(node_id,&cli_cmd_node_exit);
     cli_install_cmd(node_id,&cli_cmd_node_end);
     cli_install_cmd(node_id,&cli_cmd_history);
     cli_install_cmd(node_id,&cli_cmd_list);
     cli_install_cmd(node_id,&cli_cmd_cli_logout);
}
/*lint +e438*/

