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
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cli_cmn.h"
#include "cli_kmod_msg.h"
#ifdef HAVE_KERNEL_CLI
/*lint -e64*/
cli_int32 cli_cmd_ioctl(cli_shell_t *cli,cli_int8 * cmd ,cli_uint32 argc, cli_int8 **argv)
{
    cli_uint32 i = 0; 
    cli_cmd_msg_t msg;
    cli_int32 ioctl_fd;
    cli_int8 dev_name[32];
    cli_int32 ret = 0;

    memset(&msg, 0 , sizeof(msg));

    strcpy(msg.cmd_name,cmd);
    msg.argc = argc; 

    for(i = 0; i < argc; i++){
        strncpy(msg.argv[i],argv[i],CLI_CMD_MAX_ARG_LEN);
    }

    memset(dev_name , 0 , sizeof(dev_name));
    sprintf(dev_name, "/dev/%s", CLI_IOCTL_NAME);
    ioctl_fd = open(dev_name, O_RDWR);
    if (ioctl_fd != -1) {
        ret = ioctl(ioctl_fd, CLI_IOCTL_CMD, &msg);
        if(ret == 0){
            close(ioctl_fd);
            cli_print(cli,"%s",msg.ret_buf);
            return msg.ret;
        }
    }

    return CLI_CMD_E_IOCTL;
}

#endif
/*lint +e64*/

