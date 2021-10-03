#include <err.h>
#include <errno.h>
#include "check.h"

int main(void)
{
	int i = 0;
	char *files[] = {
		"/etc/passwd", "/tmp/mypwd", "/tmp/mypwd",
		"/etc/passwd", "/tmp/", "/tmp/passwd",
		"/etc/passwd", "/tmp", "/tmp/passwd",
		NULL
	};
	FILE *src, *dst;

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

	return 0;
}
