/* Parse UNIX /etc configuration files like /etc/protocols and /etc/services
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

#include <errno.h>
#include <stdio.h>		/* FILE */
#include <stdlib.h>		/* atoi() */
#include <string.h>		/* strlen(), strncmp(), strtok_r() */
#include <sys/param.h>		/* MAX() */

typedef struct lfile {
	FILE *fp;
	char buf[256];
	char *sep, *save;
} lfile_t;

/**
 * lfopen - Open file and return parsing context
 * @file: File to parse
 * @sep:  Separator(s) to use in lftok()
 *
 * Returns:
 * Pointer to a &lftile_t parser context, or %NULL on error.
 */
lfile_t *lfopen(char *file, char *sep)
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
 * lfclose - Close a parser context
 * @lf: Pointer to &lfile_t context from lfopen()
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
 * lftok - Get next token in file
 * @lf: Pointer to &lfile_t context from lfopen()
 *
 * Returns:
 * Next token, read from file previously opened with lfopen(),
 * or %NULL if EOF.
 */
char *lftok(lfile_t *lf)
{
	char *token;

	if (!lf || !lf->fp || !lf->sep) {
		errno = EINVAL;
		return NULL;
	}

	token = strtok_r(NULL, lf->sep, &lf->save);
	if (!token) {
		while (fgets(lf->buf, sizeof(lf->buf), lf->fp)) {
			if (lf->buf[0] == '#')
				continue;

			token = strtok_r(lf->buf, lf->sep, &lf->save);
			if (!token)
				continue;
			
			return token;
		}

		errno = ENOENT;
		return NULL;
	}

	return token;
}

/**
 * lfgetkey - Find key in file
 * @lf:  Pointer to &lfile_t context from lfopen()
 * @key: Key to look for
 *
 * Locate @key from the current position in the file parser context
 * returned from lfopen().  Please note, the search for @key does not
 * start from the beginning of the file, it searches from the current
 * position.  To restart search from the beginning use rewind() on the
 * lf->fp.
 *
 * Returns:
 * The value to @key, or %NULL if not found.
 */
char *lfgetkey(lfile_t *lf, char *key)
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
 * lfgetint - Same as lfgetkey() but returns an integer
 * @lf:  Pointer to &lfile_t context from lfopen()
 * @key: Key to look for
 *
 * This function is the same as lfgetkey() but returns the positive
 * integer value for the matching @key, if found.
 *
 * Returns:
 * The positive integer value for @key, or -1 if not found.
 */
int lfgetint(lfile_t *lf, char *key)
{
	char *token = lfgetkey(lf, key);

	if (token)
		return atoi(token);

	return -1;
}

/**
 * fgetint - Find the integer value for key in a file
 * @file: File to search for @key
 * @sep:  Separator for tokens in @file
 * @key:  Key to look for in @file
 *
 * This is a convenience wrapper for lfopen(), lfgetint(), and
 * lfclose().
 *
 * Returns:
 * The positive integer value for @key, or -1 if not found.
 */
int fgetint(char *file, char *sep, char *key)
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

#ifdef UNITTEST
int main(void)
{
	int val;
	lfile_t lf;

	val = fgetint("/etc/protocols", " \n\t", "udp");
	if (val == -1) {
		perror("Failed locating 'udp' protocol");
		return 1;
	}
	printf("udp has proto %d\n", val);

	val = fgetint("/etc/services", " /\n\t", "ftp");
	if (val == -1) {
		perror("Failed locating 'ftp' service");
		return 1;
	}
	printf("ftp is inet port %d\n", val);

	val = fgetint("/etc/group", "x:\n", "utmp");
	if (val == -1) {
		perror("Failed locating group 'utmp'");
		return 1;
	}
	printf("utmp is gid %d\n", val);

	val = fgetint("/etc/passwd", "x:\n", "nobody");
	if (val == -1) {
		perror("Failed locating user 'nobody'");
		return 1;
	}
	printf("nobody is uid %d\n", val);

	return 0;
}
#endif

/**
 * Local Variables:
 *  compile-command: "make V=1 -f lfile.mk"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
