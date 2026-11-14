#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "check.h"

static void copy_binary(size_t len)
{
	unsigned char data[3 * BUFSIZ + 37], copied[sizeof(data)];
	const char prefix[] = "destination prefix";
	FILE *src = tmpfile(), *dst = tmpfile();
	size_t i;

	fail_unless(src && dst);
	for (i = 0; i < sizeof(data); i++)
		data[i] = i % 253;

	/* Begin at the current stream positions, preserving the destination prefix. */
	fail_unless(fwrite(prefix, 1, sizeof(prefix), src) == sizeof(prefix));
	fail_unless(fwrite(data, 1, len, src) == len);
	fail_unless(fseek(src, sizeof(prefix), SEEK_SET) == 0);
	fail_unless(fwrite(prefix, 1, sizeof(prefix), dst) == sizeof(prefix));

	fail_unless(fcopyfile(src, dst) == 0);
	fail_unless(ftell(src) == (long)(sizeof(prefix) + len));
	fail_unless(ftell(dst) == (long)(sizeof(prefix) + len));
	fail_unless(fseek(dst, 0, SEEK_SET) == 0);
	fail_unless(fread(copied, 1, sizeof(prefix), dst) == sizeof(prefix));
	fail_unless(memcmp(copied, prefix, sizeof(prefix)) == 0);
	fail_unless(fread(copied, 1, sizeof(copied), dst) == len);
	fail_unless(memcmp(copied, data, len) == 0);
	fail_unless(fgetc(dst) == EOF && feof(dst));
	fail_unless(fclose(src) == 0);
	fail_unless(fclose(dst) == 0);
}

static void read_error(int partial)
{
	const char data[] = "bytes before the read error";
	char copied[sizeof(data)];
	int fd[2];
	FILE *src, *dst = tmpfile();

	fail_unless(dst && pipe(fd) == 0);
	fail_unless(fcntl(fd[0], F_SETFL, O_NONBLOCK) == 0);
	if (partial)
		fail_unless(write(fd[1], data, sizeof(data)) == sizeof(data));
	src = fdopen(fd[0], "r");
	fail_unless(src);
	errno = 0;
	fail_unless(fcopyfile(src, dst) == -1);
	fail_unless(errno == EAGAIN || errno == EWOULDBLOCK);
	fail_unless(ferror(src));
	fail_unless(ftell(dst) == (long)(partial ? sizeof(data) : 0));
	if (partial) {
		rewind(dst);
		fail_unless(fread(copied, 1, sizeof(copied), dst) == sizeof(data));
		fail_unless(memcmp(copied, data, sizeof(data)) == 0);
	}
	fclose(src);
	fclose(dst);
	close(fd[1]);
}

int main(void)
{
	const size_t lengths[] = { 0, 1, BUFSIZ - 1, BUFSIZ, BUFSIZ + 1, 3 * BUFSIZ + 37 };
	size_t i;

	for (i = 0; i < NELEMS(lengths); i++)
		copy_binary(lengths[i]);
	read_error(0);
	read_error(1);

#ifdef __linux__
	FILE *src = tmpfile();
	FILE *dst = fopen("/dev/full", "w");
	fail_unless(src && dst);
	fail_unless(fputs("data", src) >= 0);
	rewind(src);
	fail_unless(setvbuf(dst, NULL, _IONBF, 0) == 0);
	errno = 0;
	fail_unless(fcopyfile(src, dst) == -1);
	fail_unless(errno == ENOSPC);
	fail_unless(ferror(dst));
	fclose(src);
	fclose(dst);
#endif

	errno = 0;
	fail_unless(fcopyfile(NULL, NULL) == -1 && errno == EINVAL);
	return 0;
}
