#include "check.h"

struct tc {
	char *cmd;
	int   exp;
	int   res;
};

int main(void)
{
	int result = 0;
	size_t i;
	struct tc test[] = {
		{ "ls",          1 },
		{ "free",        1 },
		{ "modinfo",     1 },
		{ "useradd",     1 },
		{ "/bin/which",  1 },
		{ "/bin/ps aux", 1 },
		{ "/etc/passwd", 0 },
		{ NULL, 0 }
	};

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

	return result;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
