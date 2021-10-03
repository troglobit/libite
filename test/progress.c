#include <stdlib.h>		/* atexit() */
#include <unistd.h>		/* usleep() */

#include "check.h"

#define MAX_WIDTH    80
#define msleep(msec) usleep(msec * 1000)

static void bye(void)
{
	showcursor();
}

static void testit(int fn, int percent)
{
	if (fn)
		progress(percent, MAX_WIDTH);
	else
		progress_simple(percent);
}

static void run_test(int fn)
{
	int i, percent, block = 0, num = 85;

	while (block < num) {
		percent = block * 100 / num;
		for (i = 0; i < 10; i++) {
			testit(fn, percent);
			msleep(1);
		}
		block++;
		msleep(1);
	}
	testit(fn, 100);
}

int main(void)
{
	atexit(bye);
	hidecursor();

	printf("\nAdvanced:\n");
	run_test(1);
	printf("\nSimple:\n");
	run_test(0);
	printf("\n");

	return 0;
}
