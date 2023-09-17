#include "check.h"

#define file "/tmp/runbg.txt"

int main(void)
{
	char *cmd[] = {
		"sh", "-c", "echo 'hello world' > " file, NULL
	};

	erase(file);
	test(runbg(cmd, 100000), "Calling runbg");

	if (test(fexist(file), "Verifying %s does not yet exist", file))
		return 1;

	test(sleep(1), "Waiting for runbg");

	if (test(!fexist(file), "Verifying %s has been created", file))
		return 1;

	return 0;
}
