/* Functions for dealing with PID files (client side)
 *
 * Copyright (c) 2009-2015  Joachim Nilsson <troglobit@gmail.com>
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
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

extern char *__pidfile_name;
extern char *chomp(char *str);

/**
 * pidfile_read - Reads a PID value from a pidfile.
 * @pidfile: File containing PID, usually in /var/run/<PROC>.pid
 *
 * This function takes a @pidfile and returns the PID found therein.
 *
 * Returns:
 * On invalid @pidfile -1 and errno set to %EINVAL, when @pidfile does not exist -1
 * and errno set to %ENOENT.  When the pidfile is empty or when its contents cannot
 * be translated this function returns zero (0), on success this function returns
 * a PID value greater than one. PID 1 is reserved for the system init process.
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
 * pidfile_poll - Poll for the existence of a pidfile and return PID
 * @pidfile: Path to pidfile to poll for
 *
 * This function polls for the pidfile at @pidfile for at most 5 seconds
 * before timing out. If the file is created within that time span the
 * file is read and its PID contents returned.
 *
 * Returns:
 * The PID read from @pidfile, or zero on timeout.
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
 * pidfile_signal - Send signal to a PID and cleanup pidfile afterwards
 * @pidfile: File containing PID, usually in /var/run/<PROC>.pid
 * @signal: Signal to send to PID found in @pidfile.
 *
 * If @signal is any of %SIGTERM, %SIGKILL, or if kill() returns -1 the
 * @pidfile is removed.
 *
 * Returns:
 * POSIX OK(0) on success, non-zero otherwise.
 */
int pidfile_signal(const char *pidfile, int signal)
{
	int pid = -1, ret = -1;

	pid = pidfile_read(pidfile);
	if (pid <= 0)
		return 1;

	ret = kill(pid, signal);
	if ((ret == -1) || (signal == SIGTERM) || (signal == SIGKILL))
		(void)remove(pidfile);

	return 0;
}


/********************************* UNIT TESTS ************************************/
#ifdef UNITTEST
#include <paths.h>
#include "lite.h"

extern char *__progname;
static char PIDFILE[42];

static void sigterm_handler(int UNUSED(signo))
{
	printf("Exiting ...\n");
}

static void sigalrm_handler(int UNUSED(signo))
{
	printf("Testing pidfile() ...\n");
	pidfile(NULL);

	if (!fexist(PIDFILE))
		err(1, "pidfile() failed creating %s", PIDFILE);
}

int main(void)
{
	char cmd[80];

	snprintf(PIDFILE, sizeof(PIDFILE), "%s%s.pid", _PATH_VARRUN, __progname);

	signal(SIGTERM, sigterm_handler);
	signal(SIGALRM, sigalrm_handler);
	alarm(1);

	printf("Testing pidfile_poll() ...\n");
	if (!pidfile_poll(PIDFILE))
		printf("Timed out!\n");
	else
		printf("We got signal!\n");

	printf("Reading pid file, should be %d ...\n", getpid());
        printf("=> %d\n", pidfile_read(PIDFILE));

	printf("\nComparing __pidfile_name with our guessed PID filename ...\n");
	printf("strcmp(\"%s\",\n       \"%s\") => %s\n\n", __pidfile_name, PIDFILE,
	       !strcmp(__pidfile_name, PIDFILE) ? "OK" : "FAIL");

	/* Occular verification that calling pidfile() again updates mtime */
	snprintf(cmd, sizeof(cmd), "ls -l --full-time %s", PIDFILE);
	system(cmd);
	printf("Before ^^ pidfile() vv after ...\n");
	pidfile(NULL);
	system(cmd);
	
        printf("Signalling ourselves ...\n");

	return pidfile_signal(PIDFILE, SIGTERM);
}
#endif  /* UTEST_PIDFN */

/**
 * Local Variables:
 *  compile-command: "make V=1 -f pidfilefn.mk"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
