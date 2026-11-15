/* Existing non-directories must not satisfy a directory-creation request. */
#include <errno.h>
#include <sys/stat.h>
#include "check.h"

static int formatted(const char *path, mode_t mode)
{
	return fmkpath(mode, "%s", path);
}

static int legacy(const char *path, mode_t mode)
{
	(void)mode;
	return makepath(path);
}

int main(void)
{
	int (*create[])(const char *, mode_t) = { mkpath, formatted, legacy };
	char temp[] = "/tmp/libite-mkpath-XXXXXX";
	char file[256], link[256], fifo[256], dir[256], dirlink[256], nested[256];
	struct stat st;
	FILE *fp;
	size_t i;

	fail_unless(mkdtemp(temp) != NULL);
	snprintf(file, sizeof(file), "%s/file", temp);
	snprintf(link, sizeof(link), "%s/link", temp);
	snprintf(fifo, sizeof(fifo), "%s/fifo", temp);
	snprintf(dir, sizeof(dir), "%s/dir", temp);
	snprintf(dirlink, sizeof(dirlink), "%s/dirlink", temp);
	snprintf(nested, sizeof(nested), "%s/dirlink/child/grandchild", temp);
	fp = fopen(file, "w");
	fail_unless(fp != NULL);
	fail_unless(fputs("preserved", fp) >= 0);
	fail_unless(fclose(fp) == 0);
	fail_unless(symlink(file, link) == 0);
	fail_unless(mkfifo(fifo, 0600) == 0);
	fail_unless(mkdir(dir, 0700) == 0);
	fail_unless(symlink(dir, dirlink) == 0);

	for (i = 0; i < sizeof(create) / sizeof(create[0]); i++) {
		const char *bad[] = { file, link, fifo };
		size_t j;

		for (j = 0; j < sizeof(bad) / sizeof(bad[0]); j++) {
			errno = 0;
			fail_unless(create[i](bad[j], 0755) == -1);
			fail_unless(errno == ENOTDIR);
		}
		fail_unless(create[i](dir, 0755) == 0);
		fail_unless(create[i](dirlink, 0755) == 0);
		fail_unless(stat(dir, &st) == 0);
		fail_unless((st.st_mode & 0777) == 0700);
		fail_unless(create[i](nested, 0755) == 0);
		fail_unless(stat(nested, &st) == 0 && S_ISDIR(st.st_mode));
		fail_unless(rmdir(nested) == 0);
		snprintf(nested, sizeof(nested), "%s/dirlink/child", temp);
		fail_unless(rmdir(nested) == 0);
		snprintf(nested, sizeof(nested), "%s/dirlink/child/grandchild", temp);
	}

	fp = fopen(file, "r");
	fail_unless(fp != NULL);
	{
		char data[16] = { 0 };
		fail_unless(fread(data, 1, sizeof(data), fp) == 9);
		fail_unless(strcmp(data, "preserved") == 0);
	}
	fail_unless(fclose(fp) == 0);
	fail_unless(unlink(dirlink) == 0);
	fail_unless(rmdir(dir) == 0);
	fail_unless(unlink(fifo) == 0);
	fail_unless(unlink(link) == 0);
	fail_unless(unlink(file) == 0);
	fail_unless(rmdir(temp) == 0);

	return 0;
}
