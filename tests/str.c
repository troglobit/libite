#include "../src/strlite.h"

int main(void)
{
	int val;

	val = atonum("42");
	if (val != 42)
		return 1;
	
	if (!string_match("eth1", "eth1234"))
		return 1;

	if (string_compare("eth1", "eth1234"))
		return 1;

	return 0;
}

