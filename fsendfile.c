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
size_t fsendfile(FILE *src, FILE *dst, size_t len)
{
	char *buf;
	size_t blk = BUFSIZ, num = 0, tot = 0;

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

	return (num == (size_t)-1) ? (size_t)-1 : tot;
}

#ifdef UNITTEST
#include <err.h>
#include <unistd.h>

int main(void)
{
	int i = 0;
	char *files[] = {
		"/etc/passwd", "/tmp/tok",
		"/etc/passwd", "/tmp/passwd",
		"/etc/passwd", "/tmp/passwd",
		NULL
	};
	FILE *src, *dst;

	while (files[i]) {
                src = fopen(files[i], "r");
                dst = fopen(files[i + 1], "w");
		printf("fsendfile(%s, %s, 512)\t", files[i], files[i + 1]);
		if (-1 == fsendfile(src, dst, 512))
			err(1, "Failed fsendfile(%s, %s)", files[i], files[i + 1]);

		if (!access(files[i + 1], F_OK))
			printf("OK => %s\n", files[i + 1]);

		remove(files[i + 1]);
		i += 2;
	}

	return 0;
}
#endif /* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "make V=1 -f fsendfile.mk"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
