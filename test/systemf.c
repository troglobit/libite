#include <err.h>
#include "check.h"

int main(void)
{
	struct { char *cmd; int rc; } list[] = {
		{ "/app/enoent",  127 },
		{ "false",          1 },
		{ "true",           0 },
		{ "kill -9 $$",    -1 },
		{ "kill -QUIT $$", -1 },
		{ "kill -INT $$",  -1 }
	};

	for (size_t i = 0; i < NELEMS(list); i++) {
		int rc;

		rc = systemf("%s", list[i].cmd);
		if (rc != list[i].rc) {
			if (rc == -1)
				err(rc, "Failed %s", list[i].cmd);
			else
				errx(rc, "Failed %s, rc %d vs %d", list[i].cmd, rc, list[i].rc);
		}
	}

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */

