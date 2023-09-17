/* Run a command in the background: fork() + usleep() + execvp()
 *
 * Copyright (c) 2023  Joachim Wiberg <troglobit@gmail.com>
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
 * @file runbg.c
 * @author Joachim Wiberg
 * @date 2023
 * @copyright ISC License
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Run command in the background
 * @param cmd    NULL terminated list of command and optional arguments
 * @param delay  Microseconds after fork() to wait before calling cmd
 *
 * This function forks to run the given command @p cmd in the background.
 * To ensure there are no lingering zombies the function actuall forks
 * twice, and also calls setsid(), before it calls usleep() with the @p
 * delay argument.  The command @p cmd is handed over to execvp().
 *
 * Since it runs in the background it is not possible to get the return
 * code of the command.
 *
 * @returns on successful (first) fork(), this function returns POSIX
 * OK(0), otherwise -1 and @a errno is set to indicate the error.
 */
int runbg(char *const cmd[], int delay)
{
	int pid = fork();
	int rc;

	if (!pid) {
		int fd, maxfd;

		if (setsid() == -1)
			_exit(errno);

		/* reparent to init */
		pid = fork();
		if (pid == -1)
			_exit(errno);
		if (pid > 0)
			_exit(0);

		maxfd = sysconf(_SC_OPEN_MAX);
		if (maxfd == -1)
			maxfd = 8192;
		for (fd = 0; fd < maxfd; fd++)
			close(fd);

		usleep(delay);
		_exit(execvp(cmd[0], cmd));
	}

	if (pid == -1)
		return -1;

	if (waitpid(pid, &rc, 0) == -1)
		return -1;

	if (WIFEXITED(rc)) {
		errno = WEXITSTATUS(rc);
		if (errno)
			rc = -1;
		else
			rc = 0;
	} else if (WIFSIGNALED(rc)) {
		errno = EINTR;
		rc = -1;
	} else
		rc = -1;

	return rc;
}
