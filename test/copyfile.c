#include <err.h>
#include <errno.h>
#include "check.h"

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

int main(void)
{
	char *files[] = {
		"/etc/passwd", "/tmp/mypwd", "/tmp/mypwd",
		"/etc/passwd", "/tmp/",      "/tmp/passwd",
		"/etc/passwd", "/tmp",       "/tmp/passwd",
		NULL
	};
	FILE *src, *dst;
	int i = 0;

	printf("=>Start testing fcopyfile()\n");
	while (files[i]) {
		printf("fcopyfile(%s, %s)\t", files[i], files[i + 1]);
		src = fopen(files[i], "r");
		dst = fopen(files[i + 1], "w");
		if (fcopyfile(src, dst)) {
			if (!fisdir(files[i + 1]))
			    err(1, "Failed fcopyfile(%s, %s)", files[i], files[i + 1]);
		}

		if (src)
			fclose(src);
		if (dst)
			fclose(dst);

		if (fexist(files[i + 2]))
			printf("OK => %s", files[i + 2]);
		printf("\n");

		erase(files[i + 2]);
		i += 3;
	}

	printf("\n=>Start testing copyfile()\n");
	i = 0;
	while (files[i]) {
		printf("copyfile(%s, %s)\t", files[i], files[i + 1]);
		if (!copyfile(files[i], files[i + 1], 0, 0))
			err(1, "Failed copyfile(%s, %s)", files[i], files[i + 1]);

		if (fexist(files[i + 2]))
			printf("OK => %s", files[i + 2]);
		printf("\n");

		erase(files[i + 2]);
		i += 3;
	}

	return sz();
}
