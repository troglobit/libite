/*	$OpenBSD: strtonum.c,v 1.7 2013/04/17 18:40:58 tedu Exp $	*/

/*
 * Copyright (c) 2004 Ted Unangst and Todd Miller
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any
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
 * @file strtonum.c
 * @author Ted Unangst
 * @author Todd Miller
 * @date 2004
 * @copyright ISC License
 */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#ifndef strtonum
#define	INVALID		1		/**< internal */
#define	TOOSMALL	2		/**< internal */
#define	TOOLARGE	3		/**< internal */

#ifndef LLONG_MAX
# define LLONG_MAX	0x7fffffffffffffffLL        /**< internal */
#endif

#ifndef LLONG_MIN
# define LLONG_MIN	(-0x7fffffffffffffffLL - 1) /**< internal */
#endif

/**
 * Reliably convert string value to an integer
 * @param numstr  String to convert to a number
 * @param minval  Lower bound to check number against
 * @param maxval  Upper bound to check number against
 * @param errstrp Pointer to error string
 *
 * This function converts the string in @p numstr to a long long value.
 * The function was designed to facilitate safe, robust programming and
 * overcome the shortcomings of the atoi(3) and strtol(3) family of
 * interfaces.
 *
 * The string may begin with an arbitrary amount of whitespace (as
 * determined by isspace(3)) followed by a single optional ‘+’ or ‘-’
 * sign.
 *
 * The remainder of the string is converted to a long long value
 * according to base 10.
 *
 * The value obtained is then checked against the provided @p minval and
 * @p maxval bounds. If @p errstrp is non-NULL, strtonum() stores an
 * error string in @p *errstrp indicating the failure.
 *
 * @returns The result of the conversion, unless the value would exceed
 * the provided bounds or is invalid. On error, 0 is returned, @a errno
 * is set, and @p errstrp points to an error message. @p *errstr* is set
 * to @c NULL on success; this fact can be used to differentiate a
 * successful return of 0 from an error.
 */
long long
strtonum(const char *numstr, long long minval, long long maxval,
    const char **errstrp)
{
	long long ll = 0;
	int error = 0;
	char *ep;
	struct errval {
		const char *errstr;
		int err;
	} ev[4] = {
		{ NULL,		0 },
		{ "invalid",	EINVAL },
		{ "too small",	ERANGE },
		{ "too large",	ERANGE },
	};

	ev[0].err = errno;
	errno = 0;
	if (minval > maxval) {
		error = INVALID;
	} else {
		ll = strtoll(numstr, &ep, 10);
		if (errno == EINVAL || numstr == ep || *ep != '\0')
			error = INVALID;
		else if ((ll == LLONG_MIN && errno == ERANGE) || ll < minval)
			error = TOOSMALL;
		else if ((ll == LLONG_MAX && errno == ERANGE) || ll > maxval)
			error = TOOLARGE;
	}
	if (errstrp != NULL)
		*errstrp = ev[error].errstr;
	errno = ev[error].err;
	if (error)
		ll = 0;

	return (ll);
}
#endif
