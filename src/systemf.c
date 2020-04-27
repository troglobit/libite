/* Formatted system() which returns actual return status of command
 *
 * Copyright (c) 2020  Joachim Nilsson <troglobit@gmail.com>
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int systemf(const char *fmt, ...)
{
	va_list ap;
	char *cmd;
	int len, status;
	int rc = -1;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	cmd = alloca(++len);
	if (!cmd) {
		errno = ENOMEM;
		return -1;
	}

	va_start(ap, fmt);
	vsnprintf(cmd, len, fmt, ap);
	va_end(ap);

	status = system(cmd);

	rc = WEXITSTATUS(status);
	if (!WIFEXITED(status)) {
		if (WIFSIGNALED(status) &&
		    (WTERMSIG(status) == SIGINT ||
		     WTERMSIG(status) == SIGQUIT)) {
			errno = EINTR;
			rc = -1;
		} else if (!rc) {
			/*
			 * Alert callee that command didn't complete
			 * successfully.  Some programs don't change
			 * their exit code when signaled.
			 */
			rc = 1;
		}
	}

	return rc;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
