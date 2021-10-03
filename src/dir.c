/* Functions for operating on files in directories.
 *
 * Copyright (c) 2008-2021  Joachim Wiberg <troglobit@gmail.com>
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
 * @file dir.c
 * @author Joachim Wiberg
 * @date 2008-2021
 * @copyright ISC License
 */

#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

static const char *matcher_type = NULL;
static int (*matcher_filter) (const char *file) = NULL;

static int matcher(const struct dirent *entry)
{
	char *pos = strrchr(entry->d_name, '.');

	if (matcher_filter && !matcher_filter(entry->d_name))
		/* User matcher overrides the rest. */
		return 0;

	/* Skip current dir "." from list of files. */
	if ((1 == strlen(entry->d_name) && entry->d_name[0] == '.') ||
	    (2 == strlen(entry->d_name) && !strcmp(entry->d_name, "..")))
		return 0;

	/* filetype == "" */
	if (matcher_type[0] == 0)
		return 1;

	/* Entry has no "." */
	if (!pos)
		return 0;

	return !strcmp(pos, matcher_type);
}

/**
 * List all files of a certain type in the given directory.
 * @param dir    Base directory for dir operation.
 * @param type   File type suffix, e.g. ".cfg".
 * @param filter Optional file name filter.
 * @param list   Pointer to an array of file names.
 * @param strip  Flag, if set dir() strips the file type.
 *
 * This function returns a @a list of files, matching the @a type
 * suffix, in the given directory @a dir.
 *
 * The @a list argument is a pointer to where to store the dynamically
 * allocated list of file names.  This list should be free'd by first
 * calling free() on each file name and then on the list itself.
 *
 * If @a filter is not @c NULL it will be called for each file found.
 * If @a filter returns non-zero the @a file argument is included in the
 * resulting @a list.  If @a filter returns zero for given @a file it is
 * discarded.  If the @a strip flag is set the resulting @a list of
 * files has their file type stripped, including the dot.  So a match
 * "config0.cfg" would be returned as "config0".
 *
 * @returns The number of files in @a list, zero if no matching files of
 * @a type, or non-zero on error with @a errno set.
 */
int dir(const char *dir, const char *type, int (*filter) (const char *file), char ***list, int strip)
{
	int i, n, num = 0;
	char **files;
	struct dirent **namelist;

	if (!list) {
		errno = EINVAL;
		return -1;
	}

	if (!dir)
		/* Assuming current directory */
		dir = ".";
	if (!type)
		/* Assuming all files. */
		type = "";

	matcher_type = type;
	matcher_filter = filter;
	n = scandir(dir, &namelist, matcher, alphasort);
	if (n < 0)
		return -1;

	if (n > 0) {
		files = (char **)malloc(n * sizeof(char *));
		for (i = 0; i < n; i++) {
			if (files) {
				char *name = namelist[i]->d_name;
				char *type = strrchr(name, '.');

				if (type && strip)
					*type = 0;

				files[i] = strdup(name);
				num++;
			}
			free(namelist[i]);
		}
		if (num)
			*list = files;
	}

	if (namelist)
		free(namelist);

	return num;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
