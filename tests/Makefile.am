EXTRA_DIST      = check.h
CLEANFILES      = *~

TESTS           =
TESTS          += pidfile
TESTS          += makepath

check_PROGRAMS  = $(TESTS)

# Ignore warnings about unused result, in e.g. write()
CFLAGS          = -W -Wall -Wextra -Wno-unused-result
LDADD           = -L../ ../libite.la
