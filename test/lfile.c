#include <stdio.h>
#include "check.h"

#define FSTAB "data/lfile/fstab"

static int fstab(void)
{
	int field = 1;
	char *token;
	lfile_t *lf;

	lf = lfopen(FSTAB, " \t\n");
	if (!lf) {
		perror("Failed opening " FSTAB);
		return 1;
	}

	printheader(NULL, "FILE SYSTEM                               MOUNT     OPTIONS            D P", 0);
	while ((token = lftok(lf))) {
		if (field == 1)
			printf("%-41s ", token);
		if (field == 2)
			printf("%-9s ", token);
		if (field == 4)
			printf("%-17s  ", token);
		if (field == 5)
			printf("%d ", atoi(token));
		if (field == 6) {
			field = 0;
			printf("%d\n", atoi(token));
		}

		field++;
	}

	lfclose(lf);

	return 0;
}

int main(void)
{
	int val;

	val = fgetint("data/lfile/protocols", " \n\t", "udp");
	if (val == -1) {
		perror("Failed locating 'udp' protocol");
		return 1;
	}
	printf("udp has proto %d\n", val);

	val = fgetint("data/lfile/services", " /\n\t", "ftp");
	if (val == -1) {
		perror("Failed locating 'ftp' service");
		return 1;
	}
	printf("ftp is inet port %d\n", val);

	val = fgetint("data/lfile/group", "x:\n", "utmp");
	if (val == -1) {
		perror("Failed locating group 'utmp'");
		return 1;
	}
	printf("utmp is gid %d\n", val);

	val = fgetint("data/lfile/passwd", "x:\n", "nobody");
	if (val == -1) {
		perror("Failed locating user 'nobody'");
		return 1;
	}
	printf("nobody is uid %d\n", val);

	return fstab();
}
