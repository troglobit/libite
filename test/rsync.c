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

int check_dir_to_dir(void)
{
	int rc = 0;

	rc += rsync(SRC, DST, LITE_FOPT_KEEP_MTIME, NULL);
	rc += systemf("diff -rq %s %s", SRC, DST);
	systemf("rm -rf %s/*", DST);

	return rc;
}

int check_file_to_file(void)
{
	int rc = 0;

	touch(SRC "foo.txt");
	PRINT("SRC ==========================\n");
	if (verbose) system("ls -l " SRC);
	rc += rsync(SRC "foo.txt", DST "foo.txt", LITE_FOPT_KEEP_MTIME, NULL);
	if (!fexist(DST "foo.txt") || fisdir(DST "foo.txt")) {
		PRINT("No dst file or it's a directory\n");
		rc++;
	}

	PRINT("DST ==========================\n");
	if (verbose) system("ls -l " DST);
	systemf("rm -rf %s %s/*", SRC "foo.txt" , DST);

	return rc;
}

int check_file_to_dir(void)
{
	int rc = 0;

	touch(SRC "foo.txt");
	PRINT("SRC ==========================\n");
	if (verbose) system("ls -l " SRC);
	rc += rsync(SRC "foo.txt", DST, LITE_FOPT_KEEP_MTIME, NULL);
	if (!fexist(DST "foo.txt") || fisdir(DST "foo.txt"))
		rc++;
	PRINT("DST ==========================\n");
	if (verbose) system("ls -l " DST);
	systemf("rm -rf %s %s/*", SRC "foo.txt" , DST);

	return rc;
}

int check_file_to_nodir(void)
{
	int rc = 0;

	touch(SRC "foo.txt");
	PRINT("SRC ==========================\n");
	if (verbose) system("ls -l " SRC);
	system("rm -rf " DST);
	rc += rsync(SRC "foo.txt", DST, LITE_FOPT_KEEP_MTIME, NULL);
	if (!fexist(DST "foo.txt") || fisdir(DST "foo.txt"))
		rc++;
	PRINT("DST ==========================\n");
	if (verbose) system("ls -l " DST);
	systemf("rm -rf %s %s/*", SRC "foo.txt" , DST);

	return rc;
}

int run_test(void)
{
	int result = 0;

	setup_test();

	result += test(check_dir_to_dir(),   "Verifying src/        -> dst/");
	result += test(check_file_to_file(), "Verifying src/foo.txt -> dst/foo.txt");
	result += test(check_file_to_dir(),  "Verifying src/foo.txt -> dst/");
	result += test(check_file_to_nodir(),"Verifying src/foo.txt -> dst/ (non-existing target dir)");

	return result;
}

int main(int argc, char *argv[])
{
	if (argc > 1)
		verbose = !strncmp("-v", argv[1], 2);

	atexit(cleanup_test);

	return run_test();
}
