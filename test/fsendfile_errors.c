#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "check.h"

static void read_error(int partial, int discard, size_t limit)
{
	const char first[] = "first", next[] = "next";
	char copied[sizeof(first) + sizeof(next)];
	FILE *src, *dst = discard ? NULL : tmpfile();
	int fd[2];
	size_t len = partial ? sizeof(first) : 0;

	fail_unless(discard || dst);
	fail_unless(pipe(fd) == 0);
	fail_unless(fcntl(fd[0], F_SETFL, O_NONBLOCK) == 0);
	if (partial)
		fail_unless(write(fd[1], first, sizeof(first)) == sizeof(first));
	src = fdopen(fd[0], "r");
	fail_unless(src);
	errno = 0;
	fail_unless(fsendfile(src, dst, limit) == -1);
	fail_unless(errno == EAGAIN || errno == EWOULDBLOCK);
	fail_unless(ferror(src) && !feof(src));
	if (dst)
		fail_unless(ftell(dst) == (long)len);

	/* The caller owns the streams and can resume after clearing the error. */
	clearerr(src);
	fail_unless(write(fd[1], next, sizeof(next)) == sizeof(next));
	fail_unless(close(fd[1]) == 0);
	fail_unless(fsendfile(src, dst, 0) == sizeof(next));
	fail_unless(!ferror(src) && feof(src));
	fail_unless(fsendfile(src, dst, 0) == 0);
	if (dst) {
		rewind(dst);
		fail_unless(fread(copied, 1, sizeof(copied), dst) == len + sizeof(next));
		if (partial)
			fail_unless(memcmp(copied, first, sizeof(first)) == 0);
		fail_unless(memcmp(copied + len, next, sizeof(next)) == 0);
		fail_unless(fclose(dst) == 0);
	}
	fail_unless(fclose(src) == 0);
}

static void limited_pipe(int discard)
{
	const char data[] = "available without EOF";
	char copied[sizeof(data)];
	int fd[2];
	FILE *src, *dst = discard ? NULL : tmpfile();

	fail_unless(discard || dst);
	fail_unless(pipe(fd) == 0);
	fail_unless(fcntl(fd[0], F_SETFL, O_NONBLOCK) == 0);
	fail_unless(write(fd[1], data, sizeof(data)) == sizeof(data));
	src = fdopen(fd[0], "r");
	fail_unless(src);
	fail_unless(fsendfile(src, dst, sizeof(data)) == sizeof(data));
	fail_unless(!ferror(src) && !feof(src));
	if (dst) {
		rewind(dst);
		fail_unless(fread(copied, 1, sizeof(copied), dst) == sizeof(data));
		fail_unless(memcmp(copied, data, sizeof(data)) == 0);
		fail_unless(fclose(dst) == 0);
	}
	fail_unless(close(fd[1]) == 0);
	fail_unless(fsendfile(src, NULL, 0) == 0);
	fail_unless(feof(src) && !ferror(src));
	fail_unless(fclose(src) == 0);
}

static void copy_binary(size_t len, size_t limit)
{
	unsigned char data[2 * BUFSIZ + 17], copied[sizeof(data)];
	const char prefix[] = "prefix";
	FILE *src = tmpfile(), *dst = tmpfile();
	size_t i, expected = limit && limit < len ? limit : len;

	fail_unless(src && dst);
	for (i = 0; i < sizeof(data); i++)
		data[i] = i % 253;
	fail_unless(fwrite(prefix, 1, sizeof(prefix), src) == sizeof(prefix));
	fail_unless(fwrite(data, 1, len, src) == len);
	fail_unless(fseek(src, sizeof(prefix), SEEK_SET) == 0);
	fail_unless(fwrite(prefix, 1, sizeof(prefix), dst) == sizeof(prefix));
	fail_unless(fsendfile(src, dst, limit) == (ssize_t)expected);
	fail_unless(ftell(src) == (long)(sizeof(prefix) + expected));
	fail_unless(ftell(dst) == (long)(sizeof(prefix) + expected));
	rewind(dst);
	fail_unless(fread(copied, 1, sizeof(prefix), dst) == sizeof(prefix));
	fail_unless(memcmp(copied, prefix, sizeof(prefix)) == 0);
	fail_unless(fread(copied, 1, sizeof(copied), dst) == expected);
	fail_unless(memcmp(copied, data, expected) == 0);
	fail_unless(fclose(src) == 0);
	fail_unless(fclose(dst) == 0);
}

int main(void)
{
	const size_t lengths[] = { 0, 1, BUFSIZ - 1, BUFSIZ, BUFSIZ + 1, 2 * BUFSIZ + 17 };
	size_t i;
	int partial, discard;

	for (i = 0; i < NELEMS(lengths); i++) {
		copy_binary(lengths[i], 0);
		copy_binary(lengths[i], 1);
		copy_binary(lengths[i], BUFSIZ);
		copy_binary(lengths[i], sizeof(lengths) * BUFSIZ);
	}
	for (discard = 0; discard < 2; discard++) {
		limited_pipe(discard);
		for (partial = 0; partial < 2; partial++) {
			read_error(partial, discard, 0);
			read_error(partial, discard, BUFSIZ);
		}
	}
#ifdef __linux__
	FILE *src = tmpfile(), *dst = fopen("/dev/full", "w");

	fail_unless(src && dst);
	fail_unless(fputs("data", src) >= 0);
	rewind(src);
	fail_unless(setvbuf(dst, NULL, _IONBF, 0) == 0);
	errno = 0;
	fail_unless(fsendfile(src, dst, 0) == -1 && errno == ENOSPC);
	fail_unless(ferror(dst));
	fclose(src);
	fclose(dst);
#endif
	errno = 0;
	fail_unless(fsendfile(NULL, NULL, 0) == -1 && errno == EINVAL);
	return 0;
}
