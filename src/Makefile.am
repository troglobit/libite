lib_LTLIBRARIES     = libite.la

libite_la_CPPFLAGS  = -D_GNU_SOURCE
libite_la_CFLAGS    = -W -Wall -Wextra
libite_la_LDFLAGS   = $(AM_LDFLAGS) -version-info 4:1:0
libite_la_SOURCES   = chomp.c copyfile.c dir.c fexist.c fisdir.c fmode.c fparseln.c		\
		      fsendfile.c ifconfig.c lfile.c makepath.c pidfile.c pidfilefn.c		\
		      progress.c reallocarray.c rsync.c strlcpy.c strlcat.c strtonum.c		\
		      tempfile.c tree.c which.c

## Distribute these header files in the <lite/...> name space
pkgincludedir       = $(includedir)/lite
pkginclude_HEADERS  = lite.h queue.h conio.h tree.h strlite.h strdupa.h strndupa.h strnlen.h
