/* Check if directory exists
 *
 * Copyright (c) 2008 Claudio Matsuoka <http://helllabs.org/finit/>
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * fexist - Check if a file exists and is a directory.
 * @file: File to look for, with full path.
 *
 * Returns:
 * %TRUE(1) if the file exists and is a directory, otherwise %FALSE(0).
 */
int fisdir(char *file)
{
	struct stat sb;

	if (!stat(file, &sb) && S_ISDIR(sb.st_mode))
		return 1;

	return 0;
}

#ifdef UNITTEST
#include "lite.h"

int main(void)
{
	int i = 0;
	struct { char *file; int exist; } arr[] = {
		{ "/etc/passwd", 0 },
		{ "/etc/",       1 },
		{ "/sbin/init",  0 },
		{ "/dev/null",   0 },
		{ "/dev/",       1 },
		{ NULL,  0 },
	};
	FILE *src, *dst;

	for (i = 0; i < NELEMS(arr); i++) {
		if (fisdir(arr[i].file) != arr[i].exist)
			err(1, "Failed fisdir(%s)", arr[i].file ?: "NULL");
		else
			printf("fisdir() %-11s %-18s => OK!\n", arr[i].file ?: "NULL",
			       arr[i].exist ? "is a directory" : "is NOT a directory");
	}

	return 0;
}
#endif /* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "make V=1 -f fisdir.mk"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
