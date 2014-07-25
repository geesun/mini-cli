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


#ifndef __CLI_READ_LINE_H__
#define __CLI_READ_LINE_H__
#include "cli_utils.h"
#include "cli_tty.h"
#include "cli_history.h"

typedef struct cli_rl_s  cli_rl_t;

typedef cli_status_t cli_rl_key_func_t(
        cli_rl_t *instance,
        cli_int32    key);

#define CLI_RL_NUM_HANDLERS     256
struct cli_rl_s{
    const cli_int8      *line; 
    cli_uint32           max_line_length; /* 0 for no limit */
    
    const cli_int8      *prompt;
    cli_uint32           prompt_size;
    
    cli_int8            *buffer;
    cli_uint32           buffer_size;

    cli_int8            *screen_buffer;
    cli_uint32           screen_point;

    cli_uint32           point;
    cli_uint32           end;

    cli_boolean          done;
    cli_boolean          echo_enabled;
    cli_int8             echo_char;
    cli_boolean          session_end;
    /*cli_boolean          isatty;*/

    void                *context;

    cli_int8             *kill_str; /* for kill and yank */

    cli_rl_key_func_t   *handlers[CLI_RL_NUM_HANDLERS];

    cli_rl_key_func_t   *scanf_handlers[CLI_RL_NUM_HANDLERS];

    cli_tty_t           *term;
    cli_history_t       *history;
    cli_histroy_iter_t  hist_iter;
    /*struct termios       default_termios;*/
}; 

#define CLI_RL_MAX_MATCH_NUM  512
cli_status_t cli_rl_expand_buffer_size(
        cli_rl_t * this,
        cli_uint32 len);

void cli_rl_change_buffer(cli_rl_t *this);

cli_status_t cli_rl_insert_text(
        cli_rl_t *this,
        cli_int8  *text);

cli_status_t cli_rl_delete_text(
        cli_rl_t *this,
        cli_uint32 start,
        cli_uint32 end);

void cli_rl_echo_printf(
        cli_rl_t * this, 
        const cli_int8 * text);

cli_int32 cli_rl_printf(
        cli_rl_t * this, 
        const cli_int8 *fmt,
        va_list args);

void cli_rl_display_finish(cli_rl_t * this);

void cli_rl_move_cursor(
        cli_rl_t * this
        );

void cli_rl_display(cli_rl_t *  this);

void cli_rl_reset_line_state(
        cli_rl_t * this);

void cli_rl_readline_from_tty(
     cli_rl_t * this,
     cli_boolean isscanf);

void cli_rl_crlf(cli_rl_t *this);

cli_int32 cli_rl_get_input(
        cli_rl_t * this,
        const cli_int8* prompt,
        cli_int8 *buf,
        cli_int32 max_len);

cli_int8 * cli_rl_readline(
        cli_rl_t * this,
        void     * context,
        const cli_int8* prompt);

cli_rl_t * cli_rl_new(
           FILE   *instream,
           FILE   *outstream);

void cli_rl_delete(cli_rl_t * this);

cli_status_t  cli_rl_bind_key(
    cli_rl_t * this,
    cli_int32  key,
    cli_rl_key_func_t * func);
    

void cli_rl_echo_enable(
        cli_rl_t *  this,
        cli_boolean enable);
#endif
