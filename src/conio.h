/* A conio.h like implementation for VTANSI displays.
 *
 * Copyright (c) 2009-2021  Joachim Wiberg <troglobit@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @file conio.h
 * @author Joachim Wiberg
 * @date 2009-2021
 * @copyright ISC License
 *
 * Helper macros and functions for interacting with TTY terminals.
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LIBITE_CONIO_H_
#define LIBITE_CONIO_H_

#include <stdio.h>

/** Attributes */
#define RESETATTR    0
#define BRIGHT       1
#define DIM          2
#define UNDERSCORE   4
#define BLINK        5           /* May not work on all displays. */
#define REVERSE      7
#define HIDDEN       8

/** Colors for text and background */
#define BLACK        0x0
#define RED          0x1
#define GREEN        0x2
#define BROWN        0x3
#define BLUE         0x4
#define MAGENTA      0x5
#define CYAN         0x6
#define LIGHTGREY    0x7

#define DARKGREY     0x10
#define LIGHTRED     0x11
#define LIGHTGREEN   0x12
#define YELLOW       0x13
#define LIGHTBLUE    0x14
#define LIGHTMAGENTA 0x15
#define LIGHTCYAN    0x16
#define WHITE        0x17

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 80	/**< Fallback screen width, possible to override. */
#endif

/** Clear screen and move cursor to 1,1 (upper left) pos. */
#define clrscr()              fputs("\033[2J\033[1;1H", stdout)
/** Erases from the current cursor position to the end of the current line. */
#define clreol()              fputs("\033[K", stdout)
/** Erases the entire current line. */
#define delline()             fputs("\033[2K", stdout)
/** Moves the cursor to the specified position (coordinates) */
#define gotoxy(x,y)           fprintf(stdout, "\033[%d;%dH", y, x)
/** Hide Cursor */
#define hidecursor()          fputs("\033[?25l", stdout)
/** Show Cursor */
#define showcursor()          fputs("\033[?25h", stdout)

/** Set Graphics Mode (attr, color, val) */
#define __set_gm(a,c,v)							\
	if (!c)								\
		fprintf(stdout, "\033[%dm", a);				\
	else								\
		fprintf(stdout, "\033[%d;%dm", c & 0x10 ? 1 : 0, (c & 0xF) + v)

/** Set text attribute */
#define textattr(attr)        __set_gm(attr, 0, 0)
/** Set text color */
#define textcolor(color)      __set_gm(RESETATTR, color, 30)
/** Set text background */
#define textbackground(color) __set_gm(RESETATTR, color, 40)

void initscr(int *row, int *col);

/**
 * Print table heading, with optional leading newline.
 * @param fp    output stream
 * @param line  string to print
 * @param nl    leading newline or not
 * @param attr  attribute
 *
 * @verbatim
 * _________________ <-- Empty line with UNDERSCORE to frame first heading
 * First heading     <-- In normal/RESETATTR
 * SUBHEADING        <-- In REVERSE and with capital letters like 'top'
 * @endverbatim
 */
static inline void printhdr(FILE *fp, const char *line, int nl, int attr)
{
	if (attr < 0 || attr > 8)
		attr = REVERSE;

	fprintf(fp ?: stdout,
		"%s\033[%dm%s%*s\033[0m\n",
		nl ? "\n" : "", attr, line,
		SCREEN_WIDTH - (int)strlen(line), "");
}

/**
 * Print reverse mode table heading, e.g. black text on white background.
 * @param fp    output stream
 * @param line  string to print
 * @param nl    leading newline or not
 */
static inline void printheader(FILE *fp, const char *line, int nl)
{
	printhdr(fp, line, nl, REVERSE);
}

#endif /* LIBITE_CONIO_H_ */

#ifdef __cplusplus
}
#endif

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
