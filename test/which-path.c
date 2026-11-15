/* Empty PATH components name the current directory, in their given order. */
#include "check.h"

static void executable(const char *path, mode_t mode)
{
	FILE *file = fopen(path, "w");

	fail_unless(file != NULL);
	fail_unless(fputs("#!/bin/sh\nexit 0\n", file) >= 0);
	fail_unless(fclose(file) == 0);
	fail_unless(chmod(path, mode) == 0);
}

static void lookup(const char *search, const char *command, const char *expected)
{
	char *path;

	if (search)
		fail_unless(setenv("PATH", search, 1) == 0);
	else
		fail_unless(unsetenv("PATH") == 0);
	path = which(command);
	if (expected) {
		fail_unless(path != NULL);
		fail_unless(strcmp(path, expected) == 0);
	} else
		fail_unless(path == NULL);
	fail_unless(whichp(command) == (expected != NULL));
	free(path);
}

int main(void)
{
	char temp[] = "/tmp/libite-which-XXXXXX";
	char absolute[256];
	char *saved_path = getenv("PATH") ? strdup(getenv("PATH")) : NULL;
	int saved_cwd = open(".", O_RDONLY);

	fail_unless(saved_cwd >= 0);
	fail_unless(mkdtemp(temp) != NULL);
	fail_unless(chdir(temp) == 0);
	fail_unless(mkdir("a", 0700) == 0);
	fail_unless(mkdir("b", 0700) == 0);
	executable("tool", 0700);
	executable("local-only", 0700);
	executable("blocked", 0600);
	executable("a/tool", 0700);
	executable("b/tool", 0700);
	fail_unless(symlink("tool", "tool-link") == 0);

	lookup(":a", "tool", "./tool");
	lookup("missing:", "tool", "./tool");
	lookup("missing::b", "tool", "./tool");
	lookup("a:", "tool", "a/tool");
	lookup("a::b", "local-only", "./local-only");
	lookup("::", "tool", "./tool");
	lookup("", "tool", "./tool");
	lookup("", "tool-link", "./tool-link");
	lookup("", "local-only --flag", "./local-only");
	lookup("", "blocked", NULL);
	lookup("missing:b", "tool", "b/tool");
	lookup("missing", "tool", NULL);
	lookup(NULL, "tool", NULL);
	snprintf(absolute, sizeof(absolute), "%s/tool", temp);
	lookup(NULL, absolute, absolute);

	if (saved_path)
		fail_unless(setenv("PATH", saved_path, 1) == 0);
	else
		fail_unless(unsetenv("PATH") == 0);
	free(saved_path);
	fail_unless(unlink("tool-link") == 0);
	fail_unless(unlink("b/tool") == 0);
	fail_unless(unlink("a/tool") == 0);
	fail_unless(unlink("blocked") == 0);
	fail_unless(unlink("local-only") == 0);
	fail_unless(unlink("tool") == 0);
	fail_unless(rmdir("a") == 0);
	fail_unless(rmdir("b") == 0);
	fail_unless(fchdir(saved_cwd) == 0);
	fail_unless(close(saved_cwd) == 0);
	fail_unless(rmdir(temp) == 0);

	return 0;
}
