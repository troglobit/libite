lib_LTLIBRARIES       = libite.la

libite_la_CPPFLAGS    = -D_GNU_SOURCE
libite_la_CFLAGS      = -W -Wall -Wextra
libite_la_LDFLAGS     = $(AM_LDFLAGS) -version-info 8:0:3
libite_la_SOURCES     = chomp.c		copyfile.c			\
			conio.c		conio.h		dir.c		\
			erasef.c	fopenf.c	fremove.c	\
			fexist.c	fisdir.c			\
			fparseln.c	fsendfile.c			\
			ifconfig.c	lfile.c				\
			makepath.c	progress.c			\
			pidfile.c	pidfilefn.c			\
			reallocarray.c	rsync.c		strtrim.c	\
			strlcpy.c 	strlcat.c 	strtonum.c	\
			strdupa.h	strndupa.h	strnlen.h	\
			strmatch.c	systemf.c	touchf.c	\
			telnet.c	tempfile.c	truncatef.c	\
			yorn.c		which.c				\
			lite.h		strlite.h			\
			queue.h		tree.h


## Distribute these header files in the <lite/...> name space
pkgincludedir       = $(includedir)/lite
pkginclude_HEADERS  = lite.h queue.h conio.h tree.h strlite.h strdupa.h	\
		      strndupa.h strnlen.h
