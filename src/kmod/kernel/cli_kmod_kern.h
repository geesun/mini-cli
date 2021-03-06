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


#ifndef __CLI_KMOD_KERN_H__
#define __CLI_KMOD_KERN_H__
#include "cli_cmn.h"
#include "cli_kmod_msg.h"

typedef struct{
    cli_int8        cmd_name [CLI_CMD_NAME_MAX_LEN];
    cli_k_func_t * handler;
}cli_kern_cmd_map_t;

#endif

