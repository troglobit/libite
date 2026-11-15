#include "check.h"

struct tc {
	char *cmd;
	int   exp;
	int   res;
};

/* Empty PATH components are skipped, they do not mean current directory */
static int empty(void)
{
	const char *cmd = "executable";
	char *path[] = {
		"",
		":",
		":/nonexistent",
		"/nonexistent:",
		"/nonexistent::/nonexistent",
		NULL
	};
	int result = 0;
	size_t i;

	if (chdir("data/which"))
		return 1;

	for (i = 0; path[i]; i++) {
		setenv("PATH", path[i], 1);
		if (whichp(cmd)) {
			fprintf(stderr, "%20s : found in cwd with PATH='%s'\n", cmd, path[i]);
			result++;
		}
	}

	setenv("PATH", "/nonexistent:.", 1);
	if (!whichp(cmd)) {
		fprintf(stderr, "%20s : not found with . in PATH\n", cmd);
		result++;
	}

	return result;
}

int main(void)
{
	int result = 0;
	size_t i;
	struct tc test[] = {
		{ "executable",      1, 0 },
		{ "executable -h",   1, 0 },
		{ "executable-link", 1, 0 },
		{ "regular",         0, 0 },
		{ "ls",              1, 0 },
		{ "/usr/bin/which",  1, 0 },
		{ NULL, 0, 0 }
	};

	setenv("PATH", "data/which:/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:/usr/local/sbin", 1);
	for (i = 0; test[i].cmd; i++) {
		char *path;

		path = which(test[i].cmd);
		if (!path) {
			test[i].res = 0;
			fprintf(stderr, "%20s : Not hot dog\n", test[i].cmd);
		} else {
			test[i].res = 1;
			fprintf(stderr, "%20s : hot dog --> %s\n", test[i].cmd, path);
			free(path);
		}
	}

	for (i = 0; test[i].cmd; i++)
		result += test[i].exp != test[i].res;

	return result + empty();
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
