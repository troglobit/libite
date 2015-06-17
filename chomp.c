/* Perl inspired chomp() implementation.
 *
 * Copyright (c) 2014  Joachim Nilsson <troglobit@gmail.com>
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

#include <errno.h>
#include <string.h>

/**
 * chomp - Perl like chomp function, chop off last char if newline.
 * @str: String to chomp
 *
 * Returns:
 * If @str is a valid pointer this function returns @str, otherwise
 * @errno is set to %EINVAL and this function returns %NULL.
 */
char *chomp (char *str)
{
   char *p;

   if (!str)
   {
      errno = EINVAL;
      return NULL;
   }

   p = strrchr (str, '\n');
   if (p)
      *p = 0;

   return str;
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: nil
 *  c-file-style: "ellemtel"
 * End:
 */