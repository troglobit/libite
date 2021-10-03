AC_INIT(libite, 2.4.0, https://github.com/troglobit/libite/issues)
AC_CONFIG_AUX_DIR(aux)
AM_INIT_AUTOMAKE([1.11 foreign dist-xz])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR(src/lite.h)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([Makefile src/Makefile src/libite.pc test/Makefile])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_INSTALL
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT

AC_OUTPUT
