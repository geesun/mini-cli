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


#ifndef __CLI_KMOD_MSG_H__
#define __CLI_KMOD_MSG_H__

#define CLI_CMD_NAME_MAX_LEN   128
#define CLI_CMD_MAX_NUM        2000
#define CLI_CMD_MAX_ARG_NUM    15
#define CLI_CMD_MAX_ARG_LEN    32
#define CLI_CMD_BUFF_LEN       4096

typedef struct{
   cli_int32     ret;
   cli_int8      ret_buf[CLI_CMD_BUFF_LEN];
   cli_int8      cmd_name[CLI_CMD_NAME_MAX_LEN];
   cli_uint32    argc;
   cli_int8      argv[CLI_CMD_MAX_ARG_NUM][CLI_CMD_MAX_ARG_LEN];
}cli_cmd_msg_t;


#define CLI_IOCTL_CMD        1

#define CLI_IOCTL_NAME      "mini-cli-dev"
#define CLI_IOCTL_MAGIC     'C'



#endif
