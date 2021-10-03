/* Functions for dealing with PID files (client side)
 *
 * Copyright (c) 2009-2021  Joachim Wiberg <troglobit@gmail.com>
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
 * @file pidfilefn.c
 * @author Joachim Wiberg
 * @date 2009-2021
 * @copyright ISC License
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

extern char *chomp(char *str);

/**
 * Reads a PID value from a pidfile.
 * @param pidfile  File containing PID, usually in @c /var/run/PROCNAME.pid
 *
 * This function takes a @p pidfile and returns the PID found therein.
 *
 * @returns On invalid @p pidfile, -1 with @a errno set. If the @p
 * pidfile is empty, or when its contents cannot be translated, this
 * function returns zero (0), on success this function returns a PID
 * value greater than one.
 *
 * @note PID 1 is reserved for the system init  process.
 *
 * @exception EINVAL on invalid @p pidfile, or
 * @exception ENOENT when @p pidfile does not exist.
 */
pid_t pidfile_read(const char *pidfile)
{
	int pid = 0;
	char buf[16];
	FILE *fp;

	if (!pidfile) {
		errno = EINVAL;
		return -1;
	}

	fp = fopen(pidfile, "r");
	if (!fp)
		return -1;

	if (fgets(buf, sizeof(buf), fp)) {
                char *ptr = chomp(buf);

                if (ptr) {
                        errno = 0;
                        pid = strtoul(ptr, NULL, 0);
                        if (errno)
                                pid = 0; /* Failed conversion. */
                }
	}
	fclose(fp);

	return pid;
}

/**
 * Poll for the existence of a pidfile and return PID.
 * @param pidfile  Path to pidfile to poll for
 *
 * This function polls for the pidfile at @p pidfile for at most 5
 * seconds before timing out. If the file is created within that time
 * span the file is read and its PID contents returned.
 *
 * @returns The PID read from @p pidfile, or zero on timeout.
 */
pid_t pidfile_poll(const char *pidfile)
{
	pid_t pid = 0;
	int tries = 0;

	/* Timeout = 100 * 50ms = 5s */
	while ((pid = pidfile_read(pidfile)) <= 0 && tries++ < 100)
		usleep(50000);	/* Wait 50ms between retries */

	if (pid < 0)
		pid = 0;

	return pid;
}

/**
 * Send signal to a PID and cleanup pidfile afterwards.
 * @param pidfile  File containing PID, usually in @c /var/run/PROCNAME.pid
 * @param signal  Signal to send to PID found in @p pidfile.
 *
 * If @p signal is any of @c SIGTERM or @c SIGKILL, or if kill(2)
 * returns -1, the @p pidfile is removed.
 *
 * @returns POSIX OK(0) on success, non-zero otherwise.
 */
int pidfile_signal(const char *pidfile, int signal)
{
	int pid = -1, ret = -1;

	pid = pidfile_read(pidfile);
	if (pid <= 0)
		return 1;

	ret = kill(pid, signal);
	if (!ret && signal == SIGKILL)
		ret = remove(pidfile);

	return ret;
}

/**
 * Local Variables:
 *  compile-command: "make V=1 -f pidfilefn.mk"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
