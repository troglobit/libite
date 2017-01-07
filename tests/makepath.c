#include <errno.h>
#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include "check.h"

int checkpath(char *dir)
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

int test_makepath(char *dir)
{
	int ret = makepath(dir);

	if (!ret)
		ret = checkpath(dir);
	if (ret)
		perror("Failed");

	return ret;
}

int main(void)
{
	int i, ret = 0;
	char *list[] = {
		"/tmp/libite-test/tok/",
		"/tmp/libite-test/tok2",
		"/tmp/libite-test/ab",
		"/tmp/libite-test/b",
		"/tmp/libite-test/a/",
		"/tmp/libite-test/a/b",
		"/tmp/libite-test/a/c/",
		NULL
	};

	printf("Testing makepath() ...\n");
	mkdir("/tmp/libite-test", 0755);

	for (i = 0; list[i] && !ret; i++)
		ret |= test_makepath(list[i]);

	printf("\nCleaning up ...\n");
	system("rm -rf /tmp/libite-test");

	return ret;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style:     "linux"
 *  compile-command:  "make makepath && ./makepath"
 * End:
 */
