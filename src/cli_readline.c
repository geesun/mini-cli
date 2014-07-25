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
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#include "cli_api.h"
#include "cli_utils.h"
#include "cli_tty.h"
#include "cli_history.h"
#include "cli_readline.h"

/*lint -e49*/
/*lint -e10*/
/*lint -e40*/
/*lint -e438*/
/*lint -e650*/
#if 0
static void cli_rl_tty_set_raw_mode(cli_rl_t *this)
{
    struct termios new_termios;
    cli_int32    fd = fileno(cli_tty_get_istream(this->term));
    cli_int32    status;

    status = tcgetattr(fd,&this->default_termios);
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

static void cli_rl_tty_restore_mode(const cli_rl_t *this)
{
    cli_int32 fd = fileno(cli_tty_get_istream(this->term));
    /* Do the mode switch */
    tcsetattr(fd,TCSAFLUSH,&this->default_termios);  
}
#endif

cli_status_t cli_rl_expand_buffer_size(
        cli_rl_t * this,
        cli_uint32 len)
{
    cli_uint32   new_len = len; 
    cli_int8    *new_buffer = NULL;

#define EXPAND_SIZE   10 

    if(this->buffer_size >= len){
        return CLI_OK;
    }

    /* no limit */
    if(this->max_line_length == 0){
        if(new_len < this->buffer_size + EXPAND_SIZE){
            new_len = this->buffer_size + EXPAND_SIZE;
        }
    }else if(new_len < this->max_line_length){
        new_len = this->max_line_length ;
    }else{
        return CLI_E_ERROR;
    }

    new_buffer = cli_strrealloc(this->buffer,new_len + 1);

    if(new_buffer != NULL){
        this->line = this->buffer = new_buffer;
        this->buffer_size = new_len;
        return CLI_OK;
    }

    return CLI_E_ERROR;

}

void cli_rl_change_buffer(cli_rl_t *this)
{
    if(this->line != this->buffer){
        cli_strfree(this->buffer);
        this->line = this->buffer = cli_strdup(this->line);
        this->buffer_size = strlen(this->buffer);
    }
}

cli_status_t cli_rl_insert_text(
        cli_rl_t *this,
        cli_int8  *text)
{
    cli_uint32 len = strlen(text);
    cli_uint32 new_len = len + this->end;
    cli_status_t ret = CLI_OK;
    
    cli_rl_change_buffer(this);

    /*resize the buffer*/
    if(new_len > this->buffer_size){
        ret = cli_rl_expand_buffer_size(this,new_len);
        if(ret != CLI_OK){
            return ret;
        }
    }

    /*insert in the middle*/
    if(this->point < this->end){
        memmove(&this->buffer[this->point + len],
                &this->buffer[this->point],
                (this->end - this->point) + 1);
    }else{
        this->buffer[this->end + len] = '\0';
    }

    strncpy(&this->buffer[this->point],text,len);

    this->point += len;
    this->end   += len;

    return ret;
}


cli_status_t cli_rl_delete_text(
        cli_rl_t *this,
        cli_uint32 start,
        cli_uint32 end)
{
    cli_uint32 delta = (end - start) + 1; 


    if(start > end){
        return CLI_E_ERROR;
    }

    if(end > this->end){
        end = this->end;
    }

    cli_rl_change_buffer(this);
    if(end != this->end){
        memmove(&this->buffer[start],
                &this->buffer[start + delta],
                this->end - end);
    }
    
    if(end  >= this->end){
        this->end = start;
    }else{
        this->end -= delta;
    }

    /* make the cursor in the right place*/
    if(this->point >= start){
        if(this->point > end){
            this->point -=delta;
        }else{
            this->point = start;
        }
    }

    this->buffer[this->end] = '\0';

    return CLI_OK;

}

void cli_rl_echo_printf(
        cli_rl_t * this, 
        const cli_int8 * text)
{
    cli_uint32 i = 0;

    CLI_ASSERT(this != NULL && text != NULL);

    if(this->echo_enabled){
        cli_tty_printf(this->term,"%s",text);
    }else{
        if(this->echo_char != '\0'){
           i = strlen(text);
           while(i--){
               cli_tty_printf(this->term,"%c",this->echo_char);
           }
        }
    }
}


cli_int32 cli_rl_printf(
        cli_rl_t * this, 
        const cli_int8 *fmt,
        va_list args
        )
{
    cli_int32 len = 0;
    
    if(this->session_end){
        return 0;
    }
    len = cli_tty_vprintf(this->term, fmt, args);

    return len;
}

void cli_rl_display_finish(cli_rl_t * this)
{
    cli_tty_oflush(this->term);
    cli_strfree(this->screen_buffer);

    this->screen_buffer = cli_strdup(this->line);
    this->screen_point  = this->point;
}

void cli_rl_move_cursor(
        cli_rl_t * this
        )
{
    cli_int32 shift = (cli_int32)this->screen_point - (cli_int32)this->point;

    if(this->echo_enabled || this->echo_char != '\0'){
        if(shift > 0){ /* back */ 
            cli_tty_cursor_back(this->term,shift);
        }else if (shift < 0){ /* forward */
            cli_tty_cursor_forward(this->term,-shift);
        }
    }
}

void cli_rl_echo_enable(
        cli_rl_t *  this,
        cli_boolean enable)
{
    this->echo_enabled = enable;
}
void cli_rl_display(cli_rl_t *  this)
{
    cli_uint32 line_len = strlen(this->line);
    cli_uint32 screen_buffer_len = 0;
    cli_int32 delta = 0;

    /* nothing on the screen */
    if(this->screen_buffer == NULL){
        if(this->prompt != NULL){
            cli_tty_printf(this->term,"%s",this->prompt);
        }

        cli_rl_echo_printf(this,this->line);
        
        /*move the cursor back*/
        if(this->point <line_len){
            cli_tty_cursor_back(this->term,line_len - this->point);
        }

        cli_rl_display_finish(this);
     
        return;
    }


    /*process with the old screen buffer*/
    screen_buffer_len = strlen(this->screen_buffer);
    delta = line_len - screen_buffer_len;
    
    if(delta > 0){ 
        /* append at then end */
        if(0 == strncmp(this->line,this->screen_buffer,screen_buffer_len)){
            cli_rl_echo_printf(this,&this->line[line_len - delta]);
            cli_rl_display_finish(this);
            return;
        }
    }else if (delta < 0){
        /*delete from the end*/
        if(0 == strncmp(this->line,this->screen_buffer,line_len)){
            /*if the buffer has displayed on the screen, need erase from screen*/
            if(this->echo_enabled || this->echo_char != '\0'){
                /* make the cursor in the right place */
                cli_rl_move_cursor(this);

                cli_tty_erase(this->term,-delta);
            }
            cli_rl_display_finish(this);
            return;
        }
    }else{
        if(0 == strncmp(this->line,this->screen_buffer,line_len)){
            cli_rl_move_cursor(this);
            cli_rl_display_finish(this);
            return;
        }
    }


    /*for any edit line*/

    if(this->screen_point){ /* move the cursor to the start of the line*/
        cli_tty_cursor_back(this->term,this->screen_point);
    }

    cli_tty_erase(this->term,screen_buffer_len);
    cli_rl_echo_printf(this,this->line);  /* cursor move to the end of the line */

    /* move cursor back if necessary */
    if(this->point < line_len){
        cli_tty_cursor_back(this->term,line_len - this->point);
    }

    cli_rl_display_finish(this);
}

void cli_rl_reset_line_state(
        cli_rl_t * this)
{
    if(NULL != this->screen_buffer){
        cli_strfree(this->screen_buffer);
        this->screen_buffer = NULL;
    }

    cli_rl_display(this);
}

void cli_rl_readline_from_tty(
     cli_rl_t * this,
     cli_boolean isscanf)
{
    cli_uint32   key; 


   // cli_rl_tty_set_raw_mode(this);
    cli_rl_reset_line_state(this);

    while(!this->done){
        cli_rl_display(this);
        key = cli_tty_getchar(this->term);
        
        if(key == EOF){
           this->done = TRUE;
           this->line = NULL;
           this->session_end = TRUE;
           break;
        }
        
        if(!isscanf){
            if(CLI_OK != this->handlers[key](this,key)){
                cli_tty_ding(this->term);
            }
        }else{
            if(CLI_OK != this->scanf_handlers[key](this,key)){
                cli_tty_ding(this->term);
            }
        }

        if(this->done){
            /*remove space at the end*/
            if(this->end > 0 && isspace(this->line[this->end-1])){
                cli_rl_delete_text(this,this->end -1, this->end);
            }
        }

    }

   // cli_rl_tty_restore_mode(this);
}




void cli_rl_crlf(cli_rl_t *this)
{
    if(!this->session_end){
        cli_tty_printf(this->term,"\n");
    }
}

cli_int32 cli_rl_get_input(
        cli_rl_t * this,
        const cli_int8* prompt,
        cli_int8 *buf,
        cli_int32 max_len)
{
    cli_int8* result = NULL;
    cli_int32  len = 0;
    cli_int8 * last_buf = NULL;
    
    if(buf == NULL){
        return 0;
    }

    if(this->session_end){
        return -1;
    }
    /* initialise for reading a line */
    this->done             = FALSE;
    this->point            = 0;
    this->end              = 0;
    this->prompt           = prompt;
    this->prompt_size      = strlen(prompt);

    /*backup the last buffer if possible for history purpose*/
    if(this->buffer_size  == 0){
        if(this->screen_buffer != NULL){
            last_buf = cli_strdup(this->screen_buffer);
        }

        if(this->buffer != NULL){
            cli_strfree(this->buffer);
            this->line = this->buffer = NULL;
        }
    }else{
        last_buf = this->buffer ;
        this->line = this->buffer = NULL;
    }
    
    this->buffer           = cli_strdup("");
    this->buffer_size      = strlen(this->buffer);
    this->line             = this->buffer;

    cli_rl_readline_from_tty(this,TRUE);

    result = this->line ? cli_strdup(this->line) : NULL;

    cli_strfree(this->buffer);
    this->buffer = NULL;

    this->line = this->buffer = last_buf;
    if(last_buf != NULL){
        this->buffer_size      = strlen(this->buffer);
    }

    if(result == NULL || *result == '\0'){
        cli_rl_crlf(this);
        return 0;
    }else{
        len = strlen(result);
    }
    
    len = len > max_len? max_len:len;
    
    strncpy(buf,result,len);

    if(result != NULL){
        cli_strfree(result);
        result = NULL;
    }
    return len;
}


cli_int8 * cli_rl_readline(
        cli_rl_t * this,
        void     * context,
        const cli_int8* prompt)
{
    cli_int8* result = NULL;

    if(this->session_end){
        return NULL;
    }
    /* initialise for reading a line */
    this->done             = FALSE;
    this->point            = 0;
    this->end              = 0;
    this->buffer           = cli_strdup("");
    this->buffer_size      = strlen(this->buffer);
    this->line             = this->buffer;
    this->prompt           = prompt;
    this->prompt_size      = strlen(prompt);
    this->context          = context;

   // if(this->isatty){
        cli_rl_readline_from_tty(this,FALSE);
    //}else{
      //  cli_rl_readline_from_file(this);
    //}


    result = this->line ? cli_strdup(this->line) : NULL;


    cli_strfree(this->buffer);
    this->buffer = NULL;

    if(result == NULL || *result == '\0'){
        cli_rl_crlf(this);
    }

    return result;
}



static cli_status_t cli_rl_key_default(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;
    cli_int8   tmp[10] = {0};

    /*refer ascii table*/
    if(key > 31){
        tmp[0] = (key & 0xFF),
            tmp[1] = '\0';
    }
    else{
       /* sprintf(tmp,"~%d",key);*/
    }

    result = cli_rl_insert_text(this,tmp);

    return result;
}


static cli_status_t cli_rl_key_crlf(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_rl_crlf(this); 
    this->done = TRUE;

    return CLI_OK;
}

static cli_status_t cli_rl_key_interrupt(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_rl_delete_text(this,0,this->end);
    this->done = TRUE;
    return CLI_OK;
}

static cli_status_t cli_rl_key_backspace(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;

    if(this->point > 0){
        this->point --; 
        result = cli_rl_delete_text(
                this,this->point,this->point);
    }

    return result;
}


static cli_status_t cli_rl_key_delete(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;

    if(this->point < this->end){
        result = cli_rl_delete_text(
                this,this->point,this->point);
    }else{
        result = cli_rl_key_backspace(this,key);
    }

    return result;
}


static cli_status_t cli_rl_key_up(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;
    cli_history_item_t * item = NULL; 

    /*first time to use history*/
    if(this->buffer == this->line){
        item = cli_history_getlast(this->history,&this->hist_iter);
    }else{
        item = cli_history_getprevious(&this->hist_iter);
    }

    if(item != NULL){
        this->line = cli_history_item_get_line(item);
        this->point = this->end = strlen(this->line);
        result = CLI_OK;
    }
    return result;
}


static cli_status_t cli_rl_key_down(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_history_item_t * item = NULL; 
    
    /*at the end,not response*/
    if(this->buffer == this->line){
        return CLI_E_ERROR;
    }

    this->line = this->buffer;
    item = cli_history_getnext(&this->hist_iter);
    if(item != NULL){
        this->line = cli_history_item_get_line(item);   
    }
    
    this->point = this->end = strlen(this->line);


    return CLI_OK;
}


static cli_status_t cli_rl_key_left(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;
    
    if(this->point > 0){
        this->point --;
        result = CLI_OK;
    }
    return result;
}

static cli_status_t cli_rl_key_right(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;
    
    if(this->point < this->end){
        this->point ++;
        result = CLI_OK;
    }
    return result;
}

static cli_status_t cli_rl_key_escape(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;
    switch(cli_tty_escape_decode(this->term)){
        case CLI_TTY_CURSOR_UP:
            result = cli_rl_key_up(this,key);
            break;
        case CLI_TTY_CURSOR_DOWN:
            result = cli_rl_key_down(this,key);
            break;
        case CLI_TTY_CURSOR_LEFT:
            result = cli_rl_key_left(this,key);
            break;
        case CLI_TTY_CURSOR_RIGHT:
            result = cli_rl_key_right(this,key);
            break;
        default:
            break;
    }
    return result;
}

static cli_status_t cli_rl_key_escape_input(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;
    switch(cli_tty_escape_decode(this->term)){
        case CLI_TTY_CURSOR_LEFT:
            result = cli_rl_key_left(this,key);
            break;
        case CLI_TTY_CURSOR_RIGHT:
            result = cli_rl_key_right(this,key);
            break;
        default:
            break;
    }
    return result;
}


static cli_status_t cli_rl_key_clear_screen(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_tty_clear_screen(this->term);
    cli_tty_cursor_home(this->term);
    cli_rl_reset_line_state(this);
    return CLI_OK;
}

static cli_status_t cli_rl_key_erase_line(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;
    result = cli_rl_delete_text(
                this,0,this->point);
    this->point = 0;

    return result;
}


static cli_status_t cli_rl_key_start_of_line(
        cli_rl_t *this,
        cli_int32  key)
{
    this->point = 0;
    return CLI_OK;
}


static cli_status_t cli_rl_key_end_of_line(
        cli_rl_t *this,
        cli_int32  key)
{
    this->point = this->end;
    return CLI_OK;
}

static cli_status_t cli_rl_key_kill(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;
    
    cli_strfree(this->kill_str);

    this->kill_str = cli_strdup(&this->buffer[this->point]);

    result = cli_rl_delete_text(
                this,this->point,this->end);

    return result;
}

static cli_status_t cli_rl_key_yank(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;
    
    if(this->kill_str != NULL){
        result = cli_rl_insert_text(
                this,this->kill_str);
    }
    return result;
}

static cli_status_t cli_rl_key_tab(
        cli_rl_t *this,
        cli_int32  key)
{
    cli_status_t result = CLI_E_ERROR;

    return result;
}

static void cli_rl_init(
        cli_rl_t       *this,
        FILE           *instream,
        FILE           *outstream)
{
    cli_uint32 i;
    
    for(i = 0;i < CLI_RL_NUM_HANDLERS ; i++)
    {
        this->handlers[i] = cli_rl_key_default;
        this->scanf_handlers[i] = cli_rl_key_default;
    }

    this->handlers[KEY_CR]  = cli_rl_key_crlf;
    /*this->handlers[KEY_LF]  = cli_rl_key_crlf;*/
    this->handlers[KEY_ETX] = cli_rl_key_interrupt;
    this->handlers[KEY_DEL] = cli_rl_key_backspace;
    this->handlers[KEY_BS]  = cli_rl_key_backspace;
    this->handlers[KEY_EOT] = cli_rl_key_delete;
    this->handlers[KEY_ESC] = cli_rl_key_escape;
    this->handlers[KEY_FF]  = cli_rl_key_clear_screen;
    this->handlers[KEY_NAK] = cli_rl_key_erase_line;
    this->handlers[KEY_SOH] = cli_rl_key_start_of_line;
    this->handlers[KEY_ENQ] = cli_rl_key_end_of_line;
    this->handlers[KEY_VT]  = cli_rl_key_kill;
    this->handlers[KEY_EM]  = cli_rl_key_yank;
    this->handlers[KEY_HT]  = cli_rl_key_tab;

    this->scanf_handlers[KEY_CR]  = cli_rl_key_crlf;
    /*this->scanf_handlers[KEY_LF]  = cli_rl_key_crlf;*/
    this->scanf_handlers[KEY_ETX] = cli_rl_key_interrupt;
    this->scanf_handlers[KEY_DEL] = cli_rl_key_backspace;
    this->scanf_handlers[KEY_BS]  = cli_rl_key_backspace;
    this->scanf_handlers[KEY_EOT] = cli_rl_key_delete;
    this->scanf_handlers[KEY_ESC] = cli_rl_key_escape_input;
    this->scanf_handlers[KEY_SOH] = cli_rl_key_start_of_line;
    this->scanf_handlers[KEY_ENQ] = cli_rl_key_end_of_line;

    this->line                          = NULL;
    this->max_line_length               = 512;
    this->prompt                        = NULL;
    this->prompt_size                   = 0;
    this->buffer                        = NULL;
    this->buffer_size                   = 0;
    this->screen_buffer                 = NULL;
    this->screen_point                  = 0;
    this->done                          = FALSE;
    this->point                         = 0;
    this->end                           = 0;
    this->echo_char                     = '*';
    this->echo_enabled                  = TRUE;
    this->session_end                   = FALSE;
    /*this->isatty                        = isatty(fileno(instream)) ? TRUE : FALSE;*/
    
    this->kill_str                      = NULL;

    this->term = cli_tty_new(instream,outstream);

    this->history = cli_history_new();
}


static void cli_rl_finish(cli_rl_t * this)
{
    cli_history_delete(this->history);
    cli_tty_delete(this->term);
    
    if(this->kill_str){
        cli_strfree(this->kill_str);
        this->kill_str = NULL;
    }

    if(this->buffer != NULL){
        cli_strfree(this->buffer);
        this->buffer = NULL;
    }

    if(this->screen_buffer != NULL){
        cli_strfree(this->screen_buffer);
        this->screen_buffer = NULL;
    }
}

cli_rl_t * cli_rl_new(
           FILE   *instream,
           FILE   *outstream)
{
    cli_rl_t *this=NULL;
    
    CLI_ASSERT_RET(instream != NULL 
            && outstream != NULL, NULL);

    this = CLI_MALLOC(sizeof(cli_rl_t));
    if(NULL != this){
        cli_rl_init(this,instream,outstream);
    }
    
    return this;
}

void cli_rl_delete(cli_rl_t * this)
{
    cli_rl_finish(this);
    CLI_FREE(this);
}

cli_status_t  cli_rl_bind_key(
    cli_rl_t * this,
    cli_int32  key,
    cli_rl_key_func_t * func)
{
    cli_status_t ret = CLI_E_ERROR;

    if(key >=0 && key < 256){
        this->handlers[key] = func;
        ret = CLI_OK;
    }

    return ret;
}

/*lint +e49*/
/*lint +e10*/
/*lint +e40*/
/*lint +e438*/
/*lint +e650*/
/*lint +e451*/

