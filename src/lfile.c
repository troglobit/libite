/* Parse UNIX /etc configuration files like /etc/protocols and /etc/services
 *
 * Copyright (c) 2015-2021  Joachim Wiberg <troglobit@gmail.com>
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
 * @file lfile.c
 * @author Joachim Wiberg
 * @date 2015-2021
 * @copyright ISC License
 */

#include <errno.h>
#include <stdio.h>		/* FILE */
#include <stdlib.h>		/* atoi() */
#include <string.h>		/* strlen(), strncmp(), strtok_r() */
#include <sys/param.h>		/* MAX() */

/** @private for internal use only */
typedef struct lfile {
	FILE *fp;		/**< FILE pointer to current stream */
	char buf[256];		/**< Internal buffer, limted to 256 bytes per line */
	char *save;		/**< Internal save pointer */
	const char *sep;	/**< Record separator, from lfopen() */
} lfile_t;

/**
 * Open file and return parsing context.
 * @param file  File to parse
 * @param sep   Separator(s) to use in lftok()
 *
 * @returns Pointer to an @a lfile_t parser context, or @c NULL on error.
 */
lfile_t *lfopen(const char *file, const char *sep)
{
	lfile_t *lf;

	if (!file || !sep) {
		errno = EINVAL;
		return NULL;
	}

	/* Use calloc for clearing buf on behalf of lftok() */
	lf = calloc(sizeof(*lf), sizeof(char));
	if (lf) {
		lf->fp   = fopen(file, "r");
		lf->sep  = sep;
		lf->save = lf->buf;

		if (!lf->fp) {
			free(lf);
			return NULL;
		}
	}

	return lf;
}

/**
 * Close a parser context.
 * @param lf: Pointer to @a lfile_t parser context from lfopen()
 */
void lfclose(lfile_t *lf)
{
	if (!lf)
		return;

	if (lf->fp)
		fclose(lf->fp);
	free(lf);
}

/**
 * Get next token in file
 * @param lf: Pointer to @a lfile_t parser context from lfopen()
 *
 * @returns Next token, read from file previously opened with lfopen(),
 * or @c NULL if EOF.
 */
char *lftok(lfile_t *lf)
{
	char *token;

	if (!lf || !lf->fp || !lf->sep) {
		errno = EINVAL;
		return NULL;
	}

	token = strtok_r(NULL, lf->sep, &lf->save);
	if (token)
		return token;

	while (fgets(lf->buf, sizeof(lf->buf), lf->fp)) {
		if (lf->buf[0] == '#')
			continue;

		token = strtok_r(lf->buf, lf->sep, &lf->save);
		if (token)
			return token;
	}

	errno = ENOENT;
	return NULL;
}

/**
 * Find key in file
 * @param lf   Pointer to @a lfile_t parser context from lfopen()
 * @param key  Key to look for
 *
 * Locate @a key from the current position in the file parser context
 * returned from lfopen().  Please note, the search for @a key does not
 * start from the beginning of the file, it searches from the current
 * position.  To restart search from the beginning use rewind() on the
 * lf->fp.
 *
 * @returns The value to @a key, or @c NULL if not found.
 */
char *lfgetkey(lfile_t *lf, const char *key)
{
	char *token;

	while ((token = lftok(lf))) {
		if (token[0] == '#')
			continue;

		if (!strncmp(token, key, MAX(strlen(token), strlen(key))))
			return lftok(lf);
	}

	return NULL;
}

/**
 * Same as lfgetkey() but returns an integer.
 * @param lf   Pointer to @a lfile_t parser context from lfopen()
 * @param key  Key to look for
 *
 * This function is the same as lfgetkey() but returns the positive
 * integer value for the matching @a key, if found.
 *
 * @returns The positive integer value for @a key, or -1 if not found.
 */
int lfgetint(lfile_t *lf, const char *key)
{
	char *token = lfgetkey(lf, key);

	if (token)
		return atoi(token);

	return -1;
}

/**
 * Find the integer value for key in a file.
 * @param file  File to search for @a key
 * @param sep   Separator for tokens in @a file
 * @param key   Key to look for in @a file
 *
 * This is a convenience wrapper for lfopen(), lfgetint(), and
 * lfclose().
 *
 * @returns The positive integer value for @a key, or -1 if not found.
 */
int fgetint(const char *file, const char *sep, const char *key)
{
	int val = -1;
	lfile_t *lf;

	lf = lfopen(file, sep);
	if (lf) {
		val = lfgetint(lf, key);
		lfclose(lf);
	}

	return val;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
