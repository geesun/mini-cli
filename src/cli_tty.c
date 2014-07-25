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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include "cli_api.h"
#include "cli_utils.h"
#include "cli_tty.h"

/*lint -e40*/
/*lint -e10*/
/*lint -e650*/
/*lint -e438*/
/*lint -e158*/
/*lint -e686*/
/*lint -e526*/
/*lint -e628*/

/* This table maps the vt100 escape codes to an enumeration */
/* we can refer http://ascii-table.com/ansi-escape-sequences-vt-100.php */
static cli_tty_decode_t cli_tty_cmd[] = 
{
    {'A',    CLI_TTY_CURSOR_UP},
    {'B',    CLI_TTY_CURSOR_DOWN},
    {'C',    CLI_TTY_CURSOR_RIGHT},
    {'D',    CLI_TTY_CURSOR_LEFT},
};


cli_tty_escape_e cli_tty_escape_decode(
        cli_tty_t * tty)
{
    cli_tty_escape_e result = CLI_TTY_UNKNOW;
    cli_uint32 c , i = 0; 

    CLI_ASSERT_RET(tty != NULL,CLI_TTY_UNKNOW);
    /*
     * ANSI escape sequence is a sequence of ASCII characters, 
     * the first two of which are the ASCII "Escape" character 27 (1Bh)
     * and the left-bracket character " [ " (5Bh). 
     * The character or characters following the escape 
     * and left-bracket characters specify an alphanumeric code 
     * that controls a keyboard or display function.
     */
    c = cli_tty_getchar(tty);
    if(c == EOF || c != '[' ){
        return CLI_TTY_UNKNOW;
    }

    c = cli_tty_getchar(tty);
    if(c == EOF || c < 'A'  || c > 'D'){
        return CLI_TTY_UNKNOW;
    }

    for(i = 0; i < sizeof(cli_tty_cmd)/sizeof(cli_tty_cmd[0]); i++){
        if(c == cli_tty_cmd[i].terminator){
            result = cli_tty_cmd[i].code;
            break;
        }
    }

    return result;
}

cli_tty_t * cli_tty_new(
        FILE *istream,
        FILE *ostream)
{
    cli_tty_t * tty = NULL;

    CLI_ASSERT_RET(
            (istream != NULL && ostream != NULL),
            NULL);

    tty = CLI_MALLOC(sizeof(cli_tty_t));

    if(tty != NULL){
        tty->istream = istream;
        tty->ostream = ostream;
    }

    return tty; 
}

void cli_tty_delete(cli_tty_t * tty)
{
    CLI_ASSERT(tty != NULL);

    CLI_FREE(tty);
}



cli_uint32 cli_tty_vprintf(
        const cli_tty_t *tty,
        const cli_int8  *fmt,
        va_list   args)
{
    cli_uint32 len = 0;
    cli_int8 * format = NULL;
    cli_int8 * next = NULL;
    const cli_int8 * p = fmt;
    cli_int8   new_line = 0;

    do{
        next = strchr(p,'\n');
        new_line = 0;
        if(next != NULL){
            
            if(next > p && *(next - 1) == '\r'){
                new_line = 1;
            }

            if(format == NULL){
                if(next != p){
                    format = cli_strndup(p,next - p);
                }
            }else{
                if(next != p){
                    cli_strncat(&format,p,next - p);
                }
            }
            p = next + 1;
            if(new_line == 1){
                cli_strcat(&format,"\n");
            }else{
                cli_strcat(&format,"\r\n");
            }
        }else{
            if(format == NULL){
                format = cli_strdup(p);
            }else{
                cli_strcat(&format,p);
            }
            break;
        }
    }while(p != NULL);
    
    if(format != NULL){
        len =  CLI_VFPINTF(tty->ostream, format, args);
        cli_strfree(format);
    }
    format = NULL;
    return len;
}

cli_uint32 cli_tty_printf(
        const cli_tty_t *tty,
        const cli_int8 *fmt,
        ...)
{
    va_list args;
    cli_uint32 len;

    va_start(args, fmt);
    len = cli_tty_vprintf(tty, fmt, args);
    va_end(args);
    cli_tty_oflush(tty);
    return len;
}

cli_uint32 cli_tty_getchar(const cli_tty_t *tty)
{
    return CLI_GETC(tty->istream);
}

void cli_tty_cursor_forward(
        const cli_tty_t *tty,
        cli_uint32  count)
{
    cli_tty_printf(tty,"%c[%dC",KEY_ESC,count);
}
void cli_tty_cursor_back(
        const cli_tty_t *tty,
        cli_uint32  count)
{
    cli_tty_printf(tty,"%c[%dD",KEY_ESC,count);
}
void cli_tty_cursor_up(
        const cli_tty_t *tty,
        cli_uint32  count)
{
    cli_tty_printf(tty,"%c[%dA",KEY_ESC,count);
}
void cli_tty_cursor_down(
        const cli_tty_t *tty,
        cli_uint32  count)
{
    cli_tty_printf(tty,"%c[%dB",KEY_ESC,count);
}
void cli_tty_cursor_home(const cli_tty_t *tty)
{
    cli_tty_printf(tty,"%c[H",KEY_ESC);
}
void cli_tty_erase(
        const cli_tty_t *tty,
        cli_uint32   count)
{
    cli_tty_printf(tty,"%c[%dP",KEY_ESC,count);
}

void cli_tty_oflush(const cli_tty_t * tty)
{
    fflush(tty->ostream);
}

void cli_tty_ding(const cli_tty_t *tty)
{
    cli_tty_printf(tty,"%c",KEY_BEL);
    cli_tty_oflush(tty);
}
void cli_tty_clear_screen(const cli_tty_t *tty)
{
    cli_tty_printf(tty,"%c[2J",KEY_ESC);
}

void cli_tty_set_istream(
        cli_tty_t *tty,
        FILE   *istream)
{
    tty->istream = istream;
}
FILE * cli_tty_get_istream(const cli_tty_t *tty)
{
    return tty->istream;
}
FILE * cli_tty_get_ostream(const cli_tty_t *tty)
{
    return tty->ostream;
}

/*lint +e40*/
/*lint +e10*/
/*lint +e650*/
/*lint +e438*/
/*lint +e158*/
/*lint +e451*/
/*lint +e686*/
/*lint +e526*/
/*lint +e628*/
