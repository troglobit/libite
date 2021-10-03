#include "check.h"
#define SYSLOG_NAMES
#include <syslog.h>

#define SEVERITY_LEVEL {		     \
   CHOOSE(SEVERITY_NONE,    "none"),         \
   CHOOSE(SEVERITY_DEBUG,   "debug"),        \
   CHOOSE(SEVERITY_INFO,    "info"),         \
   CHOOSE(SEVERITY_NOTICE,  "notice"),       \
   CHOOSE(SEVERITY_WARNING, "warning"),      \
   CHOOSE(SEVERITY_ERR,     "err"),          \
   CHOOSE(SEVERITY_CRIT,    "crit"),         \
   CHOOSE(SEVERITY_ALERT,   "alert"),        \
   CHOOSE(SEVERITY_EMERG,   "emerg")         \
}

#define CHOOSE(a, b) b
const char *levels[] = SEVERITY_LEVEL;

#undef CHOOSE
#define CHOOSE(a, b) a
typedef enum SEVERITY_LEVEL severity_level_t;

int main(void)
{
	size_t i;
	int num;
	struct {
		char *str;
		int   val;
	} t[] = {
		{ "warn", SEVERITY_WARNING },
		{ "alert", SEVERITY_ALERT },
		{ NULL, 0 }
	};

	for (i = 0; t[i].str; i++) {
		num = strmatch(t[i].str, levels);
		fail_unless(num >= 0);
		fail_unless(num == t[i].val);
	}

	return 0;
}
