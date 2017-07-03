/* C implementation of UNIX which(1)
 *
 * Copyright (c) 2017  Joachim Nilsson <troglobit@gmail.com>
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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>		/* snprintf() */
#include <stdlib.h>		/* realloc(), free() */
#include <string.h>		/* strtok_r() */
#include <unistd.h>		/* access() */

/* Strip any arguments in "/path/to/bin --some args", modifies input string */
static char *strip_args(char *path)
{
	size_t i = 0;

	if (!path)
		return NULL;

	/* Find first whitespace (space/tab/etc.) */
	while (path[i] && !isspace(path[i]))
		i++;
	path[i] = 0;

	return path;
}

/*
 * Like which(1), returns a malloc'ed path to cmd on success, or NULL
 */
char *which(const char *cmd)
{
	size_t pathlen = 0;
	char *ptr, *tok, *env, *path = NULL;

	if (!cmd) {
		errno = EINVAL;
		return NULL;
	}

	if (cmd[0] == '/') {
		path = strip_args(strdup(cmd));
		if (!path)
			return NULL;

		if (!access(path, X_OK))
			return path;

		if (path)
			free(path);

		return NULL;
	}

	ptr = getenv("PATH");
	if (!ptr)
		return NULL;

	env = strdup(ptr);
	if (!env)
		return NULL;

	tok = strtok_r(env, ":", &ptr);
	while (tok) {
		size_t len = strlen(tok) + strlen(cmd) + 2;

		if (pathlen < len) {
			path = realloc(path, len);
			if (!path) {
				free(env);
				return NULL;
			}
			pathlen = len;
		}

		snprintf(path, pathlen, "%s/%s", tok, cmd);
		path = strip_args(path);
		if (!path || !access(path, X_OK)) {
			free(env);
			return path;
		}

		tok = strtok_r(NULL, ":", &ptr);
	}

	if (path)
		free(path);
	free(env);

	return NULL;
}

/*
 * Like which(), above, but only answers TRUE(1)/FALSE(0)
 */
int whichp(const char *cmd)
{
	char *path;

	path = which(cmd);
	if (path) {
		free(path);
		return 1;
	}

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
