#include <err.h>
#include <unistd.h>
#include "check.h"

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

	return 0;
}
