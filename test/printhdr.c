#include <stdlib.h>		/* atexit() */
#include <unistd.h>		/* usleep() */

#include "check.h"

#define SCREEN_WIDTH    80

int main(void)
{
	FILE *fp = stderr;
	char buf[SCREEN_WIDTH];

	printhdr(fp, "", 0, UNDERSCORE);
	printhdr(fp, "Neighbor Table", 0, 0);
	snprintf(buf, sizeof(buf), "%-15s %-15s %7s %-5s%10s %6s",
		 "Neighbor", "Interface", "Version", "Flags", "Uptime", "Expire");
	printhdr(fp, buf, 0, REVERSE);

	return 0;
}
