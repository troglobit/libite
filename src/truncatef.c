/* Formatted truncate()
 *
 * Copyright (c) 2021  Joachim Wiberg <troglobit@gmail.com>
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
 * @file truncatef.c
 * @author Joachim Wiberg
 * @date 2021
 * @copyright ISC License
 */

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Truncate a file based on the formatted string
 * @param length  Size in bytes to truncate file to, zero to empty it
 * @param fmt     Formatted string to be composed into a pathname
 *
 * This is an extension to the truncate() family, lessening the burden
 * of having to compose the filename from parts in a seprate buffer.
 *
 * @returns Upon successful completion truncate() returns POSIX OK(0),
 * otherwise, -1 is returned and @a errno is set to indicate error.
 */
int truncatef(off_t length, const char *fmt, ...)
{
	va_list ap;
	char *file;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	file = alloca(len + 1);
	if (!file) {
		errno = ENOMEM;
		return -1;
	}

	va_start(ap, fmt);
	vsnprintf(file, len + 1, fmt, ap);
	va_end(ap);

	return truncate(file, length);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
