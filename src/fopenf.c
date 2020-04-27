/* Formatted fopen()
 *
 * Copyright (c) 2020  Joachim Nilsson <troglobit@gmail.com>
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

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

/**
 * fopenf - Open a file based on the formatted string and optional arguments
 * @mode: Last argument in optional list, if omitted EINVAL
 * @fmt:  Formatted string to be composed into a pathname
 *
 * This function is an extension to the fopen() family, lessening the burden
 * of first having to compose the filename from parts in a seprate buffer.
 *
 * Returns:
 * Upon successful completion fopenf() a FILE pointer.  Otherwise, NULL
 * is returned and errno is set to indicate the error.
 */
FILE *fopenf(const char *mode, const char *fmt, ...)
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
		return NULL;
	}

	va_start(ap, fmt);
	vsnprintf(file, len + 1, fmt, ap);
	va_end(ap);

	return fopen(file, mode);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
