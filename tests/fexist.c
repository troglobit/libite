#include "check.h"

int main(void)
{
	size_t i = 0;
	struct { char *file; int exist; } arr[] = {
		{ "/etc/passwd", 1 },
		{ "/etc/kalle",  0 },
		{ "/sbin/init",  1 },
		{ "/dev/null",   1 },
		{ NULL,  0 },
	};

	for (i = 0; i < NELEMS(arr); i++) {
		if (fexist(arr[i].file) != arr[i].exist)
			err(1, "Failed fexist(%s)", arr[i].file ?: "NULL");
		else
			printf("File %-11s %-14s => OK!\n", arr[i].file ?: "NULL",
			       arr[i].exist ? "does exist" : "does not exist");
	}

	return 0;
}
