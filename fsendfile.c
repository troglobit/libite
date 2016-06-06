/* Copy data between file streams
 *
 * Copyright (c) 2013  Tobias Waldekranz <tobias@waldekranz.com>
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
#include <stdlib.h>


/**
 * fsendfile - copy data between file streams
 * @src: Source stream
 * @dst: Destination stream
 * @len: Number of bytes to copy
 *
 * @dst may be %NULL, in which case @len bytes are read and discarded
 * from @src.  This can be useful for streams where seeking is not
 * permitted.  Additionally, @len may be the special value zero (0), in
 * which case fsendfile() will copy until %EOF is seen on @src.
 *
 * Returns:
 * The number of bytes copied.  If an error is detected -1 is returned
 * and @errno will be set accordingly.
 */
ssize_t fsendfile(FILE *src, FILE *dst, size_t len)
{
	char *buf;
	size_t tot = 0;
	ssize_t blk = BUFSIZ, num = 0;

        if (!src) {
                errno = EINVAL;
                return -1;
        }
                
	buf = (char *)malloc(BUFSIZ);
	if (!buf)
		return -1;

	while (!len || tot < len) {
		if (len && ((len - tot) < BUFSIZ))
			blk = len - tot;

		num = fread(buf, 1, blk, src);
		if (num == 0)
			break;

		if (dst && (fwrite(buf, num, 1, dst) != 1)) {
			num = -1;
			break;
		}

		tot += num;
	}

	free(buf);

	return (num == -1) ? -1 : (ssize_t)tot;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
