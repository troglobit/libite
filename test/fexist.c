#include <err.h>
#include "check.h"

int main(void)
{
	size_t i = 0;
	struct { char *file; int exist; } arr[] = {
		{ "data/fexist/regular",      1 },
		{ "data/fexist/executable",   1 },
		{ "data/fexist/link",         1 },
		{ "data/fexist/nonexistence", 0 },
		{ "data/fexist/broken-link",  0 },
		{ "data/fexist/dir",          1 },
		{ "/dev/null",                1 },
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
