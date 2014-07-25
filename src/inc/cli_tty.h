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


#ifndef __CLI_TTY_H__
#define __CLI_TTY_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "cli_utils.h"
/*
 * The standard refer the vt100 control code, we implement as vt100
 */


/* define the Key codes */
#define KEY_NUL	0	/**< ^@	Null character */
#define KEY_SOH	1	/**< ^A	Start of heading, = console interrupt */
#define KEY_STX	2	/**< ^B	Start of text, maintenance mode on HP console */
#define KEY_ETX	3	/**< ^C	End of text */
#define KEY_EOT	4	/**< ^D	End of transmission, not the same as ETB */
#define KEY_ENQ	5	/**< ^E	Enquiry, goes with ACK; old HP flow control */
#define KEY_ACK	6	/**< ^F	Acknowledge, clears ENQ logon hand */
#define KEY_BEL	7	/**< ^G	Bell, rings the bell... */
#define KEY_BS	8	/**< ^H	Backspace, works on HP terminals/computers */
#define KEY_HT	9	/**< ^I	Horizontal tab, move to next tab stop */
#define KEY_LF	10	/**< ^J	Line Feed */
#define KEY_VT	11	/**< ^K	Vertical tab */
#define KEY_FF	12	/**< ^L	Form Feed, page eject */
#define KEY_CR	13	/**< ^M	Carriage Return*/
#define KEY_SO	14	/**< ^N	Shift Out, alternate character set */
#define KEY_SI	15	/**< ^O	Shift In, resume defaultn character set */
#define KEY_DLE	16	/**< ^P	Data link escape */
#define KEY_DC1	17	/**< ^Q	XON, with XOFF to pause listings; "okay to send". */
#define KEY_DC2	18	/**< ^R	Device control 2, block-mode flow control */
#define KEY_DC3	19	/**< ^S	XOFF, with XON is TERM=18 flow control */
#define KEY_DC4	20	/**< ^T	Device control 4 */
#define KEY_NAK	21	/**< ^U	Negative acknowledge */
#define KEY_SYN	22	/**< ^V	Synchronous idle */
#define KEY_ETB	23	/**< ^W	End transmission block, not the same as EOT */
#define KEY_CAN	24	/**< ^X	Cancel line, MPE echoes !!! */
#define KEY_EM	25	/**< ^Y	End of medium, Control-Y interrupt */
#define KEY_SUB	26	/**< ^Z	Substitute */
#define KEY_ESC	27	/**< ^[	Escape, next character is not echoed */
#define KEY_FS	28	/**< ^\	File separator */
#define KEY_GS	29	/**< ^]	Group separator */
#define KEY_RS	30	/**< ^^	Record separator, block-mode terminator */
#define KEY_US	31	/**< ^_	Unit separator */

#define KEY_DEL 127 /**< Delete (not a real control character...) */


typedef struct{
    FILE *istream;
    FILE *ostream;  
}cli_tty_t;


typedef enum{
    CLI_TTY_UNKNOW,
    CLI_TTY_CURSOR_UP,
    CLI_TTY_CURSOR_DOWN,
    CLI_TTY_CURSOR_LEFT,
    CLI_TTY_CURSOR_RIGHT
}cli_tty_escape_e;

typedef struct
{
    cli_uint8  terminator;
    cli_tty_escape_e code;
}cli_tty_decode_t;



cli_tty_escape_e cli_tty_escape_decode(
        cli_tty_t * tty);

cli_tty_t * cli_tty_new(
        FILE *istream,
        FILE *ostream);

void cli_tty_delete(cli_tty_t * tty);

cli_uint32 cli_tty_vprintf(
        const cli_tty_t *tty,
        const cli_int8  *fmt,
        va_list   args);

cli_uint32 cli_tty_printf(
        const cli_tty_t *tty,
        const cli_int8 *fmt,
        ...);

cli_uint32 cli_tty_getchar(const cli_tty_t *tty);

void cli_tty_cursor_forward(
        const cli_tty_t *tty,
        cli_uint32  count);
void cli_tty_cursor_back(
        const cli_tty_t *tty,
        cli_uint32  count);
void cli_tty_cursor_up(
        const cli_tty_t *tty,
        cli_uint32  count);
void cli_tty_cursor_down(
        const cli_tty_t *tty,
        cli_uint32  count);
void cli_tty_cursor_home(const cli_tty_t *tty);
void cli_tty_erase(
        const cli_tty_t *tty,
        cli_uint32   count);
void cli_tty_oflush(const cli_tty_t * tty);
void cli_tty_clear_screen(const cli_tty_t *tty);

void cli_tty_ding(const cli_tty_t *tty);

void cli_tty_set_istream(
        cli_tty_t *tty,
        FILE   *istream);
FILE * cli_tty_get_istream(const cli_tty_t *tty);
FILE * cli_tty_get_ostream(const cli_tty_t *tty);


#define CLI_VFPINTF(f,fmt,...)             vfprintf((f),(fmt),##__VA_ARGS__)
#define CLI_GETC(f)                        getc((f))

#endif
