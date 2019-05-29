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
TESTS          += str
TESTS          += strmatch
TESTS          += touch
TESTS          += tempfile
TESTS          += which
TESTS          += yorn

check_PROGRAMS  = $(TESTS)

# Ignore warnings about unused result, in e.g. write()
CFLAGS         += -Wno-unused-result
CPPFLAGS       += -D_GNU_SOURCE
LDADD           = -L../src/ ../src/libite.la
LDFLAGS        += -static
