/* test of touch(), touchf(), erase(), and erasef() */
#include <err.h>
#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>

#include "check.h"

#define FMT "%s-bar"

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
	erase(file);

	return !ret;
}

static int formatted(char *file)
{
	char vrfy[80];

	if (touchf(FMT, file))
		err(1, "Failed creating " FMT, file);

	PRINT("Created " FMT "\n", file);

	snprintf(vrfy, sizeof(vrfy), FMT, file);
	if (!fexist(vrfy))
		errx(1, "touchf() does not detect failure to create file " FMT, file);

	PRINT("File " FMT " really does exist!\n", file);

	erasef(FMT, file);
	if (fexist(vrfy)) {
		int saved = errno;

		erase(vrfy);
		errno = saved;
		errx(1, "erasef() failed removing the created file " FMT, file);
	}

	return 0;
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

	return mtime(file) || formatted(file);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style:     "linux"
 *  compile-command:  "make touch && ./touch -v"
 * End:
 */
