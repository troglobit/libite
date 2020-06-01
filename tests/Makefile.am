EXTRA_DIST      = check.h \
  data/fexist/dir/.placeholder data/fexist/executable data/fexist/link \
  data/fexist/regular data/lfile/fstab data/lfile/group data/lfile/passwd \
  data/lfile/protocols data/lfile/services data/which/executable \
  data/which/executable-link data/which/regular
CLEANFILES      = *~ *.trs *.log

TESTS_ENVIRONMENT = ([ -d data ] || ln -sf $(srcdir)/data) &&

clean-local:
	-find -type l -name data -exec rm {} \;

TESTS           =
TESTS          += chomp
TESTS          += copyfile
TESTS          += dir
TESTS          += fexist
TESTS          += fisdir
TESTS          += fopenf
TESTS          += fsendfile
TESTS          += lfile
TESTS          += makepath
TESTS          += pidfile
TESTS          += printhdr
TESTS          += progress
TESTS          += rsync
TESTS          += str
TESTS          += strmatch
TESTS          += systemf
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
