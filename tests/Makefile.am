EXTRA_DIST      = check.h
CLEANFILES      = *~ *.trs *.log

TESTS           =
TESTS          += chomp
TESTS          += copyfile
TESTS          += dir
TESTS          += fexist
TESTS          += fisdir
TESTS          += fsendfile
TESTS          += lfile
TESTS          += makepath
TESTS          += pidfile
TESTS          += progress
TESTS          += rsync
TESTS          += tempfile
TESTS          += tree

check_PROGRAMS  = $(TESTS)

# Ignore warnings about unused result, in e.g. write()
CPPFLAGS        = -D_GNU_SOURCE
LDADD           = -L../ ../libite.la
LDFLAGS         = -static
