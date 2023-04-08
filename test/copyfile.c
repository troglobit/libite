#include <err.h>
#include <errno.h>
#include "check.h"

#define ERR(fmt, args...)  { printf(fmt ": %s\n", ##args, strerror(errno));  rc = 1; }
#define ERRX(fmt, args...) { printf(fmt "\n", ##args); rc = 1; }

static char *files[] = {
	"/etc/passwd", "/tmp/mypwd", "/tmp/mypwd",
	"/etc/passwd", "/tmp/",      "/tmp/passwd",
	"/etc/passwd", "/tmp",       "/tmp/passwd",
	NULL
};

static int one_sz(const char *src, const char *dst, int sz)
{
	struct stat st;
	ssize_t len;
	int rc = 0;

	len = copyfile(src, dst, sz, 0);
	if (!len) {
		ERR("failed copying any bytes from %s", src);
	} else {
		if (stat(dst, &st))
			ERR("copied but cannot find destination %s", dst);

		if (st.st_size != sz)
			ERR("file only %ld bytes, expected %d", st.st_size, sz);

		if (len != st.st_size)
			ERRX("copied expected data %d, but copyfile() says %ld", sz, len);
	}

	return rc;
}

static int _sz()
{
	const int sz = 32768;

	return test(one_sz("/dev/zero", "/tmp/zeroes", sz), "copyfile(/dev/zero, %d)", sz);
}

static int fcopyfile_one(const char *src, const char *dst, const char *expected)
{
	FILE *from, *to;
	int rc;

	from = fopen(src, "r");
	to   = fopen(dst, "w");

	rc = fcopyfile(from, to);

	if (from)
		fclose(from);
	if (to)
		fclose(to);

	if (rc) {
		if (!src || !dst)
			rc = 0; /* expected */
		else if (fisdir(dst))
			rc = 0;	/* expected */
	} else {
		if (fexist(expected)) {
			if (systemf("diff -q %s %s", src, expected))
				rc = 1;
		} else
			ERR("cannot find %s", expected);
	}       

	erase(expected);

	return rc;
}

static int _fcopyfile()
{
	int i, rc = 0;

	for (i = 0; files[i]; i += 3) {
		char *src = files[i];
		char *dst = files[i + 1];
		char *res = files[i + 2];

		rc += test(fcopyfile_one(src, dst, res), "fcopyfile(%s, %s)", src, dst);
	}

	return rc;
}

/* only checks success, correct file size verified by _sz() */
static int copyfile_one(const char *src, const char *dst, const char *expected)
{
	int rc;

	rc = copyfile(src, dst, 0, 0);
	if (!rc)
		rc = 1;
	else if (!fexist(expected))
		rc = 1;
	else
		rc = 0;

	erase(expected);

	return rc;
}

static int _copyfile()
{
	int i, rc = 0;

	for (i = 0; files[i]; i += 3) {
		char *src = files[i];
		char *dst = files[i + 1];
		char *res = files[i + 2];

		rc += test(copyfile_one(src, dst, res), "copyfile(%s, %s)", src, dst);
	}

	return rc;
}

int main(void)
{
	return	_fcopyfile() ||
		_copyfile()  ||
		_sz();
}
