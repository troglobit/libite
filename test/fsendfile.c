#include <err.h>
#include <unistd.h>
#include "check.h"

/* A failed read is an error, not EOF */
static void read_error(void)
{
	int fd[2];
	ssize_t rc;
	FILE *src;

	if (pipe(fd) || fcntl(fd[0], F_SETFL, O_NONBLOCK))
		err(1, "Failed creating nonblocking pipe");
	if (write(fd[1], "data", 4) != 4)
		err(1, "Failed writing to pipe");

	src = fdopen(fd[0], "r");
	if (!src)
		err(1, "Failed fdopen()");

	printf("fsendfile(pipe, NULL, 4)\t");
	rc = fsendfile(src, NULL, 4);
	if (rc != 4)
		errx(1, "Failed, got %zd, expected 4", rc);
	printf("OK\n");

	printf("fsendfile(empty pipe, NULL, 0)\t");
	errno = 0;
	rc = fsendfile(src, NULL, 0);
	if (rc != -1 || errno != EAGAIN)
		errx(1, "Failed, got %zd errno %d, expected -1 and EAGAIN", rc, errno);
	printf("OK\n");

	fclose(src);
	close(fd[1]);
}

int main(void)
{
	int i = 0;
	char *files[] = {
		"/etc/passwd", "/tmp/tok",
		"/etc/passwd", "/tmp/passwd",
		"/etc/passwd", "/tmp/passwd",
		NULL
	};
	FILE *src, *dst;

	while (files[i]) {
                src = fopen(files[i], "r");
                dst = fopen(files[i + 1], "w");
		printf("fsendfile(%s, %s, 512)\t", files[i], files[i + 1]);
		if (-1 == fsendfile(src, dst, 512))
			err(1, "Failed fsendfile(%s, %s)", files[i], files[i + 1]);

		if (!access(files[i + 1], F_OK))
			printf("OK => %s\n", files[i + 1]);

		remove(files[i + 1]);
		i += 2;
	}

	read_error();

	return 0;
}
