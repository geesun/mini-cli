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


#ifndef __CLI_SERVER_H__
#define __CLI_SERVER_H__
#include "cli_utils.h"

typedef  cli_int32 cli_init_t();
typedef  cli_status_t cli_client_open_t(cli_int32 fd, FILE ** in,  FILE ** out);
typedef  void cli_client_close_t(cli_int32 fd, FILE ** in,  FILE ** out);
typedef  cli_int32 cli_accept_client_t(cli_int32 server);

typedef struct{
    cli_init_t * init;
    cli_client_open_t   * open_stream;
    cli_client_close_t* close_stream;
    cli_accept_client_t  * client_accept;
}cli_server_t;

#endif
