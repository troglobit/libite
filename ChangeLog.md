Change Log
==========

All notable changes to the project are documented in this file.


[v2.5.1][] - 2021-12-02
-----------------------

Debian-only fix.

### Fixes
- Use `dh_link(1)` to install `/usr/include/lite -> libite/`.  The
  symlink created by libite installer itself is not kept.


[v2.5.0][] - 2021-11-30
-----------------------

> **Note:** namespace change; the -lite library include files have moved
>           from `lite/*.h` to `libite/*.h` due to an unfortunate clash
>           with "LiTE is a Toobox Engine", from the DirectFB project.
>           For a transitional period, however, a compatibility symlink
>           is installed; `lite -> libite/`.

### Changes
- Header files have moved from `lite/*.h` to `libite/*.h` to prevent
  clashing with DirectFB LiTE library.
- Include guards in header files have been renamed, much in the same
  fashion as the move (above), from `LITE_foo_H_` to `LIBITE_FOO_H_`
- Add configure option to disable compatibility symlink, for systems
  that do not want name clashes with DirectFB/LiTE, or do not need to
  be backwards compatible; use `configure --without-symlink`

### Fixes
- Minor, use defensive coding in `strnlen()` replacement.  Only used
  on systems that do not have a native implementation.


[v2.4.1][] - 2021-10-09
-----------------------

### Changes
- Add doxygen API documentation to project, included in tarball, and
  automatically generated at https://codedocs.xyz/troglobit/libite/
- Relocate `tests/` to `test/` in source tree
- Relocate `docs/` to `doc/` in source tree

### Fixes
- Prefix CFLAGS/CPPFLAGS/etc with `AM_` in `test/Makefile.am` to prevent
  clashing with environment.  Fix imported from libuEv
- Replace obsolete `AM_CONFIG_HEADER` with `AC_CONFIG_HEADERS`
- Fix issue with `tempfile()` returning EOPNOTSUPP on Azure, use
  fallback to `mkostemp()` with `O_CLOEXEC` flag set


[v2.4.0][] - 2021-03-27
-----------------------

### Changes

- Add support for `touchf()`, formatted string support for `touch()`
- Add support for `erasef()`, formatted string support for `erase()`
- Add support for `strtrim()`, trims leading and trailing white-space
  from a string

### Fixes

- Fix `systemf()` improper handling of `system()` return value.  Now
  checks if exited OK, then returning the exit status of the command, or
  if not exited OK, then checks if the command was signaled, and returns
  -1 with `errno` set.  This also allows for returning 127, like
  `system()`, if the shell, or the program itself, did not exist


[v2.3.1][] - 2021-02-12
-----------------------

### Changes
- Copyright updates, including LICENSE file, year + author last name

### Fixes
- Debian packaging fixes only


[v2.3.0][] - 2021-02-12
-----------------------

### Changes
- Add support for `fremove()`, formatted `remove()` replacement
- Add support for `truncatef()`, formatted `truncate()` replacement
- Add tar.gz archives to distribution

### Fixes
- Handle `ETIMEDOUT` error for `connect()` call in `telnet_open()`


[v2.2.0][] - 2020-03-16
-----------------------

### Changes
- Add support for `systemf()`, formatted string `system()` replacement
- Add support for `fmkpath()`, formatted string `mkpath()`
- Add support for `fopenf()`, formatted string `fopen()` replacement

### Fixes
- Fix possible memory leak in `rsync()`, found by Coverity Scan
- Check return value of `fstat()` to prevent bogus destination file
  times when using `copyfile()`, found by Coverity Scan


[v2.1.2][] - 2020-02-22
-----------------------

### Fixes
- Fix Debian package upgrade from libite -> libite5


[v2.1.1][] - 2020-02-22
-----------------------

### Changes
- Add test/example for `printhdr()`
- Update copyright years

### Fixes
- chomp: Check for beginning of string while removing \n's


[v2.1.0][] - 2019-08-26
-----------------------

### Changes
- Add support for `strmatch()` and `strnmatch()`
- Add support for `yorn()`, safe yes-or-no prompt
- Add support for telnet expect/session APIs
- Add modified `printhdr()` which takes ANSI attribute

### Fixes
- Fix `pidfile_signal()`: always return result of `kill()`, or the
  result of `remove()` if we successfully sent `SIGKILL`
- Fix `strtonum()`: POSIX `strtoll()` returns `EINVAL`, check that
- Fix #13, support C++ `extern "C" {}` for header files


[v2.0.2][] - 2018-05-10
-----------------------

### Fixes
- Fix issue #12, missing includes in `strlite.h` for standalone use
- FTBFS `strtonum.c` on older systems w/o `LLONG_MAX` and `LLONG_MIN`


[v2.0.1][] - 2017-12-22
-----------------------

### Changes
- Add missing `initscr()` declaration, regression in [v2.0.0][]
- Add `#ifdef` guards around `min()` and `max()` macros
- Add `const` to `char *` args in multiple API:s:
  - `fexist()`
  - `fgetint()`
  - `fisslashdir()`
  - `copyfile()`
  - `movefile()`
  - `ifconfig()`
  - `lfopen()`
  - `lfgetkey()`
  - `lfgetint()`

### Fixes
- Fix #9, uncommon location of `which` command in unit test
- Fix #10, supply unit test with a default `$PATH`
- Fix #11, `touch()` command cannot handle relative paths when updating
  the mtime of an existing file


[v2.0.0][] - 2017-11-22
-----------------------

### Changes
- Remove `tree()`, moved to toolbox.git instead
- Remove `fmode()`, please use `stat()` instead, it's safer
- Add support for keeping mtime in `copy()` and `rsync()`
- Move source files to an `src/` sub-directory
- Move `initscr()` from `conio.h` to `conio.c`
- Update docs, recommend `mkpath()` over `makepath()`
- Bump ABI version, major changes to library

### Fixes
- Fix #7, inconsistent return value in `mkpath()` and `makepath()`
- Fix #8, constify `mkpath()` and `makepath()`


[v1.9.3][] - 2017-10-24
-----------------------

### Changes
- `initscr()`, use stdout, not stderr, like the rest of the
  `conio.h` macros.  Also, stderr may be redirected by user
- `tree()`, update unit test, use standard directries

### Fixes
- `initscr()`, verify that both stdin and stdout are real TTYs
  before messing up output or blocking on input for the user
- `tree()`, increase internal buffer slightly, output may be
  truncated otherwise.  Found by GCC 7.2


[v1.9.2][] - 2017-07-09
-----------------------

### Changes
- Add `initscr()` function to query screen size
- Convert `printheader()` to a static inline function


[v1.9.1][] - 2017-07-03
-----------------------

### Changes
- Add missing `whichp()` predicate function to accompany `which()`
- Allow `which("ps aux")`, which skips any arguments to command


[v1.9.0][] - 2017-07-02
-----------------------

### Changes
- Add `which()`, a C implementation of UNIX which(1)
- Add `printheader()`, prints a top-like table heading for client tools
- Remove `UNUSED()` macro, recommended to disable GCC warning instead

### Fixes
- Fix issue #6: `touch()` does not update mtime if file exists
- Fix non-writable `tempfile()`, now with proper unit test


[v1.8.3][] - 2017-01-08
-----------------------

### Changes
- Support absolute path in `pidfile()`.  Needed by, e.g. Inadyn
- Add initial `debian/` packaging
- Update README with note on remembering to set `umask()`

### Fixes
- In use-cases when `pidfile()` is called consecutively, e.g. on
  `SIGHUP`, recreate PID file if it no longer exists


[v1.8.2][] - 2016-09-05
-----------------------

### Changes
- Updated build instructions in README.
- Converted `touch()`, `makedir()`, `makefifo()`, and `erase()` from
  macros to inline functions.  Also removed implicit logging to stderr
  on error.

### Fixes
- Fix small memory leak in `pidfile()` ... at exit.
- Fixes for building on musl libc


[v1.8.1][] - 2016-06-25
-----------------------

### Fixes

- `fparseln()`, portability fix for NetBSD
- `pidfile()`, portability fix for NetBSD and OpenBSD


[v1.8.0][] - 2016-06-06
-----------------------

### Changes
- Change return type of `fsendfile()` from `size_t` to `ssize_t`
- Change `dir()` semantics: do not assert and always return error
  without calling `perror()` or similar.
- The OpenBSD `pidfile()` function's prefix path `_PATH_VARRUN`
  can now be overridden by setting the hidden `__pidfile_path`
  variable.  E.g., to `_PATH_TMP`.  Make sure read the docs!
- Add new unit test framework `make check`.

### Fixes
- Fix bad path in new `strndupa()` header file.
- Add missing progress bar API's to `lite.h`


[v1.7.0][] - 2016-06-05
-----------------------

### Changes

- Add support for GNU `strdupa()`, for [Inadyn][] on NetBSD.
- Add support for GNU `strndupa()` and `strnlen()`

### Fixes

- Fix `mkpath()` to handle creating relative directories.


[v1.6.0][] - 2016-05-13
-----------------------

### Changes

- Add support for NetBSD `fparaseln()` for the benefit of [Mg][]
- Add support for OpenBSD `reallocarray()` for the benefit of [Mg][]
- Constify `fisdir()` argument


[v1.5.1][] - 2016-04-17
-----------------------

Minor fix release.

### Changes
- Update README.md, add help when libite is used in autotools projects.
  Spell checking and minor cleanup.

### Fixes
- Activate use of `utimensat()` when `pidfile()` is called multiple
  times.  This reduces the overhead of calling open+write+close to
  simply updating the mtime of the PID file.
- Westermo fixes for `fcopyfile()`: nothing copied on embedded PPC
  system when using `tmpfile()` as source.


[v1.5.0][] - 2016-03-20
-----------------------

This release changes the header file namespace.  Apologies for any
problems this may cause!

### Changes
- Change namespace for header files: `libite/` to `lite/`, e.g. using
  `pkg-config` your C program must now `#include <lite/lite.h>`
- Update `pidfile()` to use `utimensat()` to update atime+mtime instead
  of opening and writing PID again.  Less overhead and nanonsecond res.


[v1.4.4][] - 2016-02-23
-----------------------

### Changes
- Add `ibite.pc.in` for `pkg-config` support.

### Fixes
- Fix issue #3: `tempfile()` fails with bad `O_TMPFILE` on some embedded
  targets, in particular PowerPC.  On systems with older GLIBC but with
  a sufficiently new kernel `O_TMPFILE` is missing and the local libite
  definition was wrong, hard coded for x86.  Fixed by Johan Askerin.


[v1.4.3][] - 2016-02-02
-----------------------

### Fixes
- Remove symlinks to Markdown files from GIT
- Distribute and install Markdown files: README.md, etc.


[v1.4.2][] - 2016-01-23
-----------------------

### Fixes
- Fix issue #2: GCC6 `-Wmisleading-indentation` causing FTBFS in pimd,
  which enables `-Werror`


[v1.4.1][] - 2016-01-22
-----------------------

### Fixes
- Fix install path for include files, regression in v1.4.0 With the
  introduction of GNU autotools include file namespace was unfortunately
  lost.  All include files were therefore installed in $(prefix)/include
  instead of $(prefix)/include/libite.


[v1.4.0][] - 2016-01-22
-----------------------

### Changes
- Change to GNU configure and build system
- Add Niels Provos' splay and red-black tree implementation from OpenBSD
  `sys/tree.h`, r1.14
- Make `pidfile()` file name accessible as `__pidfile_name`
- Step ABI revision to 2.1, with help from http://250bpm.com/blog:41


[v1.3.0][] - 2016-01-07
-----------------------

### Changes
- Add highly useful typeless `min()`/`max()` macros to `lite.h`,
  courtesy of Tobias Waldekranz
- Add BSD `queue.h` and Borland inspired `conio.h` to `install-dev`
  target.  Needed by [Finit][].
- Note, this release update `queue.h` to OpenBSD v1.43, which removes
  support for circular queues (CIRCLEQ).
- Update `strlcpy.c` to OpenBSD v1.12, readability fixes.
- Update `strlcat.c` to OpenBSD v1.14, readability fixes.
- Update `strtonum.c` to OpenBSD v1.7, tiniest of style tweaks.

### Fixes
- Only update mtime if `pidfile()` is called more than once, do not
  install multiple `atexit()` handlers.
- Add `#ifndef pidfile`, as for other BSD functions, in case the user
  already have a local copy, or a C library with one.


[v1.2.0][] - 2015-11-23
-----------------------

### Changes
-  Add simple parser for UNIX configuration files, e.g. `/etc/group`,
   `/etc/passwd`, and `/etc/protocols`:
  - `lfopen()`
  - `lfclose()`
  - `lftok()`
  - `lfgetkey()`
  - `lfgetint()`
  - `fgetint()`

### Fixes
- Do not allow `VERSION` to be overloaded by build system.
- Make sure we don't inherit `LDFLAGS` from environment.


[v1.1.1][] - 2015-09-27
-----------------------

### Fixes
* Silence annoying warnings from GNU ar
* Fix macro definitions for compat APIs


[v1.1.0][] - 2015-09-16
-----------------------

### Changes
- Support for `make distdir` used by automake based projects
- `lite.h` move from `error.h` to more BSD friendly `err.h`
- Add highly useful `NELEMS()` and bitmanip macros
- Lots of autotests added
- Add support for running autotests from Travis
- Add `progress()` simple progress bar
- Add `tree()` simple command line tree replacement
- Bump `SONAME` to `libite.so.2` on behalf of [Finit][]

### Fixes
- `pidfile()` upgrade to OpenBSD r1.11
- Lots of README updates, fixes and additions


[v1.0.0][] - 2015-07-12
-----------------------

Initial extraction of frog DNA from [Finit][].  See [README][] for API details.


[UNRELEASED]: https://github.com/troglobit/libite/compare/v2.5.1...HEAD
[v2.5.1]: https://github.com/troglobit/libite/compare/v2.5.0...v2.5.1
[v2.5.0]: https://github.com/troglobit/libite/compare/v2.4.1...v2.5.0
[v2.4.1]: https://github.com/troglobit/libite/compare/v2.4.0...v2.4.1
[v2.4.0]: https://github.com/troglobit/libite/compare/v2.3.1...v2.4.0
[v2.3.1]: https://github.com/troglobit/libite/compare/v2.3.0...v2.3.1
[v2.3.0]: https://github.com/troglobit/libite/compare/v2.2.1...v2.3.0
[v2.2.1]: https://github.com/troglobit/libite/compare/v2.2.0...v2.2.1
[v2.2.0]: https://github.com/troglobit/libite/compare/v2.1.2...v2.2.0
[v2.1.2]: https://github.com/troglobit/libite/compare/v2.1.1...v2.1.2
[v2.1.1]: https://github.com/troglobit/libite/compare/v2.1.0...v2.1.1
[v2.1.0]: https://github.com/troglobit/libite/compare/v2.0.2...v2.1.0
[v2.0.2]: https://github.com/troglobit/libite/compare/v2.0.1...v2.0.2
[v2.0.1]: https://github.com/troglobit/libite/compare/v2.0.0...v2.0.1
[v2.0.0]: https://github.com/troglobit/libite/compare/v1.9.3...v2.0.0
[v1.9.3]: https://github.com/troglobit/libite/compare/v1.9.2...v1.9.3
[v1.9.2]: https://github.com/troglobit/libite/compare/v1.9.1...v1.9.2
[v1.9.1]: https://github.com/troglobit/libite/compare/v1.9.0...v1.9.1
[v1.9.0]: https://github.com/troglobit/libite/compare/v1.8.3...v1.9.0
[v1.8.3]: https://github.com/troglobit/libite/compare/v1.8.2...v1.8.3
[v1.8.2]: https://github.com/troglobit/libite/compare/v1.8.1...v1.8.2
[v1.8.1]: https://github.com/troglobit/libite/compare/v1.8.0...v1.8.1
[v1.8.0]: https://github.com/troglobit/libite/compare/v1.7.0...v1.8.0
[v1.7.0]: https://github.com/troglobit/libite/compare/v1.6.0...v1.7.0
[v1.6.0]: https://github.com/troglobit/libite/compare/v1.5.1...v1.6.0
[v1.5.1]: https://github.com/troglobit/libite/compare/v1.5.0...v1.5.1
[v1.5.0]: https://github.com/troglobit/libite/compare/v1.4.4...v1.5.0
[v1.4.4]: https://github.com/troglobit/libite/compare/v1.4.3...v1.4.4
[v1.4.3]: https://github.com/troglobit/libite/compare/v1.4.2...v1.4.3
[v1.4.2]: https://github.com/troglobit/libite/compare/v1.4.1...v1.4.2
[v1.4.1]: https://github.com/troglobit/libite/compare/v1.4.0...v1.4.1
[v1.4.0]: https://github.com/troglobit/libite/compare/v1.3.0...v1.4.0
[v1.3.0]: https://github.com/troglobit/libite/compare/v1.2.0...v1.3.0
[v1.2.0]: https://github.com/troglobit/libite/compare/v1.1.1...v1.2.0
[v1.1.1]: https://github.com/troglobit/libite/compare/v1.1.0...v1.1.1
[v1.1.0]: https://github.com/troglobit/libite/compare/v1.0.0...v1.1.0
[v1.0.0]: https://github.com/troglobit/libite/compare/TAIL...v1.0.0
[Finit]:  https://github.com/troglobit/finit/
[Inadyn]: https://github.com/troglobit/inadyn/
[Mg]:     https://github.com/troglobit/mg
[README]: https://github.com/troglobit/libite/blob/v1.0.0/README.md
