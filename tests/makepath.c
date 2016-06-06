#include <errno.h>
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
		"/tmp/tok/",
		"/tmp/tok2",
		"/tmp/ab",
		"/tmp/b",
		"/tmp/a/",
		"/tmp/a/b",
		"/tmp/a/c/",
		"../../sometestdir/arpa/inet",
		NULL
	};

	for (i = 0; list[i]; i++)
		rmdir(list[i]);

	printf("Testing makepath() ...\n");
	for (i = 0; list[i] && !ret; i++)
		ret |= test_makepath(list[i]);

	printf("\nCleaning up ...\n");
	for (i = 0; list[i]; i++)
		rmdir(list[i]);

	return ret;
}
