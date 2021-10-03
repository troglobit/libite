#include <fcntl.h>		/* O_TMPFILE requires -D_GNU_SOURCE */
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include "check.h"

#ifndef  O_TMPFILE
#warning O_TMPFILE is missing, either too old GLIBC, Linux, or non-Linux system
#endif

#define READBACK "Reality is frequently inaccurate.\n"


int main(void)
{
	char buf[80] = "";
	FILE *fp;

	printf("Before tempfile():\n");
	system("ls -lrt " _PATH_TMP " | tail -3");
	fflush(stdout);

	fp = tempfile();
	if (fp == NULL)
		fprintf(stderr, "Failed tempfile(), errno %d: %s", errno, strerror(errno));
	fail_unless(fp != NULL);

	fputs(READBACK, fp);
	rewind(fp);
	if (fgets(buf, sizeof(buf), fp))
		fprintf(stderr, "\nRead-back from tempfile: %s", buf);
	fail_unless(!strcmp(buf, READBACK));

	printf("\nAfter tempfile(), should be same list:\n");
	system("ls -lrt " _PATH_TMP " | tail -3");
	fflush(stdout);

	return fclose(fp);
}
