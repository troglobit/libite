/* mkpath() -- Create all components leading up to a given directory
 *
 * Copyright (c) 2013-2021  Joachim Wiberg <troglobit@gmail.com>
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
 * @file makepath.c
 * @author Joachim Wiberg
 * @date 2013-2021
 * @copyright ISC License
 */

#include <errno.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "lite.h"

/**
 * makepath() but takes a mode_t argument.
 * @param dir   Directory to created, relative or absolute
 * @param mode  A &mode_t mode to create @a dir with
 *
 * @returns POSIX OK(0) on success, otherwise -1 with @a errno set.
 */
int mkpath(const char *dir, mode_t mode)
{
	struct stat sb;

	if (!dir) {
		errno = EINVAL;
		return -1;
	}

	if (!stat(dir, &sb))
		return 0;

	mkpath(dirname(strdupa(dir)), mode);

	return mkdir(dir, mode);
}

/**
 * Formatted version of mkpath().
 * @param mode  A mode_t mode to create directories with
 * @param fmt   Formatted string to be composed into a pathname
 *
 * @note Notice the swapped arguments, compared to mkpath()!
 *
 * @returns POSIX OK(0) on success, otherwise -1 with @a errno set.
 */
int fmkpath(mode_t mode, const char *fmt, ...)
{
	va_list ap;
	char *path;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	path = alloca(len + 1);
	if (!path) {
		errno = ENOMEM;
		return -1;
	}

	va_start(ap, fmt);
	len = vsnprintf(path, len + 1, fmt, ap);
	va_end(ap);

	return mkpath(path, mode);
}

/**
 * Create all components of the specified directory.
 * @param dir  Directory to create.
 *
 * @note It is strongly recommended to use mkpath() over this function
 * since it has the @a mode argument while this function default to
 * 0777, which in most cases is insecure.
 *
 * @returns POSIX OK (0) on success, otherwise -1 and @a errno set
 * appropriately.
 * 
 * @exception EINVAL on bad argument, or
 * @exception ENOMEM when it fails allocating temporary memory.
 *
 * For other error codes see the mkdir(2) syscall description.
 */
int makepath(const char *dir)
{
	return mkpath(dir, 0777);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
