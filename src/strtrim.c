/* Trim a string from whitespace
 *
 * Copyright (c) 2014  Mattias Walström <lazzer@gmail.com>
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
 * @file strtrim.c
 * @author Mattias Walström
 * @author Joachim Wiberg
 * @date 2014,2021
 * @copyright ISC License
 */

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "lite.h"

/**
 * Strip leading and trailing whitespace from a string
 * @param str  The string to trim
 *
 * Trims a string from any leading and trailing white-space, returns the
 * trimmed result in the same buffer.
 *
 * @returns If @p str is a valid, non-NULL string this function returns
 * the same string stripped from whitespace.  This function only returns
 * @c NULL if @p str itself is @c NULL.
 */
char *strtrim(char *str)
{
	char *start, *end;

	if (!str) {
		errno = EINVAL;
		return NULL;
	}

	start = str;
	while (isspace(*start))
		start++;

	if (*start == 0) {
		str[0] = 0;
		return str;
	}

	end = start + strlen(start) - 1;
	while (end > start && isspace(*end))
		end--;
	*(++end) = 0;

	memmove(str, start, end - start + 1);

	return str;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
