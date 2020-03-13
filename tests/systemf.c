#include <err.h>
#include "check.h"

int main(void)
{
	struct { char *cmd; int rc; } list[] = {
		{ "false",          1 },
		{ "true",           0 },
		{ "kill -9 $$",     1 },
		{ "kill -QUIT $$", -1 },
		{ "kill -INT $$",  -1 }
	};

	for (size_t i = 0; i < NELEMS(list); i++) {
		int rc;

		rc = systemf("%s", list[i].cmd);
		if (rc != list[i].rc)
			err(rc, "Failed command %s", list[i].cmd);
	}

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */

