/* Formatted fopen()
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
 * @file fopenf.c
 * @author Joachim Wiberg
 * @date 2021
 * @copyright ISC License
 */

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

/**
 * Similar to fopenf() except it takes a @a va_list argument
 * @param mode An fopen() mode string, e.g. "w+".
 * @param fmt  Formatted string to be composed into a pathname.
 * @param ap   List of variable arguemnts from va_start().
 *
 * See fopenf() for details.
 *
 * @returns a FILE pointer, or @c NULL on error.
 */
FILE *vfopenf(const char *mode, const char *fmt, va_list ap)
{
	va_list apc;
	char *file;
	int len;

	va_copy(apc, ap);
	len = vsnprintf(NULL, 0, fmt, apc);
	va_end(apc);

	file = alloca(len + 1);
	if (!file) {
		errno = ENOMEM;
		return NULL;
	}

	va_copy(apc, ap);
	vsnprintf(file, len + 1, fmt, apc);
	va_end(apc);

	return fopen(file, mode);
}

/**
 * Open a file based on the formatted string and optional arguments
 * @param mode An fopen() mode string, e.g. "w+".
 * @param fmt  Formatted string to be composed into a pathname.
 *
 * This function is an extension to the fopen() family, lessening the burden
 * of first having to compose the filename from parts in a seprate buffer.
 *
 * @returns Upon successful completion, fopenf() returns a FILE pointer.
 * Otherwise, @c NULL is returned and @a errno is set to indicate the
 * error.
 */
FILE *fopenf(const char *mode, const char *fmt, ...)
{
	va_list ap;
	FILE *fp;

	va_start(ap, fmt);
	fp = vfopenf(mode, fmt, ap);
	va_end(ap);
	if (!fp)
		return NULL;

	return fp;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
