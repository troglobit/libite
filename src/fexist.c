/* Check if file exists
 *
 * Copyright (c) 2008 Claudio Matsuoka <http://helllabs.org/finit/>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file fexist.c
 * @author Claudio Matsuoka
 * @date 2008
 * @copyright MIT License
 */

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include "lite.h"

/**
 * Check if a file exists in the file system.
 * @param file File to look for, with full path.
 *
 * @returns @c TRUE(1) if the @a file exists, otherwise @c FALSE(0).
 */
int fexist(const char *file)
{
	if (!file) {
		errno = EINVAL;
		return 0;	/* Doesn't exist ... */
	}

	if (-1 == access(file, F_OK))
		return 0;

	return 1;
}

/**
 * Like fexist() but with formatted string support
 * @param fmt  Formatted string to be composed into the file to look for.
 *
 * This is a wrapper for the fexist() function in lite.h, lessening the
 * burden of having to compose the filename from parts in a seprate
 * buffer.
 *
 * @returns @c TRUE(1) if the @a file exists, otherwise @c FALSE(0).
 */
int fexistf(const char *fmt, ...)
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

	return fexist(file);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
