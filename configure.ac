AC_INIT(libite, 2.5.1, https://github.com/troglobit/libite/issues)
AC_CONFIG_AUX_DIR(aux)
AM_INIT_AUTOMAKE([1.11 foreign dist-xz])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR(src/lite.h)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([Makefile doc/Makefile doc/Doxyfile src/Makefile src/libite.pc test/Makefile])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_INSTALL
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT

# Check for Doxygen and enable its features.
# For details, see m4/ax_prog_doxygen.m4 and
# http://www.bioinf.uni-freiburg.de/~mmann/HowTo/automake.html#doxygenSupport
DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_CHM_FEATURE(OFF)
DX_HTML_FEATURE(ON)
DX_MAN_FEATURE(OFF)
DX_INIT_DOXYGEN(${PACKAGE_NAME}, [${top_builddir}/doc/Doxyfile], [${top_builddir}/doc])

AC_ARG_WITH([symlink],
        AS_HELP_STRING([--without-symlink], [Disable compat /usr/include/lite -> libite symlink]),,
	[with_symlink=yes])

AM_CONDITIONAL([ENABLE_DOXYGEN], [test "x${DX_FLAG_doc}"  = x1])
AM_CONDITIONAL([ENABLE_HTML],    [test "x${DX_FLAG_html}" = x1])
AM_CONDITIONAL([COMPAT_SYMLINK], [test "x$with_symlink"  != "xno"])

AC_OUTPUT
