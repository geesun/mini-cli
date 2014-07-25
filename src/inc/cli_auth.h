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


#ifndef __CLI_AUTH_H__
#define __CLI_AUTH_H__
#include "cli_utils.h"

#define CLI_MAX_USER_NUM  10

#define CLI_MAX_USER_NAME_LEN  32
#define CLI_MAX_USER_PASSWD_LEN  32

typedef struct{
    cli_int8    perm;
    cli_int8    valid;
    cli_int8    user[CLI_MAX_USER_NAME_LEN + 1];
    cli_int8    passwd[CLI_MAX_USER_PASSWD_LEN + 1];
}cli_user_t;

cli_boolean cli_user_login(
        cli_shell_t * shell,
        cli_int32 retry_time);

void cli_auth_init(cli_boolean auth);

cli_boolean cli_user_valid(
        cli_int8 * name,
        cli_int8 * passwd);
#endif

