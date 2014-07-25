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


#include "cli_api.h"
#include "cli_node.h"
#include "cli_shell.h"
#include "cli_auth.h"


/*lint -e14*/
/*lint -e516*/
/*lint -e515*/
/*lint -e64*/
/*lint -e18*/
CLI_DEFINE(cli_node_enter_enable,cli_cmd_enter_enable,"enable","Enable the config mode")
{
    cli_enter_node(cli,CLI_NODE_ID_ENABLE,"");

    return CLI_CMD_OK;
}

CLI_DEFINE(cli_node_enter_config,cli_cmd_enter_config,"config",
        "Enter config node\n"
        )
{
    cli_enter_node(cli,CLI_NODE_ID_CONFIG,"config");

    return CLI_CMD_OK;
}


cli_status_t cli_init(cli_uint32 max_node,cli_boolean auth)
{
    cli_status_t ret = CLI_CMD_OK;

    ret = cli_shell_ctrl_init(max_node);

    if(ret != CLI_OK){
        return ret;
    }

    /* add build in node add here  */
    cli_add_root_node(CLI_NODE_ID_VIEW);
    cli_add_node(CLI_NODE_ID_ENABLE);
    cli_add_node(CLI_NODE_ID_CONFIG);
   
    cli_auth_init(auth);

    /* build in node cmd add here */
    cli_install_cmd(CLI_NODE_ID_VIEW,&cli_cmd_enter_enable);
    cli_install_cmd(CLI_NODE_ID_ENABLE,&cli_cmd_enter_config);

    return CLI_CMD_OK;
}

/*lint +e14*/
/*lint +e516*/
/*lint +e515*/
/*lint +e64*/
/*lint +e18*/

