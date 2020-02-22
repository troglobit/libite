/* Collection of frog DNA
 *
 * Copyright (c) 2008-2020  Joachim Nilsson <troglobit@gmail.com>
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

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LITE_STRING_H_
#define LITE_STRING_H_

#include <stdint.h>    /* uint8_t, uint16_t, uint32_t, INT32_MAX, etc. */
#include <string.h>
#include <sys/param.h> /* MAX(), isset(), setbit(), TRUE, FALSE, et consortes. :-) */

#include "strdupa.h"
#include "strndupa.h"
#include "strnlen.h"

#ifndef min
#define min(a,b)				\
	({					\
		__typeof__ (a) _a = (a);	\
		__typeof__ (b) _b = (b);	\
		_a < _b ? _a : _b;		\
	})
#endif
#ifndef max
#define max(a,b)				\
	({					\
		__typeof__ (a) _a = (a);	\
		__typeof__ (b) _b = (b);	\
		_a > _b ? _a : _b;		\
	})
#endif

int     strnmatch  (const char *str, const char **list, size_t num);
int     strmatch   (const char *str, const char **list);

#ifndef strlcpy
size_t  strlcpy    (char *dst, const char *src, size_t siz);
#endif
#ifndef strlcat
size_t  strlcat    (char *dst, const char *src, size_t siz);
#endif
#ifndef strtonum
long long strtonum (const char *numstr, long long minval, long long maxval, const char **errstrp);
#endif

/* Convert string to natural number (0-2147483647), returns -1 on error. */
static inline int atonum(const char *str)
{
	int val = -1;
	const char *errstr;

	if (str) {
		val = strtonum(str, 0, INT32_MAX, &errstr);
		if (errstr)
			return -1;
	}

	return val;
}

/* Validate string, non NULL and not zero length */
static inline int string_valid(const char *s)
{
   return s && strlen(s);
}

/* Relaxed comparison, e.g., sys_string_match("small", "smaller") => TRUE */
static inline int string_match(const char *a, const char *b)
{
   size_t min = MIN(strlen(a), strlen(b));

   return !strncasecmp(a, b, min);
}

/* Strict comparison, e.g., sys_string_match("small", "smaller") => FALSE */
static inline int string_compare(const char *a, const char *b)
{
   return strlen(a) == strlen(b) && !strcmp(a, b);
}

/* Strict comparison, like sys_string_compare(), but case insensitive,
 * e.g., sys_string_match("small", "SmAlL") => TRUE
 */
static inline int string_case_compare(const char *a, const char *b)
{
   return strlen (a) == strlen (b) && !strcasecmp (a, b);
}

#endif /* LITE_STRING_H_ */

#ifdef __cplusplus
}
#endif
