#include <stdio.h>
#include "check.h"

#define FSTAB "/etc/fstab"

static int fstab(void)
{
	int field = 1;
	char *token, *dev;
	lfile_t *lf;

	lf = lfopen(FSTAB, " \t\n");
	if (!lf) {
		perror("Failed opening " FSTAB);
		return 1;
	}

	while ((token = lftok(lf))) {
		if (field == 1)
			dev = token;
		if (field == 6) {
			field = 0;
			printf("Found %s passno %d\n", dev, atoi(token));
		}

		field++;
	}

	lfclose(lf);

	return 0;
}

int main(void)
{
	int val;

	val = fgetint("/etc/protocols", " \n\t", "udp");
	if (val == -1) {
		perror("Failed locating 'udp' protocol");
		return 1;
	}
	printf("udp has proto %d\n", val);

	val = fgetint("/etc/services", " /\n\t", "ftp");
	if (val == -1) {
		perror("Failed locating 'ftp' service");
		return 1;
	}
	printf("ftp is inet port %d\n", val);

	val = fgetint("/etc/group", "x:\n", "utmp");
	if (val == -1) {
		perror("Failed locating group 'utmp'");
		return 1;
	}
	printf("utmp is gid %d\n", val);

	val = fgetint("/etc/passwd", "x:\n", "nobody");
	if (val == -1) {
		perror("Failed locating user 'nobody'");
		return 1;
	}
	printf("nobody is uid %d\n", val);

	return fstab();
}
