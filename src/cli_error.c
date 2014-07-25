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

static cli_int8 * g_error_str[] = 
{
#undef xx
#define xx(ERR_CODE,VALUE,ERR_STR) ERR_STR,
    CLI_ERROR_DEF
};


cli_int8 * cli_get_error_str(cli_cmd_status_t error_code)
{
    if(error_code > CLI_CMD_E_MAX){
        return "";
    }

    return g_error_str[error_code];
}
