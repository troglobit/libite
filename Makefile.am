ACLOCAL_AMFLAGS     = -I m4

lib_LTLIBRARIES     = libite.la
pkgincludedir       = $(includedir)/lite
pkginclude_HEADERS  = lite.h queue.h conio.h tree.h strlite.h strdupa.h strndupa.h
libite_la_SOURCES   = chomp.c copyfile.c dir.c fexist.c fisdir.c fmode.c fparseln.c	\
		      fsendfile.c ifconfig.c lfile.c makepath.c pidfile.c pidfilefn.c	\
		      progress.c reallocarray.c rsync.c strlcpy.c strlcat.c strtonum.c	\
		      tempfile.c tree.c

libite_la_CPPFLAGS  = -D_GNU_SOURCE
libite_la_CFLAGS    = -W -Wall -Wextra
libite_la_LDFLAGS   = $(AM_LDFLAGS) -version-info 2:2:0

pkgconfigdir        = $(libdir)/pkgconfig
pkgconfig_DATA      = libite.pc
doc_DATA            = README.md LICENSE
EXTRA_DIST          = README.md LICENSE ChangeLog.md

## Generate MD5 checksum file
MD5 = md5sum
md5-dist:
	@for file in $(DIST_ARCHIVES); do	\
		$(MD5) $$file > $$file.md5;	\
	done

## Check if tagged in git
release-hook:
	if [ ! `git tag | grep $(PACKAGE_VERSION)` ]; then				\
		echo;									\
		printf "\e[1m\e[41mCannot find release tag $(PACKAGE_VERSION)\e[0m\n";	\
		printf "\e[1m\e[5mDo release anyway?\e[0m "; read yorn;			\
		if [ "$$yorn" != "y" -a "$$yorn" != "Y" ]; then				\
			printf "OK, aborting release.\n";				\
			exit 1;								\
		fi;									\
		echo;									\
	else										\
		echo;									\
		printf "\e[1m\e[42mFound GIT release tag $(PACKAGE_VERSION)\e[0m\n";	\
		printf "\e[1m\e[44m>>Remember to push tags!\e[0m\n";			\
		echo;									\
	fi

## Target to run when building a release
release: dist release-hook md5-dist
	@for file in $(DIST_ARCHIVES); do						\
		printf "$$file    \tDistribution tarball\n";				\
		printf "$$file.md5\t"; cat $$file.md5 | cut -f1 -d' ';			\
		mv $$file* ../;								\
	done
