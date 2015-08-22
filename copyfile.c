/* Fastinit (finit) copyfile() implementation.
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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "lite.h"


/* Tests if dst is a directory, if so, reallocates dst and appends src filename returning 1 */
static int adjust_target(char *src, char **dst)
{
	int isdir = 0;

	if (fisdir(*dst)) {
		int slash = 0;
		char *tmp, *ptr = strrchr(src, '/');

		if (!ptr)
			ptr = src;
		else
			ptr++;

		tmp = malloc(strlen(*dst) + strlen(ptr) + 2);
		if (!tmp) {
			errno = EISDIR;
			return 0;
		}

		isdir = 1;	/* Free dst before exit! */
		slash = fisslashdir(*dst);

		sprintf(tmp, "%s%s%s", *dst, slash ? "" : "/", ptr);
		*dst = tmp;
	}

	return isdir;
}

/* Actual copy loop, used by both copyfile() and fcopyfile()
 * breaks loop on error and EOF */
static ssize_t do_copy(int in, int out, size_t num, char *buffer, size_t len)
{
	ssize_t ret = 0, size = 0;

	do {
		size_t count = num > len ? len : num;

		ret = read(in, buffer, count);
		if (ret <= 0) {
			if (ret == -1 && EINTR == errno)
				continue;
			break;
		}

		if (ret > 0)
			size += write(out, buffer, ret);
		num  -= count;
	} while (num > 0);

	return size;
}

/**
 * copyfile - Copy a file to another.
 * @src: Full path name to source file.
 * @dst: Full path name to target file.
 * @len: Number of bytes to copy, zero (0) for entire file.
 * @sym: Should symlinks be recreated (1) or followed (0)
 *
 * This is a C implementation of the command line cp(1) utility.  It is one
 * of the classic missing links in the UNIX C library.  This version is from
 * the finit project, http://helllabs.org/finit/, which is a reimplementation
 * of fastinit for the Asus EeePC.
 *
 * Returns:
 * The number of bytes copied, zero may be error (check errno!), but it
 * may also indicate that @src was empty.  If @src is a directory @errno
 * will be set to %EISDIR since copyfile() is not recursive.
 */
ssize_t copyfile(char *src, char *dst, int len, int sym)
{
	char *buffer;
	int in, out, isdir = 0, saved_errno = 0;
	size_t num;
	ssize_t size = 0;
	struct stat st;

	errno = 0;

	buffer = malloc(BUFSIZ);
	if (!buffer)
		return 0;

	if (fisdir(src)) {
		saved_errno = EISDIR;	/* Error: source is a directory */
		goto exit;
	}

	/* Check if target is a directory, then append src filename. */
	isdir = adjust_target(src, &dst);

	/* Check if the source file is a symlink ... */
	if (stat(src, &st)) {
		size = -1;
		goto exit;
	}

	/* ... only try readlink() if symlink and @sym is set. */
	if (sym && (st.st_mode & S_IFMT) == S_IFLNK) {
		size = readlink(src, buffer, BUFSIZ);
		if (size > 0) {
			if (size >= (ssize_t)BUFSIZ) {
				saved_errno = ENOBUFS;
				size = -1;
			} else {
				buffer[size] = 0;
				size = !symlink(buffer, dst);
			}
		}

		/* Don't fall back to copy, that is a potentially
		 * dangerous race condition, see CWE-367. */
		goto exit;
	}

	/* 0: copy entire file */
	if (len == 0)
		num = st.st_size;
	else
		num = (size_t)len;

	in = open(src, O_RDONLY);
	if (in < 0) {
		saved_errno = errno;
		goto exit;
	}

	out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, fmode(src));
	if (out < 0) {
		close(in);
		saved_errno = errno;
		goto exit;
	}

	size = do_copy(in, out, num, buffer, BUFSIZ);
	if (!size && errno)
		saved_errno = errno;

	close(out);
	close(in);

exit:
	free(buffer);
	if (isdir)
		free(dst);
	errno = saved_errno;

	return size;
}

/**
 * movefile - Move a file to another location
 * @src: Source file.
 * @dst: Target file, or location.
 *
 * This is a C implementation of the command line mv(1) utility.
 * Usually the rename() API is sufficient, but not when moving across
 * file system boundaries.
 *
 * The @src argument must include the full path to the source file,
 * whereas the @dst argument may only be a directory, in which case the
 * same file name from @src is used.
 *
 * Returns:
 * POSIX OK(0), or non-zero with errno set.
 */
int movefile(char *src, char *dst)
{
	int isdir, result = 0;

	/* Check if target is a directory, then append the src base filename. */
	isdir = adjust_target(src, &dst);

	if (rename(src, dst)) {
		if (errno == EXDEV) {
			errno = 0;
			copyfile(src, dst, 0, 1);
			if (errno)
				result = 1;
			else
				result = remove(src);
		} else {
			result = 1;
		}
	}

	if (isdir)
		free(dst);

	return result;
}

/**
 * fcopyfile - Copy between FILE *fp.
 * @src: Source FILE.
 * @dst: Destination FILE.
 *
 * Function takes signals into account and will restart the syscalls as
 * long as error is %EINTR.
 *
 * Returns:
 * POSIX OK(0), or non-zero with errno set on error.
 */
int fcopyfile(FILE *src, FILE *dst)
{
	int ret;
	char *buffer;

	if (!src || !dst) {
		errno = EINVAL;
		return -1;
	}

	buffer = malloc(BUFSIZ);
	if (!buffer)
		return -1;

	ret = do_copy(fileno(src), fileno(dst), BUFSIZ, buffer, BUFSIZ);
	if (ret > 0)
		ret = 0;
	else if (errno)
		ret = -1;

	free(buffer);

	return ret;
}

#ifdef UNITTEST
#include <err.h>

int main(void)
{
	int i = 0;
	char *files[] = {
		"/etc/passwd", "/tmp/tok", "/tmp/tok",
		"/etc/passwd", "/tmp/", "/tmp/passwd",
		"/etc/passwd", "/tmp", "/tmp/passwd",
		NULL
	};
	FILE *src, *dst;

	printf("=>Start testing fcopyfile()\n");
	while (files[i]) {
		printf("fcopyfile(%s, %s)\t", files[i], files[i + 1]);
		src = fopen(files[i], "r");
		dst = fopen(files[i + 1], "w");
		if (fcopyfile(src, dst)) {
			if (!fisdir(files[i + 1]))
			    err(1, "Failed fcopyfile(%s, %s)", files[i], files[i + 1]);
		}

		if (src)
			fclose(src);
		if (dst)
			fclose(dst);

		if (fexist(files[i + 2]))
			printf("OK => %s", files[i + 2]);
		printf("\n");

		erase(files[i + 2]);
		i += 3;
	}

	printf("\n=>Start testing copyfile()\n");
	i = 0;
	while (files[i]) {
		printf("copyfile(%s, %s)\t", files[i], files[i + 1]);
		if (!copyfile(files[i], files[i + 1], 0, 0))
			err(1, "Failed copyfile(%s, %s)", files[i], files[i + 1]);

		if (fexist(files[i + 2]))
			printf("OK => %s", files[i + 2]);
		printf("\n");

		erase(files[i + 2]);
		i += 3;
	}

	return 0;
}
#endif /* UNITTEST */

/**
 * Local Variables:
 *  compile-command: "make V=1 -f copyfile.mk"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
