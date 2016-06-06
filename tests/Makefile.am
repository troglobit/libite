EXTRA_DIST      = check.h
CLEANFILES      = *~ *.trs *.log

TESTS           =
TESTS          += makepath
TESTS          += pidfile
TESTS          += tree

check_PROGRAMS  = $(TESTS)

# Ignore warnings about unused result, in e.g. write()
CFLAGS          = -W -Wall -Wextra -Wno-unused-result
LDADD           = -L../ ../libite.la
