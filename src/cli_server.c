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


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include "cli_api.h"
#include "cli_server.h"
/*lint -e611*/
extern void cli_init_telnet_server(cli_server_t * server);
extern void cli_init_console_server(cli_server_t * server);
extern void cli_init_ecos_telnet_server (cli_server_t * server);

static cli_boolean isecos = FALSE;
void cli_telnet_port_set(cli_int16 port);
#ifdef CLI_HAS_TELNETD
FILE * g_cli_out = NULL;
void cli_bc_print(cli_int8 * fmt,...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(g_cli_out, fmt, args);
    va_end(args);
}

void cli_telnetd()
{
    cli_server_t server;
    cli_int32 s = 0, x = 0;
    FILE * in;
    FILE * out;

    if(!isecos){
        cli_init_telnet_server(&server);
    }else{
        cli_init_ecos_telnet_server(&server);
    }

    s = server.init();

    while((x = server.client_accept(s)) != 0){
        if(CLI_OK == server.open_stream(x,&in,&out)){
            g_cli_out = out;
            cli_main(CLI_SESSION_TELNET,in,out);
            g_cli_out = NULL;
            server.close_stream(x,&in,&out);
        }
    }

}
#endif
void cli_console()
{
    cli_server_t server;
    cli_int32 s = 0, x = 0;

    FILE * in;
    FILE * out;

    cli_init_console_server(&server);

    s = server.init();
    
    x = server.client_accept(s);

    server.open_stream(x,&in,&out);
    /*use the os to create thread*/
    cli_main(CLI_SESSION_CONSOLE,in,out);
    server.close_stream(x,&in,&out);
}


void cli_server_start(cli_boolean telnetd)
{
#ifdef CLI_OS_LINUX
    pthread_t thread_id;
#endif
#ifdef CLI_HAS_TELNETD
#ifdef CLI_OS_LINUX
    pthread_t tel_thread_id = 0;
#endif
#endif


#ifdef CLI_HAS_TELNETD
    if(telnetd){
#ifdef CLI_OS_LINUX
        /*For other OS, please replace the thread function*/
        pthread_create (&tel_thread_id, NULL, (void *)*cli_telnetd, NULL);
#endif
    }
#endif

#ifdef CLI_OS_LINUX
    pthread_create (&thread_id, NULL, (void *)*cli_console, NULL);
#endif

#ifdef CLI_HAS_TELNETD
    if(telnetd){
#ifdef CLI_OS_LINUX
        pthread_join(tel_thread_id, NULL);
#endif
    }
#endif

#ifdef CLI_OS_LINUX
    pthread_join(thread_id, NULL);
#endif
}

#ifdef CLI_HAS_TELNETD
void cli_telnetd_start(cli_int16 port)
{
#ifdef CLI_OS_LINUX
    pthread_t tel_thread_id = 0;
    cli_telnet_port_set(port);
    pthread_create (&tel_thread_id, NULL, (void *)*cli_telnetd, NULL);
    pthread_join(tel_thread_id, NULL);
#endif
}
#endif
/*lint +e611*/
