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


#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include "cli_api.h"
#include "cli_server.h"
/*lint -e611*/
extern void cli_init_telnet_server(cli_server_t * server);
extern void cli_init_console_server(cli_server_t * server);
extern void cli_init_ecos_telnet_server (cli_server_t * server);

void cli_telnet_port_set(cli_int16 port);
#ifdef CLI_HAS_TELNETD
typedef struct{
    cli_int8   valid;
    cli_int32 socket; 
    FILE *    out_fd;
    cli_server_t * server;
}cli_telnet_ctrl_t;
#define CLI_TELNET_SESSION_NUM   8

cli_telnet_ctrl_t  g_telnet_ctrl[CLI_TELNET_SESSION_NUM];

#ifdef CLI_OS_LINUX
pthread_mutex_t g_telnet_lock;
#endif
FILE * g_cli_out = NULL;
void cli_bc_print(cli_int8 * fmt,...)
{
    va_list args;
    cli_int32 i = 0;

    va_start(args, fmt);
#ifdef CLI_OS_LINUX
    pthread_mutex_lock(&g_telnet_lock);
#endif
    for(i = 0; i < CLI_TELNET_SESSION_NUM;i++){
        if(g_telnet_ctrl[i].valid && g_telnet_ctrl[i].out_fd != NULL){
            vfprintf(g_telnet_ctrl[i].out_fd, fmt, args);
        }
    }
#ifdef CLI_OS_LINUX
    pthread_mutex_unlock(&g_telnet_lock);
#endif
    va_end(args);
}

void cli_telnetd_loop(void * data)
{
    cli_telnet_ctrl_t * t = (cli_telnet_ctrl_t*)data;
    FILE * in;
    FILE * out;

    CLI_ASSERT(data != NULL);

    if(CLI_OK == t->server->open_stream(t->socket,&in,&out)){
        t->out_fd = out;
        cli_main(CLI_SESSION_TELNET,in,out);
#ifdef CLI_OS_LINUX
        pthread_mutex_lock(&g_telnet_lock);
#endif
        t->out_fd = NULL;
        t->valid = 0;
        t->server->close_stream(t->socket,&in,&out);
#ifdef CLI_OS_LINUX
        pthread_mutex_unlock(&g_telnet_lock);
#endif
    }
    
}

void cli_telnetd()
{
    cli_server_t server;
    cli_int32 s = 0, x = 0;
    cli_int32 i = 0;
    cli_telnet_ctrl_t * t = NULL;
    
#ifdef CLI_OS_LINUX
    pthread_t thread_id;

    pthread_mutex_init(&g_telnet_lock, NULL);

#endif

    memset(g_telnet_ctrl,0x00,
            sizeof(cli_telnet_ctrl_t)*CLI_TELNET_SESSION_NUM);

    cli_init_telnet_server(&server);

    s = server.init();

    while((x = server.client_accept(s)) != 0){
        t = NULL;
#ifdef CLI_OS_LINUX
        pthread_mutex_lock(&g_telnet_lock);
#endif
        for(i = 0; i < CLI_TELNET_SESSION_NUM; i++){
            if(!g_telnet_ctrl[i].valid){
                t = &g_telnet_ctrl[i];
                break;
            }
        }

        if(t == NULL){
            printf("No more session available \n");
            close(x);
#ifdef CLI_OS_LINUX
            pthread_mutex_unlock(&g_telnet_lock);
#endif
            continue;
        }
        
        t->valid = 1;
        t->socket = x;
        t->server = &server;
#ifdef CLI_OS_LINUX
        pthread_mutex_unlock(&g_telnet_lock);
        pthread_create (&thread_id, NULL, (void *)*cli_telnetd_loop, (void *)t);
#endif
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
