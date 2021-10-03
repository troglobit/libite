#include "../src/strlite.h"

int main(void)
{
	char buf[10];
	int val;

	val = atonum("42");
	if (val != 42)
		return 1;

	if (!string_match("eth1", "eth1234"))
		return 1;

	if (string_compare("eth1", "eth1234"))
		return 1;

	if (!string_case_compare("somestring", "SoMeSTRING"))
		return 1;

	strlcpy(buf, "tooloongstring", sizeof(buf));
	if (buf[9] == 'r')
		return 1;

	return 0;
}

