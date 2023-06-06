/* Formatted popen()
 *
 * Copyright (c) 2023  Tobias Waldekranz <tobias@waldekranz.com>
 * Copyright (c) 2023  Joachim Wiberg <troglobit@gmail.com>
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

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

/**
 * Formatted pipe stream to or from a process
 * @param type A popen() type, "r" or "w", can also include "e" for @c O_CLOEXEC.
 * @param fmt  Formatted string to be composed into a shell command line.
 *
 * This function is an extension to popen(), lessening the burden of
 * first having to compose the filename from parts in a seprate buffer.
 *
 * @returns Upon successful completion, popenf() returns a FILE pointer.
 * Otherwise, @c NULL is returned and @a errno is set to indicate the
 * error.
 */
FILE *popenf(const char *type, const char *fmt, ...)
{
	va_list ap;
	char *cmd;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	cmd = alloca(len + 1);
	if (!cmd) {
		errno = ENOMEM;
		return NULL;
	}

	va_start(ap, fmt);
	vsnprintf(cmd, len + 1, fmt, ap);
	va_end(ap);

	return popen(cmd, type);
}
