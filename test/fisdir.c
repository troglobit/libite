#include <err.h>
#include "check.h"

int main(void)
{
	size_t i = 0;
	struct { char *file; int exist; } arr[] = {
		{ "/etc/passwd", 0 },
		{ "/etc/",       1 },
		{ "/sbin/init",  0 },
		{ "/dev/null",   0 },
		{ "/dev/",       1 },
		{ NULL,  0 },
	};

	for (i = 0; i < NELEMS(arr); i++) {
		if (fisdir(arr[i].file) != arr[i].exist)
			err(1, "Failed fisdir(%s)", arr[i].file ?: "NULL");
		else
			printf("fisdir() %-11s %-18s => OK!\n", arr[i].file ?: "NULL",
			       arr[i].exist ? "is a directory" : "is NOT a directory");
	}

	return 0;
}
