#include "check.h"

int main(void)
{
	int result = 0;
	size_t i;
	struct { char *path; int fail; } arr[] = {
		{ "/etc/passwd", -1 },
		{ "/dev",         0 },
		{ "/usr/sbin",    0 },
	};

	for (i = 0; i < NELEMS(arr); i++) {
		if (tree(arr[i].path, 1) != arr[i].fail)
			result++;
	}

	return result;
}
