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


#ifndef __CLI_ERROR_H__
#define __CLI_ERROR_H__

#define CLI_ERROR_DEF \
    xx(CLI_CMD_OK,              0,  "") \
    xx(CLI_CMD_E_PARAM,             1,  "Invalid parameter") \
    xx(CLI_CMD_E_AMBIGUOUS,         2,  "Ambiguous command") \
    xx(CLI_CMD_E_CMD_NOT_COMPLETE,  3,  "Command not complete") \
    xx(CLI_CMD_E_BAD_COMMAND,       4,  "Bad command") \
    xx(CLI_CMD_E_CMD_NOT_FOUND,     5,  "Command not found") \
    xx(CLI_CMD_E_NO_RESOURCE,       6,  "No resource") \
    xx(CLI_CMD_E_IOCTL ,            7,  "CLI ioctl error") \
    xx(CLI_CMD_E_NO_KERNEL_HDL ,    8,  "Not found handle in kernel") \
    xx(CLI_CMD_E_USER_EXIST,        9,  "User already exist") \
    xx(CLI_CMD_E_USER_NOT_EXIST,    10, "User not exist") \
    xx(CLI_CMD_E_USER_TOO_MANY,     11, "User num reach the maximum") \
    xx(CLI_CMD_E_USER_WRONG_PASSWD, 12, "User password is wrong") \
    xx(CLI_CMD_E_USER_NEW_PASSWD_NOT_MATCH, 13, "User new password and confirm password not matched") \
    xx(CLI_CMD_E_MAX,               14, "") \

typedef enum{
#undef xx
#define xx(ID,VALUE,ERROR) ID = VALUE,
    CLI_ERROR_DEF
}cli_cmd_status_t;

char * cli_get_error_str(cli_cmd_status_t error_code);
#endif
