/* Safe yes-or-no with prompt
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
 * @file yorn.c
 * @author Joachim Wiberg
 * @date 2009-2021
 * @copyright ISC License
 */

#include <stdio.h>
#include <stdio_ext.h>		/* __fpurge() */
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>

static char rawgetch(void)
{
	struct termios savemodes, modmodes;
	char val;

	if (!isatty(STDIN_FILENO))
		return getchar();

	/* Backup terminal settings. */
	if (tcgetattr(STDIN_FILENO, &savemodes) < 0) {
		return -1;
	}

	/* "stty cbreak -echo" */
	modmodes = savemodes;
	modmodes.c_lflag &= ~ICANON;
	modmodes.c_lflag &= ~ECHO;
	modmodes.c_cc[VMIN] = 1;
	modmodes.c_cc[VTIME] = 0;

	/* Set terminal in raw mode. */
	if (tcsetattr(STDIN_FILENO, TCSANOW, &modmodes) < 0) {
		tcsetattr(STDIN_FILENO, TCSANOW, &savemodes);
		return -1;
	}

	val = getchar();

	/* Restore terminal to previous state. */
	tcsetattr(STDIN_FILENO, TCSANOW, &savemodes);

	return val;
}

/**
 * Pose a a Yes or No question and return answer
 * @param fmt  Standard printf() style argument(s).
 *
 * This function prints the given question on screen, waits for user
 * input in the form of yes or no.
 *
 * @returns TRUE(1) or FALSE(0).  True if the answer is yes.
 */
int yorn(const char *fmt, ...)
{
	va_list ap;
	char yorn;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	__fpurge(stdin);
	yorn = rawgetch();
	printf("%c\n", yorn);
	fflush(stdout);
	if (yorn != 'y' && yorn != 'Y')
		return 0;

	return 1;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
