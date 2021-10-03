/* Simple string matcher function, finds partial matches.
 *
 * Copyright (c) 2009-2021  Joachim Wiberg <troglobit@gmail.com>
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
 * @file strmatch.c
 * @author Joachim Wiberg
 * @date 2009-2021
 * @copyright ISC License
 */

#include <errno.h>
#include <string.h>


/**
 * Finds matching strings from a finite list
 * @param str   String to look for
 * @param list  List of strings to search.
 * @param num   Number of entries in @p list.
 *
 * This function searches the @p list of strings for @p str.  If a
 * (partial) match is found it returns the index in the @p list.
 *
 * Very similar in function to strmatch(), but works for sets of strings
 * that are not @c NUL terminated.
 *
 * @returns -1 on error, otherwise the index to the matching string.
 */
int strnmatch(const char *str, const char **list, size_t num)
{
	size_t i;

	if (!str || !list) {
		errno = EINVAL;
		return -1;
	}

	for (i = 0; i < num; i++) {
		if (!strncasecmp (str, list[i], strlen (str)))
			return i;
	}

	errno = ENOENT;
	return -1;
}

/**
 * Finds matching strings from a list
 * @param str   String to look for.
 * @param list  NUL terminated list of strings to search.
 *
 * This function searches the @p list of strings for @p str.  If a
 * (partial) match is found it returns the index in the @p list.
 *
 * Please note, the @p list MUST be terminated by a NUL element.  If
 * that is not possible, we recommend using strnmatch() instead.
 *
 * @returns -1 on error, otherwise the index to the matching string.
 */
int strmatch(const char *str, const char **list)
{
	size_t i;

	if (!list) {
		errno = EINVAL;
		return -1;
	}

	for (i = 0; list[i]; i++)
		;

	return strnmatch(str, list, i);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
