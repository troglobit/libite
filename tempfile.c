/* A secure tmpfile() replacement.
 *
 * Copyright (c) 2015  Joachim Nilsson <troglobit@gmail.com>
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

#include <paths.h>
#include <fcntl.h>		/* O_TMPFILE requires -D_GNU_SOURCE */
#ifdef __linux__ 
#include <linux/version.h>
#endif
#include <stdlib.h>		/* mkstemp() */
#include <stdio.h>		/* fdopen() */
#include <sys/stat.h>		/* umask() */

#ifdef __linux__ 
#ifndef  O_TMPFILE		/* Too old GLIBC or kernel */
#warning O_TMPFILE missing on your system, tempfile() may not work!
#define  __O_TMPFILE 020000000
#define  O_TMPFILE (__O_TMPFILE | O_DIRECTORY) /* Define and let it fail at runtime */
#endif
#endif

/**
 * tempfile - A secure tmpfile() replacement
 *
 * This is the secure replacement for tmpfile() that does not exist in
 * GLIBC.  The function uses the Linux specific %O_TMPFILE and %O_EXCL
 * for security.  When the %FILE is fclose()'ed the file contents is
 * lost.  The file is hidden in the %_PATH_TMP directory on the system.
 *
 * This function requires Linux 3.11, or later, due to %O_TMPFILE.
 *
 * Returns:
 * An open %FILE pointer, or %NULL on error.
 */
FILE *tempfile(void)
{
#ifndef __linux__
#warning Not on Linux, reverting to wrap tmpfile(), which may or may not be safe ...
	return tmpfile();
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
	int fd;
	mode_t oldmask;

	oldmask = umask(0077);
	fd = open(_PATH_TMP, O_TMPFILE | O_RDWR | O_EXCL | O_CLOEXEC, S_IRUSR | S_IWUSR);
	umask(oldmask);
	if (-1 == fd)
		return NULL;

	return fdopen(fd, "rw");
#else
#warning Too old kernel, reverting to wrap unsafe tmpfile() ...
	return tmpfile();
#endif
#endif
}

#ifdef UNITTEST
int main(void)
{
	FILE *fp = tempfile(); system("ls -lrt "
	_PATH_TMP " | tail -10"); return fclose(fp);
}
#endif

/**
 * Local Variables:
 *  compile-command: "make V=1 -f tempfile.mk"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
