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
	int ret;
	FILE *fp;
	char buf[80] = "";

	printf("Before tempfile():\n");
	ret = system("ls -lrt " _PATH_TMP " | tail -3");

	fp = tempfile();
	fail_unless(fp != NULL);

	ret = fputs(READBACK, fp);
	rewind(fp);
	if (fgets(buf, sizeof(buf), fp))
		fprintf(stderr, "\nRead-back from tempfile: %s", buf);
	fail_unless(!strcmp(buf, READBACK));

	printf("\nAfter tempfile(), should be same list:\n");
	ret = system("ls -lrt " _PATH_TMP " | tail -3");

	return fclose(fp);
}
