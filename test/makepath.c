#include <errno.h>
#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include "check.h"

#define BASEDIR "/tmp/libite-test"

int checkpath(const char *dir)
{
	char tmp[256];
	struct stat sb;

	snprintf(tmp, sizeof(tmp), "ls -ld %s", dir);
	if (system(tmp))
		perror("system");

	if (!stat(dir, &sb) && S_ISDIR(sb.st_mode))
		return 0;

	errno = ENOTDIR;
	return 1;
}

int test_makepath(const char *dir)
{
	int ret = makepath(dir);

	if (!ret)
		ret = checkpath(dir);
	if (ret)
		perror("Failed");

	return ret;
}

int test_fmkpath(const char *subdir)
{
	int ret;

	ret = fmkpath(0755, "%s/%s", BASEDIR, subdir);
	if (!ret) {
		char tmp[256];

		snprintf(tmp, sizeof(tmp), "%s/%s", BASEDIR, subdir);
		ret = checkpath(tmp);
	} else
		perror("Failed");

	return ret;
}

/* mkpath() must apply mode as given, not mode & ~umask */
int test_umask(void)
{
	const char *dirs[] = { BASEDIR "/um", BASEDIR "/um/sub", NULL };
	mode_t old = umask(0077);
	struct stat sb;
	int i, ret;

	ret = mkpath(dirs[1], 0755);
	umask(old);
	if (ret) {
		perror("Failed");
		return ret;
	}

	for (i = 0; dirs[i]; i++) {
		if (checkpath(dirs[i]) || stat(dirs[i], &sb))
			return 1;
		if ((sb.st_mode & 07777) != 0755) {
			fprintf(stderr, "Failed: %s mode %04o, expected 0755\n",
				dirs[i], sb.st_mode & 07777);
			return 1;
		}
	}

	return 0;
}

int main(void)
{
	int i, ret = 0;
	char *list[] = {
		BASEDIR "/tok/",
		BASEDIR "/tok2",
		BASEDIR "/ab",
		BASEDIR "/b",
		BASEDIR "/a/",
		BASEDIR "/a/b",
		BASEDIR "/a/c/",
		NULL
	};

	mkdir(BASEDIR, 0755);

	printf("Pass 1/3: makepath() =========================\n");
	for (i = 0; list[i] && !ret; i++)
		ret |= test_makepath(list[i]);

	printf("Pass 2/3: fmkpath() ==========================\n");
	for (i = 0; list[i] && !ret; i++)
		ret |= test_fmkpath(list[i] + strlen(BASEDIR) + 1);

	printf("Pass 3/3: mkpath() with umask ================\n");
	if (!ret)
		ret |= test_umask();

	printf("Done: Cleaning up ============================\n");
	system("rm -rf " BASEDIR);

	return ret;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style:     "linux"
 *  compile-command:  "make makepath && ./makepath"
 * End:
 */
