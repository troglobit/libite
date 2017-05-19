#include <err.h>
#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>

#include "check.h"

int verbose = 0;


static int mtime(char *file)
{
	int ret;
	char buf[80];
	struct stat before, after;

	PRINT("Calling touch() to create file ...\n");
	touch(file);
	stat(file, &before);

	/* Must sleep a while here otherwise we execute too fast => no mtime change :-( */
	usleep(10000);

	PRINT("Calling touch() again to update mtime ...\n");
	if (touch(file)) {
		erase(file);
		err(1, "Failed creating %s", file);
	}
	stat(file, &after);

	ret = timespec_newer(&after.st_mtim, &before.st_mtim);
	PRINT("Before: %s\n", timespec2str(&before.st_mtim, buf, sizeof(buf)));
	PRINT("After : %s\n", timespec2str(&after.st_mtim, buf, sizeof(buf)));

	return !ret;
}

int main(int argc, char *argv[])
{
	int fd;
	char file[42] = "/tmp/touch_test.XXXXXX";

	if (argc > 1 && !strcmp(argv[1], "-v"))
		verbose = 1;

	fd = mkstemp(file);
	if (fd < 0)
		err(1, "Failed creating tempfile, %s", file);
	close(fd);

	return mtime(file);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style:     "linux"
 *  compile-command:  "make pidfile && ./pidfile -v"
 * End:
 */
