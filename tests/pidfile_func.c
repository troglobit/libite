#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>

#include "check.h"

extern char *__progname;
extern char *__pidfile_name;
extern char *__pidfile_path;

static char PIDFILE[42];
static int verbose = 0;
static struct stat before, after;

static void sigterm_handler(int UNUSED(signo))
{
	if (verbose)
		printf("Exiting ...\n");
}

static void sigalrm_handler(int UNUSED(signo))
{
	pidfile(NULL);
	if (!fexist(PIDFILE))
		err(1, "failed creating %s", PIDFILE);
	stat(PIDFILE, &before);
}

static int mtime()
{
	int ret;
	char buf[80];

	/* Must sleep a while here otherwise we execute too fast => no mtime change :-( */
	usleep(10000);

	if (verbose)
		printf("Calling pidfile() again to update mtime ...\n");
	pidfile(NULL);
	stat(PIDFILE, &after);

	ret = timespec_newer(&after.st_mtim, &before.st_mtim);
	if (verbose) {
		printf("Before: %s\n", timespec2str(&before.st_mtim, buf, sizeof(buf)));
		printf("After : %s\n", timespec2str(&after.st_mtim, buf, sizeof(buf)));
	}

	return !ret;
}

int main(int argc, char *argv[])
{
	if (argc > 1 && !strcmp(argv[1], "-v"))
		verbose = 1;

	/* Override default _PATH_VARRUN in pidfile() */
	__pidfile_path = _PATH_TMP;
	snprintf(PIDFILE, sizeof(PIDFILE), "%s%s.pid", __pidfile_path, __progname);

	signal(SIGTERM, sigterm_handler);
	signal(SIGALRM, sigalrm_handler);
	alarm(1);

	return     test(pidfile_poll(PIDFILE) != getpid(), "Testing pidfile_poll()")
		|| test(strcmp(__pidfile_name, PIDFILE),   "Testing __pidfile_name vs guessed PID filename")
		|| test(mtime(),                           "Testing mtime update of followup pidfile() call")
		|| test(pidfile_signal(PIDFILE, SIGTERM),  "Testing signalling ourselves")
		;

	return 0;
}

