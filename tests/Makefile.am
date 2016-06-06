EXTRA_DIST      = check.h
CLEANFILES      = *~

TESTS           =
TESTS          += pidfile_func

check_PROGRAMS  = $(TESTS)

# Ignore warnings about unused result, in e.g. write()
CFLAGS          = -Wno-unused-result
LDADD           = -L../ ../libite.la
