#include <err.h>
#include <errno.h>
#include "check.h"

#define ERR(fmt, args...)  { printf(fmt, ##args);  rc = 1; }
#define ERRX(fmt, args...) { printf(fmt, ##args); rc = 1; }

static char *files[] = {
	"/etc/passwd", "/tmp/mypwd", "/tmp/mypwd",
	"/etc/passwd", "/tmp/",      "/tmp/passwd",
	"/etc/passwd", "/tmp",       "/tmp/passwd",
	NULL
};

static int sz()
{
	struct stat st;
	ssize_t len;

	printf("=>Start copyfile(/dev/zero, 32768)\n");
	len = copyfile("/dev/zero", "/tmp/zeroes", 32768, 0);
	if (!len)
		err(1, "failed copying any bytes from /dev/zero");

	if (stat("/tmp/zeroes", &st))
		err(1, "copied but cannot find destination");
	if (st.st_size != 32768)
		err(1, "file only %ld bytes, expected 32768", st.st_size);

	if (len != st.st_size)
		errx(1, "copied correct num bytes, 32768, but copyfile() returns %ld", len);

	printf("OK => %ld\n", len);

	return 0;
}

static int check_fcopyfile()
{
	FILE *src, *dst;
	int rc, i;

	printf("=>Start testing fcopyfile()\n");
	for (i = 0; files[i]; i += 3) {
		printf("fcopyfile(%s, %s)\t", files[i], files[i + 1]);
		src = fopen(files[i], "r");
		dst = fopen(files[i + 1], "w");

		rc = fcopyfile(src, dst);

		if (src)
			fclose(src);
		if (dst)
			fclose(dst);

		if (rc) {
			if (!fisdir(files[i + 1]))
				err(1, "Failed fcopyfile(%s, %s)", files[i], files[i + 1]);

			/* Expected to fail, user tried to copy to fp=NULL due to dir */
			printf("OK\n");
			continue;
		}

		if (fexist(files[i + 2])) {
			if (systemf("diff -q %s %s", files[i], files[i + 2])) {
				ERRX("src and dst differ");
			} else
				printf("OK => %s", files[i + 2]);
		} else
			ERR("cannot find %s", files[i + 2]);
		printf("\n");

		erase(files[i + 2]);

		if (rc)
			return rc;
	}

	return 0;
}

static int check_copyfile()
{
	int i;

	printf("\n=>Start testing copyfile()\n");
	for (i = 0; files[i]; i += 3) {
		printf("copyfile(%s, %s)\t", files[i], files[i + 1]);
		if (!copyfile(files[i], files[i + 1], 0, 0))
			err(1, "Failed copyfile(%s, %s)", files[i], files[i + 1]);

		if (fexist(files[i + 2]))
			printf("OK => %s", files[i + 2]);
		printf("\n");

		erase(files[i + 2]);
	}

	return 0;
}

int main(void)
{
	return	check_fcopyfile() ||
		check_copyfile()  ||
		sz();
}
