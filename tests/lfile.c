#include <stdio.h>
#include "check.h"

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

	return 0;
}
