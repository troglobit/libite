#include <errno.h>
#include "check.h"

int main(void)
{
	char path[] = "/tmp/libite-procval-XXXXXX";
	char buf[32];
	long long llval;
	int fd, val;

	fd = mkstemp(path);
	fail_unless(fd >= 0);
	close(fd);

	fail_unless(writesf("first", "w", "%s", path) == 0);
	fail_unless(writesf("second", "a", "%s", path) == 0);
	fail_unless(readsnf(buf, sizeof(buf), "%s", path) == buf);
	fail_unless(!strcmp(buf, "first"));

	fail_unless(writesf("42", "w", "%s", path) == 0);
	fail_unless(readdf(&val, "%s", path) == 0);
	fail_unless(val == 42);

	fail_unless(writedf(-7, "w", "%s", path) == 0);
	fail_unless(readdf(&val, "%s", path) == 0);
	fail_unless(val == -7);

	fail_unless(writellf(1LL << 40, "w", "%s", path) == 0);
	fail_unless(readllf(&llval, "%s", path) == 0);
	fail_unless(llval == 1LL << 40);

	/* Not open for writing, fprintf() fails but fclose() does not */
	errno = 0;
	fail_unless(writesf("data", "r", "%s", path) == -1);
	fail_unless(errno == EBADF);
	fail_unless(writedf(1, "r", "%s", path) == -1);
	fail_unless(writellf(1, "r", "%s", path) == -1);

	if (!access("/dev/full", W_OK)) {
		char big[4 * BUFSIZ];

		memset(big, 'x', sizeof(big) - 1);
		big[sizeof(big) - 1] = 0;

		errno = 0;
		fail_unless(writesf(big, "w", "/dev/full") == -1);
		fail_unless(errno == ENOSPC);
	}

	fail_unless(unlink(path) == 0);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style:     "linux"
 *  compile-command:  "make procval && ./procval"
 * End:
 */
