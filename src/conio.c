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
 * @file conio.c
 * @author Joachim Wiberg
 * @date 2009-2021
 * @copyright ISC License
 */

#include <poll.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

/**
 * Probe terminal size
 * @param row pointer to integer to store number of rows
 * @param col pointer to integer to store number of columns
 *
 * This function checks if stdin and stdout isatty() and then sets the
 * TTY in raw mode to silently ask the size using ANSI escape sequences.
 * This is achieved by trying to go to corner 999,999 followed by
 * querying the cursor position.  Afterwards the TTY is returned to the
 * state if was before, e.g. cooked.  The number of rows and columns is
 * returned in the input arguments to this function.
 *
 * If stdio is @a not a TTY, then a default 24x80 is returned.
 */
void initscr(int *row, int *col)
{
	if (!row || !col)
		return;

#if defined(TCSANOW) && defined(POLLIN)
	struct termios tc, saved;
	struct pollfd fd = { STDIN_FILENO, POLLIN, 0 };

	if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO)) {
		*row = 24;
		*col = 80;
		return;
	}

	/* Disable echo to terminal while probing */
	tcgetattr(STDOUT_FILENO, &tc);
	saved = tc;
	tc.c_cflag |= (CLOCAL | CREAD);
	tc.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	tcsetattr(STDOUT_FILENO, TCSANOW, &tc);

	/*
	 * Save cursor pos+attr
	 * Diable top+bottom margins
	 * Set cursor at the far bottom,right pos
	 * Query term for resulting pos
	 */
	fprintf(stdout, "\e7" "\e[r" "\e[999;999H" "\e[6n");
	fflush(stdout);

	/*
	 * Wait here for terminal to echo back \e[row,lineR ...
	 */
	if (poll(&fd, 1, 300) <= 0 || scanf("\e[%d;%dR", row, col) != 2) {
		*row = 24;
		*col = 80;
	}

	/*
	 * Restore above saved cursor pos+attr
	 */
	fprintf(stdout, "\e8");
	fflush(stdout);

	/* Restore terminal */
	tcsetattr(STDOUT_FILENO, TCSANOW, &saved);

	return;
#else
	*row = 24;
	*col = 80;
#endif
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
