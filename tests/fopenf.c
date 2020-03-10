#include "check.h"

int main(void)
{
	FILE *fp;
	char *dir = "/tmp";
	char *file = "__foo__";

	fp = fopenf("w", "%s/%s", dir, file);
	if (fp) {
		fputs("foo", fp);
		fclose(fp);
	}

	fail_unless(fexist("/tmp/__foo__"));

	return 0;
}
