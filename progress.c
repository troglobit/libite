/* Very simple termios based progress bar
 *
 * Copyright (c) 2012, 2013  Joachim Nilsson <troglobit@gmail.com>
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

#include <limits.h>		/* INT_MAX */
#include <stdio.h>

#include "conio.h"

static char spinner(void)
{
	static int i = INT_MAX;
	char *states = "|/-\\";
//	char *states = ".oOo";
//	char *states = ".oO°Oo.";
//	char *states = "v<^>";
//	char *states = ".oO@*";

	/* Subtract to get clockwise spinning. */
	return states[i-- % 4];	/* % Number of states */
}

void progress(int percent, int max_width)
{
	int i, bar = percent * max_width / 100;

	if (0 == percent)
		hidecursor();

	fprintf(stderr, "\r%3d%% %c [", percent, spinner());

	for (i = 0; i < max_width; i++) {
		if (i > bar)
			fputc(' ', stderr);
		else if (i == bar)
			fputc('>', stderr);
		else
			fputc('=', stderr);
	}

	fprintf(stderr, "]");
	if (100 == percent) {
		showcursor();
		fputc('\n', stderr);
	}
}

#ifdef UNITTEST
#include <stdlib.h>		/* atexit() */
#include <unistd.h>		/* usleep() */

#define MAX_WIDTH    80
#define msleep(msec) usleep(msec * 1000)

static void bye(void)
{
	showcursor();
}

int main(void)
{
	int i, percent, block = 0, num = 85;

	atexit(bye);
	hidecursor();

	while (block < num) {
		percent = block * 100 / num;
		for (i = 0; i < 10; i++) {
			progress(percent, MAX_WIDTH);
			msleep(2);
		}
		block++;
		msleep(50);
	}
	progress(100, MAX_WIDTH);

	return 0;
}
#endif	/* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "make V=1 -f progress.mk"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
