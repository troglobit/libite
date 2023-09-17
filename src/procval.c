/* Reading and writing values/strings to/from /proc and /sys style files
 *
 * Copyright (c) 2023  Tobias Waldekranz <tobias@waldekranz.com>
 * Copyright (c) 2023  Joachim Wiberg <troglobit@gmail.com>
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
 * @file procval.c
 * @author Joachim Wiberg
 * @date 2023
 * @copyright ISC License
 */

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "lite.h"

/**
 * Similar to readsnf() except it takes a @a va_list argument
 * @param line Pointer to line buffer.
 * @param len  Size of line buffer.
 * @param fmt  Formatted string to be composed into a pathname.
 * @param ap   List of variable arguemnts from va_start().
 *
 * For details, see readsnf().
 *
 * @returns same as readsnf().
 */
char *vreadsnf(char *line, size_t len, const char *fmt, va_list ap)
{
	va_list apc;
	FILE *fp;

	va_copy(apc, ap);
	fp = vfopenf("r", fmt, apc);
	va_end(apc);
	if (!fp)
		return NULL;

	if (!fgets(line, len, fp)) {
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	return chomp(line);
}


/**
 * Read first line from a file composed from fmt and optional args.
 * @param line Pointer to line buffer.
 * @param len  Size of line buffer.
 * @param fmt  Formatted string to be composed into a pathname.
 *
 * @returns On success, this function returns the @p line read from the
 * file, with any trailing '\n' chomp()ed out.  On error, @c NULL.
 */
char *readsnf(char *line, size_t len, const char *fmt, ...)
{
	va_list ap;
	char *ln;

	va_start(ap, fmt);
	ln = vreadsnf(line, len, fmt, ap);
	va_end(ap);

	return ln;
}

/**
 * Write a string buffer to a file composed from fmt and optional args.
 * @param str  Pointer to string buffer, may be multiple lines.
 * @param mode An fopen() mode string, e.g. "w+".
 * @param fmt  Formatted string to be composed into a pathname.
 *
 * @returns result of operation, with @a errno set on error.
 */
int writesf(const char *str, const char *mode, const char *fmt, ...)
{
	va_list ap;
	FILE *fp;

	va_start(ap, fmt);
	fp = vfopenf(mode, fmt, ap);
	va_end(ap);
	if (!fp)
		return -1;

	fprintf(fp, "%s\n", str);
	return fclose(fp);
}

/**
 * Same as readllf() except it takes a @a va_list argument.
 */
int vreadllf(long long *value, const char *fmt, va_list ap)
{
	char line[0x100];

	if (!vreadsnf(line, sizeof(line), fmt, ap))
		return -1;

	errno = 0;
	*value = strtoll(line, NULL, 0);

	return errno ? -1 : 0;
}

/**
 * Read 64-bit integer value from a file composed from fmt and optional args.
 * @param value Pointer to where to store read 64-bit integer value.
 * @param mode An fopen() mode string, e.g. "w+".
 * @param fmt  Formatted string to be composed into a pathname.
 *
 * @returns result of operation, with @a errno set on error.
 */
int readllf(long long *value, const char *fmt, ...)
{
	va_list ap;
	int rc;

	va_start(ap, fmt);
	rc = vreadllf(value, fmt, ap);
	va_end(ap);

	return rc;
}


/**
 * Read integer value from a file composed from fmt and optional args.
 * @param value Pointer to where to store read integer value.
 * @param mode An fopen() mode string, e.g. "w+".
 * @param fmt  Formatted string to be composed into a pathname.
 *
 * @returns result of operation, with @a errno set on error.
 */
int readdf(int *value, const char *fmt, ...)
{
	long long tmp;
	va_list ap;
	int rc;

	va_start(ap, fmt);
	rc = vreadllf(&tmp, fmt, ap);
	va_end(ap);

	if (rc)
		return rc;

	if (tmp < INT_MIN || tmp > INT_MAX) {
		errno = ERANGE;
		return -1;
	}

	*value = tmp;
	return 0;
}

/**
 * Write 64-bit integer value to a file composed from fmt and optional args.
 * @param value 64-bit integer value to write.
 * @param mode An fopen() mode string, e.g. "w+".
 * @param fmt  Formatted string to be composed into a pathname.
 *
 * @returns result of operation, with @a errno set on error.
 */
int writellf(long long value, const char *mode, const char *fmt, ...)
{
	va_list ap;
	FILE *fp;

	va_start(ap, fmt);
	fp = vfopenf(mode, fmt, ap);
	va_end(ap);
	if (!fp)
		return -1;

	fprintf(fp, "%lld\n", value);
	return fclose(fp);
}


/**
 * Write integer value to a file composed from fmt and optional args.
 * @param value Integer value to write.
 * @param mode An fopen() mode string, e.g. "w+".
 * @param fmt  Formatted string to be composed into a pathname.
 *
 * @returns result of operation, with @a errno set on error.
 */
int writedf(int value, const char *mode, const char *fmt, ...)
{
	va_list ap;
	FILE *fp;

	va_start(ap, fmt);
	fp = vfopenf(mode, fmt, ap);
	va_end(ap);
	if (!fp)
		return -1;

	fprintf(fp, "%d\n", value);
	return fclose(fp);
}


/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
