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


/*lint -e451*/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#ifdef CLI_OS_LINUX
#include <termios.h>
#endif
#include "cli_api.h"
#include "cli_server.h"

#ifdef CLI_OS_LINUX

struct termios       default_termios;

static void cli_console_set_raw_mode(FILE  * in )
{
    struct termios new_termios;
    cli_int32    status;
    cli_int32 fd = fileno(in);

    status = tcgetattr(fd,&default_termios);
    if(-1 != status)
    {
        status = tcgetattr(fd,&new_termios);
        if(-1 != status){
            new_termios.c_iflag     = 0;
            new_termios.c_oflag     = OPOST | ONLCR;
            new_termios.c_lflag     = 0;
            new_termios.c_cc[VMIN]  = 1;
            new_termios.c_cc[VTIME] = 0;
            /* Do the mode switch */
            tcsetattr(fd,TCSAFLUSH,&new_termios); 
        }
    }
}

static void cli_console_restore_mode(FILE  * in )
{
    cli_int32 fd = fileno(in);
    /* Do the mode switch */
    tcsetattr(fd,TCSAFLUSH,&default_termios);  
}
#endif

cli_int32  cli_console_init()
{
    return 0;
}

/* currently,we only support one console
 * If we need more than one console, 
 * let platform guys implement this func
 * */
cli_int32 cli_console_accept(cli_int32 s)
{
    static cli_int32 x = 0;
    x++;
    if(x == 1){
        return x;
    }
    return 0;
}

cli_status_t cli_console_open(cli_int32 fd, FILE ** in,  FILE ** out)
{
    CLI_ASSERT_RET(in != NULL && out != NULL,CLI_E_PARAM);

    *in = stdin;
    *out = stdout;
#ifdef CLI_OS_LINUX
    cli_console_set_raw_mode(*in);
#endif
    return CLI_OK;
}

void cli_console_close(cli_int32 fd, FILE ** in,  FILE ** out)
{
#ifdef CLI_OS_LINUX
    cli_console_restore_mode(*in);
#endif
}


void cli_init_console_server(cli_server_t * server)
{
    CLI_ASSERT(server != NULL);

    server->init = cli_console_init;
    server->open_stream = cli_console_open;
    server->close_stream = cli_console_close;
    server->client_accept = cli_console_accept;
}
/*lint +e451*/

