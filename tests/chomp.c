#include "check.h"

int main(void)
{
	size_t i;
	char t[][16] = {
		"hej\ndej",
		"Slime\n\n\\n",
		"Tripple\n\n\n"
	};

	for (i = 0; i < NELEMS(t); i++) {
		char *str = t[i];
		size_t len;

		chomp(str);
		len = strlen(str);
		printf("[%02zd]: '%s'\n", i, str);

		fail_unless(str[len] != '\n');
	}

	return 0;
}
