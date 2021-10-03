/* Micro "rsync" implementation.
 *
 * Copyright (c) 2011-2021  Joachim Wiberg <troglobit@gmail.com>
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

/**
 * @file rsync.c
 * @author Joachim Wiberg
 * @date 2011-2021
 * @copyright ISC License
 */

#include <errno.h>
#include <fcntl.h>	/* AT_* macros */
#include <stdlib.h>	/* NULL, free() */
#include <string.h>	/* strlen() */
#include <strings.h>	/* rindex() */
#include <stdio.h>
#include <sys/param.h>	/* MAX(), isset(), setbit(), TRUE, FALSE, et consortes. :-) */
#include <sys/stat.h>
#include <sys/types.h>

#include "lite.h"

static int copy(char *src, char *dst, int opt);
static int mdir(char *buf, size_t buf_len, char *dir, char *name, struct stat *st);
static int prune(char *dst, char **new_files, int new_num);
static int set_mtime(char *fn, struct stat *st);


/**
 * Synchronize contents and optionally remove non-existing backups
 * @param src     Source directory
 * @param dst     Destination directory
 * @param opt     An option mask of ::LITE_FOPT_RSYNC_DELETE, ::LITE_FOPT_KEEP_MTIME
 * @param filter  Optional filtering function for source directory.
 *
 * This is a miniature implementation of the famous rsync for local use
 * only.  In fact, it is not even a true rsync since it copies all files
 * from @p src to @p dst.  The ::LITE_FOPT_RSYNC_DELETE @p opt flag is
 * useful for creating backups, when set all files removed from src
 * since last backup are pruned from the destination (backup) directory.
 *
 * The @p opt parameter to rsync() is an option mask for the most common
 * rsync(1) options.  Previously this argument was called @p delete and
 * to maintain backwards compatibility the value 1 is reserved:
 *
 * %LITE_FOPT_RSYNC_DELETE: Prune files from @p dst that no longer exist in @p src.
 * %LITE_FOPT_KEEP_MTIME:   Preserve modification time
 *
 * The filter callback, @p filter, if provided, is used to determine
 * what files to include from the source directory when backing up.  If
 * a file is to be skipped the callback should simply return zero.
 *
 * Returns:
 * POSIX OK(0), or non-zero with @a errno set on error.
 */
int rsync(char *src, char *dst, int opt, int (*filter)(const char *file))
{
	char source[256];
	char dest[256];
	int delete = (opt & LITE_FOPT_RSYNC_DELETE) != 0;
	int keep_mtim = (opt & LITE_FOPT_KEEP_MTIME);
	int i = 0, num = 0, result = 0, do_mtim = 0;
	char **files;		/* Array of file names. */
	struct stat st;

	if (!fisdir(dst))
		makedir(dst, 0755);

	if (!fisdir(src)) {
		if (!fexist(src))
			return 1;

		if (copy(src, dst, keep_mtim))
			result++;

		return errno;
	}

	/* Copy dir as well? */
	if (!fisslashdir(src)) {
		char *ptr = rindex(src, '/');

		if (!ptr)
			ptr = src;
		else
			ptr++;

		if (stat(src, &st))
			return 1;

		if (mdir(dest, sizeof(dest), dst, ptr, &st))
			return 1;
		dst = dest;
		do_mtim = keep_mtim;
	}

	num = dir(src, "", filter, &files, 0);
	for (i = 0; i < num; i++) {
		/* Recursively copy sub-directries */
		snprintf(source, sizeof(source), "%s%s%s", src, fisslashdir(src) ? "" : "/", files[i]);
		if (fisdir(source)) {
			char dst2[256];
			struct stat sb;

			strcat(source, "/");
			if (stat(source, &sb)) {
				result++;
				continue;
			}

			if (mdir(dst2, sizeof(dst2), dst, files[i], &sb)) {
				result++;
				continue;
			}

			rsync(source, dst2, opt, filter);
			if (keep_mtim)
				set_mtime(dst2, &sb);

			continue;	/* Next file/dir in @src to copy... */
		}

		if (copy(source, dst, keep_mtim))
			result++;
	}

	if (do_mtim)
		set_mtime(dest, &st);

	/* We ignore any errors from the pruning, that phase albeit useful is only
	 * cosmetic. --Jocke 2011-03-24 */
	if (delete)
		prune(dst, files, num);

	if (num) {
		for (i = 0; i < num; i++)
			free(files[i]);
		free(files);
	}

	return result;
}

static int copy(char *src, char *dst, int opt)
{
	copyfile(src, dst, 0, opt | LITE_FOPT_COPYFILE_SYM);
	if (errno) {
		if (errno != EEXIST)
			return 1;

		errno = 0;
	}

	return 0;
}

/* Creates dir/name @mode ... skipping / if dir already ends so. */
static int mdir(char *buf, size_t buf_len, char *dir, char *name, struct stat *st)
{
	snprintf(buf, buf_len, "%s%s%s", dir, fisslashdir(dir) ? "" : "/", name);
	if (mkdir(buf, st->st_mode)) {
		if (EEXIST != errno)
			return 1;

		errno = 0;
	}

	return 0;
}


static int find(char *file, char **files, int num)
{
	int n;

	for (n = 0; n < num; n++)
		if (!strncmp (files[n], file, MAX(strlen(files[n]), strlen(file))))
			return 1;

	return 0;
}


/* Prune old files, no longer existing on source, from destination directory. */
static int prune(char *dst, char **new_files, int new_num)
{
	int num, result = 0;
	char **files;

	num = dir(dst, "", NULL, &files, 0);
	if (num) {
		int i;

		for (i = 0; i < num; i++) {
			if (!find(files[i], new_files, new_num)) {
				char *name;
				size_t len = strlen(files[i]) + 2 + strlen(dst);

				name = malloc(len);
				if (name) {
					snprintf(name, len, "%s%s%s", dst, fisslashdir(dst) ? "" : "/", files[i]);
					if (remove(name))
						result++;
					free(name);
				}
			}
			free(files[i]);
		}
		free(files);
	}

	return result;
}

static int set_mtime(char *fn, struct stat *st)
{
	struct timespec tv[2];

	tv[0] = st->st_atim;
	tv[1] = st->st_mtim;

	return utimensat(AT_FDCWD, fn, tv, AT_SYMLINK_NOFOLLOW);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
