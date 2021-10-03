#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "check.h"

int main(void)
{
	int chan[2];
	int status;

	pipe(chan);

	if (fork() > 0) {
		dup2(chan[0], STDIN_FILENO);
		if (yorn("Do you really wanna do this (y/N)? ")) {
			printf("Affirmative!\n");
			_exit(0);
		}

		printf("OK, aborting!\n");
		_exit(1);
	}

	sleep(1);
	if (write(chan[1], "Y", 1) != 1)
		return 1;

	wait(&status);

	return WEXITSTATUS(status);
}
