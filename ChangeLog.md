Change Log
==========

All notable changes to the project are documented in this file.


[UNRELEASED][] - v1.3.0
-----------------------

### Changes
- Add highly useful typeless `min()`/`max()` macros to `lite.h`,
  courtesy of Tobias Waldekranz
- Add BSD `queue.h` and Borland inspired `conio.h` to `install-dev`
  target.  Needed by [Finit][].


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


[1.0.0][] - 2015-07-12
----------------------

Initial extraction of frog DNA from [Finit][].  See [README][] for API details.

[UNRELEASED]: https://github.com/troglobit/libuev/compare/v1.2.0...HEAD
[v1.2.0]: https://github.com/troglobit/libite/compare/v1.1.1...v1.2.0
[v1.1.1]: https://github.com/troglobit/libite/compare/v1.1.0...v1.1.1
[v1.1.0]: https://github.com/troglobit/libite/compare/v1.0.0...v1.1.0
[v1.0.0]: https://github.com/troglobit/libite/compare/TAIL...v1.0.0
[Finit]:  https://github.com/troglobit/finit/
[TODO.md]: https://github.com/troglobit/libite/blob/master/TODO.md
[README.md]: https://github.com/troglobit/libite/blob/v1.0.0/README.md
[ChangeLog.md]: https://github.com/troglobit/libite/blob/master/ChangeLog.md
