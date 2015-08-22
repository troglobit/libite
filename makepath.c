/* mkpath() -- Create all components leading up to a given directory
 *
 * Copyright (c) 2013  Joachim Nilsson <troglobit@gmail.com>
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
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int mkpath(char *dir, mode_t mode)
{
	if (!dir) {
		errno = EINVAL;
		return 1;
	}

	if (strlen(dir) == 1 && dir[0] == '/')
		return 0;

	mkpath(dirname(strdupa(dir)), mode);

	return mkdir(dir, mode);
}

/**
 * makepath - Create all components of the specified directory.
 * @dir: Directory to create.
 *
 * Returns:
 * POSIX OK (0) on success, otherwise -1 and errno set appropriately.
 * This function returns EINVAL on bad argument, or ENOMEM when it
 * fails allocating temporary memory.  For other error codes see the
 * mkdir() syscall description.
 */
int makepath(char *dir)
{
	return mkpath(dir, 0777);
}

/********************************* UNIT TESTS ************************************/
#ifdef UNITTEST
#include "lite.h"

int checkpath(char *dir)
{
	char tmp[256];
	struct stat sb;

	snprintf(tmp, sizeof(tmp), "ls -ld %s", dir);
	if (system(tmp))
		perror("system");

	if (!stat(dir, &sb) && S_ISDIR(sb.st_mode))
		return 0;

	errno = ENOTDIR;
	return 1;
}

int test_makepath(char *dir)
{
	int ret = makepath(dir);

	if (!ret)
		ret = checkpath(dir);
	if (ret)
		perror("Failed");

	return ret;
}

int main(void)
{
	int i, ret = 0;
	char *list[] = {
		"/tmp/tok/",
		"/tmp/tok2",
		"/tmp/ab",
		"/tmp/b",
		"/tmp/a/",
		"/tmp/a/b",
		"/tmp/a/c/",
		NULL
	};

	for (i = 0; list[i]; i++)
		rmdir(list[i]);

	printf("Testing makepath() ...\n");
	for (i = 0; list[i] && !ret; i++)
		ret = test_makepath(list[i]);

	printf("\nCleaning up ...\n");
	for (i = 0; list[i]; i++)
		rmdir(list[i]);

	return ret;
}
#endif  /* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "make V=1 -f makepath.mk"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
