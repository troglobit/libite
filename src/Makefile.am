lib_LTLIBRARIES       = libite.la

libite_la_CPPFLAGS    = -D_GNU_SOURCE
libite_la_CFLAGS      = -W -Wall -Wextra
libite_la_LDFLAGS     = $(AM_LDFLAGS) -version-info 8:1:3
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


## Distribute these header files in the <libite/...> namespace
pkgincludedir       = $(includedir)/libite
pkginclude_HEADERS  = lite.h queue.h conio.h tree.h strlite.h strdupa.h	\
		      strndupa.h strnlen.h

pkgconfigdir        = $(libdir)/pkgconfig
pkgconfig_DATA      = libite.pc

## For compatibility with previous <lite/...> namespace
if COMPAT_SYMLINK
install-data-hook:
	link=$(DESTDIR)$(includedir)/lite;                   \
	if [ ! -f $$link ]; then                             \
		$(LN_S) libite $$link;                       \
	fi

uninstall-hook:
	link=$(shell readlink $(DESTDIR)$(includedir)/lite); \
	if [ "$$link" = "libite" ]; then                     \
		$(RM) $(DESTDIR)$(includedir)/lite;          \
	fi
endif
