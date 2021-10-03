#include <stdlib.h>
#include "check.h"

#define BASE "/tmp/.unittest/"
#define SRC  BASE "src/"
#define DST  BASE "dst/"

static int verbose = 0;
static char *files[] = {
	SRC "sub1/1.tst",
	SRC "sub1/2.tst",
	SRC "sub1/3.tst",
	SRC "sub2/4.tst",
	SRC "sub2/5.tst",
	SRC "sub2/6.tst",
	SRC "sub3/7.tst",
	SRC "sub3/8.tst",
	SRC "sub3/9.tst",
	NULL
};

void cleanup_test(void)
{
	system("rm -rf " BASE);
}

void setup_test(void)
{
	int i;
	char cmd[256];
	mode_t dir_modes[]  = { 755, 700 };
	mode_t file_modes[] = { 644, 600 };

	cleanup_test();

	mkdir(BASE, 0755);
	mkdir(SRC, 0755);
	mkdir(DST, 0755);

	for (i = 0; files[i]; i++) {
		snprintf(cmd, sizeof(cmd), "mkdir -m %d -p `dirname %s`",
			 dir_modes[i % 2], files[i]);
		system(cmd);

		snprintf(cmd, sizeof(cmd), "touch %s; chmod %d %s", files[i],
			 file_modes[i % 2], files[i]);
		system(cmd);
	}
}

static void check_tree(char *heading, char *dir)
{
	if (verbose) {
		if (heading)
			puts(heading);
	}

	// XXX: Fixme, add code to list tree, nftw() possibly
}

int run_test(void)
{
	int result = 0;

#if 0
	setup_test();
	check_tree("Before:", BASE);

	result += rsync(SRC, DST, 0, NULL);
	check_tree("After:", BASE);
	cleanup_test();
#endif

	setup_test();
	sleep(1);
	result += rsync(BASE "src", DST, LITE_FOPT_KEEP_MTIME, NULL);
	check_tree("Only partial rsync of src <-- No slash!", BASE);
#if 0
	cleanup_test();
	setup_test();
	result += rsync(BASE "src/sub1", BASE "dst", 0, NULL);
	check_tree("Only partial rsync of src/sub1 <-- No slashes!!", BASE);

	cleanup_test();
	setup_test();
	result += rsync(BASE "src/sub1/", DST, 0, NULL);
	check_tree("Only partial rsync of src/sub1/", BASE);

	cleanup_test();
	setup_test();
	result += rsync(BASE "src/sub1", DST, 0, NULL);
	check_tree("Only partial rsync of src/sub1 <-- No slash!", BASE);

	result += rsync("/etc", "/var/tmp", 0, NULL);
	check_tree("Real life test:", "/var/tmp");
#endif

	return result;
}

int main(int argc, char *argv[])
{
	if (argc > 1)
		verbose = !strncmp("-v", argv[1], 2);

	atexit(cleanup_test);

	return run_test();
}
