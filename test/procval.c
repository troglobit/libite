#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include "check.h"

static void write_file(const char *path)
{
	const char first[] = "first\nsecond", next[] = "third";
	char copied[128];
	long long ll;
	int n;
	FILE *fp;

	fail_unless(writesf(first, "w", "%s", path) == 0);
	fail_unless(writesf(next, "a", "%s", path) == 0);
	fp = fopen(path, "r");
	fail_unless(fp);
	fail_unless(fread(copied, 1, sizeof(copied), fp) == sizeof(first) + sizeof(next));
	fail_unless(memcmp(copied, "first\nsecond\nthird\n", sizeof(first) + sizeof(next)) == 0);
	fail_unless(fclose(fp) == 0);

	/* Empty strings still write a newline; formatted paths and readers work. */
	fail_unless(writesf("", "w", "%s", path) == 0);
	fail_unless(readsnf(copied, sizeof(copied), "%s", path) == copied);
	fail_unless(!copied[0]);
	fail_unless(writesf("-42", "w", "%s", path) == 0);
	fail_unless(readdf(&n, "%s", path) == 0 && n == -42);
	fail_unless(readllf(&ll, "%s", path) == 0 && ll == -42);
}

#ifdef __linux__
static void write_full(size_t len)
{
	char *data = malloc(len + 1);
	int before, after, rc, saved_errno;

	fail_unless(data);
	memset(data, 'x', len);
	data[len] = 0;
	before = open("/dev/null", O_RDONLY);
	fail_unless(before >= 0 && close(before) == 0);
	errno = 0;
	rc = writesf(data, "w", "%s", "/dev/full");
	saved_errno = errno;
	after = open("/dev/null", O_RDONLY);
	fail_unless(after == before);
	fail_unless(close(after) == 0);
	free(data);
	fail_unless(rc == -1 && saved_errno == ENOSPC);
}
#endif

int main(void)
{
	char path[] = "procval-XXXXXX";
	int fd = mkstemp(path);

	fail_unless(fd >= 0 && close(fd) == 0);
	write_file(path);
	errno = 0;
	fail_unless(writesf("data", "r", "%s", path) == -1 && errno == EBADF);
	write_file(path);
	fail_unless(unlink(path) == 0);
	errno = 0;
	fail_unless(writesf("data", "w", "%s/missing", path) == -1 && errno == ENOENT);
#ifdef __linux__
	const size_t lengths[] = { 0, 1, BUFSIZ - 1, BUFSIZ, BUFSIZ + 1, 3 * BUFSIZ };
	size_t i;
	int repeat;

	for (repeat = 0; repeat < 8; repeat++)
		for (i = 0; i < NELEMS(lengths); i++)
			write_full(lengths[i]);
#endif
	return 0;
}
