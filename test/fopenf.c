#include "check.h"

int main(void)
{
	FILE *fp;
	char *dir = "/tmp";
	char *file = "__foo__";

	printf("Pass 1/2: fopenf() ===========================\n");
	fp = fopenf("w", "%s/%s", dir, file);
	if (fp) {
		fputs("foo", fp);
		fclose(fp);
	}

	fail_unless(fexist("/tmp/__foo__"));

	printf("Pass 2/2: fremove() ==========================\n");
	fremove("%s/%s", dir, file);
	fail_unless(!fexist("/tmp/__foo__"));

	printf("DONE =========================================\n");

	return 0;
}
