#include <fcntl.h>		/* O_TMPFILE requires -D_GNU_SOURCE */
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include "check.h"

#ifndef  O_TMPFILE
#warning O_TMPFILE is missing, either too old GLIBC, Linux, or non-Linux system
#endif

int main(void)
{
	FILE *fp;

	printf("Before tempfile():\n");
	system("ls -lrt " _PATH_TMP " | tail -3");

	fp = tempfile();
	fail_unless(fp != NULL);

	printf("\nAfter tempfile(), should be same list:\n");
	system("ls -lrt " _PATH_TMP " | tail -3");

	return fclose(fp);
}
