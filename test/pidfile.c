#include <err.h>
#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>

#include "check.h"

extern char *__pidfile_name;
extern char *__pidfile_path;

static char *ident;
static char *pidfile_arg;
static char PIDFILE[42];
static int verbose = 0;
static struct stat before, after;

static void sigterm_handler(int signo)
{
	(void)signo;

	PRINT("Exiting ...\n");
	remove(PIDFILE);
}

static void sigalrm_handler(int signo)
{
	(void)signo;

	PRINT("Calling pidfile(%s)\n", pidfile_arg ?: "NULL");
	if (pidfile(pidfile_arg))
		err(1, "failed creating1 %s", pidfile_arg);
	if (!fexist(PIDFILE))
		err(1, "failed creating2 %s, created %s", PIDFILE, __pidfile_name);
	stat(PIDFILE, &before);
}

static int mtime()
{
	int ret;
	char buf[80];

	/* Must sleep a while here otherwise we execute too fast => no mtime change :-( */
	usleep(10000);

	PRINT("Calling pidfile() again to update mtime ...\n");
	if (pidfile(pidfile_arg))
		err(1, "failed creating3 %s", pidfile_arg);
	stat(PIDFILE, &after);

	ret = timespec_newer(&after.st_mtim, &before.st_mtim);
	PRINT("Before: %s\n", timespec2str(&before.st_mtim, buf, sizeof(buf)));
	PRINT("After : %s\n", timespec2str(&after.st_mtim, buf, sizeof(buf)));

	return !ret;
}

int test_pidfile(char *arg0)
{
	/* pidfile() argument */
	pidfile_arg = arg0;

	/* Expected PID file name */
	if (!arg0)
		snprintf(PIDFILE, sizeof(PIDFILE), "%s%s.pid", __pidfile_path, ident);
	else if (arg0[0] != '/')
		snprintf(PIDFILE, sizeof(PIDFILE), "%s%s.pid", __pidfile_path, arg0);
	else
		snprintf(PIDFILE, sizeof(PIDFILE), "%s", arg0);

	PRINT("Verifying pidfile(%s) ... PID: %d\n", PIDFILE, getpid());

	/* Remove any lingering files from previous test runs */
	remove(PIDFILE);

	signal(SIGTERM, sigterm_handler);
	signal(SIGALRM, sigalrm_handler);

	/* For signalling test */
	alarm(1);

	return     test(pidfile_poll(PIDFILE) != getpid(), "Testing pidfile_poll()")
		|| test(strcmp(__pidfile_name, PIDFILE),   "Testing __pidfile_name vs guessed PID filename")
		|| test(mtime(),                           "Testing mtime update of followup pidfile() call")
		|| test(pidfile_signal(PIDFILE, SIGTERM),  "Testing signalling ourselves")
		;
}

static char *progname(char *arg0)
{
       char *nm;

       nm = strrchr(arg0, '/');
       if (nm)
	       nm++;
       else
	       nm = arg0;

       return nm;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	size_t i;
	char *pidfiles[] = {
		NULL,
		"pidfile_test1",
		"/var/tmp/pidfile_test2.pid",
	};

	if (argc > 1 && !strcmp(argv[1], "-v"))
		verbose = 1;

	/* Override default _PATH_VARRUN in pidfile() */
	__pidfile_path = _PATH_TMP;

	/* Find __progname, not all C libraries support this */
	ident = progname(argv[0]);

	for (i = 0; i < NELEMS(pidfiles); i++)
		ret += test_pidfile(pidfiles[i]);

	return ret;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style:     "linux"
 *  compile-command:  "make pidfile && ./pidfile -v"
 * End:
 */
