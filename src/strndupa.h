/* ==========================================================================
 * strndupa.h - Re-implementation of glibc strndupa.
 * --------------------------------------------------------------------------
 * Copyright (c) 2009  William Ahern
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ==========================================================================
 */

/**
 * @file strndupa.h
 * @author William Ahern
 * @date 2009
 * @copyright MIT License
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LIBITE_STRNDUPA_H_
#define LIBITE_STRNDUPA_H_

#if !defined(HAVE_STRNDUPA)
#if defined(strndupa)
#define HAVE_STRNDUPA	1
#endif
#endif

#if !HAVE_STRNDUPA
#if defined(__GNUC__)
#include <stddef.h>	/* size_t */
#include <string.h>	/* memcpy(3) */
#include "strnlen.h"

/**
 * Duplicate part of string on stack
 * @param src string to duplicate
 * @param lim number of bytes to dupliate
 * @returns the result of memcpy(3)
 */
#define strndupa(src, lim) (__extension__ ({		\
	size_t len_	= strnlen(src, lim);		\
	char *dst_	= __builtin_alloca(len_ + 1);	\
	dst_[len_]	= '\0';				\
	(char *)memcpy(dst_, src, len_);		\
}))

#else  /* If not GCC, e.g. Clang */
+#error strndupa() may use an unsupported GNU C API, please forward any fix to maintainer, cheers!
#endif /* __GNUC__ */
#endif /* !HAVE_STRNDUPA */

#endif /* LIBITE_STRNDUPA_H */

#ifdef __cplusplus
}
#endif
