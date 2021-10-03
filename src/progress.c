/* Simple termios based progress bar
 *
 * Copyright (c) 2012-2021  Joachim Wiberg <troglobit@gmail.com>
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
 * @file progress.c
 * @author Joachim Wiberg
 * @date 2012-2021
 * @copyright ISC License
 */

#include <limits.h>		/* INT_MAX */
#include <stdio.h>
#include <string.h>

#include "conio.h"

#define SPINNER_THROB   ".oOo"
#define SPINNER_PULSAR  ".oO°Oo."
#define SPINNER_ARROW   "v<^>"
#define SPINNER_STAR    ".oO@*"
#define SPINNER_DEFAULT "|/-\\"

static char spinner(char *style)
{
	size_t num;
	static unsigned int i = 0;

	if (!style)
		style = SPINNER_DEFAULT;
	num = strlen(style);

	return style[i++ % num]; /* % Number of states in style */
}

/**
 * Advanced ASCII progress bar with spinner
 * @param percent    Start first call with this set to 0, end with 100
 * @param max_width  Max width of progress bar, in total characters.
 *
 * This function draws an advanced ASCII progressbar at the current
 * line.  It always start from the first column.
 *
 * The progress bar will hide the cursor if started with @p percent 0
 * and show it again at the end, when called with @p percent 100.
 *
 * While being called with the same percentage the spinner will spin,
 * to show the user the process hasn't frozen.
 *
 * If the output TTY cannot interpret control characters, like `\r`, it
 * is advised to instead used the progress_simple() function.
 */
void progress(int percent, int max_width)
{
	int i, bar;

	/* Adjust for progress bar overhead */
	max_width -= 10;

	if (0 == percent)
		hidecursor();

	fprintf(stderr, "\r%3d%% %c [", percent, spinner(NULL));

	bar = percent * max_width / 100;
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

/**
 * Alternative progress bar on systems where progress() doesn't work
 * @param percent  Start first call with this set to 0, end with 100
 */
void progress_simple(int percent)
{
	static int last = 1;
	int ratio, numout;

	if (!percent && last) {
		last = 0;
		fputs("0%       25%       50%       75%       100%\n"
		      "|---------+---------+---------+---------|\n"
		      "|", stderr);
		return;
	}

	ratio = 40 * percent / 100;
	numout = ratio - last;

	if (ratio <= last)
		return;

	last = ratio;

	while (numout--) {
		if (ratio != 40 || numout)
			putc('=', stderr);
		else
			putc('|', stderr);
	}
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
