#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "check.h"

#define DIR_TYPE_IMAGE  ".img"
#define DIR_TYPE_SYSLOG ""
#define DIR_TYPE_CONFIG ".cfg"
#define STARTUP_CONFIG  "startup-config.cfg"

int simulate_files(int creat)
{
	size_t i;
	char *files[] =
	    { "config0.cfg", "config1.cfg", "config2.cfg", "config3.cfg",
		"rr109.img", "rx100.img", "rx107.img", "rm957.img", "messages"
	};

	for (i = 0; i < NELEMS(files); i++) {
		if (creat)
			touch(files[i]);
		else
			erase(files[i]);
	}

	if (creat)
		symlink("config2.cfg", STARTUP_CONFIG);
	else
		erase(STARTUP_CONFIG);

	return 0;
}

static int cfg_dir_filter(const char *file)
{
	/* Skip the STARTUP_CONFIG file, it is a symbolic link to the
	 * current startup configuration. */
	return ! !strcmp(file, STARTUP_CONFIG);
}

static int is_startup_config(const char *entry)
{
	static int once = 1;
	static char file[80];

	if (once) {
		int len = readlink(STARTUP_CONFIG, file, sizeof(file));

		if (len == -1)
			return 0;

		file[len] = 0;
		once = 0;	/* Only once per call to dir() */
	}
	//printf ("Comparing link %s with entry %s\n", file, entry);
	return !strcmp(file, entry);
}

int main(int argc, char *argv[])
{
	int i, num;
	char *type = DIR_TYPE_CONFIG;
	char **files;

	simulate_files(1);

	if (argc >= 2) {
		if (!strcasecmp("CONFIG", argv[1])) {
			type = DIR_TYPE_CONFIG;
			system("ls -l *" DIR_TYPE_CONFIG);
		}
		if (!strcasecmp("IMAGE", argv[1])) {
			type = DIR_TYPE_IMAGE;
			system("ls -l *" DIR_TYPE_IMAGE);
		}
		if (!strcasecmp("SYSLOG", argv[1])) {
			type = DIR_TYPE_SYSLOG;
			system("ls -l *");
		}
	}

	num = dir(NULL, type, cfg_dir_filter, &files, 0);
	if (num) {
		for (i = 0; i < num; i++) {
			printf("%s", files[i]);
			if (is_startup_config(files[i]))
				printf(" --> startup-config");
			printf("\n");

			free(files[i]);
		}
		free(files);
	}

	simulate_files(0);

	return 0;
}
