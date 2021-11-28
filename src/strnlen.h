/* strnlen.h - Re-implementation of GLIBC strnlen()
 *
 * Copyright (c) 2016-2021  Joachim Wiberg <troglobit@gmail.com>
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
 * @file strnlen.h
 * @author Joachim Wiberg
 * @date 2016-2021
 * @copyright ISC License
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LIBITE_STRNLEN_H_
#define LIBITE_STRNLEN_H_

#if !defined(HAVE_STRNLEN)
#if defined(strnlen)
#define HAVE_STRNLEN	1
#endif
#endif

#if !HAVE_STRNLEN
#include <stddef.h>	/* size_t */

#define strnlen(str, lim) xstrnlen(str, lim) /**< Wrapper for xstrnlen() */

/**
 * Reimplementation of GLIBC strnlen()
 * @param str string to return length of
 * @param lim max number of bytes to read
 * @returns length of @p str in bytes, but at most @lim bytes.
 */
static inline size_t xstrnlen(const char *str, size_t lim)
{
	size_t i = 0;

	while (i < lim && str[i])
		i++;

	return i;
}
#endif

#endif /* LIBITE_STRNLEN_H_ */

#ifdef __cplusplus
}
#endif
