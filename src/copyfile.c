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

/**
 * @file copyfile.c
 * @author Claudio Matsuoka
 * @date 2008
 * @copyright MIT License
 */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include "lite.h"

/* Tests if dst is a directory, if so, reallocates dst and appends src filename returning 1 */
static int adjust_target(const char *src, char **dst)
{
	int isdir = 0;

	if (fisdir(*dst)) {
		int slash = 0;
		char *tmp, *ptr = strrchr(src, '/');

		if (!ptr)
			ptr = (char *)src;
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

static void set_mtime(int in, int out)
{
	struct stat st;
	struct timespec tv[2];

	if (fstat(in, &st))
		return;

	tv[0] = st.st_atim;
	tv[1] = st.st_mtim;
	futimens(out, tv);
}

/**
 * Copy a file to another.
 * @param src Full path name to source file.
 * @param dst Full path name to target file.
 * @param len Number of bytes to copy, zero (0) for entire file.
 * @param opt An option mask of ::LITE_FOPT_COPYFILE_SYM, ::LITE_FOPT_KEEP_MTIME
 *
 * This is a C implementation of the command line cp(1) utility.  It is one
 * of the classic missing links in the UNIX C library.  This version is from
 * the finit project, http://helllabs.org/finit/, which is a reimplementation
 * of fastinit for the Asus EeePC.
 *
 * The @a opt field replaces the @a sym argument in previous releases
 * and works as follows.  To maintain backwards compatibility with @a
 * sym the ::LITE_FOPT_COPYFILE_SYM has a value of @c 1.  Supported
 * option flags are:
 *
 * - ::LITE_FOPT_COPYFILE_SYM  Recreate symlink or follow to copy target
 * - ::LITE_FOPT_KEEP_MTIME    Preserve modification time
 *
 * @returns The number of bytes copied, or zero, which may be an error
 * (check @a errno, see Exceptions below), but it may also indicate that
 * @a src was empty.  See exceptions, below.
 *
 * @exception EISDIR if @a src is a directory, since copyfile() is not recursive.
 */
ssize_t copyfile(const char *src, const char *dst, int len, int opt)
{
	char *buffer, *dest = (char *)dst;
	int sym = (opt & LITE_FOPT_COPYFILE_SYM) != 0;
	int keep_mtim = (opt & LITE_FOPT_KEEP_MTIME) != 0;
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
	isdir = adjust_target(src, &dest);

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
				size = !symlink(buffer, dest);
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

	out = open(dest, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);
	if (out < 0) {
		close(in);
		saved_errno = errno;
		goto exit;
	}

	size = do_copy(in, out, num, buffer, BUFSIZ);
	if (!size && errno)
		saved_errno = errno;
	else if (keep_mtim)
		set_mtime(in, out);

	close(out);
	close(in);

exit:
	free(buffer);
	if (isdir)
		free(dest);
	errno = saved_errno;

	return size;
}

/**
 * Move a file to another location
 * @param src Source file.
 * @param dst Target file, or location.
 *
 * This is a C implementation of the command line mv(1) utility.
 * Usually the rename() API is sufficient, but not when moving across
 * file system boundaries.
 *
 * The @p src argument must include the full path to the source file,
 * whereas the @p dst argument may only be a directory, in which case
 * the same file name from @p src is used.
 *
 * @returns POSIX OK(0), or non-zero with @a errno set.
 */
int movefile(const char *src, const char *dst)
{
	char *dest = (char *)dst;
	int isdir, result = 0;

	/* Check if target is a directory, then append the src base filename. */
	isdir = adjust_target(src, &dest);

	if (rename(src, dest)) {
		if (errno == EXDEV) {
			errno = 0;
			copyfile(src, dest, 0, 1);
			if (errno)
				result = 1;
			else
				result = remove(src);
		} else {
			result = 1;
		}
	}

	if (isdir)
		free(dest);

	return result;
}

/**
 * Copy between FILE *fp.
 * @param src Source FILE.
 * @param dst Destination FILE.
 *
 * @returns POSIX OK(0), or non-zero with @a errno set on error.
 */
int fcopyfile(FILE *src, FILE *dst)
{
	char *buf;

	if (!src || !dst) {
		errno = EINVAL;
		return -1;
	}

	buf = malloc(BUFSIZ);
	if (!buf)
		return -1;

	while (fgets(buf, BUFSIZ, src))
		fputs(buf, dst);
	
	free(buf);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
